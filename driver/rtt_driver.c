#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "SEGGER_RTT.h"
#include "x_frame.h"  

#if ENABLE_RTT_DRV

static int32_t drv_rtt_init        (void)
{
    SEGGER_RTT_Init();
    return 0;
}

static int32_t drv_rtt_write    (uint8_t* pSrc,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    uint32_t write_len = 0;
    write_len =   SEGGER_RTT_Write(0,(char*)pSrc,len);
    if(p_real!=NULL) {*p_real = write_len; }
    return 0;
}

static int32_t drv_rtt_read    (uint8_t* p_dest,uint32_t len    ,uint32_t offset,uint32_t* p_real)
{
    uint32_t read_len = 0;
    read_len =   SEGGER_RTT_Read(0,(char*)p_dest,len);
    if(p_real!=NULL) {*p_real = read_len; }
    return 0;
}

int fputc(int ch, FILE *f)
{
    char data = ch;    
    SEGGER_RTT_Write(0, &data, 1);
    return (int)data;
}

REG_DEV(   
            DRV_RTT,                        /*driver id         */
            DEV_AUTO_INIT,                  /*driver auto init  */
            drv_rtt_init,                   /*driver init func  */
            drv_rtt_write,                  /*driver write func */
            drv_rtt_read,                   /*driver read  func */
            NULL,                           /*driver ctrl  func */
            NULL                            /*driver close func */
        );  

#endif


