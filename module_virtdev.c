/*
 * module_virtdev.c
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "utils_sockets.h"
#include "fcfutils.h"
#include "fcfmain.h"

static void finalize_virtdev (const char *devname, int *fd);

static unsigned char buffer[1000];
static int fd_gyr = -1;
static int fd_acc = -1;
static const char *VIRT_GYR = "virt_gyr";
static const char *VIRT_ACC = "virt_acc";


static int common_cb(const char *src, int fd){
	int rc = readsocket(fd, buffer, sizeof(buffer));
	if (rc > 0) {
		fcf_callback_virtdev(src, buffer, rc);
	}
	return rc;
}

//active fd
static void virtgyr_cb (struct pollfd *pfd) {
	int rc = common_cb(VIRT_GYR, pfd->fd);
	if (rc == -1) {
		printf("%s: connection closed\n", VIRT_GYR);
		finalize_virtdev (VIRT_GYR, &fd_gyr);
	}
}

static void virtacc_cb (struct pollfd *pfd) {
	int rc = common_cb(VIRT_ACC, pfd->fd);
	if (rc == -1) {
		printf("%s: connection closed\n", VIRT_ACC);
		finalize_virtdev (VIRT_ACC, &fd_acc);
	}
}


static int initvirtdev (const char *devname, int port, pollfd_callback cb) {
	printf ("probing %s: (waiting for connection localhost:%d)\n", devname, port);
	int fd = getsocket(port);
	if (fd >= 0) {
		int rc = fcf_add_fd(fd, POLLIN, cb);
		if (rc >= 0) {
			return fd;
		}
	}
	return -1;
}

static void finalize_virtdev (const char *devname, int *fd) {
	if (*fd >= 0) {
		fcf_remove_fd(*fd);
		int rc = close(*fd);
		if (rc == -1) {
			fprintf (stderr, "finalize_virtdev: error closing socket for %s: %s\n", devname, strerror(errno));
		}
		*fd = -1;
	}
}


void init_virtgyr() {
	fd_gyr = initvirtdev (VIRT_GYR, 8081, virtgyr_cb);
}

void init_virtacc() {
	fd_acc = initvirtdev (VIRT_ACC, 8082, virtacc_cb);
}


void finalize_virtgyr() {
	finalize_virtdev (VIRT_GYR, &fd_gyr);
}

void finalize_virtacc() {
	finalize_virtdev (VIRT_ACC, &fd_acc);
}

