#ifndef _FCFUTILS_
#define _FCFUTILS_

#define TRUE             1
#define FALSE            0

typedef void (*pollfd_callback)(int fd_idx);
extern void fcf_add_fd(const char*, int, pollfd_callback);
extern int fcf_remove_all_fd(const char*);
extern pollfd * fcf_get_fd(int);
extern void fcf_get_fd_structure(struct pollfd **_fds, struct fcffd **_fdx, int *_nfds);
extern int run_poll_loop(void);

#endif
