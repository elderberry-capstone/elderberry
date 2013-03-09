/*
 * module_profile.c
 *
 */
#include <sys/timerfd.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "module_profile.h"
#include "fcfutils.h"

#define MAX_COUNT 10000000
static const int PROFILEMODE = 1;

static int count = 0; //!< The number of times the loop has run.
static unsigned char buf[1024];
static int fd;	//!< timer fd
static struct itimerspec t;
static struct timeval start;
static struct timeval end;


/**
 * The callback function for profiling.
 * Sent data is ignored, so we will fall through poll(2) next time around.
 * @fn profiling1_cb
 * @param None
 * @return None
 */
static void profiling1_cb (struct pollfd * pfd) {
	sendMessage_profile (NULL, 0);	//send messages
}

/**
 * The callback function for profiling.
 * Sent data is consumed and a new timer fd is set up.
 * @fn profiling2_cb
 * @param None
 * @return None
 */
static void profiling2_cb (struct pollfd * pfd) {
	int act_len = read (pfd->fd, buf, sizeof(buf));
	sendMessage_profile (buf, act_len);	//send messages
	timerfd_settime(fd, 0, &t, NULL);	//set up next timer
}
/**
 * Initialize the profiling system.
 * @fn init_profiling
 * @param None
 * @return None
 */
void init_profiling() {
	gettimeofday(&start,NULL);

	pollfd_callback cb = NULL;
	t.it_interval.tv_sec = 0;
	t.it_interval.tv_nsec = 0;
	t.it_value.tv_sec = 0;
	t.it_value.tv_nsec = 1;
	fd = timerfd_create(CLOCK_MONOTONIC, 0);
	timerfd_settime(fd, 0, &t, NULL);
	switch (PROFILEMODE) {
	case 1:
		cb = profiling1_cb;
		break;
	case 2:
		cb = profiling2_cb;
		break;
	default:
		printf ("\nno profiling\n");
		return;
	}
	fcf_add_fd (fd, POLLIN, cb);
	printf ("\nprofile fd : %d", fd);
}


/**
 * Receive the message. if we've received MAX_COUNT, stop the loop.
 * @fm getMessage_profile
 * @param int count
 * @return
 */
void getMessage_profile(unsigned char *buf, int len) {
	count++;
	//printf("\nReceived %d out of %d messages.", count, MAX_COUNT);
	if (count == MAX_COUNT) {
		gettimeofday(&end, NULL);
		printf("\n\nFinsished with count: %d in %d.%d03 sec\n\n", count, (int)(end.tv_sec - start.tv_sec), (int)(end.tv_usec - start.tv_usec));
		fcf_stop_main_loop();
	}
}
