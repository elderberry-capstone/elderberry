#ifndef _FCFMAIN_
#define _FCFMAIN_

#define TRUE             1
#define FALSE            0


extern void fcf_callback_gyr(char *buff, int length);
extern void fcf_callback_acc(char *buff, int length);
extern void fcf_callback_mouse(unsigned char *buff, int length);
extern void fcf_callback_mouse2(unsigned char *buff, int length);
extern void fcf_callback_mouse3(unsigned char *buff, int length);
extern void fcf_callback_mouse4(unsigned char *buff, int length);
extern void fcf_callback_temp(char *buff, int length);
extern void fcf_callback_gps(unsigned char, unsigned char *, int);
extern void fcf_callback_theo_imu(const char *, unsigned char *, int);
extern void fcf_callback_virtdev(const char * src, unsigned char *buff, int length);

#endif
