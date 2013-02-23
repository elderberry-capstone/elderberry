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

#include "mouse.h"
#include "mouse2.h"
#include "mouse3.h"
#include "testIMU.h"
#include "testLoggerDisk.h"
#include "testLoggerScreen.h"
#include "fcfutils.h"


/***************	CODE GENERATED SPACE	**************/

void fcf_init() {
	// Calls all init functions
	init_fcf();
	init_diskLogger();
	init_mouse();
	init_mouse3();
	//init_theo_imu();
	
}

void fcf_callback_gyr(char *buff, int length) {
	screenLogger_getMessage("gyr", buff, length);
	diskLogger_getMessage("gyr", buff, length);
}

void fcf_callback_acc(char *buff, int length) {
	screenLogger_getMessage("acc", buff, length);
	diskLogger_getMessage("acc", buff, length);
}

void fcf_callback_mouse(unsigned char *buff, int length){
	screenLogger_getMouseMessage("mouse", buff, length);
}

void fcf_callback_mouse2(unsigned char *buff, int length){
	screenLogger_getMouseMessage("mouse", buff, length);
}


/***************	END CODE GENERATED SPACE	**************/