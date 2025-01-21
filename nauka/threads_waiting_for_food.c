#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int q = 0;
pthread_mutex_t mx;
pthread_cond_t cond;

void eat(int thread_id){
    pthread_mutex_lock(&mx);

    while (q < 1){
        printf("Thread %d: Tried to eat it, but there is nothing there. Waiting...\n", thread_id);
        pthread_cond_wait(&cond, &mx);
    }

    printf("Thread %d: *Nom-nom-nom*\n", thread_id);

    pthread_mutex_unlock(&mx);
}

void increment_q(){
    pthread_mutex_lock(&mx);

    ++q;
    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&mx);
}

void *threadSubscriber(void *info)
{
    int thread_id = *(int *)info;
    printf("Thread %d ready\n", thread_id);

    eat(thread_id);

    return NULL;
}

int main()
{
    // Initializing threads, mutexes and condition variables
    int N = 5;
    pthread_t th[N];
    int thread_id[N];

    pthread_mutex_init(&mx, NULL);
    pthread_cond_init(&cond, NULL);
    for (int i = 0; i < N; i++)
    {
        thread_id[i] = i + 1;
        // usleep(100000);
        pthread_create(&th[i], NULL, threadSubscriber, &thread_id[i]);
    }

    // 
    for (int i = 0; i < N; i++){
        // sleep(3);
        increment_q();
    }

    // Waiting for all threads to end
    for (int i = 0; i < N; i++)
    {
        pthread_join(th[i], NULL);
    }
    printf("\nAll threads have returned\n");

    return 0;
}