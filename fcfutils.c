<<<<<<< HEAD
/**
 *  @file fcfutils.c
 *  @brief Utility functions for the flight control framework
 *  @author Ron Astin
 *  @date February 8th, 2013
 */
=======

/**
*
*	\brief API code for framework.
*
*	This file includes the functions needed to add and remove file
*	descriptors, as well as store the file descriptor arrays.
*/
>>>>>>> 7890465c3e3d2b37b87354337013ee759c0ab4e7

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include "fcfutils.h"
#include "fcfmain.h"


/**
*	\struct fcffd
*	
*	This struct holds the callback functions and souce tokens of the devices.
*/
struct fcffd {
	const char *token;	/**< The short name of the device. */
	pollfd_callback callback;
};

//static int MAXFD = 100;
static struct pollfd fds[100];
static struct fcffd fdx[100];
static int nfds = 0;
7
	fds[nfds].fd = fd;
	fds[nfds].events = events;
	fdx[nfds].token = token;
	fdx[nfds].callback = cb;
	nfds++;
	printf("Added %s fd: %d. FD count: %d\n", token, fd, nfds);
	return nfds-1;
}

int fcf_remove_all_fd(const char *fd_src) {
  // Remove all file descriptors that were added under given source token.

	// If there are no fds, return 0 -- or error code.
	if(nfds <= 0)
		return 0;

	int i = 0, removed = 0;
	
	for(i=0; i<nfds; i++){
		// NOTE: if non-null-terminated strings, use strncmp with length
		if(strcmp(fdx[i].token, fd_src) == 0){

			// If matching fd is last in array.
			if(nfds - 1 == i){
				nfds--;
				removed++;
				return removed;
			}
			
			// Replace fd at index i with last fd in array.
			memmove(&fds[i], &fds[nfds - 1], sizeof(struct pollfd));

			// Replace fcffd at index i with last fcffd in array.
			memmove(&fdx[i], &fdx[nfds - 1], sizeof(struct fcffd));
			
			// Decrement number of fds and increment amount removed.
			nfds--;
			removed++;
			printf("FD removed at index %d. Total: %d\n", i, nfds);
		}
	}

	return removed;
}

struct pollfd *fcf_get_fd(int idx){
	return &fds[idx];
}


int fcf_run_poll_loop(void){
	int rc;
	int timeout = -1 * 1000;	// in ms

	for (;;)
	{
		/**
		*	Call poll() and wait for it to complete.
		*/
		//printf("Waiting on poll()...\n");
		rc = poll(fds, nfds, timeout);

		/**
		*	Check to see if the poll call failed. 
		*/
		if (rc < 0){
			perror("  poll() failed");
			break;
		}

		/**
		*	poll timed out 
		*/
		if (rc == 0){
			//do something useful, e.g. call into libusb so that libusb can deal with timeouts
			printf("poll() timed out.\n");
			//mouse_handler(0);
		}


		/**
		*	One or more descriptors are readable.  Need to 
		*	determine which ones they are. 
		*/
		int current_size = nfds;
		for (int i = 0; i < current_size; i++){
			/*********************************************************/
			/* Loop through to find the descriptors that returned    */
			/* POLLIN and determine whether it's the listening       */
			/* or the active connection.                             */
			/*********************************************************/

			

			if(fds[i].revents == 0) {
				continue;
			}

			int re = fds[i].revents;
			if (re & POLLERR) printf("POLLERR - Error condition\n");
			if (re & POLLHUP) printf("POLLHUP - Hang up\n");
			if (re & POLLNVAL) printf("POLLNVAL - Invalid request: fd not open\n");

			/*********************************************************/
			/* If revents is not POLLIN, it's an unexpected result,  */
			/* log and end the server.                               */
			/*********************************************************/
//			if(fds[i].revents != POLLIN)
//			{
//				printf("  Error! revents = %d\n", fds[i].revents);
//
//			}

			//printf("  Descriptor %d is readable\n", fds[i].fd);

			fdx[i].callback(i);
		}
	}

	return 0;
}
