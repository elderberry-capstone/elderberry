

int start_usb_transfer(libusb_device_handle * handle, unsigned int ep, libusb_transfer_cb_fn cb, void * data, int packet_size, unsigned int timeout);
int init_device(char * dev_name, int vid, int pid, const int endpoint, libusb_transfer_cb_fn cb);