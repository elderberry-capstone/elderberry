#include "looper.h"
#include <stdio.h>

int loop(int (*cb) (void), int times){
    int i = 0;
    for (i = 0; i < times; ++i){
        cb();
    }
    
    printf ("\nFunction successfully ran %d times.\n", times);
}
