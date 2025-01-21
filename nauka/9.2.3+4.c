#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int b = 12;

void *worker(void *info)
{
    void *pb = &b;

    for (int i = 0; i < 2; i++)
    {
        sleep(1);

        printf("\nThread 1\n");
        if(info == NULL){
            printf("void *info is NULL\n");
        }
        else{
            printf("info = %p\n*(int*)info = %d\n", info, *(int*)info);
        }

        printf("pb = %p\n", pb);
        printf("b = %d", *(int*)pb);
    }

    return pb;
}
int main()
{
    int a = 5;
    int *pa = &a;
    pthread_t th;
    pthread_create(&th, NULL, worker, pa);
    for (int i = 0; i < 2; i++)
    {
        sleep(1);
        printf("\nMain program\n");
    }

    void *pt;
    void **ppt = &pt;
    
    pthread_join(th, ppt);

    int t = *(int*)pt;

    printf("\n\nReturned to the main program.\n");
    if(!pt){
        printf("The returned pointer is NULL.\n");
    }
    else{
        printf("pt = %p\n", pt);
        printf("t = %d\n", t);
    }
}