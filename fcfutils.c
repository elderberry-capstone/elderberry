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
#include "fcfutils.h"

/***
*  So I've been told that when file descriptors are removed we remove
*  all of them for a device and re-add everything. The tokens can be
*  used to that only 1 call to our API is needed to remove all file
*  descriptors for any device. It is assumed the device code would then
*  re-add what new ones it discovers.
*
*  From Josef's early research it looks like CAN and Ethernet can also
*  be interfaced by using file descriptors. I've added stubs for those.
*
*  Adding FileDescriptors:
*
*	Tokens - User supplied modules define their own token when
*	supplying file descriptors to the API. The point of tokens
*	is to allow easy removal of all old file descriptors in the
*	event of a reset. See fcf_remove_all_fd.
*
*	FD - The numeric file descriptor being added to the system.
*
*	Callback - The name of the handling function in the user
*	supplied module. Callbacks match the name configured in the
*	MIML file.
*/

struct fcffd {
	const char *token;
	char *callback;
};

static const int MAXFD = 100;
static struct pollfd fds[MAXFD];
static struct fcffd fds2[MAXFD];
static int nfds = 0;

// Add file descriptor to array of FDs.

/**
 *  @brief Add file descriptor to array of existing file descriptors
 *  @param token Source token for file descriptor (will be used for removal?)
 *  @param fd integer number for file descriptor
 *  @param callbackname Character pointer for name used to refer to file descriptor.
 */
extern void fcf_add_fd(const char *token, int fd, char *callbackname) {
	fds[nfds].fd = fd;
	fds[nfds].events = POLLIN | POLLPRI;
	fds2[nfds].token = token;
	fds2[nfds].callback = NULL;
	nfds++;
}



//extern void fcf_add_usb_fd(char *token, int fd, char *callback) {
//  // Add file descriptor to array for USB device.
//}
//
//extern void fcf_add_eth_fd(char *token, int fd, char *callback) {
//  // Add file descriptor to array for Ethernet device.
//}
//
//extern void fcf_add_can_fd(char *token, int fd, char *callback) {
//  // Add file descriptor to array for CAN device.
//}
//

/**
 *  @brief removes all file descriptors added under the given source token
 *  @details will return 0 or error code if there are no file descriptors.
 *  @param fd_source Source token to be removed from array of file descriptors.
 */
int fcf_remove_all_fd(const char *fd_src) {
  // Remove all file descriptors that were added under given source token.

	// If there are no fds, return 0 -- or error code.
	if(nfds <= 0)
		return 0;

	int i = 0, removed = 0;
	
	for(i=0; i<nfds; i++){
		// NOTE: if non-null-terminated strings, use strncmp with length
		if(strcmp(fds2[i].token, fd_src) == 0){

			// If matching fd is last in array.
			if(nfds - 1 == i){
				nfds--;
				removed++;
				return removed;
			}
			
			// Replace fd at index i with last fd in array.
			memmove(&fds[i], &fds[nfds - 1], sizeof(struct pollfd));

			// Replace fcffd at index i with last fcffd in array.
			memmove(&fds2[i], &fds2[nfds - 1], sizeof(struct fcffd));
			
			// Decrement number of fds and increment amount removed.
			nfds--;
			removed++;
		}
	}

	return removed;
}


// We also need some functions so that our framework can poll this data.
// I'm assuming we should just put it in this library. Or we could make this
// library only a wrapper for where the real structure lives. 

void fcf_get_fd_structure(struct pollfd **_fds, int *_nfds) {
	*_fds = fds;
	*_nfds = nfds;
}

// What does struct_fc look like?
//
// 	#include <libusb.h>
//
//	struct libusb_pollfd {
//	  int fd,
//	  short events,
//	};
// 
// We need to add 2 more pieces of information to this structure
//
//	1) The token used by the user supplied module for adding all FDs.
//
//	2) A callback function to use for each FD when that FD is responsive.
//
// We also need to manage growth/shrinkage of our structure/array.
//
//	How much space do we allocate initially?
//		0/1 seems shortsighted.
//	Should the token be a number? Easier to compare with.
//	The point of token is to allow a single call to remove all previous FDs.
//	Would it be easier to remove token and force users to remove each FD individually?
//



