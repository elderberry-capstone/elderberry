/*
 * auto generated fcf
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fcfutils.h"

/**		User Modules	*/

#include "module_mouse_clark.h"
#include "module_mouse_clark2.h"
#include "module_mouse_jm.h"
#include "module_mouse_jm2.h"
#include "module_fakemouse.h"
#include "module_testLoggerScreen.h"
#include "module_profile.h"
#include "module_testLoggerDisk.h"
#include "module_gps.h"
#include "module_theo-imu.h"
#include "module_virtdev.h"



/***************	CODE GENERATED SPACE	**************/

void fcf_initialize() {
	// User module inits
	//init_diskLogger();	// This should be listed first.
	//init_temp();
	//init_mouse_clark();
	//init_mouse_clark2();
	//init_mouse_jm();
	//init_mouse_jm2();
	//init_diskLogger();
	//init_test();
	//init_gps();
	//init_theo_imu();
	init_virtgyr();
	init_virtacc();
	//init_profiling();
	//init_virtFAKEMOUSE();
}

void fcf_finalize() {
	//finalize_gps();
	finalize_virtgyr();
	finalize_virtacc();
	//finalize_mouse_jm2();
	//finalize_mouse_jm();
	//finalize_mouse_clark2();
	//finalize_mouse_clark();
	//finalize_theo_imu();
	//finalize_diskLogger();
}

void fcf_callback_virtdev(const char * src, unsigned char *buff, int length) {
	screenLogger_getMessage(src, buff, length);
}

void fcf_callback_temp(char *buff, int length){
	screenLogger_getTemp("temp", buff, length);
}

void fcf_callback_mouse_clark(unsigned char *buff, int length){
	screenLogger_getMouseMessage("mouse_clark", buff, length);
}

void fcf_callback_mouse_clark2(unsigned char *buff, int length){
	screenLogger_getMouseMessage("mouse_clark2", buff, length);
}

void fcf_callback_mouse_jm(unsigned char *buff, int length){
	screenLogger_getMouseMessage("mouse_jm", buff, length);
}

void fcf_callback_mouse_jm2(unsigned char *buff, int length){
	screenLogger_getMouseMessage("mouse_jm2", buff, length);
}

void fcf_callback_profile(unsigned char *buff, int length){
	profile_getMessage(buff, length);
}

void fcf_callback_gps(unsigned char ret, unsigned char * buff, int length){
}

void fcf_callback_theo_imu(const char * src, unsigned char * buff, int length){
}


/***************	END CODE GENERATED SPACE	**************/
