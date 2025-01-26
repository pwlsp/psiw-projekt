#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include "queue.h"

int N = 3;

typedef struct routineArg
{
    int num;
    TQueue *queue;
    void *msg1;
    void *msg2;
    void *msg3;
} routineArg;

void introduce(int num)
{
    printf("========================================\n");
    printf("#  Thread num = %d\n", num);
    printf("========================================\n\n");

    return;
}

void verboseIntroduce(int num, pthread_t myThreadID)
{
    printf("========================================\n");
    printf("#  Thread %d = %ld\n", gettid(), myThreadID);
    printf("#  num = %d\n", num);
    printf("========================================\n\n");

    return;
}

void *userRoutine(void *_rou_arg)
{
    routineArg rou_arg = *(routineArg *)_rou_arg;
    int num = rou_arg.num;
    TQueue *queue = rou_arg.queue;
    void *msg1 = rou_arg.msg1;
    void *msg2 = rou_arg.msg2;
    void *msg3 = rou_arg.msg3;

    pthread_t myThreadID = pthread_self();

    switch (num)
    {
    case 1:
        subscribe(queue, myThreadID);
        addMsg(queue, msg1);
        printQueue(queue);
        unsubscribe(queue, myThreadID);
        printQueue(queue);
        addMsg(queue, msg2);
        printQueue(queue);

        // printQueue(queue);
        // unsubscribe(queue, myThreadID);
        // printQueue(queue);
        // addMsg(queue, msg2);
        // printQueue(queue);

        break;
    case 2:

        break;
    case 3:
        // subscribe(queue, myThreadID);
        // usleep(1000000);
        // getMsg(queue, myThreadID);
        

        break;

    default:
        addMsg(queue, msg1);
        addMsg(queue, msg2);
        addMsg(queue, msg3);
        break;
    }

    return NULL;
}

int main()
{
    void *msg1 = (void *)malloc(16 * sizeof(char));
    void *msg2 = (void *)malloc(16 * sizeof(char));
    void *msg3 = (void *)malloc(16 * sizeof(char));

    strcpy((char *)msg1, "m1");
    strcpy((char *)msg2, "m2");
    strcpy((char *)msg3, "m3");

    // Initializing threads, mutexes and condition variables
    pthread_t th[N];

    int qsize = 3;
    TQueue *queue = createQueue(qsize);

    routineArg rou_arg[N];
    for (int i = 0; i < N; i++)
    {
        rou_arg[i].num = i + 1;
        rou_arg[i].queue = queue;
        rou_arg[i].msg1 = msg1;
        rou_arg[i].msg2 = msg2;
        rou_arg[i].msg3 = msg3;
    }

    for (int i = 0; i < N; i++)
    {
        pthread_create(&th[i], NULL, userRoutine, &rou_arg[i]);
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