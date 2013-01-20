#ifndef _FCFUTILS_
#define _FCFUTILS_

#define TRUE             1
#define FALSE            0

extern void fcf_add_fd(const char*, int, char*);
extern void fcf_remove_all_fd(const char*);
extern void fcf_get_fd_structure(struct pollfd **_fds, int *_nfds);
#endif
