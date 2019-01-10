#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "stm32f0xx.h"
#include "x_frame.h"

void bsp_uart_init(uint8_t uart_num);
int32_t bsp_uart_send       (uint8_t uart_port,uint8_t* pbuf,int32_t len);
int32_t bsp_uart_read        (uint8_t uart_port,uint8_t* p_dest,int32_t len);

#endif //__BSP_UART_H__
