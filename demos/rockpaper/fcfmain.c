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
#include "game.h"
#include "testLoggerDisk.h"
#include "testLoggerScreen.h"
#include "fcfutils.h"


/***************	CODE GENERATED SPACE	**************/

void fcf_init() {
	// Calls all init functions
	init_mouse();
	init_mouse2();
	init_game();
	init_diskLogger();
}

void fcf_callback_game(char *buff, int length) {
	//screenLogger_getMessage("game", buff, length);
	diskLogger_getMessage("game", buff, length);
}

void fcf_callback_mouse(unsigned char *buff, int length){
	game_getDeviceClick(buff, 0);
}

void fcf_callback_mouse2(unsigned char *buff, int length){
	game_getDeviceClick(buff, 1);
}

/****************	END CODE GENERATED SPACE	*****************/

