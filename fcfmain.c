#include "fcfutils.h"
#include "module_logger_screen.h"
#include "module_virtdev.h"


void fcf_initialize() {
    init_logger_screen();
    init_virtdev();
}


void fcf_finalize() {
    finalize_virtdev();
    finalize_logger_screen();
}


void sendMessage_virtdev(const char* src, unsigned char* buffer, int length) {
    getMessage_logger_screen(src, buffer, length);
}

