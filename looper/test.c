#include <stdio.h>
#include <stdlib.h>
#include "looper.h"

int hello(){
    printf("Hello, World!\n");
    return 1;
}

int main(int argc, char **argv){
    if (argc != 2){
        printf("ERROR: Expected a number as an argument.\n");
        return 0;
    }
    int iterations = atoi(argv[1]);
    loop(&hello, iterations);
    return 1;
}
