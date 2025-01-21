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

int N = 1;

typedef struct routineArg
{
    TQueue *queue;
    void *msg;
} routineArg;

void *userRoutine(void *_rou_arg){
    routineArg rou_arg = *(routineArg *)_rou_arg;
    TQueue *queue = rou_arg.queue;
    void *msg = rou_arg.msg;

    pthread_t pthid = pthread_self();
    printf("Thread %d = %ld\n", gettid(), pthid);

    // 1
    printQueue((TQueue*)queue);

    // 2
    addMsg(queue, msg);

    // 3
    printQueue((TQueue*)queue);

    // 4
    addMsg(queue, msg);

    // 5
    printQueue((TQueue*)queue);

    // last
    sleep(5);

    return NULL;
}

int main()
{
    void *msg1 = (void *)malloc(16*sizeof(char));
    void *msg2 = (void *)malloc(16*sizeof(char));
    void *msg3 = (void *)malloc(16*sizeof(char));

    strcpy((char *)msg1, "elo");
    strcpy((char *)msg2, "makrela");
    strcpy((char *)msg3, "kurs robi");

    // Initializing threads, mutexes and condition variables
    pthread_t th[N];

    int qsize = 5;
    TQueue *queue = createQueue(qsize);

    routineArg rou_arg1 = {queue, msg1};

    // pthread_mutex_init(&mx, NULL);
    // pthread_cond_init(&cond, NULL);
    for (int i = 0; i < N; i++)
    {
        pthread_create(&th[i], NULL, userRoutine, (void *)&rou_arg1);
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