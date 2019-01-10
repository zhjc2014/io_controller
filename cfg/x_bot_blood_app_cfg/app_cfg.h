#ifndef __APP_CFG_H__
#define __APP_CFG_H__


#define DEFAULT_DEBUG_DEV   DRV_RTT//DRV_USB_VCP//DRV_RTT//DRV_UART1
#define DBG_BAUD            115200


#define 			PC_LINK_ID           1        					 
#define 			PC_LINK_DEV          DRV_RTT//DRIVER_USB_VCP//DRIVER_UART_2    
#define 			PC_LINK_PROTOCOL     PROTOCOL_X_BOT 
#define 			PC_LINK_PRIO         0 //0 very hi  


#define 			SUB_LINK_ID          2        					 
#define 			SUB_LINK_DEV         DRV_UART2//DRIVER_USB_VCP//DRIVER_UART_2    
#define 			SUB_LINK_PROTOCOL    PROTOCOL_X_BOT 
#define 			SUB_LINK_PRIO        0 //0 very hi  


#endif //__APP_CFG_H__
