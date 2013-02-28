
#include <sys/time.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "utils_libusb-1.0.h"

/**	START DATA */

static const int VID = 0x046d;
static const int PID = 0xc03e;
static const int EPT = 0x81;

extern void fcf_callback_mouse3(unsigned char *, int);
extern int init_device(char *, int, int, const int, libusb_transfer_cb_fn);


/**	START FUNCTIONS */

static void data_callback(struct libusb_transfer *transfer){
	unsigned char *buf = NULL;
    int act_len;
    int retErr;

	switch(transfer->status){
    case LIBUSB_TRANSFER_COMPLETED:

        buf = transfer->buffer;
        act_len = transfer->actual_length;
        retErr = libusb_submit_transfer(transfer);
		if(retErr){
            //print_libusb_transfer_error(transfer->status, "common_cb resub");
        }

		/**
		*	Data handler:
		*	Place call into code generated space here.
		*/
		fcf_callback_mouse3(buf, act_len);

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


int init_mouse3(){
	init_device("mouse3", VID, PID, EPT, data_callback);
	return 0;
}



