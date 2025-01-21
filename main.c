#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "queue.h"

// int q = 0;
// pthread_mutex_t mx;
// pthread_cond_t cond;

int N = 1;

void *userRoutine(void *queue){
    pthread_t pthid = pthread_self();
    printf("Thread %d = %ld\n", gettid(), pthid);

    // 1
    printQueue((TQueue*)queue);

    // 2
    void *some_msg = (void *) 4;
    addMsg(queue, some_msg);

    // 3
    printQueue((TQueue*)queue);

    // 4
    some_msg = (void *) 5;
    addMsg(queue, some_msg);

    // 5
    printQueue((TQueue*)queue);

    // last
    sleep(5);

    return NULL;
}

int main()
{
    // Initializing threads, mutexes and condition variables
    pthread_t th[N];

    int qsize = 5;
    TQueue *queue = createQueue(qsize);

    // pthread_mutex_init(&mx, NULL);
    // pthread_cond_init(&cond, NULL);
    for (int i = 0; i < N; i++)
    {
        pthread_create(&th[i], NULL, userRoutine, queue);
        usleep(1000000);
    }

    // Waiting for all threads to end
    for (int i = 0; i < N; i++)
    {
        pthread_join(th[i], NULL);
    }
    printf("\nAll threads have returned\n");

    destroyQueue(queue);

    return 0;
}