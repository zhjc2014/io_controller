#ifndef __BSP_IWDG_H__
#define __BSP_IWDG_H__

#include <stdint.h>

void bsp_iwdg_init(void);
void bsp_iwdg_write(void);
int bsp_iwdg_read(uint16_t IWDG_FLAG);

#endif //__BSP_PWM_H__

