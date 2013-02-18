/**
 * profile.c
 *
 *  Created on: Feb 11, 2013
 *      Author: jordan
 */
#include <sys/timerfd.h>
#include <stdio.h>
#include "usbutils.h"
#include "fc.h"
#include "profile.h"
#define MAX_COUNT 10

// profile.c

static int count = 0; //!< The number of times the loop has run.

/**
 * Initialize the profiling system.
 * @fn InitProfiling
 * @param None
 * @return None
 *
 */
void InitProfiling(libusbSource * src) {
    int thefd = timerfd_create(CLOCK_REALTIME, 0);
    fcf_addfd(thefd, POLLIN, NULL);
}

/**
 * The callback function for profiling.
 * @fn profiling_bp
 * @param None
 * @return None
 *
 */
void profiling_cb() {
    FCF_ProfSendMsg("");
}

/**
 * Recieve the message. if we've recieved MAX_COUNT, stop the loop.
 * @fm profReceiveMsg
 * @param int count
 * @return
 *
 */
void profReceiveMsg (const char *msg) {// user receives message here
    printf("Received %d out of %d messages.\n", count, MAX_COUNT);
    if (count == MAX_COUNT){
        stop_main_loop();
    }
    count++;
}
