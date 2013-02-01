/**
*	\file fcutils.c
*	\brief API code for framework.
*
*	This file includes the functions needed to add and remove file
*	descriptors, as well as store the file descriptor arrays.
*/

#ifndef _FCFUTILS_
#define _FCFUTILS_

#define TRUE             1
#define FALSE            0

extern int fcf_add_fd(const char*, int, char*);
extern int fcf_remove_all_fd(const char*);
extern void fcf_get_fd_structure(struct pollfd **_fds, int *_nfds);
#endif
