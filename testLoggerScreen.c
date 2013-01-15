#include <libusb.h>

/***
*  Assumptions about LibUSB:
*
*	So this is a first attempt at hacking libusb code, I'm less concerned
*	with it working than in trying to get a correct process flow for
*	how most USB communication is done.
*
*	int libusb_init ( libusb_context** ctx )
*		Called to initialize a libusb context. Context allow for multiple simultaneous uses of libusb for
*		different devices. The context can also be used in the below functions. 
*
*	libusb_device_handle* libusb_open_device_with_vid_pid (libusb_context *ctx, uint16_t vendor_id, uint16_t product_id)
*		PSAS does not use this function but it seems a good choice for finding a device that you know about.
*		For Theo-IMU we want 0xFFFF vendor_id and 0x0005 product_id.
*/

static void init_screenLogger () {
  // maybe nothing to do?
}

static void screenLogger_getMessage(somedata *buffer) {
  // somedata has been passed into this function for consumption.
} 


// Other private functions to do stuff.
