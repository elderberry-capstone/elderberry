/*
 * profile.c
 *
 *  Created on: Feb 11, 2013
 *      Author: jordan
 */
#include <sys/timerfd.h>

// profile.c
static int count = 0;
void InitProfiling() {
    int thefd = timerfd_create(CLOCK_REALTIME, 0);
    fcf_add_fd("timer", thefd, NULL);
}

void *profiling_cb(void) {
    FCF_ProfSendMsg(count);
}

void profReceiveMsg (int count) {// user receives message here
    if (count = 1000000){
        stop_main_loop();
    }
    count++;
}
