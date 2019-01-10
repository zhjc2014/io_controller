#ifndef __MOVE_CTRL_H
#define __MOVE_CTRL_H
#include <stdint.h>

void yaw_ctrl(int16_t yaw_in, int16_t* yaw_out);
void yaw_ctrl_clear(void);

#endif

