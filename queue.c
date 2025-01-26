#include "queue.h"

// ===============================================
//
//  -   sprawdzić na koniec, czy przy tworzeniu
//      queue wszystko jest w porządku (mutex)
//
//  -   ...
//
// ===============================================

pthread_mutex_t mx_queue, mx_com;
pthread_cond_t cond_new_message, cond_free_space;

void printAddressees(TQElement *element) // ADDITIONAL Prints addressees as a list
{
    if (element == NULL)
    {
        printf("printAllMsgs: error");
        return;
    }

    if (element->addr_count == 0)
    {
        return;
    }

    for (int i = 0; i < element->addr_count; i++)
    {
        printf("%ld   ", element->addressees[i]);
    }
}

void printMsg(void *msg) // ADDITIONAL
{
    if (msg == NULL)
    {
        printf("NULL\n");
        return;
    }

    printf("%s", (char *)msg);
    fflush(stdout);

    return;
}

void printGet(void *msg) // ADDITIONAL
{
    pthread_mutex_lock(&mx_queue);

    if (msg == NULL)
    {
        printf("NULL\n");
        return;
    }

    printf("%s <- getMsg()\n", (char *)msg);

    pthread_mutex_unlock(&mx_queue);

    return;
}

void printAllMsgs(TQueue *queue) // ADDITIONAL Prints messages in the printQueue format
{
    if (queue == NULL)
    {
        printf("printAllMsgs: error");
        return;
    }

    TQElement *pt = queue->head;

    printf("\n");
    for (int i = 0; i < queue->msgs_count; i++)
    {
        printf("\t\t%d. \"%s\"\taddr_size = %d, addr_count = %d, addressees: ", i + 1, (char *)pt->msg, pt->addr_size, pt->addr_count);
        printAddressees(pt);
        printf(", next = ");
        if(pt->next != NULL)
        {
            printMsg(pt->next->msg);
        }
        else
        {
            printf("NULL");
            fflush(stdout);
        }
        printf("\n");
        pt = pt->next;
    }
    printf("\n");
}

void printQueue(TQueue *queue) // ADDITIONAL
{
    if (queue == NULL)
    {
        printf("printQueue: error");
        return;
    }

    pthread_mutex_lock(&mx_queue);

    printf("\nPrinting TQueue variables:\n");
    printf("\tsize        -->\t%d\n", queue->size);
    printf("\thead        -->\t%p\n", queue->head);
    printf("\ttail        -->\t%p\n", queue->tail);
    printf("\tsubs_size   -->\t%d\n", queue->subs_size);
    printf("\tsubs_count  -->\t%d\n", queue->subs_count);

    printf("\tsubscribers -->\t");
    for (int i = 0; i < queue->subs_count; i++)
    {
        int s = queue->subscribers[i];

        if (s)
        {
            printf("%ld ", queue->subscribers[i]);
        }
    }
    printf("\n");

    printf("\tmsgs_count  -->\t%d\n", queue->msgs_count);
    printf("\tmessages    -->\n");
    printAllMsgs(queue);
    pthread_mutex_unlock(&mx_queue);
}

void removeEveryMsg(TQueue *queue, void *msg) // ADDITIONAL
{
    if (queue == NULL)
    {
        return;
    }

    if (queue->msgs_count <= 0) // If the queue is empty
    {
        return;
    }

    while (queue->head->msg == msg) // If the message is the head (or the head and the next message(s))
    {
        TQElement *oldHead = queue->head;
        queue->head = queue->head->next;

        free(oldHead->addressees);
        free(oldHead);

        --queue->msgs_count;

        if (queue->msgs_count == 0) // Checking if the head was the only message and if the queue is empty now
        {
            queue->tail = NULL;
            return;
        }
    }

    TQElement *element = queue->head;

    while (element->next != NULL)
    {
        if (element->next->msg == msg)
        {
            if (element->next == queue->tail) // Checking if we remove the last message
            {
                queue->tail = element;
            }

            TQElement *oldNext = element->next;
            element->next = element->next->next;

            free(oldNext->addressees);
            free(oldNext);

            --queue->msgs_count;

            continue;
        }

        element = element->next;
    }
}

TQueue *createQueue(int size) // READY - tu chyba nie potrzeba zamków co nie?
{
    TQueue *queue = (TQueue *)malloc(sizeof(TQueue));
    if (queue == NULL)
    {
        return NULL;
    }
    queue->size = size;
    queue->subs_size = 1;
    queue->msgs_count = 0;
    queue->subs_count = 0;
    queue->head = NULL;
    queue->tail = NULL;
    queue->subscribers = (pthread_t *)malloc(queue->subs_size * sizeof(pthread_t));
    if (queue->subscribers == NULL)
    {
        free(queue);
        return NULL;
    }
    for (int i = 0; i < queue->subs_size; i++)
    {
        queue->subscribers[i] = 0;
    }

    return queue;
}

void destroyQueue(TQueue *queue) // seems READY bez zamków
{
    if (queue == NULL)
    {
        return;
    }

    TQElement *element = queue->head;

    while (element != NULL)
    {
        TQElement *next = element->next;
        free(element->addressees);
        free(element);
        element = next;
    }

    free(queue->subscribers);
    free(queue);
}

void subscribe(TQueue *queue, pthread_t thread) // seems READY bez zamków
{
    if (queue == NULL)
    {
        return;
    }

    int already_there = 0;
    for (int i = 0; i < queue->subs_count; i++)
    {
        if (queue->subscribers[i] == thread)
        {
            already_there = 1;
            break;
        }
    }

    if (!already_there)
    {
        if (queue->subs_count < queue->subs_size)
        {
            queue->subscribers[queue->subs_count] = thread;
            ++queue->subs_count;
        }
        else // If the array is too small - realloc() with bigger size
        {
            queue->subscribers = realloc(queue->subscribers, queue->subs_size + 1);
            ++queue->subs_size;

            queue->subscribers[queue->subs_count] = thread;
            ++queue->subs_count;
        }
    }

    else
    {
        printf("The thread is already subscribed.\n");
    }
}

void unsubscribe(TQueue *queue, pthread_t thread) // seems READY bez zamków
{
    if (queue == NULL)
    {
        return;
    }

    int is_there = 0;
    for (int i = 0; i < queue->subs_count; i++)
    {
        if (queue->subscribers[i] == thread)
        {
            is_there = 1;
        }

        if (is_there && i < queue->subs_count - 1)
        {
            queue->subscribers[i] = queue->subscribers[i + 1];
        }
    }
    if (is_there)
    {
        queue->subscribers[queue->subs_count - 1] = 0; // Removing last element from subscribers (either the one we should remove or a duplicate creating while shifting the array to the left)
        --queue->subs_count;

        TQElement *element = queue->head;
        while (element != NULL) // Checking every element if it had the removed subscriber in its addressees
        {
            int is_there2 = 0;

            for (int i = 0; i < element->addr_count; i++)
            {
                if (element->addressees[i] == thread)
                {
                    is_there2 = 1;
                }

                if (is_there2 && i < element->addr_count - 1)
                {
                    element->addressees[i] = element->addressees[i + 1];
                }
            }

            if (is_there2)
            {
                element->addressees[element->addr_count - 1] = 0;
                --element->addr_count;
            }

            element = element->next;
        }
    }
    else
    {
        printf("The thread was not subscribed.\n");
    }
}

void addMsg(TQueue *queue, void *msg)
{
    if (queue == NULL)
    {
        return;
    }

    pthread_mutex_lock(&mx_queue);

    while (queue->msgs_count >= queue->size) // Checking if there is free space for a message
    {
        printf("addMsg: Waiting...\n");
        pthread_mutex_lock(&mx_com);
        pthread_mutex_unlock(&mx_queue);
        pthread_cond_wait(&cond_free_space, &mx_com);
        pthread_mutex_lock(&mx_queue);
        pthread_mutex_unlock(&mx_com);
    }

    if (queue->subs_count == 0) // If there is no subscribers - exit
    {
        printf("addMsg: No subscribers\n");
        pthread_mutex_unlock(&mx_queue);
        return;
    }

    TQElement *element = (TQElement *)malloc(sizeof(TQElement));
    if (element == NULL)
    {
        printf("addMsg: Memory allocation error\n");
        pthread_mutex_unlock(&mx_queue);
        return;
    }

    element->addr_size = queue->subs_count;
    element->addr_count = queue->subs_count;

    element->addressees = (pthread_t *)malloc(queue->subs_count * sizeof(pthread_t));
    if (element->addressees == NULL)
    {
        free(element);
        printf("addMsg: Memory allocation error\n");
        pthread_mutex_unlock(&mx_queue);
        return;
    }

    for (int i = 0; i < queue->subs_count; i++) // Assigning current subscribers as addressees
    {
        element->addressees[i] = queue->subscribers[i];
    }

    element->msg = msg;
    element->next = NULL;

    if (queue->msgs_count == 0)
    {
        queue->head = element;
    }
    else
    {
        queue->tail->next = element;
    }

    queue->tail = element;
    queue->msgs_count += 1;


    pthread_mutex_lock(&mx_com);
    pthread_cond_signal(&cond_new_message);
    pthread_mutex_unlock(&mx_com);
    
    printf("addMsg: Message added\n");
    
    pthread_mutex_unlock(&mx_queue);
}

void *getMsg(TQueue *queue, pthread_t thread)
{
    if (queue == NULL)
    {
        return NULL;
    }

    int is_subscribed = 0;

    pthread_mutex_lock(&mx_queue);

    for (int i = 0; i < queue->subs_count; i++) // Checking if the thread is subscribed
    {
        if (queue->subscribers[i] == thread)
        {
            is_subscribed = 1;
            break;
        }
    }

    if (is_subscribed == 0) // If the thread is not subscribed - return NULL and quit
    {
        pthread_mutex_unlock(&mx_queue);
        return NULL;
    }

    while(1) // Loop, because we want to search for the message every time there is a new one until we find one
    {
        TQElement *element = queue->head;

        while (element != NULL) // Checking every element if it has a message for this thread
        {
            int is_addressee = 0;
            void *msg = element->msg;

            for (int i = 0; i < element->addr_count; i++) // "Is the message in this element?"
            {
                if (element->addressees[i] == thread)
                {
                    is_addressee = 1;
                }

                if (is_addressee && i < element->addr_count - 1)
                {
                    element->addressees[i] = element->addressees[i + 1];
                }
            }

            if (is_addressee) // "We found the message"
            {
                element->addressees[element->addr_count - 1] = 0;
                --element->addr_count;

                if (element->addr_count == 0) // If it was the last addressee, we delete the message and send a signal about a new free space
                {
                    removeMsg(queue, msg);
                    pthread_mutex_lock(&mx_com);
                    pthread_cond_signal(&cond_free_space);
                    pthread_mutex_unlock(&mx_com);
                }

                pthread_mutex_unlock(&mx_queue);
                return msg;
            }

            element = element->next;
        }
        
        printf("getMsg: Waiting...\n");
        pthread_mutex_lock(&mx_com);
        pthread_mutex_unlock(&mx_queue);
        pthread_cond_wait(&cond_new_message, &mx_com);
        pthread_mutex_lock(&mx_queue);
        pthread_mutex_unlock(&mx_com);
    }
}

int getAvailable(TQueue *queue, pthread_t thread) // seems READY bez zamków
{
    if (queue == NULL)
    {
        return -1;
    }

    int available = 0;
    TQElement *element = queue->head;

    while (element != NULL)
    {
        for (int i = 0; i < element->addr_count; i++)
        {
            if (element->addressees[i] == thread)
            {
                ++available;
            }
        }

        element = element->next;
    }

    return available;
}

void removeMsg(TQueue *queue, void *msg) // seems READY bez zamków
{
    if (queue == NULL)
    {
        return;
    }

    if (queue->msgs_count <= 0) // If the queue is empty
    {
        return;
    }

    if (queue->head->msg == msg) // If the message is the head
    {
        TQElement *oldHead = queue->head;
        queue->head = queue->head->next;

        free(oldHead->addressees);
        free(oldHead);

        --queue->msgs_count;

        if (queue->msgs_count == 0) // Checking if the head was the only message and if the queue is empty now
        {
            queue->tail = NULL;
        }
        return;
    }

    TQElement *element = queue->head;

    while (element->next != NULL)
    {
        if (element->next->msg == msg)
        {
            if (element->next == queue->tail) // Checking if we remove the last message
            {
                queue->tail = element;
            }

            TQElement *oldNext = element->next;
            element->next = element->next->next;

            free(oldNext->addressees);
            free(oldNext);

            --queue->msgs_count;

            return;
        }

        element = element->next;
    }
}

void setSize(TQueue *queue, int size) // seems READY bez zamków
{
    if (size >= queue->msgs_count)
    {
        queue->size = size;
        return;
    }
    else
    {
        queue->size = size;
        TQElement *element = queue->head;

        for (int i = 0; i < size - 1; i++)
        {
            element = element->next;
        }

        TQElement *next = element->next;
        element->next = NULL;
        element = next;

        while (element != NULL)
        {
            next = element->next;
            free(element->addressees);
            free(element);

            --queue->msgs_count;

            element = next;
        }
    }
}
