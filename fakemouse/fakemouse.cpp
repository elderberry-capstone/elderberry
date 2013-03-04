#include <cstdio>
#include <libusb-1.0/libusb.h>
#include <iostream>
#include <cstdlib>
using namespace std;

#define ERROR cerr << "\033[31;mERROR:\t"
#define WARN cerr << "\033[33;mWARNING:\t"
#define INFO cout << "\033[36;mINFO:\t"
#define CRITICAL cerr << "\033[5;41;37;mCRITICAL:\t"
#define DEBUG cout << "\033[37;mDEBUG:\t"
#define ENDL ";\033[0;m"


int main(int argc, char **argv){
    struct libusb_context *ctx;
    struct libusb_device_handle *handle;

    // Initialize
    libusb_init(&ctx);

    // Set up the device
    uint16_t vid = atoi(argv[1]),
             pid = atoi(argv[2]);
    handle = libusb_open_device_with_vid_pid(ctx, vid, pid);
    INFO << "Set up device " << vid << ", " << pid << ENDL;
    libusb_exit(ctx);
}
