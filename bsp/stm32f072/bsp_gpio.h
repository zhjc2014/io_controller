#ifndef __BSP_PWM_H__
#define __BSP_PWM_H__

#include "stm32f0xx.h"
#include "x_frame.h"

void bsp_gpio_init(void);
int32_t bsp_gpio_write        (uint8_t pwm_ch,uint32_t val);
uint32_t bsp_gpio_read(uint8_t ch);
#endif //__BSP_PWM_H__
