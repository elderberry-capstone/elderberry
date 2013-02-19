#include <sys/timerfd.h>
#include <stdio.h>
#include "usbutils.h"
#include "fc.h"
#include "miml.h"
void InitProfiling(libusbSource * src);
int profiling_cb(void);
void profReceiveMsg(const char *msg);
//extern int fcf_add_fd(int, short, pollCallback);
