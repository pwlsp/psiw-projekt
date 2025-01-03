#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    int *myHeap = malloc(5 * sizeof(int));
    if (NULL != myHeap){
        printf("Allocation success\n");
    }

    *myHeap = 51;
    *(myHeap + 4) = 55;

    printf("%d\n", *myHeap);
    printf("%d\n", *(myHeap + 4));

    free(myHeap);

    return 0;
}