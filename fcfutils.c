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

extern void fcf_add_usb_fd(char *token, int fd, char *callback) {
  // Add file descriptor to array for USB device.
}

extern void fcf_add_eth_fd(char *token, int fd, char *callback) {
  // Add file descriptor to array for Ethernet device.
}

extern void fcf_add_can_fd(char *token, int fd, char *callback) {
  // Add file descriptor to array for CAN device.
}

extern void fcf_remove_all_fd(char*) {
  // Remove all file descriptors that were added under given source token.
}


// We also need some functions so that our framework can poll this data.
// I'm assuming we should just put it in this library. Or we could make this
// library only a wrapper for where the real structure lives. 

extern void fcf_get_fd_structure(somedatatype *buffer) {
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
