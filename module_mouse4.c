
#include <sys/time.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "fcfutils.h"


/**	START DATA */

#define VID 0x045e
#define PID 0x0053

static const int g_dev_IN_EP = 0x81;
static libusb_context *context;
static struct libusb_transfer * transfer[1];
static int packet_size;

extern void fcf_callback_mouse4(unsigned char *, int);

static struct timeval nonblocking = {
		.tv_sec = 0,
		.tv_usec = 0
};


/**	START FUNCTIONS */

void mouse4_handler(int idx){
	//printf("Callback function for fd %d\n", idx);
	libusb_handle_events_timeout(context, &nonblocking);
}


 static int start_usb_transfer(libusb_device_handle * hand, unsigned int ep, libusb_transfer_cb_fn cb, void * data, unsigned int timeout){
	
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

	libusb_fill_interrupt_transfer(transfer[0], hand, ep, buf, packet_size, cb, data, timeout);

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


static void data_callback4(){
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
		fcf_callback_mouse4(buf, act_len);

        break;
    case LIBUSB_TRANSFER_CANCELLED:
		printf("transfer cancelled\n");
        //do nothing.
        break;
    default:
        //print_libusb_transfer_error(transfer->status, "common_cb");
		printf("data_callback4() error\n");
        break;
    }
}


static libusb_device * find_device(libusb_device ** devices, int cnt){
	
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
		if(desc.idVendor==0x045E){
			if(desc.idProduct==0x00E1){
				//ID 045e:00e1 Microsoft Corp. Wireless Laser Mouse 6000 Reciever
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


int init_mouse4(){
	
	int error, cnt;
	libusb_device **devs, *device2; // Can be local.
	libusb_device_handle *handle2;
	const struct libusb_pollfd ** fds2;
	
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
	device2 = find_device(devs, cnt);
	if(!device2){
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
	error = libusb_open(device2, &handle2);
	if(error!=0){
		printf("Error code on open: %s.\n", libusb_error_name(error)); 
		return -1;
	}

	// Although handle shouldn't be NULL at this point, this will release
	// libusb device list and exit.
	if (handle2==NULL) {
		printf("No handle found.\n");
        libusb_free_device_list(devs, 1);
        libusb_exit(NULL);
        return -1;
    }

	// In case mouse is being used by OS, this fucntion will detach
	// the kernal driver so we can claim interface. I commented out
	// the error code since it will error when already having been 
	// previsouly detached on earlier run of program.
	error = libusb_detach_kernel_driver(handle2, 0);
	if(error!=0){
		printf("Error code on kernel detach: %s.\n", libusb_error_name(error)); 
		//return -1;
	}

	// Tries setting the configuration
	error = libusb_set_configuration(handle2, 1);
	if(error!=0){
		printf("Error code on set config: %s.\n", libusb_error_name(error)); 
		return -1;
	}
  
	if(libusb_claim_interface(handle2, 0) < 0){ 
		printf("Could not claim interface\n"); 
		libusb_close(handle2); 
		return 1; 
	} 
  
	
  
	fds2 = libusb_get_pollfds(context);
	
	int num = 0;
	for(num=0; fds2[num] != NULL; num++){
		fcf_add_fd("mouse4", fds2[num]->fd, fds2[num]->events, mouse4_handler);
	}

	packet_size = libusb_get_max_packet_size (libusb_get_device (handle2), g_dev_IN_EP);
	start_usb_transfer(handle2, g_dev_IN_EP, data_callback4, NULL, 0);

	return 0;
}



