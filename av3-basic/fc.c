/*
 * A stripped down version of the PSAS av3-fc code found here:
 * http://git.psas.pdx.edu/av3-fc.git
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "libusb-basic.h"
#include "logging.h"
#include "miml.h"


void signalhandler(int signum);
void signalhandler(int signum) {
	printf ("\n **signal handler: signum = %d", signum);
}

int main(int argc, char **argv)
{
	signal (SIGUSR1, signalhandler);
	signal (SIGUSR2, signalhandler);

	init_logging();

	libusbSource * usb_source = libusbSource_new();
	if(usb_source == NULL){
		exit(1);
	}

	FCF_Init(usb_source);

	run_main_loop(usb_source);

	return 0;
}
