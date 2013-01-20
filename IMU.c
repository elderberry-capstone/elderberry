//#include <libusb.h>

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

static void init_theo_imu () {
/*
	// create libusb_context
	libusb_context *context = null;
	libusb_init(&libusb_context);
	libusb_device_handle *libusb_dh = libusb_open_device_with_vid_pid(context, 0xFFFF, 0x0005);

	// Maybe use init as a reset function too....
	fcf_remove_all_fd('demoimu');
	// At some point we have file descriptors from libusb. Pass these to FCF
	fcf_add_usb_fd('demoimu', fd1, 'fileA_handler');
	fcf_add_usb_fd('demoimu', fd2, 'fileB_handler');
	// there can be more of these.
*/
}

static void fileA_handler(int fd, char **buffer) {
/***
*  In this function we point buffer at a message from device.
*  This function handles a particular message type (fileA).
*/
  // allocate memory for message, perhaps one static bufffer for life or app?
  // change the pointer that points to buffer to point to this data.
  // This function hides all the ugly work of getting "The Message" off of the
  // hardware device. Every hardware interfacting module that sends messages will
  // have specific callback functions for each message type to send.

} 


static void fileB_handler(int fd, char **buffer) {
/***
*  In this function we point buffer at a message from device.
*  This function handles a particular message type (fileB).
*/
  // allocate memory for message, perhaps one static bufffer for life or app?
  // change the pointer that points to buffer to point to this data.
  // This function hides all the ugly work of getting "The Message" off of the
  // hardware device. Every hardware interfacting module that sends messages will
  // have specific callback functions for each message type to send.

} 

// Other private functions to do stuff.
