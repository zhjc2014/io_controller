#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_uart.h"

typedef struct {
    //uart-clk      tx-pin  rx-pin  def_baud irq_num  tx-buf   rx-buf
    USART_TypeDef*    uart_port;
    uint32_t        uart_clk;
    uint32_t        uart_def_baud;
    uint16_t        uart_data_bits;
    uint16_t        uart_stop_bits;
    uint16_t        uart_check_bits;
    uint16_t        uart_irq_num;
    IRQn_Type       uart_irq_src;
    GPIO_TypeDef*    uart_tx_port;
    uint32_t        uart_tx_port_clk;
    uint16_t        uart_tx_pin;
    GPIO_TypeDef*    uart_rx_port;
    uint32_t        uart_rx_port_clk;
    uint16_t        uart_rx_pin;
    uint8_t*        uart_tx_buf;
    uint8_t*        uart_rx_buf;
    kfifo_t            uart_tx_fifo;
    kfifo_t            uart_rx_fifo;
    volatile uint8_t   flag_is_in_sending;
    uint8_t         send_char;
    uint8_t         recv_char;

} uart_init_tab_t;

#define ADD_UART_PORT(port,uart_clk,baud,stop,chk,irq,irq_src,tx_port,tx_pin,rx_port,rx_pin,tx_buf,tx_buf_size,rx_buf,rx_buf_size) \
                {port,uart_clk,baud,USART_WordLength_8b,stop,chk,irq,irq_src,tx_port,RCC_APB2Periph_##tx_port,tx_pin,rx_port,RCC_APB2Periph_##rx_port,rx_pin,tx_buf,rx_buf,{NULL,tx_buf_size,tx_buf,0,0},{NULL,rx_buf_size,rx_buf,0,0}}


uint8_t uart1_tx_buf[UART1_TX_BUF_SIZE];
uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];


#define SBUS_STOP   USART_StopBits_2
#define SBUS_CHK    USART_Parity_Even

uart_init_tab_t g_uart_init_tab[] = {
    ADD_UART_PORT(USART1, RCC_APB2Periph_USART1, UART1_DEF_BAUD, DEF_STOP, DEF_CHK, UART1_IRQ, USART1_IRQn, GPIOA, GPIO_Pin_9, GPIOA, GPIO_Pin_10, uart1_tx_buf, UART1_TX_BUF_SIZE, uart1_rx_buf, UART1_RX_BUF_SIZE),
};

//=============================================
static uint32_t send_mode[5] = {0};

static void bsp_uart_config (uint8_t uart_port, uint32_t baud, uint32_t data_bits, uint32_t stop_bit, uint32_t chk_bit, uint8_t mode)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;


    if(uart_port >= ARRY_ITEMS_NUM(g_uart_init_tab)) {
        DBG_E("bsp_uart_init err!:%d", uart_port);
        return ;
    }

    RCC_APB2PeriphClockCmd(g_uart_init_tab[uart_port].uart_rx_port_clk, ENABLE);
    RCC_APB2PeriphClockCmd(g_uart_init_tab[uart_port].uart_tx_port_clk, ENABLE);
    if(g_uart_init_tab[uart_port].uart_clk == RCC_APB2Periph_USART1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    } else {
        RCC_APB1PeriphClockCmd(g_uart_init_tab[uart_port].uart_clk, ENABLE);
    }

#if ENABLE_UARTS1_REMAP
    if (g_uart_init_tab[uart_port].uart_port == USART1) {
        GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
    }
#endif

#if ENABLE_UARTS2_REMAP
    if (g_uart_init_tab[uart_port].uart_port == USART2) {
        GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    }
#endif

#if ENABLE_UARTS3_REMAP
    if (g_uart_init_tab[uart_port].uart_port == USART3) {
        GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
    }
#endif

    GPIO_InitStructure.GPIO_Pin = g_uart_init_tab[uart_port].uart_tx_pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //复用推挽输出
    GPIO_Init(g_uart_init_tab[uart_port].uart_tx_port, &GPIO_InitStructure);//初始化GPIOA.9

    //USART1_RX
    GPIO_InitStructure.GPIO_Pin = g_uart_init_tab[uart_port].uart_rx_pin;;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(g_uart_init_tab[uart_port].uart_rx_port, &GPIO_InitStructure);//初始化GPIOA.10

    //Usart1 NVIC 配置
////    NVIC_InitStructure.NVIC_IRQChannel = g_uart_init_tab[uart_port].uart_irq_src;
////    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
////    NVIC_InitStructure.NVIC_IRQChannelSubPriority = g_uart_init_tab[uart_port].uart_irq_num;        //子优先级3
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ通道使能
//    NVIC_Init(&NVIC_InitStructure);    //根据指定的参数初始化VIC寄存器
    NVIC_SetPriority(g_uart_init_tab[uart_port].uart_irq_src, g_uart_init_tab[uart_port].uart_irq_num);
    NVIC_EnableIRQ(g_uart_init_tab[uart_port].uart_irq_src);

    //USART 初始化设置

    USART_InitStructure.USART_BaudRate = baud;//串口波特率
    USART_InitStructure.USART_WordLength = data_bits;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = stop_bit;//一个停止位
    USART_InitStructure.USART_Parity = chk_bit;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //收发模式

    USART_Init(g_uart_init_tab[uart_port].uart_port, &USART_InitStructure); //初始化串口1

    if(mode == 0) {
        USART_ITConfig(g_uart_init_tab[uart_port].uart_port, USART_IT_RXNE, ENABLE);//开启串口接受中断
        USART_ITConfig(g_uart_init_tab[uart_port].uart_port, USART_IT_PE, ENABLE); //-----------
        USART_ITConfig(g_uart_init_tab[uart_port].uart_port, USART_IT_ERR, ENABLE); //-------------
    } else {
        USART_ITConfig(g_uart_init_tab[uart_port].uart_port, USART_IT_TXE, DISABLE);
        USART_ITConfig(g_uart_init_tab[uart_port].uart_port, USART_IT_RXNE, DISABLE);//开启串口接受中断
        USART_ITConfig(g_uart_init_tab[uart_port].uart_port, USART_IT_PE, DISABLE); //-----------
        USART_ITConfig(g_uart_init_tab[uart_port].uart_port, USART_IT_ERR, DISABLE); //-------------
    }

    USART_Cmd(g_uart_init_tab[uart_port].uart_port, ENABLE);                    //使能串口1
}

void bsp_uart_set(uint8_t uart_port, uint32_t baud, uint32_t data_bits, uint8_t is_wait_mod)
{
    bsp_uart_config(uart_port, baud,
                    data_bits,
                    g_uart_init_tab[uart_port].uart_stop_bits,
                    g_uart_init_tab[uart_port].uart_check_bits,
                    is_wait_mod  //默认中断模式
                   );
}


void bsp_uart_init(uint8_t uart_port)
{
    //bsp_uart_config(uint8_t uart_port,uint32_t baud, uint32_t data_bits,uint32_t stop_bit,uint8_t mode)
    bsp_uart_config(uart_port, g_uart_init_tab[uart_port].uart_def_baud,
                    g_uart_init_tab[uart_port].uart_data_bits,
                    g_uart_init_tab[uart_port].uart_stop_bits,
                    g_uart_init_tab[uart_port].uart_check_bits,
                    0  //默认中断模式
                   );
}


int32_t bsp_uart_send(uint8_t uart_port, uint8_t* pbuf, int32_t len)
{
    int32_t real_len = 0;
    //if(len==0){return 0;}

    if(uart_port >= ARRY_ITEMS_NUM(g_uart_init_tab)) {
        DBG_E("bsp_uart_send err!:%d", uart_port);
        return 0;
    }

    if(send_mode[uart_port]) {
        uint16_t i = 0;
        for(i = 0; i < len; i++) {
            USART_SendData(g_uart_init_tab[uart_port].uart_port, pbuf[i]);
            while(USART_GetFlagStatus(g_uart_init_tab[uart_port].uart_port, USART_FLAG_TC) != SET);
        }
    } else {
        //USART_ITConfig( g_uart_init_tab[uart_port].uart_port, USART_IT_TXE, DISABLE);
        real_len = kfifo_push_in(&g_uart_init_tab[uart_port].uart_tx_fifo, pbuf, len);

        if(g_uart_init_tab[uart_port].flag_is_in_sending == 0) {
            //kfifo_pull_out(&g_uart_init_tab[uart_port].uart_tx_fifo,&g_uart_init_tab[uart_port].send_char,1);
            //USART_SendData(g_uart_init_tab[uart_port].uart_port, g_uart_init_tab[uart_port].send_char);
            g_uart_init_tab[uart_port].flag_is_in_sending = 1;
        }
        USART_ITConfig(g_uart_init_tab[uart_port].uart_port, USART_IT_TXE, ENABLE);

    }
    return real_len;
}

int32_t bsp_uart_read        (uint8_t uart_port, uint8_t* p_dest, int32_t len    )
{
    int32_t real_len = 0;
    if(uart_port >= ARRY_ITEMS_NUM(g_uart_init_tab)) {
        DBG_E("bsp_uart_read err!:%d", uart_port);
        return 0;
    }
    real_len = kfifo_pull_out(&g_uart_init_tab[uart_port].uart_rx_fifo, p_dest, len);
    return real_len;
}


void  bsp_uart_set_mode        (uint8_t uart_port, uint32_t mode)
{
    send_mode[uart_port] = mode;
    bsp_uart_config(uart_port, g_uart_init_tab[uart_port].uart_def_baud,
                    g_uart_init_tab[uart_port].uart_data_bits,
                    g_uart_init_tab[uart_port].uart_stop_bits,
                    g_uart_init_tab[uart_port].uart_check_bits,
                    mode  //默认中断模式
                   );

}





void uart_irq_action(uint8_t uart_port)
{
    if(uart_port >= ARRY_ITEMS_NUM(g_uart_init_tab)) {
        DBG_E("uart_irq_action err!:%d", uart_port);
        return ;
    }


    if( USART_GetITStatus(g_uart_init_tab[uart_port].uart_port, USART_IT_TXE) != RESET) {
        //if(USART_GetFlagStatus(g_uart_init_tab[uart_port].uart_port, USART_FLAG_TC) == SET)
        {
            if(kfifo_pull_out(&g_uart_init_tab[uart_port].uart_tx_fifo, &g_uart_init_tab[uart_port].send_char, 1)) {
                USART_SendData(g_uart_init_tab[uart_port].uart_port, g_uart_init_tab[uart_port].send_char);
            } else {
                g_uart_init_tab[uart_port].flag_is_in_sending = 0;
                USART_ITConfig(g_uart_init_tab[uart_port].uart_port, USART_IT_TXE, DISABLE);
            }
        }
    }


    if(USART_GetITStatus(g_uart_init_tab[uart_port].uart_port, USART_IT_RXNE) != RESET) {
        USART_ClearITPendingBit(g_uart_init_tab[uart_port].uart_port, USART_IT_RXNE);
        g_uart_init_tab[uart_port].recv_char = USART_ReceiveData(g_uart_init_tab[uart_port].uart_port);
        kfifo_push_in(&g_uart_init_tab[uart_port].uart_rx_fifo, &g_uart_init_tab[uart_port].recv_char, 1);

    }


    if (USART_GetFlagStatus(g_uart_init_tab[uart_port].uart_port, USART_FLAG_ORE) == SET) {
        //USART_ClearITPendingBit(g_uart_init_tab[uart_port].uart_port,USART_IT_ORE);
        g_uart_init_tab[uart_port].recv_char = USART_ReceiveData(g_uart_init_tab[uart_port].uart_port);
        kfifo_push_in(&g_uart_init_tab[uart_port].uart_rx_fifo, &g_uart_init_tab[uart_port].recv_char, 1);
        USART_ClearFlag(g_uart_init_tab[uart_port].uart_port, USART_FLAG_ORE); //读SR

    }

    if(USART_GetFlagStatus(g_uart_init_tab[uart_port].uart_port, USART_FLAG_PE) != RESET) {
        //?  @arg USART_IT_PE     : Parity Error interrupt
        USART_ClearFlag(g_uart_init_tab[uart_port].uart_port, USART_FLAG_PE);
    }

    if(USART_GetFlagStatus(g_uart_init_tab[uart_port].uart_port, USART_FLAG_FE) != RESET) {
        //?  @arg USART_IT_PE     : Parity Error interrupt
        USART_ClearFlag(g_uart_init_tab[uart_port].uart_port, USART_FLAG_FE);
    }

    if(USART_GetFlagStatus(g_uart_init_tab[uart_port].uart_port, USART_FLAG_NE) != RESET) {
        //?  @arg USART_IT_NE     : Noise Error interrupt
        USART_ClearFlag(g_uart_init_tab[uart_port].uart_port, USART_FLAG_NE);
    }


//    if (USART_GetITStatus(g_uart_init_tab[uart_port].uart_port, USART_IT_TC) == SET)
//    {
//        USART_ClearITPendingBit(g_uart_init_tab[uart_port].uart_port,USART_IT_TC);
//    }
}

void USART1_IRQHandler(void)
{
    uart_irq_action(0);
}

void USART2_IRQHandler(void)
{
    uart_irq_action(1);
}

void USART3_IRQHandler(void)
{
    uart_irq_action(2);
}



