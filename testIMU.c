/**
 *  @file testIMU.c
 *  @brief Brief description needs to go here
 *  @details More detailed desciption needs to go here
 *  @author Ron Astin
 *  @date February 5th, 2013
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#include "fcfutils.h"
#include "testIMU.h"

extern void fcf_callback_gyr(char *, int);
extern void fcf_callback_acc(char *, int);

static int readsocket(int fd, char *buffer, int bufsize);
static int getsocket(int serverport);

static char _buffer [1024];

void gyr_handler(int idx) {
	struct pollfd *fd = fcf_get_fd(idx);
	int length = readsocket(fd->fd, _buffer, sizeof(_buffer));
	fcf_callback_gyr(_buffer, length);
}


void acc_handler(int idx) {
	struct pollfd *fd = fcf_get_fd(idx);
	int length = readsocket(fd->fd, _buffer, sizeof(_buffer));
	fcf_callback_acc(_buffer, length);
}
/**
 *  @brief Initializes sockets to communicate with the IMU.
 *  @details Starts by removing all file descriptors that were added under the "IMU" source token [place code snippet here].  After removing the file descriptors, we add the fild descriptors from socket 8081, and 8082 with the IMU token.
 */
void init_theo_imu() {
	fcf_remove_all_fd("IMU");
	short tevents = 0;

	printf ("probing gyro: (waiting for connection localhost:8081)\n");
	int fd1 = getsocket(8081);
	fcf_add_fd ("gyr", fd1, tevents, gyr_handler);

	printf ("probing acc: (waiting for connection localhost:8082)\n");
	int fd2 = getsocket(8082);
	fcf_add_fd ("acc", fd2, tevents, acc_handler);
}


int fileA_handler(int fd, char *buffer, int bufsize) {
  /***
*  In this function we point buffer at a message from device.
*  This function handles a particular message type (fileA).
*/
	// allocate memory for message, perhaps one static buffer for life or app?
	// change the pointer that points to buffer to point to this data.
	// This function hides all the ugly work of getting "The Message" off of the
	// hardware device. Every hardware interfacing module that sends messages will
	// have specific callback functions for each message type to send.

	return readsocket(fd, buffer, bufsize);
}

/**
 *  @brief This function handles a particular message type (fileB)
 *  @details This function hides all of the ugly work of getting "The Message" off of the hardware device. Every hardware interfacing module that sends messages will have specific callback functionsfor each message type to send.
 *  @param fd File Descriptor
 *  @param buffer Contents of character buffer
 *  @param bufsize Size/length of buffer
 */
int fileB_handler(int fd, char *buffer, int bufsize) {
/***
*  In this function we point buffer at a message from device.
*  This function handles a particular message type (fileB).
*/
  // allocate memory for message, perhaps one static bufffer for life or app?
  // change the pointer that points to buffer to point to this data.
  // This function hides all the ugly work of getting "The Message" off of the
  // hardware device. Every hardware interfacting module that sends messages will
  // have specific callback functions for each message type to send.

	return readsocket(fd, buffer, bufsize);
}

// Other private functions to do stuff.


/**
 *  @brief Recieve data from a socket for a file descriptor
 *  @details Receive data on this connection until the recv fails with EWOULDBLOCK. If any other failur occures, we will termintate the connection
 *  @param fd File Descriptor
 *  @param buffer Contents of character buffer
 *  @param bufsize Size/length of buffer
 */
static int readsocket(int fd, char *buffer, int bufsize) {
	/*****************************************************/
	/* Receive data on this connection until the         */
	/* recv fails with EWOULDBLOCK. If any other        */
	/* failure occurs, we will close the                 */
	/* connection.                                       */
	/*****************************************************/

	int rc = recv(fd, buffer, bufsize, 0);
	if (rc < 0)
	{
		if (errno != EWOULDBLOCK)
		{
			perror("  recv() failed");
			return -1;
		}
		return 0;
	}

	/*****************************************************/
	/* Check to see if the connection has been           */
	/* closed by the client                              */
	/*****************************************************/
	if (rc == 0)
	{
		printf("  Connection closed\n");
		return -1;
	}

	/*****************************************************/
	/* Data was received                                 */
	/*****************************************************/
	printf("  %d bytes received\n", rc);

	return rc;
}

/**
 *  @brief Create a socket to receive on.
 *  @details Create an AF_INET stream socket to receive incoming connections on. We bind the cocket, then set the listen backlog, then accept each incoming connection. If the accept fails with EWOULDBLOCK, then we have accepted all connections. Any other failure on accept will cause us to end the server.
 *  @param serverport 
 */
static int getsocket(int serverport) {
	int listen_sd;
	int rc;
	//int on = 1;
	struct sockaddr_in addr;

	/*************************************************************/
	/* Create an AF_INET stream socket to receive incoming       */
	/* connections on                                            */
	/*************************************************************/
	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sd < 0)
	{
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

//	/*************************************************************/
//	/* Allow socket descriptor to be reuseable                   */
//	/*************************************************************/
//	rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
//	if (rc < 0)
//	{
//		perror("setsockopt() failed");
//		close(listen_sd);
//		exit(-1);
//	}

	/*************************************************************/
	/* Set socket to be nonblocking. All of the sockets for    */
	/* the incoming connections will also be nonblocking since  */
	/* they will inherit that state from the listening socket.   */
	/*************************************************************/
//	rc = ioctl(listen_sd, FIONBIO, (char *)&on);
//	if (rc < 0)
//	{
//		perror("ioctl() failed");
//		close(listen_sd);
//		exit(EXIT_FAILURE);
//	}

	/*************************************************************/
	/* Bind the socket                                           */
	/*************************************************************/
	memset(&addr, 0, sizeof(addr));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //INADDR_ANY
	addr.sin_port        = htons(serverport);
	rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0)
	{
		perror("bind() failed");
		close(listen_sd);
		exit(EXIT_FAILURE);
	}

	/*************************************************************/
	/* Set the listen back log                                   */
	/*************************************************************/
	rc = listen(listen_sd, 1);
	if (rc < 0)
	{
		perror("listen() failed");
		close(listen_sd);
		exit(EXIT_FAILURE);
	}

	/*****************************************************/
	/* Accept each incoming connection. If               */
	/* accept fails with EWOULDBLOCK, then we            */
	/* have accepted all of them. Any other              */
	/* failure on accept will cause us to end the        */
	/* server.                                           */
	/*****************************************************/
	int new_sd = accept(listen_sd, NULL, NULL);
	if (new_sd < 0)
	{
		if (errno != EWOULDBLOCK)
		{
			perror("  accept() failed");
			exit (EXIT_FAILURE);
		}
	}


	return new_sd;
}
