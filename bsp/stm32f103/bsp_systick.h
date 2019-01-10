#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "stm32f10x.h"
#include "x_frame.h"

void bsp_sys_tick_init(void);
void bsp_write(uint32_t tick);
uint32_t bsp_read(void);

#endif //__BSP_UART_H__
