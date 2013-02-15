/*
 * fcfutils.c
 *
 */
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "fcfutils.h"
#include "miml.h"

#define FCF_POLLSIZE 100


struct pollfd fds[FCF_POLLSIZE];
static int nfds;
static pollCallback callbacks[FCF_POLLSIZE];
static int run_fc = 0;


int fcf_addfd (int fd, short events, pollCallback cb) {
	int *p = &(nfds);

	if (*p >=  FCF_POLLSIZE) {
		return -1;
	}

	fds[*p].fd = fd;
	fds[*p].events = events;
	callbacks[*p] = cb;
	(*p)++;
	return 0;
}

//remove file descriptor
int fcf_removefd (int fd) {
	int *p = &(nfds);

	for (int i = 0; i < *p; i++) {
		if (fds[i].fd == fd) {
			//we found fd we want to remove
			//remove fd at index i by overwriting it with last fd
			if (*p > 1) {
				fds[i] = fds[*p - 1];
				callbacks[i] = callbacks[*p - 1];
				(*p)--;
				return 0;
			}
		}
	}

	return -1; //fd was not in array
}


void stop_main_loop() {
	run_fc = 0;
}


void run_main_loop() {

	run_fc = 1;
	int count = 0;

	while (run_fc) {

		for (int i = 0; i < nfds; i++) {
			printf("\nbefore poll<<< fd[%d]: fd=%d events=%X", i, fds[i].fd, fds[i].events);
		}
		printf("\nwaiting");
		fflush (stdout);

		errno = 0;
		int rc = poll(fds, nfds, -1);
		printf ("\n%d. poll returned with rc=%d ", count++, rc);
		for (int i = 0; i < nfds; i++) {
			printf("\n   after poll>>> fd[%d]: fd=%d events=%X revents=%X", i, fds[i].fd, fds[i].events, fds[i].revents);
		}
		fflush (stdout);


		switch (rc) {
		case -1: //error
			FCFERR_Poll();
			break;
		case 0: //timeout
			printf("poll timed out");
			break;
		default:
			for (int i = 0; i < nfds; i++) {
				if(fds[i].revents != 0) {

					int re = fds[i].revents;
					if (re & POLLERR) printf("\nPOLLERR - Error condition");
					if (re & POLLHUP) printf("\nPOLLHUP - Hang up");
					if (re & POLLNVAL) printf("\nPOLLNVAL - Invalid request: fd not open");

					//active fd or error
					printf("\nfd[%d] is active, fd=%d events=%X revents=%X", i, fds[i].fd, fds[i].events, fds[i].revents);
					callbacks[i](&fds[i]);
					//libusb_handle_events_timeout(usb_source->context, &nonblocking);
				}

			}
			break;
		}

	}

	printf("\n exiting main loop");

}


