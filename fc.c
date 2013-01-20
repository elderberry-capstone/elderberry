/*
main program
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern void fcf_init();
extern void fcf_main_loop_run();



int main(int argc, char *argv[]) {

	puts("FC");

	fcf_init();
	fcf_main_loop_run();

	return EXIT_SUCCESS;
}
