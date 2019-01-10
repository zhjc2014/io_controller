#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_iwdg.h"
#include "x_frame.h"


#if ENABLE_IWDG_DRV
int32_t drv_iwdg_init   (void)
{
    bsp_iwdg_init();
    return 0;
}


int32_t drv_iwdg_write  (uint8_t* pSrc,uint32_t len	,uint32_t offset,uint32_t* p_real)
{
    bsp_iwdg_write();
    return 0;
}

int32_t drv_iwdg_read   (uint8_t* dest,uint32_t len	,uint32_t offset,uint32_t* p_real)
{
    int ret;
    ret = bsp_iwdg_read(offset);
    *(uint32_t*)dest = ret;
    return 0;
}  

REG_DEV(    
            DRV_IWDG,                        /*driver id         */
            DEV_AUTO_INIT,                   /*driver auto init  */
            drv_iwdg_init,                   /*driver init func  */
            drv_iwdg_write,                  /*driver write func */
            drv_iwdg_read,                   /*driver read  func */
            NULL,                           /*driver ctrl  func */
            NULL                            /*driver close func */
        );  
#endif 
