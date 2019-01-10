#ifndef __HAL_APP_INTERFACE_H__
#define __HAL_APP_INTERFACE_H__

#include "hal_core.h"

int32_t  hal_app_write	        (uint32_t dev_handle,uint8_t* pbuf, uint32_t len, uint32_t offset,uint32_t* p_real_bytes);
int32_t  hal_app_read		    (uint32_t dev_handle,uint8_t* pbuf, uint32_t len, uint32_t offset,uint32_t* p_real_bytes);
int32_t  hal_app_ctrl		    (uint32_t dev_handle,uint32_t ctrl_cmd,uint8_t* pDest,uint32_t len,uint32_t* p_real_bytes);

#endif //__HAL_APP_INTERFACE_H__
