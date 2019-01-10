#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_flash.h"

#if ENABLE_FALSH_DRV
static int32_t drv_flash_init        (void)
{
    return 0;
}

int32_t drv_flash_write        (uint8_t* pSrc,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    uint32_t write_len = 0;
    write_len = bsp_flash_write(offset,pSrc,len);
    if(p_real!=NULL) {*p_real = write_len; }
    return 0;
}
int32_t drv_flash_read        (uint8_t* pSrc,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    uint32_t read_len = 0;
    read_len = bsp_flash_read(offset,pSrc,len);
    if(p_real!=NULL) {*p_real = read_len; }
    return 0;
}

int32_t drv_flash_ctrl (uint32_t ctrl_cmd ,uint8_t* pDest,uint32_t len ,uint32_t * real_c_bytes)
{
    DBG_I("drv_flash_ctrl ctrl_cmd:%d",ctrl_cmd);
    switch(ctrl_cmd)
    {
        case ERASE_FLASH_CMD:
        {
            uint32_t addr = (uint32_t)pDest;
            DBG_I("bsp_flash_erase add:0x%x  len:%d",addr,len);
            bsp_flash_erase(addr,len);
        }
        break;
        
        default: break;
    }
    
    return 0;
}
        
REG_DEV(    
            DRV_FALSH,                      /*driver id         */
            DEV_AUTO_INIT,                  /*driver auto init  */
            drv_flash_init ,                /*driver init func  */
            drv_flash_write,                /*driver write func */
            drv_flash_read,                 /*driver read  func */
            drv_flash_ctrl,                 /*driver ctrl  func */
            NULL                            /*driver close func */
        );  

#endif 

