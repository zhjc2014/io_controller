#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "x_frame.h"
#include "bsp_systick.h"

static int32_t timer_init		(void)
{
	bsp_sys_tick_init();
	return RET_OK;
}

static int32_t drv_sys_tick_write   	(uint8_t* pSrc, uint32_t len	, uint32_t offset, uint32_t* p_real) 
{
    bsp_write(*(uint32_t *)pSrc);
    return 0;
}

static int32_t drv_sys_tick_read		(uint8_t* pSrc, uint32_t len	, uint32_t offset, uint32_t* p_real) 
{
    *(uint32_t *)pSrc = bsp_read();
    return 0;
}

REG_DEV(    
            DRV_SYSTICK,                /*driver id         */
            DEV_AUTO_INIT,              /*driver auto init  */
            timer_init,                 /*driver init func  */
            drv_sys_tick_write,         /*driver write func */
            drv_sys_tick_read,          /*driver read  func */
            NULL,                       /*driver ctrl  func */
            NULL                        /*driver close func */
        );  








