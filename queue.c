#include "queue.h"

void printQueue(TQueue *queue) // ADDITIONAL
{
    if(queue == NULL){
        return;
    }

    printf("Printing TQueue variables:\n");
    printf("\tsize        -->\t%d\n", queue->size);
    printf("\thead        -->\t%p\n", queue->head);
    printf("\ttail        -->\t%p\n", queue->tail);
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
}

TQueue *createQueue(int size)
{ // READY
    TQueue *queue = (TQueue *)malloc(sizeof(TQueue));
    if (queue == NULL)
    {
        return NULL;
    }
    queue->size = size;
    queue->elems_count = 0;
    queue->subs_count = 0;
    queue->head = NULL;
    queue->tail = NULL;
    queue->subscribers = (pthread_t *)malloc(size * sizeof(pthread_t));
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

void destroyQueue(TQueue *queue)
{
    if(queue == NULL){
        return;
    }

    free((void *)queue->subscribers);
    free((void *)queue);
}

void subscribe(TQueue *queue, pthread_t thread) // READY
{
    if(queue == NULL){
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
        printf("Check\n");
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
    if(queue == NULL){
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
        printf("Check\n");
        queue->subscribers[queue->subs_count] = 0;
        --(queue->subs_count);
    }
    else
    {
        printf("The thread was not subscribed.\n");
    }
}

void addMsg(TQueue *queue, void *msg)
{
    if(queue == NULL){
        return;
    }

    if (queue->elems_count >= queue->size) // Change it into locking later [ ]
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

    element->addr_size = queue->subs_count;

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

    if(queue->elems_count == 0){
        queue->head = element;
    }
    else{   
        queue->tail->next = element;
    }
    queue->tail = element;

    queue->elems_count += 1;
}