#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"
#include "bsp_dmp.h"

#if ENABLE_DMP_DEV

static int32_t drv_dmp_init    (void)
{
    return dmp_init(0);
}

static int32_t drv_dmp_write    (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    return 0;
}

static int32_t drv_dmp_read    (uint8_t* p_dest, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    int result;
    ST_DMP_DATA *pdmpData = (ST_DMP_DATA *)p_dest;
    result = read_dmp(pdmpData);    
    return result;
}


REG_DEV(
    DRV_MPU6500_DMP,                /*driver id         */
    DEV_AUTO_INIT,                  /*driver auto init  */
    drv_dmp_init,                   /*driver init func  */
    drv_dmp_write,                  /*driver write func */
    drv_dmp_read,                   /*driver read  func */
    NULL,                           /*driver ctrl  func */
    NULL                            /*driver close func */
);

#endif


