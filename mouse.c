
#include <sys/time.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "mouse.h"
#include "fcfutils.h"


/**	START DATA */

#define VID 0x045e
#define PID 0x0053

static const int g_dev_IN_EP = 0x81;

libusb_context *context;
struct libusb_transfer * transfer[1];
static int packet_size;

extern void fcf_callback_mouse(unsigned char *, int);

static struct timeval nonblocking = {
		.tv_sec = 0,
		.tv_usec = 0
};


/**	START FUNCTIONS */

void mouse_handler(int idx){
	//printf("Callback function for fd %d\n", idx);
	libusb_handle_events_timeout(context, &nonblocking);
}


int start_usb_transfer(libusb_device_handle * handle, unsigned int ep, libusb_transfer_cb_fn cb, void * data, unsigned int timeout){
	
	unsigned char * buf;
	int usb_err;
	transfer[0] = libusb_alloc_transfer(0);
        
	if(transfer[0] == NULL){
		libusb_free_transfer(transfer[0]);
		return -1;
	}
        
	buf  = calloc(packet_size, sizeof(unsigned char));

	if(buf == NULL){
		libusb_free_transfer(transfer[0]);
		return -1;
	}

	libusb_fill_interrupt_transfer(transfer[0], handle, ep, buf, packet_size, cb, data, timeout);

	transfer[0]->flags = LIBUSB_TRANSFER_FREE_BUFFER;
	usb_err = libusb_submit_transfer(transfer[0]);
	
	if(usb_err != 0){
		libusb_cancel_transfer(transfer[0]);
		//todo: handle error besides LIBUSB_ERROR_NOT_FOUND for cancel
		libusb_free_transfer(transfer[0]);
		printf("USB ERROR\n");
		return usb_err;
	}

    return 0;
}


void data_callback(){
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


libusb_device * find_device(libusb_device ** devices, int cnt){
	
	struct libusb_device_descriptor desc;
	int i, error;

	// Cycle through list of USB devices and see if one matches
	// the product and vendor IDs of the mouse. 
	for(i=0; i<cnt; i++){
		error = libusb_get_device_descriptor(devices[i], &desc);
		if(error < 0){
			printf("Could not get device descriptor.\n");
			continue; // If no descriptor, go to next device.
		}

		// Conditional for finding specific device.
		if(desc.idVendor==VID){
			if(desc.idProduct==PID){
				return devices[i];
			}
		}
		if(desc.idVendor==0x046D){
			if(desc.idProduct==0xC03E){
				//ID 046d:c03e Logitech, Inc. Premium Optical Wheel Mouse (M-BT58)
				libusb_set_debug(context, 3);
				return devices[i];
			}
		}
        if(desc.idVendor==0x062A){
            if(desc.idProduct==0x0252){
                //ID 046d:c03e Logitech, Inc. Premium Optical Wheel Mouse (M-BT58)
                libusb_set_debug(context, 3);
                return devices[i];
            }
        }
	}
	return NULL;
}

int init_mouse(){
	
	int error, cnt;
	libusb_device_handle *handle;
	libusb_device **devs, *device;
	const struct libusb_pollfd ** fds;
	
	error = libusb_init(&context);
	if(error){
		printf("libusb init failed.\n");
		return -1;
	}

	// Get a list of all the devices. Return on error.
 	cnt = libusb_get_device_list(context, &devs);
   	if(cnt < 0) {
        printf("Could not get device list.\n");
		return -1;
    }

	// Find device in list of USB device. Return on error.
	device = find_device(devs, cnt);
	if(!device){
		printf("No device with matching vid/pid found.\n");
		return -1;
	}

	// Open device.
	/*	I had trouble here for a long while, but didn't correctly
		pass the handle variable. I originally defined the pointer
		as a libusb_handle ** without passing handle with the address
		operator. When I changed it ot this, it worked, or at least
		stopped the segfaults.
	*/
	error = libusb_open(device, &handle);
	if(error!=0){
		printf("Error code on open: %s.\n", libusb_error_name(error)); 
		return -1;
	}

	// Although handle shouldn't be NULL at this point, this will release
	// libusb device list and exit.
	if (handle==NULL) {
		printf("No handle found.\n");
        libusb_free_device_list(devs, 1);
        libusb_exit(NULL);
        return -1;
    }

	// In case mouse is being used by OS, this fucntion will detach
	// the kernal driver so we can claim interface. I commented out
	// the error code since it will error when already having been 
	// previsouly detached on earlier run of program.
	error = libusb_detach_kernel_driver(handle, 0);
	if(error!=0){
		//printf("Error code on open: %s.\n", libusb_error_name(error)); 
		//return -1;
	}

	// Tries setting the configuration
	error = libusb_set_configuration(handle, 1);
	if(error!=0){
		printf("Error code on open: %s.\n", libusb_error_name(error)); 
		return -1;
	}
  
	if(libusb_claim_interface(handle, 0) < 0){ 
		printf("Could not claim interface\n"); 
		libusb_close(handle); 
		return 1; 
	} 
  
	
  
	fds = libusb_get_pollfds(context);
	
	int num = 0;
	for(num=0; fds[num] != NULL; num++){
		fcf_add_fd("mouse", fds[num]->fd, fds[num]->events, mouse_handler);
	}

	packet_size = libusb_get_max_packet_size (libusb_get_device (handle), g_dev_IN_EP);
	start_usb_transfer(handle, g_dev_IN_EP, data_callback, NULL, 0);

	return 0;
}



