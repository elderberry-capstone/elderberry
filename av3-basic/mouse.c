/*
 * mouse.c (based on theo-imu.c)
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "libusb-basic.h"
#include "mouse.h"
#include "logging.h"


static int g_mouse_packet_size = 4;
static int g_mouse_IN_EP = 0x81;


static int is_mouse(libusb_device * device){
    struct libusb_device_descriptor descr;
    int retErr = libusb_get_device_descriptor(device, &descr);
    if(retErr){
        print_libusb_error(retErr,"is_imu libusb_get_device_descriptor");
        return 0;
    }


    if(descr.idVendor == 0x046D && descr.idProduct == 0xC03E){
        return 1;
    }
    //add your IDs here
//    if(descr.idVendor == 0xFFFF && descr.idProduct == 0xFFFF){
//    	//hack: set g_mouse_packet_size and g_mouse_IN_EP if necessary (?)
//    	return 1;
//    }

    return 0;
}

static void common_cb(struct libusb_transfer *transfer, uint32_t fourcc){
    unsigned char *buf = NULL;
    int act_len;
    int retErr;

    switch(transfer->status){
    case LIBUSB_TRANSFER_COMPLETED:

//        printf_tagged_message(FOURCC('L', 'O', 'G', 'S'), "\nJM3 transfer completed");
//        flush_buffers();

        buf = transfer->buffer;

        act_len = transfer->actual_length;
        if(act_len != g_mouse_packet_size){
            write_tagged_message(fourcc, buf, act_len);
        }else{
//            if(IMU_ADDR(buf[0]) == ADDR_GYR){
//                write_tagged_message(fourcc, buf, act_len);
//            }else{
                write_tagged_message(fourcc, buf, act_len -1);
//            }
        }
        retErr = libusb_submit_transfer(transfer);
        if(retErr){
            print_libusb_transfer_error(transfer->status, "imu_cb resub");
        }
        break;
    case LIBUSB_TRANSFER_CANCELLED:
        //do nothing.
        break;
    default:
        print_libusb_transfer_error(transfer->status, "imu_cb");
        break;
    }
}

static void mag_cb(struct libusb_transfer *transfer){

//    printf_tagged_message(FOURCC('L', 'O', 'G', 'S'), "\nJM2 inside mag_cb");
//    flush_buffers();

    if(transfer->actual_length != g_mouse_packet_size){
        common_cb(transfer, FOURCC('M','A','G','E'));
    }else{
        common_cb(transfer, FOURCC('M','A','G','N'));
    }
}


static int start_mouse_transfer(libusb_device_handle * handle,
        unsigned int ep, libusb_transfer_cb_fn cb, void * data,
        unsigned int timeout)
{
    int iso_packets = 0, usb_err, i, num_urbs_in_flight = 8;

    num_urbs_in_flight = 1;	//JM

    struct libusb_transfer * trans[num_urbs_in_flight];
    unsigned char * buf = NULL;
    int packet_size = g_mouse_packet_size;//libusb_get_max_iso_packet_size(device, ep);

    if(packet_size < 0){
        return packet_size;
    }

    for(i = 0; i < num_urbs_in_flight; ++i){
        trans[i] = libusb_alloc_transfer(iso_packets);
        if(trans[i] == NULL){
            for(--i; i >= 0; --i){
                libusb_free_transfer(trans[i]);
            }
            return LIBUSB_ERROR_NO_MEM;
        }
        buf  = calloc(packet_size, sizeof(unsigned char));
        if(buf == NULL){
            for(; i>= 0; --i)
                libusb_free_transfer(trans[i]);
            return LIBUSB_ERROR_NO_MEM;
        }
        //libusb_fill_bulk_transfer(trans[i], handle, ep, buf, packet_size, cb, data, timeout);
        libusb_fill_interrupt_transfer(trans[i], handle, ep, buf, packet_size, cb, data, timeout);


        trans[i]->flags = LIBUSB_TRANSFER_FREE_BUFFER;
        usb_err = libusb_submit_transfer(trans[i]);
        if(usb_err != 0){

            printf_tagged_message(FOURCC('L', 'O', 'G', 'S'), "\n JM  error ");
            flush_buffers();

            for(--i; i >=0; --i){
                libusb_cancel_transfer(trans[i]);
                //todo: handle error besides LIBUSB_ERROR_NOT_FOUND for cancel
                libusb_free_transfer(trans[i]);
            }
            return usb_err;
        }
    }
    return 0;
}

void init_mouse(libusbSource * usb_source){
    int iface_nums[1] = {0};
    libusb_device_handle * mousehandle = open_usb_device_handle(usb_source, is_mouse, iface_nums, 1);
    if(!mousehandle) {
        return;
    }

//    struct libusb_transfer * ctrl = libusb_alloc_transfer(0);
//    unsigned char * ctrl_buf = calloc(LIBUSB_CONTROL_SETUP_SIZE,
//            sizeof(unsigned char));
//    libusb_fill_control_setup(ctrl_buf, LIBUSB_RECIPIENT_OTHER |
//            LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT,
//            ADDR_ALL | INST_GO, 0, 0, 0);
//    libusb_fill_control_transfer(ctrl, imu, ctrl_buf, ctrl_cb, NULL, 0);
//    libusb_submit_transfer(ctrl);


    start_mouse_transfer(mousehandle, g_mouse_IN_EP, mag_cb, NULL, 0);


}
