#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    int myHeap[5];

    *myHeap = 512;
    *(myHeap + 4) = 552;

    printf("%d\n", *myHeap);
    printf("%d\n\n", *(myHeap + 4));

    printf("%d\n", *(&myHeap[0]));
    printf("%d\n", *(&myHeap[0] + 4));

    return 0;
}