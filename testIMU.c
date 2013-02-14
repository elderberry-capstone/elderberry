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

void init_theo_imu() {
	fcf_remove_all_fd("IMU");

	printf ("probing gyro: (waiting for connection localhost:8081)\n");
	int fd1 = getsocket(8081);
	fcf_add_fd ("gyr", fd1, gyr_handler);

	printf ("probing acc: (waiting for connection localhost:8082)\n");
	int fd2 = getsocket(8082);
	fcf_add_fd ("acc", fd2, acc_handler);
}


// Other private functions to do stuff.



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
