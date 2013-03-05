#include <stdio.h>

#include "utils_sockets.h"
#include "fcfutils.h"
#include "fcfmain.h"

static unsigned char buffer[1000];


static void common_cb(const char * src, int fd){
	int rc = readsocket(fd, buffer, sizeof(buffer));
	if (rc > 0) {
		fcf_callback_virtdev(src, buffer, rc);
	}
}

//active fd
static void virtgyr_cb (struct pollfd *pfd) {
	common_cb("virt_gyr", pfd->fd);
}

static void virtacc_cb (struct pollfd *pfd) {
	common_cb("virt_acc", pfd->fd);
}


static int initvirtdev (const char* devname, int port, pollfd_callback cb) {
	printf ("probing %s: (waiting for connection localhost:%d)\n", devname, port);
	int fd = getsocket(port);
	int rc = fcf_add_fd(fd, POLLIN, cb);
	return rc;
}



//instead of having multiple callback functions,
//have one and pass through parameters?

void init_virtgyr() {
	initvirtdev ("virt_gyr", 8081, virtgyr_cb);
}

void init_virtacc() {
	initvirtdev ("virt_acc", 8082, virtacc_cb);
}

