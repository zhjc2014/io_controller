#ifndef __HAL_CORE_H__
#define __HAL_CORE_H__

#include "platform_define.h"
#include "auto_reg_module.h"

typedef struct _dev_obj_ dev_obj_t;
typedef struct _dev_push_t dev_push_t;
typedef struct _hal_obj_t  hal_obj_t;

                                    
int32_t	hal_init(void);
void hal_dev_notify(uint32_t dev_id,uint32_t code,uint8_t* pbuf, uint32_t len);
hal_obj_t*	hal_get_handler(void);
                                    

#define ERR_DRV_ID              0xff
#define DRV_ID_SYS_TICK         0x0  //系统时钟是必须的一个驱动设备，必须注册
#define RET_OK  0

typedef struct
{
	uint32_t 	sys_second;
	uint32_t	sys_ms;
	uint32_t	sys_us;
	uint32_t 	sys_ms_tick;
}sys_time_t;

//enum defined in the usb_type
//#define TRUE  1
//#define FALSE 0

uint32_t is_big_endian(void)  ;

#define SWAP_U16(a)  ((a>>8)|((a & 0x00ff)<<8))
#define SWAP_U32(a)  ((a >> 24)  \
                    | ((a & 0x00ff0000) >> 8)  \
                    | ((a & 0x0000ff00) << 8)  \
                    | (a << 24)  )


#define BE_16(s)        is_big_endian() ? s : SWAP_U16(s)  
#define LE_16(s)        is_big_endian() ? SWAP_U16(s) : s  
#define BE_32(l)        is_big_endian() ? l : SWAP_U32(l)  
#define LE_32(l)        is_big_endian() ? SWAP_U32(l) : l  


#define SWAP_VAR_INT(x, y)\
            (x) = (x)^(y);\
            (y = (x)^(y);\
            (x = (x)^(y);



#endif //__HAL_CORE_H__
