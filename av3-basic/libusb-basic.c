/*
 * libusb-basic.c (based on libusb-gsource.c)
 *
 * Functions for integrating libusb with a glib main event loop. Also some
 * useful libusb utils.
 */

#include <time.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>

#include "libusb-basic.h"
#include "logging.h"

#define FCF_POLLSIZE 100


//TODO: separate FCF structs from USB types so that FCF is hw independent
struct libusbSource {
//	GSource source;
//	GSList * devices;
//	GSList * fds;
	struct pollfd pollfds[FCF_POLLSIZE];	//FCF
	int nfds;								//FCF
	pollCallback callbacks[FCF_POLLSIZE];	//FCF
	int timeout_error;
	int handle_events_error;
	libusb_context * context;
};


// add file descriptor
int fcf_addfd (int fd, short events, pollCallback cb, libusbSource *src) {
	int *p = &(src->nfds);

	if (*p >=  FCF_POLLSIZE) {
		return -1;
	}

	src->pollfds[*p].fd = fd;
	src->pollfds[*p].events = events;
	src->callbacks[*p] = cb;
	(*p)++;
	return 0;
}

//remove file descriptor
int fcf_removefd (int fd, libusbSource *src) {
	int *p = &(src->nfds);

	for (int i = 0; i < *p; i++) {
		if (src->pollfds[i].fd == fd) {
			//we found fd we want to remove
			//remove fd at index i by overwriting it with last fd
			if (*p > 1) {
				src->pollfds[i] = src->pollfds[*p - 1];
				src->callbacks[i] = src->callbacks[*p - 1];
				(*p)--;
				return 0;
			}
		}
	}

	return -1; //fd was not in array
}

static struct timeval nonblocking = {
		.tv_sec = 0,
		.tv_usec = 0,
};

static libusbSource *g_usb_source = NULL;

//active fd
static int libusb_cb(struct pollfd *pfd) {
//	printf_tagged_message(FOURCC('L', 'O', 'G', 'S'), "\n libusb_cb");
//	flush_buffers();

	libusb_handle_events_timeout(g_usb_source->context, &nonblocking);
	return 0;
}

static void usb_fd_added_cb(int fd, short events, void * source){
	printf_tagged_message(FOURCC('L', 'O', 'G', 'S'), "\n usb_fd_added_cb");
	flush_buffers();

	fcf_addfd (fd, events, libusb_cb, (libusbSource *)source);
}

static void usb_fd_removed_cb(int fd, void* source){
	printf_tagged_message(FOURCC('L', 'O', 'G', 'S'), "\n usb_fd_removed_cb");
	flush_buffers();

	fcf_removefd (fd, (libusbSource *)source);
}

static int init_usb_fds(libusbSource * usb_source){
	int numfds = 0;
	const struct libusb_pollfd ** usb_fds = libusb_get_pollfds(usb_source->context);
	if(!usb_fds)
		return -1;

	for(numfds = 0; usb_fds[numfds] != NULL; ++numfds){
		usb_fd_added_cb(usb_fds[numfds]->fd, usb_fds[numfds]->events, usb_source);
	}

	free(usb_fds);
	libusb_set_pollfd_notifiers(usb_source->context, usb_fd_added_cb, usb_fd_removed_cb, usb_source);
	return 0;
}


libusbSource * libusbSource_new(void){
	libusb_context * context;
	int usbErr = libusb_init(&context);
	if(usbErr){
		print_libusb_error(usbErr, "libusb_init");
		return NULL;
	}
	libusb_set_debug(context, 3);

	if(!libusb_pollfds_handle_timeouts(context)){
		printf ("TODO");
	}

	libusbSource * usb_source = malloc (sizeof(libusbSource));

//	usb_source->devices = NULL; //important to set null because g_source_destroy calls finalize
//	usb_source->fds = NULL; //important to set null because g_source_destroy calls finalize
	usb_source->nfds = 0;
	usb_source->context = context;
	usb_source->timeout_error = 0;
	usb_source->handle_events_error = 0;

	if(init_usb_fds(usb_source)){
		free (usb_source);
		return NULL;
	}

	return usb_source;
}


static libusb_device * find_usb_device(libusbSource * usb_source, is_device is_device){
	libusb_device **list = NULL;
	libusb_device *found = NULL;
	ssize_t num_usb_dev = 0;
	ssize_t i = 0;

	num_usb_dev = libusb_get_device_list(usb_source->context, &list);
	if(num_usb_dev < 0){
		print_libusb_error(num_usb_dev, "Could not get device list");
		return NULL;
	}
	//look through the list for the device matching is_device
	for(i = 0; i < num_usb_dev; ++i){
		if(is_device(list[i])){
			found = list[i];
			libusb_ref_device(found);
			break;
		}
	}
	if(!found){
		fprintf(stderr, "Device not found\n");
	}

	libusb_free_device_list(list, 1);
	return found;
}


static libusb_device_handle * open_device_interface(libusbSource * usb_source, libusb_device * dev, int * iface_num, int num_ifaces){
	libusb_device_handle *handle = NULL;
	int i = 0;
	int retErr = 0;
	int kd_stat = 0;
	if(!dev)
		return NULL;
	retErr = libusb_open(dev, &handle);
	if(retErr){
		print_libusb_error(retErr, "Could not open device");
		return NULL;
	}
	//claim requested interfaces on the device
	for(i=0; i < num_ifaces; ++i){
		//if the kernel driver is active on the interfaces we want, detach it
		kd_stat = libusb_kernel_driver_active(handle, iface_num[i]);
		if(kd_stat < 0){
			print_libusb_error(kd_stat,"Failure finding kernel driver status");
			libusb_close(handle);
			return NULL;
		}
		if(kd_stat > 0){ //the kernel driver is active (kd_stat = 1)
			retErr = libusb_detach_kernel_driver(handle, iface_num[i]);
			if(retErr){
				print_libusb_error(retErr, "Could not detach kernel driver");
				libusb_close(handle);
				return NULL;
			}
		}

		retErr = libusb_claim_interface(handle, iface_num[i]);
		if(retErr){
			print_libusb_error(retErr, "Could not claim device interface");
			libusb_attach_kernel_driver(handle, iface_num[i]);
			libusb_close(handle);
			return NULL;
		}
	}

	//TODO usb_source->devices = g_slist_prepend(usb_source->devices, handle);
	return handle;
}

libusb_device_handle * open_usb_device_handle(libusbSource * usb_source,
	is_device is_device, int * iface_num, int num_ifaces){
	libusb_device * dev = find_usb_device(usb_source, is_device);
	return open_device_interface(usb_source, dev, iface_num, num_ifaces);

}

void print_libusb_error(int libusberrno, const char* str) {
	switch(libusberrno) {
	case LIBUSB_SUCCESS:
		fprintf(stderr, "**%s: SUCCESS\n",str);
		break;
	case LIBUSB_ERROR_IO:
		fprintf(stderr, "**%s: ERROR_IO\n",str);
		break;
	case LIBUSB_ERROR_INVALID_PARAM:
		fprintf(stderr, "**%s: ERROR_INVALID_PARAM\n",str);
		break;
	case LIBUSB_ERROR_ACCESS:
		fprintf(stderr, "**%s: ERROR_ACCESS\n",str);
		break;
	case LIBUSB_ERROR_NO_DEVICE:
		fprintf(stderr, "**%s: ERROR_NO_DEVICE\n",str);
		break;
	case LIBUSB_ERROR_NOT_FOUND:
		fprintf(stderr, "**%s: ERROR_NOT_FOUND\n",str);
		break;
	case LIBUSB_ERROR_BUSY:
	   fprintf(stderr, "**%s: ERROR_BUSY\n",str);
	   break;
	case LIBUSB_ERROR_TIMEOUT:
		fprintf(stderr, "**%s: ERROR_TIMEOUT\n",str);
		break;
	case LIBUSB_ERROR_OVERFLOW:
		fprintf(stderr, "**%s: ERROR_OVERFLOW\n",str);
		break;
	case LIBUSB_ERROR_PIPE:
		fprintf(stderr, "**%s: ERROR_PIPE\n",str);
		break;
	case LIBUSB_ERROR_INTERRUPTED:
		fprintf(stderr, "**%s: ERROR_INTERRUPTED\n",str);
		break;
	case LIBUSB_ERROR_NO_MEM:
		fprintf(stderr, "**%s: ERROR_NO_MEM\n",str);
		break;
	case LIBUSB_ERROR_NOT_SUPPORTED:
		fprintf(stderr, "**%s: ERROR_NOT_SUPPORTED\n",str);
		break;
	case LIBUSB_ERROR_OTHER:
		fprintf(stderr, "**%s: ERROR_OTHER\n",str);
		break;
	default:
		fprintf(stderr, "***%s:  unknown error %i ***\n", str, libusberrno);
		break;
	}
/*  fprintf(stderr, "**%s: %s, %d\n", str, libusb_error_name(libusberrno),
 *	        libusberrno);
 *  libusb_error_name() only occurs in libusb1.0.9, 1.0.8 is common
 */
}

void print_libusb_transfer_error(int status, const char* str){
	switch(status){
	case LIBUSB_TRANSFER_COMPLETED:
		fprintf(stderr, "**%s: LIBUSB_TRANSFER_COMPLETED\n", str);
		break;
	case LIBUSB_TRANSFER_ERROR:
		fprintf(stderr, "**%s: LIBUSB_TRANSFER_ERROR\n", str);
		break;
	case LIBUSB_TRANSFER_TIMED_OUT:
		fprintf(stderr, "**%s: LIBUSB_TRANSFER_TIMED_OUT\n", str);
		break;
	case LIBUSB_TRANSFER_CANCELLED:
		fprintf(stderr, "**%s: LIBUSB_TRANSFER_CANCELLED\n", str);
		break;
	case LIBUSB_TRANSFER_STALL:
		fprintf(stderr, "**%s: LIBUSB_TRANSFER_STALL\n", str);
		break;
	case LIBUSB_TRANSFER_NO_DEVICE:
		fprintf(stderr, "**%s: LIBUSB_TRANSFER_NO_DEVICE\n", str);
		break;
	case LIBUSB_TRANSFER_OVERFLOW:
		fprintf(stderr, "**%s: LIBUSB_TRANSFER_OVERFLOW\n", str);
		break;
	default:
		fprintf(stderr, "***%s: Unknown transfer status %i***\n", str, status);
		break;
	}
}


void run_main_loop(libusbSource * usb_source) {

	g_usb_source = usb_source;	//TODO find clean solution so that callback has access to usbcontext
	int count = 0;
	for (;;) {
		struct pollfd *fds = usb_source->pollfds;
		int nfds = usb_source->nfds;

		for (int i = 0; i < nfds; i++) {
			printf("\npolling fd[%d]: fd=%d events=%d", i, fds[i].fd, fds[i].events);
		}
		fflush (stdout);

		int rc = poll(fds, nfds, 2000);
		printf ("\n%d. poll returned with rc=%d ", count++, rc);
		fflush (stdout);

//		printf_tagged_message(FOURCC('L', 'O', 'G', 'S'), "\n after poll");
//		flush_buffers();

		switch (rc) {
		case -1:
			printf("poll failed!");
			break;
		case 0: //timeout
			printf("poll timed out");
			//TODO fine tune timeouts
			libusb_handle_events_timeout(usb_source->context, &nonblocking);
			break;
		default:
			for (int i = 0; i < nfds; i++) {
				if(fds[i].revents != 0) {

					int re = fds[i].revents;
					if (re & POLLERR) printf("\nPOLLERR - Error condition");
					if (re & POLLHUP) printf("\nPOLLHUP - Hang up");
					if (re & POLLNVAL) printf("\nPOLLNVAL - Invalid request: fd not open");

					//active fd or error
					printf("\nfd[%d] is active, fd=%d events=%X revents=%X", i, fds[i].fd, fds[i].events, fds[i].revents);
					usb_source->callbacks[i](&fds[i]);
					//libusb_handle_events_timeout(usb_source->context, &nonblocking);
				}

			}
			printf_tagged_message(FOURCC('L', 'O', 'G', 'S'), "\n after poll");
			flush_buffers();
			break;
		}

	}

}


