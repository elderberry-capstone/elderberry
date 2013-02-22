/**
 *  @file fcfutils.c
 *  @brief Utility functions for the flight control framework
 *  @author Ron Astin
 *  @date February 8th, 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <libusb-1.0/libusb.h>
#include "fcfutils.h"


/**
*	\struct fcffd
*	
*	This struct holds the callback functions and souce tokens of the devices.
*/
struct fcffd {
	const char *token;	/**< The short name of the device. */
	pollfd_callback callback;
};

static const char standard = 0;	///< standard callback
static const char ppc = 1;		///< per poll cycle callback

//static int MAXFD = 100;
static struct pollfd fds[100];
static struct fcffd fdx[100];
static char cbCat[100];			///< the type (i.e. category) of the callback (standard, ppc)
static int nfds = 0;
static int run_fc = 0;	//< main loop is running true/false


static void debug_fd (const char *msg, int i, struct pollfd *pfd);


int fcf_add_fd(const char *token, int fd, short events, pollfd_callback cb){
	fds[nfds].fd = fd;
	fds[nfds].events = events;
	fdx[nfds].token = token;
	fdx[nfds].callback = cb;
	cbCat[nfds] = standard;
	nfds++;
	printf("Added %s fd: %d. FD count: %d\n", token, fd, nfds);
	return nfds-1;
}


//add per poll cycle file descriptor
int fcf_addfd_ppc (const char *token, int fd, short events, pollfd_callback cb)
{
	int i = fcf_add_fd (token, fd, events, cb);
	if (i >= 0) {
		cbCat[i] = ppc;
	}
	return i;
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


void fcf_stop_main_loop() {
	run_fc = 0;
}


static void fcf_start_main_loop() {
	run_fc = 1;
}


//currently returns -1 on error; 0 on success
int fcf_run_poll_loop() {
	pollfd_callback ppc[nfds];
	int nppc= 0;
	int count = 0;
	int ret = 0;

	fcf_start_main_loop();
	while (run_fc) {

		for (int i = 0; i < nfds; i++) {
			debug_fd("\nbefore poll<<< ", i, &fds[i]);
		}
		printf("\nwaiting");
		fflush (stdout);

		errno = 0;
		int rc = poll(fds, nfds, -1);
		printf ("\n%d. poll returned with rc=%d errno=%d", count++, rc, errno);
		for (int i = 0; i < nfds; i++) {
			debug_fd("\n   after poll>>> ", i, &fds[i]);
		}

		switch (rc) {
		case -1: //error
			if (errno != EINTR) {
				perror ("run_main_loop: poll returned with error");
				ret = -1;
				fcf_stop_main_loop();
			}
			break;
		case 0: //timeout
			printf("poll timed out");
			break;
		default:
			nppc = 0;
			for (int i = 0; i < nfds && rc > 0; i++) {
				if(fds[i].revents != 0) {
					debug_fd("\n active fd ", i, &fds[i]);
					rc--;
					if (cbCat[i] == standard) {
						//callback for this active fd is a standard callback
						fdx[i].callback(i);
					} else {
						//callback for this active fd is a "per poll cycle" callback
						int j;
						for (j = 0; j < nppc && ppc[j] != fdx[i].callback; j++)
						{ /*empty*/ }
						if (j == nppc) {
							//a new ppc callback
							//add to ppc so that callback will be called at end of poll cycle
							ppc[nppc++] = fdx[i].callback;
						} else {
							//we have seen this callback before
							printf ("\n multiple active ppc, ignoring callback for fd[%d]: fd=%d", i, fds[i].fd);
						}
					}
				} // (revents set)
			} // (for i)

			//handle ppc callbacks
			for (int j = 0; j < nppc; j++) {
				//if callback wants to access the fds, callback
				//is expected to know the indices into the fds array
				//i.e., module must store return values it gets from fcf_addfdPpc
				printf ("\n calling ppc callback [%d]", j);
				ppc[j](-1);	//we cannot pass in one value here
			}

			break;
		}

	}

	printf("\n exiting main loop");
	return ret;
}

static void debug_fd (const char *msg, int i, struct pollfd *pfd) {
	printf("%s fd[%d]: fd=%d events=%X revents=%X", msg, i, pfd->fd, pfd->events, pfd->revents);
	int re = pfd->revents;
	if (re & POLLERR) printf("\nPOLLERR - Error condition");
	if (re & POLLHUP) printf("\nPOLLHUP - Hang up");
	if (re & POLLNVAL) printf("\nPOLLNVAL - Invalid request: fd not open");
	fflush (stdout);
}
