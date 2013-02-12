/*
 * gps.h
 *
 */

#ifndef GPS_H_
#define GPS_H_

#include "libusb-basic.h"

void set_gps_devicepath (const char *dev);
void init_gps(libusbSource * src);

#endif /* GPS_H_ */
