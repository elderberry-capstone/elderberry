// Do we need header files for every code file?

extern void init_screenLogger();

extern void screenLogger_getMessage(const char *src, char *buffer, int len);
extern void screenLogger_getMouseMessage(const char *src, unsigned char *buffer, int len);
