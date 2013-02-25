
#include <sys/time.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "utils_libusb-1.0.h"

/**	START DATA */

static const int VID = 0x045e;
static const int PID = 0x0053;
static const int EPT = 0x81;

static void data_callback(struct libusb_transfer *transfer){
	unsigned char *buf = NULL;
    int act_len;
    int retErr;

	switch(transfer[0]->status){
    case LIBUSB_TRANSFER_COMPLETED:

        buf = transfer[0]->buffer;
        act_len = transfer[0]->actual_length;

        retErr = libusb_submit_transfer(transfer[0]);
		//printf("Data from mouse: %02x \n", (char)buf[0]);

		if(retErr){
            //print_libusb_transfer_error(transfer->status, "common_cb resub");
        }

		// Call to CGS mouse handler.
		fcf_callback_mouse(buf, act_len);

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



int init_mouse(){
	init_device("mouse", VID, PID, EPT, data_callback);
	return 0;
}



