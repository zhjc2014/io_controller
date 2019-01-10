#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "bsp_uart.h"

#if ENABLE_UART_1_DRV
static int32_t drv_uart1_init		(void)
{
    bsp_uart_init(0);
    return 0;
}

int32_t drv_uart1_write		(uint8_t* pSrc, uint32_t len	, uint32_t offset, uint32_t* p_real)
{
    uint32_t write_len = 0;
    write_len = bsp_uart_send(0, pSrc, len);
    if(p_real != NULL) {
        *p_real = write_len;
    }
    return 0;
}
int32_t drv_uart1_read		(uint8_t* pSrc, uint32_t len	, uint32_t offset, uint32_t* p_real)
{
    uint32_t read_len = 0;
    read_len = bsp_uart_read(0, pSrc, len);
    if(p_real != NULL) {
        *p_real = read_len;
    }
    return 0;
}
int32_t drv_uart1_ctrl		(uint32_t ctrl_cmd, uint8_t* pDest, uint32_t len, uint32_t* p_real)
{
    return 0;
}


REG_DEV(
    DRV_UART1,                      /*driver id         */
    DEV_AUTO_INIT,                  /*driver auto init  */
    drv_uart1_init,                 /*driver init func  */
    drv_uart1_write,                /*driver write func */
    drv_uart1_read,                 /*driver read  func */
    NULL,                           /*driver ctrl  func */
    NULL                            /*driver close func */
);
#endif

#if ENABLE_UART_2_DRV
static int32_t drv_uart2_init		(void)
{
    bsp_uart_init(1);
    return 0;
}

int32_t drv_uart2_write		(uint8_t* pSrc, uint32_t len	, uint32_t offset, uint32_t* p_real)
{
    uint32_t write_len = 0;
    write_len = bsp_uart_send(1, pSrc, len);
    if(p_real != NULL) {
        *p_real = write_len;
    }
    return 0;
}
int32_t drv_uart2_read		(uint8_t* pSrc, uint32_t len	, uint32_t offset, uint32_t* p_real)
{
    uint32_t read_len = 0;
    read_len = bsp_uart_read(1, pSrc, len);
    if(p_real != NULL) {
        *p_real = read_len;
    }
    return 0;
}
int32_t drv_uart2_ctrl		(uint32_t ctrl_cmd, uint8_t* pDest, uint32_t len, uint32_t* p_real)
{
    return 0;
}

REG_DEV(
    DRV_UART2,                      /*driver id         */
    DEV_AUTO_INIT,                  /*driver auto init  */
    drv_uart2_init,                 /*driver init func  */
    drv_uart2_write,                /*driver write func */
    drv_uart2_read,                 /*driver read  func */
    NULL,                           /*driver ctrl  func */
    NULL                            /*driver close func */
);
#endif


#if ENABLE_UART_3_DRV
static int32_t drv_uart3_init		(void)
{
    bsp_uart_init(2);
    return 0;
}

int32_t drv_uart3_write		(uint8_t* pSrc, uint32_t len	, uint32_t offset, uint32_t* p_real)
{
    uint32_t write_len = 0;
    write_len = bsp_uart_send(2, pSrc, len);
    if(p_real != NULL) {
        *p_real = write_len;
    }
    return 0;
}
int32_t drv_uart3_read		(uint8_t* pSrc, uint32_t len	, uint32_t offset, uint32_t* p_real)
{
    uint32_t read_len = 0;
    read_len = bsp_uart_read(2, pSrc, len);
    if(p_real != NULL) {
        *p_real = read_len;
    }
    return 0;
}
int32_t drv_uart3_ctrl		(uint32_t ctrl_cmd, uint8_t* pDest, uint32_t len, uint32_t* p_real)
{
    return 0;
}

REG_DEV(
    DRV_UART3,                      /*driver id         */
    DEV_AUTO_INIT,                  /*driver auto init  */
    drv_uart3_init,                 /*driver init func  */
    drv_uart3_write,                /*driver write func */
    drv_uart3_read,                 /*driver read  func */
    NULL,                           /*driver ctrl  func */
    NULL                            /*driver close func */
);
#endif
