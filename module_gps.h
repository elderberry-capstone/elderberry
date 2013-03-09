/*
 * gps.h
 *
 */

#ifndef GPS_H_
#define GPS_H_


void set_gps_devicepath(const char *dev);
void init_gps(void);
void finalize_gps(void);
extern void sendMessage_gps(unsigned char ret, unsigned char * buff, int length);

#endif /* GPS_H_ */
