/*
 * A stripped down version of the PSAS av3-fc code found here:
 * http://git.psas.pdx.edu/av3-fc.git
 */
#include <stdio.h>
#include <stdlib.h>

#include "libusb-basic.h"
#include "logging.h"
#include "mouse.h"
#include "mouse2.h"
#include "gps.h"
#include "virtdevsrv.h"



int main(int argc, char **argv)
{
	init_logging();

	libusbSource * usb_source = libusbSource_new();
	if(usb_source == NULL){
		exit(1);
	}

	//comment out the devices you don't have or want
	init_gps(usb_source);
	init_mouse(usb_source);		//read from usb mouse; set your hw values in is_mouse()
	init_mouse2(usb_source);	//read from a 2nd usb mouse; set your hw values in is_mouse2()
	init_virtgyro(usb_source);	//read from socket

	run_main_loop(usb_source);

	return 0;
}
