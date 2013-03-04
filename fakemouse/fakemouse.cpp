#include <cstdio>
#include <libusb-1.0/libusb.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
using namespace std;

#define ERROR cerr << "\033[31;1mERROR:\t"
#define WARN cerr << "\033[33mWARNING:\t"
#define INFO cout << "\033[36mINFO:\t"
#define CRITICAL cerr << "\033[5;41;37mCRITICAL:\t"
#define DEBUG cout << "\033[37mDEBUG:\t"
#define ENDL "\033[0m" << endl


int main(int argc, char **argv){
    if (argc != 3){
        ERROR << argv[0] << " requires 2 arguments. " << (argc-1) << " given."
            << ENDL;
        exit(EXIT_FAILURE);
        return 0;
    }
    struct libusb_context *ctx;
    struct libusb_device **list;
    struct libusb_device_handle *handle;
    libusb_init(&ctx);

    int base = 16;
    uint16_t vid = stoll (argv[1], NULL, base);
    uint16_t pid = stoll (argv[2], NULL, base);

    handle = libusb_open_device_with_vid_pid(ctx, vid, pid);
    if (handle == NULL){
        ERROR << "Handle not found for device " << vid << ":" << pid << ENDL;
        exit(EXIT_FAILURE);
    }

    endpoint = libusb_get_device(handle);
    libusb_interrupt_transfer(handle, endpoint, data, length, transferred,
        timeout)
    

    libusb_close(handle);
    libusb_exit(ctx);
    return 1;
}
