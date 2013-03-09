/*
 * module_mouse_jm2.h
 *
 */

#ifndef MODULE_MOUSE_JM2_H_
#define MODULE_MOUSE_JM2_H_

void init_mouse_jm2 (void);
void finalize_mouse_jm2 (void);
extern void sendMessage_mouse_clark(unsigned char *buffer, int length);

#endif /* MODULE_MOUSE_JM2_H_ */
