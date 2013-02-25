#include <sys/time.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>

#include "fcfutils.h"

libusb_context *context;

static struct timeval nonblocking = {
		.tv_sec = 0,
		.tv_usec = 0,
};


//active fd
void libusb_cb(int idx) {
	libusb_handle_events_timeout(context, &nonblocking);
}


static void usb_fd_added_cb(int fd, short events, void * source){
	
	// NOTE: possible solution to token problem
	/*static int offset = 1;
	if(source==NULL){
		char * dev_name;
		sprintf(dev_name, "dev_%d", offset);
		source = dev_name;
		offset++;
	}*/

	fcf_add_fd(source, fd, events, libusb_cb);
}


static void usb_fd_removed_cb(int fd, void* source){
	fcf_remove_fd (fd);
}


int start_usb_transfer(libusb_device_handle * handle, unsigned int ep, libusb_transfer_cb_fn cb, void * data, int packet_size, unsigned int timeout){
	
	unsigned char * buf;
	int usb_err;
	struct libusb_transfer * transfer;
	
	transfer = libusb_alloc_transfer(0);
        
	if(transfer == NULL){
		libusb_free_transfer(transfer);
		return -1;
	}
        
	buf  = calloc(packet_size, sizeof(unsigned char));

	if(buf == NULL){
		libusb_free_transfer(transfer);
		return -1;
	}

	libusb_fill_interrupt_transfer(transfer, handle, ep, buf, packet_size, cb, data, timeout);

	transfer->flags = LIBUSB_TRANSFER_FREE_BUFFER;
	usb_err = libusb_submit_transfer(transfer);
	
	if(usb_err != 0){
		libusb_cancel_transfer(transfer);
		libusb_free_transfer(transfer);
		printf("USB ERROR\n");
		return usb_err;
	}

    return 0;
}


libusb_device_handle * get_handle(char *dev_name, libusb_device ** devs, libusb_device * device){
	int iface_num[1] = {0}, rc;
	libusb_device_handle *handle;

	rc = libusb_open(device, &handle);
	if(rc!=0){
		printf("[%s] libusb: Error code on open: %s.\n", dev_name, libusb_error_name(rc)); 
		return NULL;
	}

	// Although handle shouldn't be NULL at this point, this will release
	// libusb device list and exit.
	if (handle==NULL) {
		printf("[%s] libusb: No handle found.\n", dev_name);
        libusb_free_device_list(devs, 1);
        return NULL;
    }

	rc = libusb_kernel_driver_active(handle, iface_num[0]);
	if(rc < 0){
		printf("[%s] libusb: Failure finding kernel driver status.\n", dev_name);
		libusb_close(handle);
		return NULL;
	}
	if(rc > 0){ 
		rc = libusb_detach_kernel_driver(handle, iface_num[0]);
		if(rc){
			printf("[%s] libusb: Could not detach kernel driver.\n", dev_name);
			libusb_close(handle);
			return NULL;
		}
	}

	if(libusb_claim_interface(handle, 0) < 0){ 
		printf("[%s] libusb: Could not claim interface\n", dev_name); 
		libusb_close(handle); 
		return NULL; 
	} 

	return handle;
}


libusb_device * find_device(libusb_device ** devices, int cnt, int vid, int pid){
	
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
		if(desc.idVendor==vid){
			if(desc.idProduct==pid){
				return devices[i];
			}
		}

		// TODO: Remove this from final code...
		if(desc.idVendor==0x046D){
			if(desc.idProduct==0xC03E){
				//ID 046d:c03e Logitech, Inc. Premium Optical Wheel Mouse (M-BT58)
				libusb_set_debug(context, 3);
				return devices[i];
			}
		}
	}
	return NULL;
}


int init_device(char * dev_name, int vid, int pid, const int endpoint, libusb_transfer_cb_fn cb){
	int rc, cnt, packet_size;
	libusb_device_handle *handle;
	libusb_device **devs, *device;
	const struct libusb_pollfd ** fds;

	if(context==NULL){
		rc = libusb_init(&context);
		if(rc){
			printf("[%s] libusb: init failed.\n", dev_name);
			return -1;
		}
	}

	// Get a list of all the devices. Return on error.
 	cnt = libusb_get_device_list(context, &devs);
   	if(cnt < 0) {
        printf("[%s] libusb: Could not get device list.\n", dev_name);
		return -1;
    }

	// Find device in list of USB device. Return on error.
	device = find_device(devs, cnt, vid, pid);
	if(!device){
		printf("[%s] libusb: No device with matching vid/pid found.\n", dev_name);
		return -1;
	}

	handle = get_handle(dev_name, devs, device);
	if(handle == NULL){
		printf("[%s] Error: Invalid handle returned by get_handle().\n", dev_name);
		return -1;
	}

	fds = libusb_get_pollfds(context);

	for(cnt=0; fds[cnt] != NULL; cnt++){
		fcf_add_fd(dev_name, fds[cnt]->fd, fds[cnt]->events, libusb_cb);
	}

	free(fds);

	packet_size = libusb_get_max_packet_size (libusb_get_device(handle), endpoint);
	start_usb_transfer(handle, endpoint, cb, NULL, packet_size, 0);

	libusb_set_pollfd_notifiers(context, usb_fd_added_cb, usb_fd_removed_cb, NULL);


	return 0;
}
