/*
 * module_mouse_jm.h
 *
 */

#ifndef MODULE_MOUSE_JM_H_
#define MODULE_MOUSE_JM_H_

void init_mouse_jm (void);
void finalize_mouse_jm (void);
extern void sendMessage_mouse_jm(const char *src, unsigned char *buffer, int length);

#endif /* MODULE_MOUSE_JM_H_ */
