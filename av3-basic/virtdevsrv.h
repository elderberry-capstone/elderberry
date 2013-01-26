/*
 * virtdevsrv.h
 *
 *  Created on: Jan 26, 2013
 *      Author: peps
 */

#ifndef VIRTDEVSRV_H_
#define VIRTDEVSRV_H_

#include "socket.h"
#include "libusb-basic.h"

int init_virtgyro(libusbSource *src);
int init_virtacc(libusbSource *src);

#endif /* VIRTDEVSRV_H_ */
