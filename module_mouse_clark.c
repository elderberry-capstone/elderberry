
#include <sys/time.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "utils_libusb-1.0.h"

extern void fcf_callback_mouse_clark(unsigned char *, int);

/**	START DATA */
// Microsoft Corp. Optical Mouse
static const int VID = 0x062a;
static const int PID = 0x0252;
static const int EPT = 0x81;

static void data_callback(struct libusb_transfer *transfer){
	unsigned char *buf = NULL;
    int act_len;
    int retErr;

	switch(transfer->status){
    case LIBUSB_TRANSFER_COMPLETED:

        buf = transfer->buffer;
        act_len = transfer->actual_length;

        retErr = libusb_submit_transfer(transfer);
		//printf("Data from mouse: %02x \n", (char)buf[0]);

		if(retErr){
            //print_libusb_transfer_error(transfer->status, "common_cb resub");
        }

		// Call to CGS mouse handler.
		fcf_callback_mouse_clark(buf, act_len);

        break;
    case LIBUSB_TRANSFER_CANCELLED:
		printf("transfer cancelled\n");
        //do nothing.
        break;
    default:
        //print_libusb_transfer_error(transfer->status, "common_cb");
		printf("data_callback() error\n");
        break;
    }
}



int init_mouse_clark(){
	init_device("mouse_clark", VID, PID, EPT, data_callback);
	return 0;
}



