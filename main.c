#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include "queue.h"

// int q = 0;
// pthread_mutex_t mx;
// pthread_cond_t cond;

int N = 2;

typedef struct routineArg
{
    int num;
    TQueue *queue;
    void *msg1, *msg2;
} routineArg;

void *userRoutine(void *_rou_arg)
{
    routineArg rou_arg = *(routineArg *)_rou_arg;
    int num = rou_arg.num;
    TQueue *queue = rou_arg.queue;
    void *msg1 = rou_arg.msg1;
    void *msg2 = rou_arg.msg2;

    pthread_t myThreadID = pthread_self();
    printf("========================================\n");
    printf("Thread %d = %ld\n", gettid(), myThreadID);
    printf("num = %d\n", num);
    printf("msg1 = %s\n", (char *)msg1);
    printf("msg2 = %s\n", (char *)msg2);
    printf("Message address = %p\n", msg1);
    printf("========================================\n\n");

    if (num == 1)
    {
        addMsg(queue, msg1);
        printQueue(queue);

        subscribe(queue, myThreadID);
        printQueue(queue);

        addMsg(queue, msg1);
        printQueue(queue);
        
        sleep(2);
    }

    else if (num == 2)
    {
        addMsg(queue, msg2);
        printQueue(queue);

        subscribe(queue, myThreadID);
        printQueue(queue);

        addMsg(queue, msg2);
        addMsg(queue, msg2);
        addMsg(queue, msg2);
        addMsg(queue, msg2);
        addMsg(queue, msg2);
        printQueue(queue);

        unsubscribe(queue, queue->subscribers[1]);
        printQueue(queue);
        unsubscribe(queue, queue->subscribers[0]);
        printQueue(queue);
        
        sleep(2);
    }

    return NULL;
}

int main()
{
    void *msg1 = (void *)malloc(16 * sizeof(char));
    void *msg2 = (void *)malloc(16 * sizeof(char));
    void *msg3 = (void *)malloc(16 * sizeof(char));

    strcpy((char *)msg1, "elo");
    strcpy((char *)msg2, "makrela");
    strcpy((char *)msg3, "kurs");

    // Initializing threads, mutexes and condition variables
    pthread_t th[N];

    int qsize = 5;
    TQueue *queue = createQueue(qsize);

    routineArg rou_arg[N];
    for (int i = 0; i < N; i++)
    {
        rou_arg[i].num = i + 1;
        rou_arg[i].queue = queue;
        rou_arg[i].msg1 = msg1;
        rou_arg[i].msg2 = msg2;
    }

    // pthread_mutex_init(&mx, NULL);
    // pthread_cond_init(&cond, NULL);
    for (int i = 0; i < N; i++)
    {
        pthread_create(&th[i], NULL, userRoutine, (void *)&rou_arg[i]);
        usleep(1000000);
    }

    // Waiting for all threads to end
    for (int i = 0; i < N; i++)
    {
        pthread_join(th[i], NULL);
    }
    printf("\nAll threads have returned\n");

    destroyQueue(queue);

    free(msg1);
    free(msg2);
    free(msg3);

    return 0;
}