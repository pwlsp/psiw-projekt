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
    void *msg1, *msg2, *msg3, *msg4, *msg5, *msg6, *msg7;
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
    void *msg4 = rou_arg.msg4;
    void *msg5 = rou_arg.msg5;
    void *msg6 = rou_arg.msg6;
    void *msg7 = rou_arg.msg7;

    pthread_t myThreadID = pthread_self();

    switch (num)
    {
    case 1:
        // verboseIntroduce(num, myThreadID);
        // addMsg(queue, msg1);
        // sleep(2);

        // introduce(num);
        // addMsg(queue, msg2);
        // sleep(5);

        void *pt = getMsg(queue, myThreadID);
        printf("Getting message... : %p : %s\n", pt, (char *)pt);

        addMsg(queue, msg1);
        subscribe(queue, myThreadID);
        printQueue(queue);
        pt = getMsg(queue, myThreadID);
        printf("elo\n");
        printMsg(pt);

        break;
    case 2:
        sleep(1);

        verboseIntroduce(num, myThreadID);
        subscribe(queue, myThreadID);
        sleep(2);

        introduce(num);
        printf("%d <- getAvail()\n", getAvailable(queue, myThreadID));
        sleep(2);

        break;
    case 3:
        sleep(3);

        usleep(500000);
        verboseIntroduce(num, myThreadID);
        subscribe(queue, myThreadID);
        sleep(1);

        printQueue(queue);
        printMsg(getMsg(queue, myThreadID));
        printf("halo\n");

        break;
    case 4:

        break;
    case 5:

        break;
    case 6:

        break;
    case 7:

        break;

    default:
        break;
    }

    return NULL;
}

int main()
{
    void *msg1 = (void *)malloc(16 * sizeof(char));
    void *msg2 = (void *)malloc(16 * sizeof(char));
    void *msg3 = (void *)malloc(16 * sizeof(char));
    void *msg4 = (void *)malloc(16 * sizeof(char));
    void *msg5 = (void *)malloc(16 * sizeof(char));
    void *msg6 = (void *)malloc(16 * sizeof(char));
    void *msg7 = (void *)malloc(16 * sizeof(char));

    strcpy((char *)msg1, "m1");
    strcpy((char *)msg2, "m2");
    strcpy((char *)msg3, "m3");
    strcpy((char *)msg4, "m4");
    strcpy((char *)msg5, "m5");
    strcpy((char *)msg6, "m6");
    strcpy((char *)msg7, "m7");

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
        rou_arg[i].msg4 = msg4;
        rou_arg[i].msg5 = msg5;
        rou_arg[i].msg6 = msg6;
        rou_arg[i].msg7 = msg7;
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