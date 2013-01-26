/*
 * socket.h
 *
 *  Created on: Jan 26, 2013
 *      Author: peps
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include <poll.h>
int readsocket(int fd, unsigned char *buffer, int bufsize);
int getsocket(int serverport);


#endif /* SOCKET_H_ */
