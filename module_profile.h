/*
 * module_profile.h
 *
 */

#ifndef MODULE_PROFILE_H_
#define MODULE_PROFILE_H_

extern void init_profiling(void);
extern void getMessage_profile (unsigned char *buf, int len);
extern void sendMessage_profile(unsigned char *buff, int length);

#endif /* MODULE_PROFILE_H_ */
