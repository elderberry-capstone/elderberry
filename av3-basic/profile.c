/**
 * profile.c
 *
 *  Created on: Feb 11, 2013
 *      Author: jordan
 */
#include "profile.h"
#include "fcfutils.h"
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
    struct pollfd pfd = {
      .fd = thefd,
      .events = 0,
      .revents = 0
    };
    fcf_addfd(thefd, POLLIN, (pollCallback) &pfd);
}

/**
 * The callback function for profiling.
 * @fn profiling_cb
 * @param None
 * @return None
 *
 */
int profiling_cb() {
    FCF_ProfSendMsg("");
    return 1;
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
