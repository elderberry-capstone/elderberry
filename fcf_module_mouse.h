/*
 * fcf_module_mouse.h
 *
 * 
 */

#ifndef FCF_MODULE_MOUSE_H_
#define FCF_MODULE_MOUSE_H_

#include <libusb-1.0/libusb.h>
#include "fcfutils.h"

extern int mouse_init(void);
extern int mouse_callback(int);

#endif
