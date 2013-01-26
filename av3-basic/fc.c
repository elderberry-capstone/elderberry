/*
 * A stripped down version of the PSAS av3-fc code found here:
 * http://git.psas.pdx.edu/av3-fc.git
 */
#include <stdio.h>
#include <stdlib.h>

#include "libusb-basic.h"
#include "logging.h"
#include "mouse.h"


int main(int argc, char **argv)
{
	init_logging();

	libusbSource * usb_source = libusbSource_new();
	if(usb_source == NULL){
		exit(1);
	}

	init_mouse(usb_source);

	run_main_loop(usb_source);

	return 0;
}
