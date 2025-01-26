#ifndef LCL_QUEUE_H
#define LCL_QUEUE_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Structures:

typedef struct TQElement
{
    void *msg;
    int addr_count;
    int addr_size;
    pthread_t *addressees;
    struct TQElement *next;
} TQElement;

typedef struct TQueue
{
    pthread_mutex_t mx_queue;
    // pthread_mutex_t mx_com;
    pthread_cond_t cond_new_message, cond_free_space;
    int size;
    int msgs_count;
    int subs_count;
    int subs_size;
    TQElement *head;
    TQElement *tail;
    pthread_t *subscribers;
} TQueue;

// Functions:

TQueue* createQueue(int size);

void destroyQueue(TQueue *queue);

void subscribe(TQueue *queue, pthread_t thread);

void unsubscribe(TQueue *queue, pthread_t thread);

void addMsg(TQueue *queue, void *msg);

void* getMsg(TQueue *queue, pthread_t thread);

int getAvailable(TQueue *queue, pthread_t thread);

void removeMsg(TQueue *queue, void *msg);

void setSize(TQueue *queue, int size);

// Utility functions: (not used in the problem)

// void printAddressees(TQElement *element);

// void printMsg(void *msg);

// void printGet(void *msg);

// void printAllMsgs(TQueue *queue);

// void printQueue(TQueue *queue);

// void removeEveryMsg(TQueue *queue, void *msg);

#endif //LCL_QUEUE_H