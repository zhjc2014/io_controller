#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_can.h"
#include "stm32f10x.h"  
#include "stm32f10x_can.h"         
#include "stm32f10x_gpio.h"
#include "x_frame.h"

#define MAX_DATA_LEN    8    
#define EN_FILTER       1

#define CAN_TX_BUF_SIZE CAN_FIFO_SIZE
#define CAN_RX_BUF_SIZE CAN_FIFO_SIZE

CanTxMsg can_tx_buf[CAN_TX_BUF_SIZE];
CanRxMsg can_rx_buf[CAN_RX_BUF_SIZE];

static kfifo2_t can_tx_fifo = {"can_tx_fifo", CAN_TX_BUF_SIZE, sizeof(CanTxMsg), (uint8_t*)can_tx_buf, 0, 0};
static kfifo2_t can_rx_fifo = {"can_rx_fifo", CAN_RX_BUF_SIZE, sizeof(CanRxMsg), (uint8_t*)can_rx_buf, 0, 0};

#if EN_FILTER

//static uint16_t default_id = IO_BOARD_ADDR + 0x00;

static CAN_FilterInitTypeDef can_rxfilter = {0x0000, 0x0000, 0xFFFF, 0xFFFF, CAN_Filter_FIFO0, 0, CAN_FilterMode_IdMask, CAN_FilterScale_32bit, ENABLE};

//const static uint8_t can_rxfilter_len = ARRY_ITEMS_NUM(can_rxfilter);
#endif


void bsp_can_set_filter(uint32_t id);

void can_nvic_cfg (void)  
{  
    NVIC_InitTypeDef NVIC_InitStructure;      
    /* enabling interrupt */     
    NVIC_InitStructure.NVIC_IRQChannel=USB_LP_CAN1_RX0_IRQn;     
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;     
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     
    NVIC_Init(&NVIC_InitStructure); 

    NVIC_InitStructure.NVIC_IRQChannel=USB_HP_CAN1_TX_IRQn;     
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;     
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;     
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     
    NVIC_Init(&NVIC_InitStructure); 
}

void bsp_can_init(void)
{
    CAN_InitTypeDef CAN_InitStructure;
    
    GPIO_InitTypeDef GPIO_InitStruct;
    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);
    //RCC enable
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    //mode setting
    CAN_InitStructure.CAN_ABOM = ENABLE;                       
    CAN_InitStructure.CAN_AWUM = DISABLE;                       //软件唤醒
    CAN_InitStructure.CAN_TTCM = DISABLE;                       //禁止时间触发通信模式
    CAN_InitStructure.CAN_NART = ENABLE;                        //禁止自动重传
    CAN_InitStructure.CAN_TXFP = DISABLE;                       //优先级由报文的标识符来决定
    CAN_InitStructure.CAN_RFLM = DISABLE;                       //接受溢出时FIFO不锁定，下一个收到的报文覆盖原有报文
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;               //CAN硬件工作Normal模式
//    CAN_InitStructure.CAN_Mode = CAN_Mode_LoopBack;             //CAN硬件工作Normal模式
    //1M bps
    CAN_InitStructure.CAN_Prescaler = 4;
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
//    CAN_InitStructure.CAN_BS1 = CAN_BS1_6tq;
//    CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;
    
    CAN_Init(CAN1, &CAN_InitStructure);
    //filter config
    //http://blog.csdn.net/flydream0/article/details/8148791 
    //GPIO init
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
//    GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);

    //can interrupt config
    can_nvic_cfg();
    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
//    CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);
}

void bsp_can_deinit(void)
{
    CAN_DeInit(CAN1);
}

int bsp_can_read(can_data_t *can_rxdata)
{
    int i;
    int len = kfifo2_have_len(&can_rx_fifo);
//    DBG_I("have len is %d", len);
    for (i = 0; i < len; i++){
        int j;
        CanRxMsg can_rxmsg;
        kfifo2_pull_out(&can_rx_fifo, (uint8_t*)&can_rxmsg, 1);
        can_rxdata[i].id = can_rxmsg.StdId;
        can_rxdata[i].len = can_rxmsg.DLC;
//        memcpy(can_rxdata[i].data, can_rxmsg.Data, can_rxdata[i].len);
        for (j = 0; j < can_rxdata[i].len; j++){
            can_rxdata[i].data[j] = can_rxmsg.Data[j];
        }
    }
    return len;
}

int  bsp_can_write(can_data_t *can_txdata)
{
    int len;
    CanTxMsg can_txmsg;
    can_txmsg.StdId = can_txdata->id;
    can_txmsg.IDE = CAN_Id_Standard;
    can_txmsg.RTR = CAN_RTR_Data;
    can_txmsg.DLC = can_txdata->len;
    memcpy(can_txmsg.Data, can_txdata->data , can_txdata->len);
    kfifo2_push_in(&can_tx_fifo, (uint8_t*)&can_txmsg, 1);
    len = kfifo2_have_len(&can_tx_fifo); 
    return len;
}

void bsp_can_start_send(void)
{
    CanTxMsg can_txmsg;
    int len =  kfifo2_have_len(&can_tx_fifo);
    if (len){
        kfifo2_pull_out(&can_tx_fifo, (uint8_t*)&can_txmsg, 1);
        CAN_Transmit(CAN1, &can_txmsg);
        CAN_ITConfig(CAN1, CAN_IT_TME, ENABLE);    
    }
}

void bsp_can_set_filter(uint32_t id)
{
#if EN_FILTER
    can_rxfilter.CAN_FilterIdHigh = id << 5;
    can_rxfilter.CAN_FilterIdLow  = 0 | CAN_ID_STD;
    CAN_FilterInit(&can_rxfilter);
#endif
}


void USB_LP_CAN1_RX0_IRQHandler(void)  
{  
    ITStatus Status;    
    Status = CAN_GetITStatus(CAN1, CAN_IT_FMP0);         //判断接受到过滤器中断信号
    if(Status == SET){
        CanRxMsg can_rxmsg;
        CAN_Receive(CAN1, CAN_Filter_FIFO0, &can_rxmsg);
        kfifo2_push_in(&can_rx_fifo, (uint8_t*)&can_rxmsg, 1);
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    }
}

void USB_HP_CAN1_TX_IRQHandler(void)  
{  
    ITStatus Status;    
    Status = CAN_GetITStatus(CAN1, CAN_IT_TME);         //判断接受到过滤器中断信号
    if(Status == SET){
        int len;
        len =  kfifo2_have_len(&can_tx_fifo);
        if (len){
            CanTxMsg can_txmsg;
            kfifo2_pull_out(&can_tx_fifo, (uint8_t*)&can_txmsg, 1);
            CAN_Transmit(CAN1, &can_txmsg);
        }
        CAN_ClearITPendingBit(CAN1, CAN_IT_TME);
        if (len <= 1){
            CAN_ITConfig(CAN1, CAN_IT_TME, DISABLE);
        }
    }
}


