/*
 * theo-imu.c
 *
 * Initializes and automatically starts data transfers from theo-imu, and logs
 * the recorded data.
 */

#include <stdlib.h>
#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include "fcfutils.h"
#include "fcfmain.h"
#include "utils_libusb-1.0.h"


#define BULK0_IN_EP 0x82
#define BULK1_IN_EP 0x85
#define BULK2_IN_EP 0x88
#define BULK3_IN_EP 0x8B
#define ACC_EP BULK0_IN_EP
#define GYR_EP BULK1_IN_EP
#define MAG_EP BULK2_IN_EP
#define CAC_EP BULK3_IN_EP

#define CTRL_OUT_EP             0x00
#define ADDR_ACC                0x80
#define ADDR_GYR                0x40
#define ADDR_MAG                0x20
#define ADDR_CAC                0x10
#define ADDR_ALL                0xF0
#define INST_RESET              0x01
#define INST_GO                 0x02
#define INST_STOP               0x03
#define INST_INC_SPEED          0x04
#define INST_DEC_SPEED          0x05

#define IMU_INST(X)             ((X) & 0x0F)
#define IMU_ADDR(X)             ((X) & 0xF0)

#define IMU_PACKET_SIZE 13
#define SENSOR_DATA_OFFSET 6

static const int VID = 0xFFFF;
static const int PID = 0x0005;


static void data_callback(struct libusb_transfer *transfer, const char * src){
    unsigned char *buf = NULL;
    unsigned int act_len;
    int retErr;

    switch(transfer->status){
    case LIBUSB_TRANSFER_COMPLETED:
        buf = transfer->buffer;

        act_len = transfer->actual_length;
        
		fcf_callback_theo_imu(src, buf, act_len);

        retErr = libusb_submit_transfer(transfer);
        if(retErr){
            //print_libusb_transfer_error(transfer->status, "imu_cb resub");
        }
        break;
    case LIBUSB_TRANSFER_CANCELLED:
        //do nothing.
        break;
    default:
        //print_libusb_transfer_error(transfer->status, "imu_cb");
        break;
    }
}

static void mag_cb(struct libusb_transfer *transfer){
	data_callback(transfer, "theo_imu_mag");
}
static void acc_cb(struct libusb_transfer *transfer){
    data_callback(transfer, "theo_imu_acc");
}
static void gyr_cb(struct libusb_transfer *transfer){
    data_callback(transfer, "theo_imu_gyr");
}
static void cac_cb(struct libusb_transfer *transfer){
   data_callback(transfer, "theo_imu_cac");
}

/*	Clark: Shouldn't need these, but holding onto them just in case.

static void ctrl_cb(struct libusb_transfer *transfer){

}

tatic int start_bulk_transfer(libusb_device_handle * handle,
        unsigned int ep, libusb_transfer_cb_fn cb, void * data,
        unsigned int timeout)
{
    int iso_packets = 0, usb_err, i, num_urbs_in_flight = 8;
    struct libusb_transfer * trans[num_urbs_in_flight];
    unsigned char * buf = NULL;
    int packet_size = IMU_PACKET_SIZE;//libusb_get_max_iso_packet_size(device, ep);
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
        libusb_fill_bulk_transfer(trans[i], handle, ep, buf, packet_size, cb,
                data, timeout);

        trans[i]->flags = LIBUSB_TRANSFER_FREE_BUFFER;
        usb_err = libusb_submit_transfer(trans[i]);
        if(usb_err != 0){
            for(--i; i >=0; --i){
                libusb_cancel_transfer(trans[i]);
                //todo: handle error besides LIBUSB_ERROR_NOT_FOUND for cancel
                libusb_free_transfer(trans[i]);
            }
            return usb_err;
        }
    }
    return 0;
}*/

int init_theo_imu(){
	
	init_device("theo_imu_mag", VID, PID, MAG_EP, mag_cb);
	init_device("theo_imu_gyr", VID, PID, GYR_EP, gyr_cb);
	init_device("theo_imu_acc", VID, PID, ACC_EP, acc_cb);
	init_device("theo_imu_cac", VID, PID, CAC_EP, cac_cb);

    /* Clark: Might need these, depends on transfer protocol needed.
	start_bulk_transfer(imu, MAG_EP, mag_cb, NULL, 0);
    start_bulk_transfer(imu, GYR_EP, gyr_cb, NULL, 0);
    start_bulk_transfer(imu, ACC_EP, acc_cb, NULL, 0);
    start_bulk_transfer(imu, CAC_EP, cac_cb, NULL, 0);*/

	return 0;
}
