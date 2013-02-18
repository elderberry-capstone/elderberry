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
	/* MAKE SURE DISKLOGGER INIT IS FIRST! */
	init_diskLogger();
	init_mouse();
	init_mouse2();
	init_game();
	
}

void fcf_callback_game(char *buff, int length) {
	screenLogger_getMessage("game", buff, length);
	diskLogger_getGameMessage("game", buff, length);
}

void fcf_callback_mouse(unsigned char *buff, int length){
	game_getDeviceClick(buff, 0);
	diskLogger_getMouseMessage("mouse1", buff, length);
}

void fcf_callback_mouse2(unsigned char *buff, int length){
	game_getDeviceClick(buff, 1);
	diskLogger_getMouseMessage("mouse2", buff, length);
}

/****************	END CODE GENERATED SPACE	*****************/

