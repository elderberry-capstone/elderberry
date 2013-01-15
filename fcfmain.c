#include "testIMU.h"
#include "testLoggerDisk.h"
#include "testLoggerScreen.h"
#include "fcfutils.h"

struct_FD *all_file_descriptors;

void init() {
  // Calls all init functions
  init_theo_imu();
  // Fetch all FileDescriptors
  fcf_get_fd_structure(all_file_descriptors);
}

void testIMU_MessageA_Handler(int fd) {
  // generated function that is called when a filedescriptor
  // that testIMU maps to MessageA is responsive.
  somedata *msgA;
  fileA_handler(fd, &msgA);
  screenLogger_getMessage(msgA);
  diskLogger_getMessage(msgA);
}

void testIMU_MessageB_Handler(int fd) {
  // generated function that is called when a filedescriptor
  // that testIMU maps to MessageB is responsive.
  somedata *msgB;
  fileB_handler(fd, &msgB);
  screenLogger_getMessage(msgB);
  diskLogger_getMessage(msgB);  
}

int main() {
  // Perform polling loop with file descriptors
  // For each responsive hit on a file descriptor run the callback function
  // and process messages.
  while(1) {
    filedescriptor_poll(all_file_descriptors);
    // For each responsive hit
	// fire callback function for hit
        // if a file descriptor of type Message A was responsive call:
        testIMU_MessageA_Handler(fd);
        // if a file descriptor of type Message B was responsive call:
        testIMU_MessageB_Handler(fd);
  }
  return 0;
}
