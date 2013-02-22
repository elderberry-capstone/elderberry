/*
main program
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "fcfutils.h"

extern void fcf_init();

static void signalhandler(int signum) {
    printf ("\n **signal handler: signum = %d", signum);

    if (signum == SIGINT)  {
    	fcf_stop_main_loop ();
    }
}


int main(int argc, char *argv[]) {

	puts("FC");
	signal (SIGINT, signalhandler);

	fcf_init();
	int rc = fcf_run_poll_loop();

	return EXIT_SUCCESS;
}
