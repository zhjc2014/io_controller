#ifndef __BSP_USB_INTERFACET_H__
#define __BSP_USB_INTERFACET_H__

#include "stm32f0xx.h"
#include "x_frame.h"

void bsp_usb_init(void);
int32_t bsp_hid_send(uint8_t* pbuf,int32_t len);
int32_t bsp_hid_read(uint8_t* p_dest,int32_t len);
int32_t bsp_vcp_send(uint8_t* pbuf,int32_t len);
int32_t bsp_vcp_read(uint8_t* p_dest,int32_t len);



#endif //__BSP_USB_INTERFACET_H__
