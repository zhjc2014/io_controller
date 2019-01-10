#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_can.h"
#include "x_frame.h"  

#if ENABLE_CAN
static int32_t drv_can_init(void)
{
    bsp_can_init();
    return RET_OK;
}

int32_t drv_can_write(uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    return bsp_can_write((can_data_t*)(pSrc));
}

static int32_t drv_can_read(uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    int ret;
    ret = bsp_can_read((can_data_t*)(pSrc));
    if (p_real){
        *p_real = ret;
    }
    return ret;
}

int32_t drv_can_ctrl (uint32_t ctrl_cmd, uint8_t* pDest, uint32_t len, uint32_t* p_real)
{
    if (CAN_START_SEND == ctrl_cmd){
        bsp_can_start_send();
    }
    return RET_OK;
}

REG_DEV(
    DRV_CAN,                        /*driver id         */
    DEV_AUTO_INIT,                  /*driver auto init  */
    drv_can_init,                   /*driver init func  */
    drv_can_write,                  /*driver write func */
    drv_can_read,                   /*driver read  func */
    drv_can_ctrl,                   /*driver ctrl  func */
    NULL                            /*driver close func */
);
#endif

