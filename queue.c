#include "queue.h"

// Not freed objects in all cases yet:

void printAddressees(TQElement *element) // ADDITIONAL Prints addressees as a list
{
    if (element == NULL)
    {
        printf("printMsgs: error");
        return;
    }

    if (element->addr_count == 0)
    {
        printf("0 addressees");
        return;
    }

    printf("%d addressees: ", element->addr_count);

    for (int i = 0; i < element->addr_count; i++)
    {
        printf("%ld   ", element->addressees[i]);
    }
}

void printMsgs(TQueue *queue) // ADDITIONAL Prints messages in the printQueue format
{
    if (queue == NULL)
    {
        printf("printMsgs: error");
        return;
    }

    TQElement *pt = queue->head;

    printf("\n");
    for (int i = 0; i < queue->msgs_count; i++)
    {
        printf("\t\t%d. \"%s\"\tsent to ", i + 1, (char *)pt->msg);
        printAddressees(pt);
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

    printf("Printing TQueue variables:\n");
    printf("\tsize        -->\t%d\n", queue->size);
    printf("\thead        -->\t%p\n", queue->head);
    printf("\ttail        -->\t%p\n", queue->tail);
    printf("\tsubs_count  -->\t%d\n", queue->subs_count);
    printf("\tmsgs_count  -->\t%d\n", queue->msgs_count);

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

    printf("\tmessages    -->\n");
    printMsgs(queue);
}

TQueue *createQueue(int size) // subscribers jest źle (rozmiar)
{
    TQueue *queue = (TQueue *)malloc(sizeof(TQueue));
    if (queue == NULL)
    {
        return NULL;
    }
    queue->size = size;
    queue->subs_size = 10; // ZMIENIĆ TO TRZEBA!
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
    for (int i = 0; i < N; i++)
    {
        queue->subscribers[i] = 0;
    }
    return queue;
}

void destroyQueue(TQueue *queue) // READY bez zamków
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

void subscribe(TQueue *queue, pthread_t thread) // READY bez zamków
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
        queue->subscribers[queue->subs_count] = thread;
        ++(queue->subs_count);
    }

    else
    {
        printf("The thread is already subscribed.\n");
    }
}

void unsubscribe(TQueue *queue, pthread_t thread) 
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
        queue->subscribers[queue->subs_count] = 0;
        --(queue->subs_count);
    }
    else
    {
        printf("The thread was not subscribed.\n");
    }
}

void addMsg(TQueue *queue, void *msg) // READY bez zamków
{
    if (queue == NULL)
    {
        return;
    }

    if (queue->msgs_count >= queue->size) // Change it into locking later [ ]
    {
        printf("Not enough space.\n");
        return;
    }
    TQElement *element = (TQElement *)malloc(sizeof(TQElement));
    if (element == NULL)
    {
        printf("addMsg: Memory allocation error\n");
        return;
    }

    element->addr_count = queue->subs_count;

    element->addressees = (pthread_t *)malloc(queue->subs_count * sizeof(pthread_t));
    if (element->addressees == NULL)
    {
        free(element);
        return;
    }

    for (int i = 0; i < queue->subs_count; i++)
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
}

void removeMsg(TQueue *queue, void *msg) // READY bez zamków
{
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
