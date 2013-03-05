#include <cstdio>
#include <libusb-1.0/libusb.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <boost/lexical_cast.hpp>
using namespace std;

#define ERROR cerr << "\033[31;1mERROR:\t"
#define WARN cerr << "\033[33mWARNING:\t"
#define INFO cout << "\033[36mINFO:\t"
#define CRITICAL cerr << "\033[5;41;37mCRITICAL:\t"
#define DEBUG cout << "\033[37mDEBUG:\t"
#define ENDL "\033[0m" << endl

const unsigned int MY_TIMEOUT = 10000;


int main(int argc, char **argv){
    if (!(argc == 3 || argc == 2)){
        ERROR << argv[0] << " requires 2 or 3 arguments. " << (argc-1) <<
            " given." << ENDL;
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

    libusb_device *endpoint = libusb_get_device(handle);
    int err = libusb_claim_interface(handle, 0);
    if (err != 0){
        ERROR << libusb_error_name(err) << ENDL;
    }else{
        DEBUG << "Claimed interface." << ENDL;
    }

    unsigned char data[8] = "";

    uint8_t addr1 = libusb_get_device_address(endpoint);
    // Convert addr1 to a character array (yuck!)
    stringstream out;
    out << addr1;
    const char *addr = out.str().c_str();

    int length = sizeof(data);
    int *transferred;
    unsigned int timeout = MY_TIMEOUT;
    libusb_interrupt_transfer(handle, 0x1, data, length, transferred,
        timeout);
    
    err = libusb_release_interface(handle, 0);
    if (err != 0){
        ERROR << libusb_error_name(err) << ENDL;
    }else{
        DEBUG << "Released interface." << ENDL;
    }

    libusb_close(handle);
    libusb_exit(ctx);
    return 1;
}
