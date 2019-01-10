#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "stm32f0xx.h"
#include "x_frame.h"

int32_t bsp_flash_write       (uint32_t addr,uint8_t* pbuf,int32_t len);
int32_t bsp_flash_read		  (uint32_t addr,uint8_t* p_dest,int32_t len);
int32_t bsp_flash_erase		  (uint32_t address,uint32_t len);

#endif //__BSP_FLASH_H__
