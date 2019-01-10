#ifndef __BSP_DRIVER_USB_H__
#define __BSP_DRIVER_USB_H__


#include "platform_var.h"


void USB_Cable_Config (uint8_t NewState);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void Get_SerialNum(void);

int32_t usb_push_data_to_rx_fifo(uint8_t* pbuf,uint16_t len);


//----给hal 驱动调用的

void bsp_usb_init(void);
int32_t bsp_usb_write	(uint8_t* pSrc,uint32_t len);
int32_t bsp_usb_read	(uint8_t* pDest,uint32_t len);
void usb_run_ontick(void);
#endif//__BSP_DRIVER_USB_H__
