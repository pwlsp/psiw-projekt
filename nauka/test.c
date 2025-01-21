#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

int main()
{
    void *a = NULL;
    printf("%p\n", a);

    return 0;
}