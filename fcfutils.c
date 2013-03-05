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
#include <signal.h>
#include "fcfutils.h"

#define FDS_INIT_SIZE 1
#define FDS_EXPANSION_FACTOR 2


/*	
 *	This struct holds the callback functions and souce tokens of the devices.
 */
struct fcffd{
  const char *token;	/**< The short name of the device. */
  pollfd_callback callback;
  char cb_cat;
};

static const char STANDARD = 0;	//< Standard callback
static const char PPC = 1;		//< Per poll cycle callback

static struct pollfd * fds;		//< File descriptor array
static struct fcffd  * fdx;		//< File description array
static int nfds;				//< Number of file descriptors in arrays
static int fd_array_size;		//< Allocated size of file descriptor array, fds
static int run_fc;				//< Main loop is running true/false


static void debug_fd (const char *msg, int i, struct pollfd *pfd);

/*
 * Initialization for fcf data structures
 */
int init_fcf(){
  fd_array_size = FDS_INIT_SIZE;
  fds = (struct pollfd *) malloc(fd_array_size * sizeof(struct pollfd));
  fdx = (struct fcffd *) malloc(fd_array_size * sizeof(struct fcffd));
  // TODO: Need error checking code here for malloc calls
	
  nfds = 0;
  run_fc = 0;

  return 0;
}


/*
 *	Increases size of the file desciptor and file description arrays
 */
static int expand_arrays(){
  struct pollfd * fds_temp;
  struct fcffd * fdx_temp;

  printf("** Expanding arrays from %d to ", fd_array_size);
  fd_array_size *= FDS_EXPANSION_FACTOR; // Expand array by pre-defined factor
  printf("%d\n", fd_array_size);
	

  fds_temp = realloc(fds, fd_array_size * sizeof(struct pollfd));
  if(fds_temp == NULL){
    // TODO: Add necessary logging/abort codes
    return -1;
  }
  fds = fds_temp;
	
  fdx_temp = realloc(fdx, fd_array_size * sizeof(struct fcffd));
  if(fdx_temp == NULL){
    // TODO: Add necessary logging/abort codes
    return -1;
  }
  fdx = fdx_temp;

  return 0;
}

int fcf_add_fd(int fd, short events, pollfd_callback cb){
	// Checks to see if fd arrays are full
	if(fd_array_size == nfds){
		expand_arrays();
	}

	fds[nfds].fd = fd;
	fds[nfds].events = events;
	//fdx[nfds].token = token;
	fdx[nfds].callback = cb;
	fdx[nfds].cb_cat = STANDARD;
	nfds++;
	printf("Added fd: %d\tevents: %d\tFD count: %d\n", fd, events, nfds);
	//printf("Added %s\tfd: %d\tevents: %d\tFD count: %d\n", token, fd, events, nfds);
	return nfds-1;
}


//add per poll cycle file descriptor
int fcf_add_fd_ppc (/*const char *token,*/ int fd, short events, pollfd_callback cb)
{
	int i = fcf_add_fd (/*token,*/ fd, events, cb);
	if (i >= 0) {
		fdx[i].cb_cat = PPC;
	}
	return i;
}


void fcf_remove_fd(int fd) {
 
	// If there are no fds, return 0 -- or error code.
	if(nfds <= 0)
		return;

	int i = 0;
	
	for(i=0; i<nfds; i++){
		if(fds[i].fd == fd && i==(nfds-1)){
			//printf("Removed %s\tfd: %d\tevents: %d\tFD count: %d\n", fdx[i].token, fds[i].fd, fds[i].events, nfds-1);
			nfds--;
		}
		else if(fds[i].fd == fd){
			//printf("Removed %s\tfd: %d\tevents: %d\tFD count: %d\n", fdx[i].token, fds[i].fd, fds[i].events, nfds-1);
			memmove(&fds[i], &fds[nfds - 1], sizeof(struct pollfd));
			memmove(&fdx[i], &fdx[nfds - 1], sizeof(struct fcffd));
			nfds--;
		}
	}
}

struct pollfd *fcf_get_fd(int idx){
  return &fds[idx];
}

void fcf_stop_main_loop(){
  run_fc = 0;
}

static void fcf_start_main_loop(){
  run_fcc = 1;
}

//currently returns -1 on error; 0 on success
static int fcf_run_poll_loop() {
	pollfd_callback ppc[nfds];
	int nppc= 0;
	//int count = 0;
	int ret = 0;

	fcf_start_main_loop();
	while (run_fc) {

		/*for (int i = 0; i < nfds; i++) {
			debug_fd("\nbefore poll<<< ", i, &fds[i]);
		}*/
		//printf("\nwaiting");
		fflush (stdout);

		errno = 0;
		int rc = poll(fds, nfds, -1);

		//printf ("\n%d. poll returned with rc=%d errno=%d", count++, rc, errno);
		/*for (int i = 0; i < nfds; i++) {
			debug_fd("\n   after poll>>> ", i, &fds[i]);
		}*/

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
					if (fdx[i].cb_cat == STANDARD) {
						//callback for this active fd is a standard callback
						fdx[i].callback(&fds[i]);
					} 
					else {
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
				ppc[j](fds);
			}

			break;
		}

	}

	printf("\n exiting main loop");
	return ret;
}

static void debug_fd (const char *msg, int i, struct pollfd *pfd) {
	//printf("%s fd[%d]: fd=%d events=%X revents=%X [%s]", msg, i, pfd->fd, pfd->events, pfd->revents, fdx[i].token);
	int re = pfd->revents;
	if (re & POLLERR) printf("\nPOLLERR - Error condition");
	if (re & POLLHUP) printf("\nPOLLHUP - Hang up");
	if (re & POLLNVAL) printf("\nPOLLNVAL - Invalid request: fd not open");
	fflush (stdout);
}


static void signalhandler(int signum){
  printf ("\n **signal handler: signum = %d", signum);

  if (signum == SIGINT){
    fcf_stop_main_loop ();
  }
}


int main(int argc, char *argv[]){
  
  printf("Flight Control Framework v0.1  Copyright (C) 2013\n"
	 "Team Elderberry [Ron Astin, Clark Wachsmuth, Chris Glasser, Josef Mihalits, Jordan Hewitt, Michael Hooper]\n"
	 "This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.\n"
	 "This is free software, and you are welcome to redistribute it\n"
	 "under certain conditions; type `show c' for details.\n");
  signal (SIGINT, signalhandler);

	fcf_init();
	int rc = fcf_run_poll_loop();

	if (rc == 0) {
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}
