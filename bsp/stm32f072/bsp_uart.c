#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_uart.h"




typedef struct _uart_obj
{
    USART_TypeDef*	        uart_port;
    uint32_t                uart_rcc;
	uint32_t				uart_def_baud;
    IRQn_Type               uart_irq_src;
    uint8_t                 irq_prio;
    
	GPIO_TypeDef*		    uart_pin_port;
    uint32_t                uart_pin_rcc;
    uint16_t				uart_tx_pin;
    uint8_t                 tx_pin_source;
	uint16_t				uart_rx_pin;
    uint8_t                 rx_pin_source;
    
    kfifo_t				    uart_tx_fifo;
	kfifo_t				    uart_rx_fifo;
    
    volatile uint8_t        flag_is_in_sending;
	uint8_t 				send_char;
	uint8_t 				recv_char;

}uart_obj_t;

#define ADD_UART_ITEM(uart,rcc_uart,baud,irq_prio,port,tx_pin,rx_pin,tx_size,tx_pbuf,rx_size,rx_pbuf) \
            {uart,RCC_##rcc_uart##Periph_##uart,baud,uart##_IRQn,irq_prio,port,RCC_AHBPeriph_##port,GPIO_Pin_##tx_pin,GPIO_PinSource##tx_pin,GPIO_Pin_##rx_pin,GPIO_PinSource##rx_pin,\
            {NULL,tx_size,tx_pbuf,0,0},\
            {NULL,rx_size,rx_pbuf,0,0},\
            0,0,0}
           

#define UART1_TX_SIZE  1024
#define UART1_RX_SIZE  256
static uint8_t u1_tx_buf[UART1_TX_SIZE] = {0};        
static uint8_t u1_rx_buf[UART1_RX_SIZE] = {0};        
static uint8_t u2_tx_buf[UART1_TX_SIZE] = {0};        
static uint8_t u2_rx_buf[UART1_RX_SIZE] = {0};    

#define USART4_IRQn  USART3_4_IRQn
            
uart_obj_t uart_tab[]=
{
    ADD_UART_ITEM(USART1,APB2,DBG_BAUD,2,GPIOA,9,10,UART1_TX_SIZE,u1_tx_buf,UART1_RX_SIZE,u1_rx_buf ),
    ADD_UART_ITEM(USART2,APB1,115200,3,GPIOA,2,3 ,UART1_TX_SIZE,u2_tx_buf,UART1_RX_SIZE,u2_rx_buf ),
};

;

            
#define UART_OBJ uart_tab[uart_num]            

void bsp_uart_init(uint8_t uart_num)
{
    GPIO_InitTypeDef                                GPIO_InitStructure;//定义一个GPIO_InitTypeDef类型的结构体
    USART_InitTypeDef                               USART_InitStructure;
    NVIC_InitTypeDef                                NVIC_InitStructure;//定义一个NVIC_InitTypeDef类型的结构体
   
    if(uart_num>=ARRY_ITEMS_NUM(uart_tab)){DBG_E("bsp_uart_init error! uart_num:%d the max:%d",uart_num,ARRY_ITEMS_NUM(uart_tab));}
   
    GPIO_InitStructure.GPIO_Pin                     = UART_OBJ.uart_tx_pin | UART_OBJ.uart_rx_pin;//选择要控制的GPIOA引脚
    GPIO_InitStructure.GPIO_Mode                    = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed                   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType                   = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd                    = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL;

	USART_InitStructure.USART_BaudRate              = UART_OBJ.uart_def_baud;
	USART_InitStructure.USART_WordLength            = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits              = USART_StopBits_1;
	USART_InitStructure.USART_Parity                = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                  = USART_Mode_Rx | USART_Mode_Tx;

	NVIC_InitStructure.NVIC_IRQChannel              = UART_OBJ.uart_irq_src;
	NVIC_InitStructure.NVIC_IRQChannelPriority      = UART_OBJ.irq_prio;
	NVIC_InitStructure.NVIC_IRQChannelCmd           = ENABLE;

    if(UART_OBJ.uart_port == USART1)
    {
        RCC_APB2PeriphClockCmd(UART_OBJ.uart_rcc    , ENABLE);
    }
    else
    {
        RCC_APB1PeriphClockCmd(UART_OBJ.uart_rcc    , ENABLE);
    }
    
    RCC_AHBPeriphClockCmd(UART_OBJ.uart_pin_rcc, ENABLE);
    GPIO_Init(UART_OBJ.uart_pin_port, &GPIO_InitStructure); 
    
    if(UART_OBJ.uart_port == USART4)
    {
        GPIO_PinAFConfig(UART_OBJ.uart_pin_port, UART_OBJ.tx_pin_source, GPIO_AF_4);  
        GPIO_PinAFConfig(UART_OBJ.uart_pin_port, UART_OBJ.rx_pin_source, GPIO_AF_4);  
    }
    else
    {
        GPIO_PinAFConfig(UART_OBJ.uart_pin_port, UART_OBJ.tx_pin_source, GPIO_AF_1);  
        GPIO_PinAFConfig(UART_OBJ.uart_pin_port, UART_OBJ.rx_pin_source, GPIO_AF_1);  
    }
    
	NVIC_Init(&NVIC_InitStructure);
    
	USART_Init      (UART_OBJ.uart_port, &USART_InitStructure);
	USART_ITConfig  (UART_OBJ.uart_port, USART_IT_RXNE, ENABLE);
    USART_ITConfig  (UART_OBJ.uart_port, USART_IT_PE  , ENABLE); //-----------
    USART_ITConfig  (UART_OBJ.uart_port, USART_IT_ERR , ENABLE); //-------------
    
	USART_Cmd(UART_OBJ.uart_port, ENABLE);
}

int32_t bsp_uart_send(uint8_t uart_num,uint8_t* pbuf,int32_t len)
{
    int32_t real_len = 0;
	if(len==0){return 0;}
    if(uart_num>=ARRY_ITEMS_NUM(uart_tab)){DBG_E("bsp_uart_init error! uart_num:%d the max:%d",uart_num,ARRY_ITEMS_NUM(uart_tab));}

    real_len = kfifo_push_in(&UART_OBJ.uart_tx_fifo,pbuf,len);

    if(UART_OBJ.flag_is_in_sending==0){UART_OBJ.flag_is_in_sending=1;}
    
    USART_ITConfig(UART_OBJ.uart_port, USART_IT_TXE, ENABLE);
	return real_len;
}

int32_t bsp_uart_read		(uint8_t uart_num,uint8_t* p_dest,int32_t len	)
{
	int32_t real_len = 0;
    if(uart_num>=ARRY_ITEMS_NUM(uart_tab)){DBG_E("bsp_uart_init error! uart_num:%d the max:%d",uart_num,ARRY_ITEMS_NUM(uart_tab));}
	real_len = kfifo_pull_out(&UART_OBJ.uart_rx_fifo,p_dest,len);
	return real_len;
}

void uart_irq_action(uint8_t uart_num)
{
    if( USART_GetITStatus(UART_OBJ.uart_port, USART_IT_TXE) != RESET)
    {
        if(kfifo_pull_out(&UART_OBJ.uart_tx_fifo,&UART_OBJ.send_char,1))
        {
            USART_SendData(UART_OBJ.uart_port, UART_OBJ.send_char);				
        }
        else
        {
            UART_OBJ.flag_is_in_sending=0;
            USART_ITConfig(UART_OBJ.uart_port, USART_IT_TXE, DISABLE);
        }
    }
	
	if(USART_GetITStatus(UART_OBJ.uart_port, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(UART_OBJ.uart_port, USART_IT_RXNE);
		UART_OBJ.recv_char = USART_ReceiveData(UART_OBJ.uart_port);
		kfifo_push_in(&UART_OBJ.uart_rx_fifo,&UART_OBJ.recv_char,1);
	}

	if (USART_GetFlagStatus(UART_OBJ.uart_port, USART_FLAG_ORE) == SET)  
	{  
	    //USART_ClearITPendingBit(g_uart_init_tab[uart_port].uart_port,USART_IT_ORE);  
//        UART_OBJ.recv_char = USART_ReceiveData(UART_OBJ.uart_port);
//		kfifo_push_in(&UART_OBJ.uart_rx_fifo,&UART_OBJ.recv_char,1);
		USART_ClearFlag(UART_OBJ.uart_port,USART_FLAG_ORE);  //读SR
	}
    
    if(USART_GetFlagStatus(UART_OBJ.uart_port, USART_FLAG_PE) != RESET)
    {//?  @arg USART_IT_PE     : Parity Error interrupt
        USART_ClearFlag(UART_OBJ.uart_port, USART_FLAG_PE);
    }
    
    if(USART_GetFlagStatus(UART_OBJ.uart_port, USART_FLAG_FE) != RESET)
    {//?  @arg USART_IT_PE     : Parity Error interrupt
        USART_ClearFlag(UART_OBJ.uart_port, USART_FLAG_FE);
    }
    
    if(USART_GetFlagStatus(UART_OBJ.uart_port, USART_FLAG_NE) != RESET)
    {//?  @arg USART_IT_NE     : Noise Error interrupt
        USART_ClearFlag(UART_OBJ.uart_port, USART_FLAG_NE);
    }
    
}

void USART1_IRQHandler(void)
{
    uart_irq_action(0);
}

void USART2_IRQHandler(void)
{
    uart_irq_action(1);
}

//void USART3_4_IRQHandler(void)
//{
//    uart_irq_action(1);
//}

