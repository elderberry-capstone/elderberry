/*
main program
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern void fcf_init();
extern int fcf_run_poll_loop();



int main(int argc, char *argv[]) {

	fcf_init();
	fcf_run_poll_loop();

	return EXIT_SUCCESS;
}
