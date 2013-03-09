#include "fcfutils.h"
#include "module_mouse_clark.h"
#include "module_logger_screen.h"


void fcf_initialize() {
    init_mouse_clark();
    init_logger_screen();
}


void fcf_finalize() {
    finalize_logger_screen();
    finalize_mouse_clark();
}


void sendMessage_mouse_clark(const char* src, unsigned char* buffer, int length) {
    getMouseMessage_logger_screen(src, buffer, length);
}

