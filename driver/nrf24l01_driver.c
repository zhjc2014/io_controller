#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "drv_RF24L01.h"
#include "drv_delay.h"
#include "x_frame.h"


#if ENABLE_RF24L01_0_DRV || \
    ENABLE_RF24L01_1_DRV || \
    ENABLE_RF24L01_2_DRV || \
    ENABLE_RF24L01_3_DRV || \
    ENABLE_RF24L01_4_DRV || \
    ENABLE_RF24L01_5_DRV
    
extern uint32_t rf_init_flag;

volatile static int tx_flag = 0;

static int32_t drv_rf24l01_init        (void)
{
    static int init = 0;
    if (!init){
        init = 1;
        drv_delay_init( );
        drv_spi_init( );
        NRF24L01_Gpio_Init( );
//        NRF24L01_check( );    
        RF24L01_Init( );
//        RF24L01_Set_Mode( MODE_RX );  
//        DBG_I("%s", __FUNCTION__);
    }
    return 0;
}

#if ENABLE_RF24L01_0_DRV

static int32_t drv_rf24l01_0_write        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    int32_t result = 0;
    tx_flag = 1;
    RF24L01_Set_Mode( MODE_TX );
    result = NRF24L01_TxPacket_Channel(0, pSrc, len);
    #if EN_ACK_WAIT
    if (result == TX_OK)
    #else
    if (result == TX_OK \
        || result == MAX_TX)
    #endif    
    {
        if (p_real){
            *p_real = len;
        }
        result = 0;
    }else{
        result = -1;
        if (p_real){
            *p_real = 0;
        }
    }
    RF24L01_Set_Mode( MODE_RX );
    tx_flag = 0;
    return result;
}
static int32_t drv_rf24l01_0_read        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    uint32_t recLen;
    recLen = get_nrf24l01_data(pSrc, 0);
    if (p_real){
        *p_real = recLen;
    }
    return 0;
}
static int32_t drv_rf24l01_0_ctrl        (uint32_t ctrl_cmd, uint8_t* pDest, uint32_t len, uint32_t* p_real)
{
    if (SET_NRF_FREQ == ctrl_cmd){
        uint8_t channel;
        channel = *pDest;
        if (channel > 127) {
            return -1;
        }
        NRF24L01_Write_Reg( FLUSH_RX,0xff );    //flush rx fifo
        RF24LL01_Write_Hopping_Point(channel);
    }else if (SET_NRF_RXMODE == ctrl_cmd){
        RF24L01_Set_Mode( MODE_RX );  
//        DBG_I("set nrf24l01 rx mode");
    }else if (FLUSH_RX_BUFF == ctrl_cmd){
        NRF24L01_Write_Reg( FLUSH_RX,0xff );    //flush rx fifo
    }
    return 0;
}

REG_DEV(
    DRV_RF24L01_0,                      /*driver id         */
    DEV_AUTO_INIT,                      /*driver auto init  */
    drv_rf24l01_init,                   /*driver init func  */
    drv_rf24l01_0_write,                /*driver write func */
    drv_rf24l01_0_read,                 /*driver read  func */
    drv_rf24l01_0_ctrl,                 /*driver ctrl  func */
    NULL                                /*driver close func */
);

#endif

#if ENABLE_RF24L01_1_DRV

static int32_t drv_rf24l01_1_write        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    int32_t result = 0;
    tx_flag = 1;
    RF24L01_Set_Mode( MODE_TX );
    result = NRF24L01_TxPacket_Channel(1, pSrc, len);
    #if EN_ACK_WAIT
    if (result == TX_OK)
    #else
    if (result == TX_OK \
        || result == MAX_TX)
    #endif    
    {
        if (p_real){
            *p_real = len;
        }
        result = 0;
    }else{
        result = -1;
        if (p_real){
            *p_real = 0;
        }
    }
    RF24L01_Set_Mode( MODE_RX );
    tx_flag = 0;
    return result;
}
static int32_t drv_rf24l01_1_read        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    uint32_t recLen;
    recLen = get_nrf24l01_data(pSrc, 1);
    if (p_real){
        *p_real = recLen;
    }
    return 0;
}
static int32_t drv_rf24l01_1_ctrl        (uint32_t ctrl_cmd, uint8_t* pDest, uint32_t len, uint32_t* p_real)
{
    return 0;
}

REG_DEV(
    DRV_RF24L01_1,                      /*driver id         */
    DEV_AUTO_INIT,                      /*driver auto init  */
    drv_rf24l01_init,                   /*driver init func  */
    drv_rf24l01_1_write,                /*driver write func */
    drv_rf24l01_1_read,                 /*driver read  func */
    drv_rf24l01_1_ctrl,                 /*driver ctrl  func */
    NULL                                /*driver close func */
);

#endif


#if ENABLE_RF24L01_2_DRV

static int32_t drv_rf24l01_2_write        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    int32_t result = 0;
    tx_flag = 1;
    RF24L01_Set_Mode( MODE_TX );
    result = NRF24L01_TxPacket_Channel(2, pSrc, len);
    #if EN_ACK_WAIT
    if (result == TX_OK)
    #else
    if (result == TX_OK \
        || result == MAX_TX)
    #endif    
    {
        if (p_real){
            *p_real = len;
        }
        result = 0;
    }else{
        result = -1;
        if (p_real){
            *p_real = 0;
        }
    }
    RF24L01_Set_Mode( MODE_RX );
    tx_flag = 0;
    return result;
}
static int32_t drv_rf24l01_2_read        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    uint32_t recLen;
    recLen = get_nrf24l01_data(pSrc, 2);
    if (p_real){
        *p_real = recLen;
    }
    return 0;
}
static int32_t drv_rf24l01_2_ctrl        (uint32_t ctrl_cmd, uint8_t* pDest, uint32_t len, uint32_t* p_real)
{
    return 0;
}

REG_DEV(
    DRV_RF24L01_2,                      /*driver id         */
    DEV_AUTO_INIT,                      /*driver auto init  */
    drv_rf24l01_init,                   /*driver init func  */
    drv_rf24l01_2_write,                /*driver write func */
    drv_rf24l01_2_read,                 /*driver read  func */
    drv_rf24l01_2_ctrl,                 /*driver ctrl  func */
    NULL                                /*driver close func */
);

#endif


#if ENABLE_RF24L01_3_DRV

static int32_t drv_rf24l01_3_write        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    int32_t result = 0;
    tx_flag = 1;
    RF24L01_Set_Mode( MODE_TX );
    result = NRF24L01_TxPacket_Channel(3, pSrc, len);
    #if EN_ACK_WAIT
    if (result == TX_OK)
    #else
    if (result == TX_OK \
        || result == MAX_TX)
    #endif    
    {
        if (p_real){
            *p_real = len;
        }
        result = 0;
    }else{
        result = -1;
        if (p_real){
            *p_real = 0;
        }
    }
    RF24L01_Set_Mode( MODE_RX );
    tx_flag = 0;
    return result;
}
static int32_t drv_rf24l01_3_read        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    uint32_t recLen;
    recLen = get_nrf24l01_data(pSrc, 3);
    if (p_real){
        *p_real = recLen;
    }
    return 0;
}
static int32_t drv_rf24l01_3_ctrl        (uint32_t ctrl_cmd, uint8_t* pDest, uint32_t len, uint32_t* p_real)
{
    return 0;
}

REG_DEV(
    DRV_RF24L01_3,                      /*driver id         */
    DEV_AUTO_INIT,                      /*driver auto init  */
    drv_rf24l01_init,                   /*driver init func  */
    drv_rf24l01_3_write,                /*driver write func */
    drv_rf24l01_3_read,                 /*driver read  func */
    drv_rf24l01_3_ctrl,                 /*driver ctrl  func */
    NULL                                /*driver close func */
);

#endif

#if ENABLE_RF24L01_4_DRV

static int32_t drv_rf24l01_4_write        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    int32_t result = 0;
    tx_flag = 1;
    RF24L01_Set_Mode( MODE_TX );
    result = NRF24L01_TxPacket_Channel(4, pSrc, len);
    #if EN_ACK_WAIT
    if (result == TX_OK)
    #else
    if (result == TX_OK \
        || result == MAX_TX)
    #endif    
    {
        if (p_real){
            *p_real = len;
        }
        result = 0;
    }else{
        result = -1;
        if (p_real){
            *p_real = 0;
        }
    }
    RF24L01_Set_Mode( MODE_RX );
    tx_flag = 0;
    return result;
}
static int32_t drv_rf24l01_4_read        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    uint32_t recLen;
    recLen = get_nrf24l01_data(pSrc, 4);
    if (p_real){
        *p_real = recLen;
    }
    return 0;
}
static int32_t drv_rf24l01_4_ctrl        (uint32_t ctrl_cmd, uint8_t* pDest, uint32_t len, uint32_t* p_real)
{
    return 0;
}

REG_DEV(
    DRV_RF24L01_4,                      /*driver id         */
    DEV_AUTO_INIT,                      /*driver auto init  */
    drv_rf24l01_init,                   /*driver init func  */
    drv_rf24l01_4_write,                /*driver write func */
    drv_rf24l01_4_read,                 /*driver read  func */
    drv_rf24l01_4_ctrl,                 /*driver ctrl  func */
    NULL                                /*driver close func */
);

#endif

#if ENABLE_RF24L01_5_DRV

static int32_t drv_rf24l01_5_write        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    int32_t result = 0;
    tx_flag = 1;
    RF24L01_Set_Mode( MODE_TX );
    result = NRF24L01_TxPacket_Channel(5, pSrc, len);
    #if EN_ACK_WAIT
    if (result == TX_OK)
    #else
    if (result == TX_OK \
        || result == MAX_TX)
    #endif    
    {
        if (p_real){
            *p_real = len;
        }
        result = 0;
    }else{
        result = -1;
        if (p_real){
            *p_real = 0;
        }
    }
    RF24L01_Set_Mode( MODE_RX );
    tx_flag = 0;
    return result;
}
static int32_t drv_rf24l01_5_read        (uint8_t* pSrc, uint32_t len    , uint32_t offset, uint32_t* p_real)
{
    uint32_t recLen;
    recLen = get_nrf24l01_data(pSrc, 5);
    if (p_real){
        *p_real = recLen;
    }
    return 0;
}
static int32_t drv_rf24l01_5_ctrl        (uint32_t ctrl_cmd, uint8_t* pDest, uint32_t len, uint32_t* p_real)
{
    return 0;
}

REG_DEV(
    DRV_RF24L01_5,                      /*driver id         */
    DEV_AUTO_INIT,                      /*driver auto init  */
    drv_rf24l01_init,                   /*driver init func  */
    drv_rf24l01_5_write,                /*driver write func */
    drv_rf24l01_5_read,                 /*driver read  func */
    drv_rf24l01_5_ctrl,                 /*driver ctrl  func */
    NULL                                /*driver close func */
);

#endif

static uint32_t irq_counter = 0;
static void irq_check(void)
{
    irq_counter++;
    if (irq_counter > 300){    //no irq for 3 second
        RF24L01_Init( );    //reinit the rf24l01 module
        rf_init_flag = 0;
        irq_counter = 0;
//        DBG_I("rf_reinit");
    }
}
REG_TASK(ID_TASK, 10, irq_check);


void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line1) != RESET) {
        irq_counter = 0;    //clear the counter
        if (!tx_flag){
            #if EN_ACK_WAIT
//            drv_delay_ms(1);
            #endif
            NRF24L01_RxPacket_int();
        }
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

#endif


