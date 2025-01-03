#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char *myHeap = (char*)malloc(1024);
    if (NULL != myHeap){
        strcpy(myHeap, "Hey there, memory!\n");
    }

    printf("%s", myHeap);

    free(myHeap);

    return 0;
}