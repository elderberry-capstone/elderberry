/*
 * auto generated fcf
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

#include "testIMU.h"
#include "testLoggerDisk.h"
#include "testLoggerScreen.h"
#include "fcfutils.h"


static struct pollfd *fds;
static int nfds = 0;

static char buffer [1024];


void fcf_init() {
	// Calls all init functions
	init_theo_imu();
	// Fetch all FileDescriptors
	fcf_get_fd_structure(&fds, &nfds);
}

void testIMU_MessageA_Handler(int fd) {
	// generated function that is called when a file descriptor
	// that testIMU maps to MessageA is responsive.
	int rc = fileA_handler(fd, buffer, sizeof(buffer));

	if (rc > 0) {
		int len = rc;
		screenLogger_getMessage("gyr", buffer, len);
		//diskLogger_getMessage("gyr", buffer, len);
	}
}


void testIMU_MessageB_Handler(int fd) {
	// generated function that is called when a file descriptor
	// that testIMU maps to MessageA is responsive.
	int rc = fileB_handler(fd, buffer, sizeof(buffer));

	if (rc > 0) {
		int len = rc;
		screenLogger_getMessage("acc", buffer, len);
		//diskLogger_getMessage("acc", buffer, len);
	}
}



int fcf_main_loop_run() {

// starting code for socket/poll was this:
// http://publib.boulder.ibm.com/infocenter/iseries/v6r1m0/index.jsp?topic=/rzab6/poll.htm
//
//Perform polling loop with file descriptors
//For each responsive hit on a file descriptor run the callback function
//and process messages.
//
//	while(1) {
//	filedescriptor_poll(all_file_descriptors);
//	For each responsive hit
//	fire callback function for hit
//
//	if a file descriptor of type Message A was responsive call:
//		testIMU_MessageA_Handler(fd);
//	if a file descriptor of type Message B was responsive call:
//		testIMU_MessageB_Handler(fd);
//	}


	//int len;
	//int compress_array = FALSE;
	//int close_conn;


	int rc;
	int timeout = 5 * 1000;	// in ms

	for (;;)
	{
		/***********************************************************/
		/* Call poll() and wait for it to complete.      */
		/***********************************************************/
		printf("Waiting on poll()...\n");
		rc = poll(fds, nfds, timeout);

		/***********************************************************/
		/* Check to see if the poll call failed.                   */
		/***********************************************************/
		if (rc < 0)
		{
			perror("  poll() failed");
			break;
		}

		/***********************************************************/
		/* poll timed out                                          */
		/***********************************************************/
		if (rc == 0)
		{
			//do something useful, e.g. call into libusb so that libusb can deal with timeouts
			printf("  poll() timed out.\n");
		}


		/***********************************************************/
		/* One or more descriptors are readable.  Need to          */
		/* determine which ones they are.                          */
		/***********************************************************/
		int current_size = nfds;
		for (int i = 0; i < current_size; i++)
		{
			/*********************************************************/
			/* Loop through to find the descriptors that returned    */
			/* POLLIN and determine whether it's the listening       */
			/* or the active connection.                             */
			/*********************************************************/
			if(fds[i].revents == 0) {
				continue;
			}

			/*********************************************************/
			/* If revents is not POLLIN, it's an unexpected result,  */
			/* log and end the server.                               */
			/*********************************************************/
//			if(fds[i].revents != POLLIN)
//			{
//				printf("  Error! revents = %d\n", fds[i].revents);
//
//			}

			printf("  Descriptor %d is readable\n", fds[i].fd);

			//FIXIT: this cannot be hardcoded, needs to be function pointer or something
			switch (i) {
			case 0:
				testIMU_MessageA_Handler (fds[i].fd);
				break;
			case 1:
				testIMU_MessageB_Handler (fds[i].fd);
				break;
			}



//			/*******************************************************/
//			/* If the close_conn flag was turned on, we need       */
//			/* to clean up this active connection. This           */
//			/* clean up process includes removing the              */
//			/* descriptor.                                         */
//			/*******************************************************/
//			if (close_conn)
//			{
//				close(fds[i].fd);
//				fds[i].fd = -1;
//				compress_array = TRUE;
//			}

		} /* end loop file desc */

//	/***********************************************************/
//	/* If the compress_array flag was turned on, we need       */
//	/* to squeeze together the array and decrement the number  */
//	/* of file descriptors. We do not need to move back the    */
//	/* events and revents fields because the events will always*/
//	/* be POLLIN in this case, and revents is output.          */
//	/***********************************************************/
//	if (compress_array)
//	{
//		compress_array = FALSE;
//		for (i = 0; i < nfds; i++)
//		{
//			if (fds[i].fd == -1)
//			{
//				for(j = i; j < nfds; j++)
//				{
//					fds[j].fd = fds[j+1].fd;
//				}
//				nfds--;
//			}
//		}
//	}

	} /* end main loop */

//	/*************************************************************/
//	/* Clean up all of the sockets that are open                  */
//	/*************************************************************/
//	for (i = 0; i < nfds; i++)
//	{
//	if(fds[i].fd >= 0)
//		close(fds[i].fd);
//	}

	return 0;
}
