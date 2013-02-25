/**
*	fcutils.c
*	
*	API code for framework.
*
*	This file includes the functions needed to add and remove file
*	descriptors, as well as store the file descriptor arrays.
*/

#ifndef _FCFUTILS_
#define _FCFUTILS_

#define TRUE             1
#define FALSE            0

typedef void (*pollfd_callback)(int fd_idx);
extern int fcf_add_fd(const char*, int, short, pollfd_callback);
extern int fcf_remove_fd(int);
extern int fcf_remove_fd_by_token(const char *);
extern struct pollfd * fcf_get_fd(int);
extern int init_fcf(void);
extern void fcf_stop_main_loop(void);
extern void fcf_init();

#endif
