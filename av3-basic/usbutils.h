/*
 * libusb-simple.h (based on libusb-gsource.h)
 *
 * Functions for integrating libusb. Also some
 * useful libusb utils.
 */

#ifndef LIBUSB_GSOURCE_H_
#define LIBUSB_GSOURCE_H_
#include <libusb-1.0/libusb.h>
//#include <libusb.h>
#include <poll.h>

//typedef void (*libusbSourceErrorCallback)(int, int, gpointer);
typedef int (*is_device)(libusb_device * device);

/* Opaque type for a GSource that embeds libusb in a Glib main-loop. */
typedef struct libusbSource libusbSource;

//void run_main_loop(libusbSource * usb_source);

libusbSource * libusbSource_new(void);
libusb_device_handle * open_usb_device_handle(libusbSource * usb_source,
    is_device is_device, int * iface_num, int num_ifaces);
void print_libusb_error(int libusberrno, const char* str);
void print_libusb_transfer_error(int status, const char* str);

typedef int (*pollCallback)(struct pollfd *pfd);
//int fcf_addfd (int fd, short events, pollCallback cb, libusbSource *src);
//int fcf_removefd (int fd, libusbSource *src);
void stop_main_loop(void);



#endif /* LIBUSB_GSOURCE_H_ */
