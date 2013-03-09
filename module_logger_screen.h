// Do we need header files for every code file?

void init_logger_screen(void);
void finalize_logger_screen(void);
extern void getMessage_logger_screen(const char *src, unsigned char *buffer, int length);
extern void getTemp_logger_screen(const char *src, char *buffer, int length);
extern void getMouseMessage_logger_screen(const char *src, unsigned char *buffer, int length);
