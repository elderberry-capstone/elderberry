/*
 * module_virtdev.h
 *
 *  Created on: Jan 26, 2013
 */

#ifndef VIRTDEVSRV_H_
#define VIRTDEVSRV_H_

void init_virtdev(void);
void finalize_virtdev(void);
extern void sendMessage_virtdev(const char * src, unsigned char *buffer, int length);

#endif /* VIRTDEVSRV_H_ */
