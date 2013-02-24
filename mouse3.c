
#include <sys/time.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "mouse3.h"
#include "utils_libusb-1.0.h"

/**	START DATA */

static const int VID = 0x1532;
static const int PID = 0x000a;

extern void fcf_callback_mouse3(unsigned char *, int);
extern int init_device(char *, int, int, const int, libusb_transfer_cb_fn);

/**	START FUNCTIONS */


static void data_callback(struct libusb_transfer *transfer){
	printf("***********************	TOTALLY CALLING HERE!!!! *******************************\n");	
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

		// Call to CGS mouse handler.
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
	init_device("mouse3", VID, PID, 0x81, data_callback);
	return 0;
}



