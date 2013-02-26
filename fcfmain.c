/*
 * auto generated fcf
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fcfutils.h"

/**		User Modules	*/
#include "module_mouse.h"
#include "module_mouse2.h"
#include "module_mouse3.h"
#include "module_mouse4.h"
#include "module_testIMU.h"
#include "module_testLoggerDisk.h"
#include "module_testLoggerScreen.h"
#include "module_profile.h"
//#include "module_test_framework.h"



/***************	CODE GENERATED SPACE	**************/

void fcf_init() {
	// System inits
	init_fcf();			//< Special FCF init that sets up fd structures

	// User module inits
	init_diskLogger();
	//init_mouse3();
	//init_mouse();
	init_theo_imu();
	//init_test();
	//init_profiling();
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


void fcf_callback_mouse3(unsigned char *buff, int length){
	screenLogger_getMouseMessage("mouse3", buff, length);
}

void fcf_callback_profile(unsigned char *buff, int length){
	profile_getMessage(buff, length);
}


/***************	END CODE GENERATED SPACE	**************/
