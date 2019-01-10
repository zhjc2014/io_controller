#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_sys.h"
#include "stm32f10x.h"                  
#include "x_frame_lib.h"
#include "x_frame.h"
#include "x_frame_cfg.h"

/**
  * @brief  Sets the priority of an interrupt.
  * @param  IRQn: External interrupt number
  *         This parameter can be an enumerator of IRQn_Type enumeration
  *         (For the complete STM32 Devices IRQ Channels list, please refer to the appropriate CMSIS device file (stm32f10xxx.h))
  * @param  PreemptPriority: The pre-emption priority for the IRQn channel.
  *         This parameter can be a value between 0 and 15
  *         A lower priority value indicates a higher priority 
  * @param  SubPriority: the subpriority level for the IRQ channel.
  *         This parameter can be a value between 0 and 15
  *         A lower priority value indicates a higher priority.          
  * @retval None
  */
void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t prioritygroup = 0x00;
  
  /* Check the parameters */
  assert_param(IS_NVIC_SUB_PRIORITY(SubPriority));
  assert_param(IS_NVIC_PREEMPTION_PRIORITY(PreemptPriority));
  
  prioritygroup = NVIC_GetPriorityGrouping();
  
  NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));
}

void bsp_timer_init(void)
{
    SystemInit();
    /* Setup SysTick Timer for 10 msec interrupts  */
    if (SysTick_Config(SystemCoreClock / MAX_SYS_TASK_FREQ))
    { 
        /* Capture error */ 
        while (1);
    }  
    //for systick
    HAL_NVIC_SetPriority(SysTick_IRQn, 0x00, 0x00);
}



//NVIC_PriorityGroup_3  |    0-7    |    0-1     |   3 bits for pre-emption priority
//                      |           |            |   1 bits for subpriority
void bsp_nvic_preinit(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    //group setting
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
    //for systick
    HAL_NVIC_SetPriority(SysTick_IRQn, 0x00, 0x00);
    //for ch375
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Pre_Init(&NVIC_InitStructure);
    //for sonar
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Pre_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Pre_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Pre_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Pre_Init(&NVIC_InitStructure);
    //for uart
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Pre_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Pre_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Pre_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_Pre_Init(&NVIC_InitStructure);
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Pre_Init(&NVIC_InitStructure);
    //for sdio sd card
    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_Pre_Init(&NVIC_InitStructure);
    //for spi sd card
    NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Pre_Init(&NVIC_InitStructure);
    //for usb
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Pre_Init(&NVIC_InitStructure);
    //for adc2
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_Pre_Init(&NVIC_InitStructure);
}


void system_reset(void)
{
    __disable_fault_irq();
    NVIC_SystemReset();  //唤醒系统后应自动重启初始化
}


static RET_TYPE shell_sys_reset(uint8_t argc, uint8_t *argv[])
{
    system_reset();

    return 0;
}

REG_SHELL_CMD(reset,0,shell_sys_reset,"sys_reset", "sys_reset!!! \r\n ");


/*
详细说明参照：

http://blog.sina.com.cn/s/blog_9d9054cb0102x9t8.html

*/


void show_fault_info(hard_fault_data_t* p_fault_data)
{
    LOG_FAULT("\n\n[Hard fault handler - all numbers in hex]");
    LOG_FAULT("R0      = 0x%x",p_fault_data->r0);
    LOG_FAULT("R1      = 0x%x",p_fault_data->r1);
    LOG_FAULT("R2      = 0x%x",p_fault_data->r2);
    LOG_FAULT("R3      = 0x%x",p_fault_data->r3);
    LOG_FAULT("R12     = 0x%x",p_fault_data->r12);
    LOG_FAULT("LR[R14] = 0x%x",p_fault_data->lr );
    LOG_FAULT("PC[R15] = 0x%x",p_fault_data->pc);
    LOG_FAULT("PSR     = 0x%x",p_fault_data->psr);
    LOG_FAULT("SHCSR   = 0x%x",p_fault_data->SHCSR);
    LOG_FAULT("MFSR    = 0x%x",p_fault_data->MFSR);
    LOG_FAULT("BFSR    = 0x%x",p_fault_data->BFSR);
    LOG_FAULT("UFSR    = 0x%x",p_fault_data->UFSR);
    LOG_FAULT("HFSR    = 0x%x",p_fault_data->HFSR);
    LOG_FAULT("DFSR    = 0x%x",p_fault_data->DFSR);
    LOG_FAULT("MMAR    = 0x%x",p_fault_data->MMAR );
    LOG_FAULT("BFAR    = 0x%x",p_fault_data->BFAR );
}

void hard_fault_handler_c(unsigned int * hardfault_args)
{
    static hard_fault_data_t fault_data;
    __disable_irq();
    hal_app_ctrl(DEBUG_DRV,1,NULL,0);  //此时的中断系统已经不能使用，只能切换到查询模式等待串口发送完毕
    
    fault_data.r0     = ((unsigned long) hardfault_args[0]);
    fault_data.r1     = ((unsigned long) hardfault_args[1]);
    fault_data.r2     = ((unsigned long) hardfault_args[2]);
    fault_data.r3     = ((unsigned long) hardfault_args[3]);
    fault_data.r12     = ((unsigned long) hardfault_args[4]);
    fault_data.lr     = ((unsigned long) hardfault_args[5]); 
    fault_data.pc     = ((unsigned long) hardfault_args[6]);
    fault_data.psr     = ((unsigned long) hardfault_args[7]);
    fault_data.SHCSR = (*((volatile unsigned long *)(0xE000ED24)));
    fault_data.MFSR = (*((volatile unsigned char *)(0xE000ED28)));
    fault_data.BFSR = (*((volatile unsigned char *)(0xE000ED29)));
    fault_data.UFSR = (*((volatile unsigned short int *)(0xE000ED2A)));
    fault_data.HFSR = (*((volatile unsigned long *)(0xE000ED2C)));  
    fault_data.DFSR = (*((volatile unsigned long *)(0xE000ED30)));
    fault_data.MMAR = (*((volatile unsigned long *)(0xE000ED34)));
    fault_data.BFAR = (*((volatile unsigned long *)(0xE000ED38))); 
    
    show_fault_info(&fault_data);
    
    //hal_app_write(SYS_RTC_DRV,(uint8_t*)&fault_data,sizeof(hard_fault_data_t),1);

    while (1);

} 



//[4  ][bsp_sys.c][E]: R0      = 0x2580
//[4  ][bsp_sys.c][E]: R1      = 0x8008fc5
//[4  ][bsp_sys.c][E]: R2      = 0x0
//[4  ][bsp_sys.c][E]: R3      = 0x1
//[4  ][bsp_sys.c][E]: R12     = 0x21c
//[4  ][bsp_sys.c][E]: LR[R14] = 0x800174f
//[4  ][bsp_sys.c][E]: PC[R15] = 0x8002800
//[4  ][bsp_sys.c][E]: PSR     = 0x1000000
//[4  ][bsp_sys.c][E]: SHCSR   = 0x0
//[4  ][bsp_sys.c][E]: MFSR    = 0x0
//[4  ][bsp_sys.c][E]: BFSR    = 0x82
//[4  ][bsp_sys.c][E]: UFSR    = 0x0
//[4  ][bsp_sys.c][E]: HFSR    = 0x40000000
//[4  ][bsp_sys.c][E]: DFSR    = 0x1
//[4  ][bsp_sys.c][E]: MMAR    = 0x28009951
//[4  ][bsp_sys.c][E]: BFAR    = 0x28009951


RET_TYPE fault_demo(uint8_t argc, uint8_t *argv[])
{
    uint8_t* pbuf=NULL;
    DBG_I("fault_demo!!");
    *pbuf = 0x12;
    return RET_OK;
}

REG_SHELL_CMD(fault,0,fault_demo,"demo hard fault", "this is the demo demo hard fault test \r\n ");


#define GPIO_INIT_EX(gpiox, GPIO_InitStruct) do{gpio_init_ex_func(gpiox, GPIO_InitStruct, __MODULE__, __LINE__);}while(0)


void gpio_init_ex_func(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct, const char *filename, uint32_t line)
{
    if(IS_GPIO_ALL_PERIPH(GPIOx) == 0)
        DBG_E("%s %u\r\n", filename, line);

    GPIO_Init(GPIOx, GPIO_InitStruct);
}

void jump_to_app(uint32_t app_address)
{
    typedef void (*_func)(void);
    
    GPIO_InitTypeDef GPIO_InitStruct;

    __disable_irq();
    
    
    /* MCU peripherals re-initial. */

    

    

    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStruct.GPIO_Pin &= ~(GPIO_Pin_13 | GPIO_Pin_14); /* SWDIO/SWCLK */

    GPIO_INIT_EX(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
    GPIO_INIT_EX(GPIOB, &GPIO_InitStruct);
    GPIO_INIT_EX(GPIOC, &GPIO_InitStruct);
    GPIO_INIT_EX(GPIOD, &GPIO_InitStruct);
    GPIO_INIT_EX(GPIOE, &GPIO_InitStruct);
    GPIO_INIT_EX(GPIOF, &GPIO_InitStruct);
    GPIO_INIT_EX(GPIOG, &GPIO_InitStruct);

    /* reset systick */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    /* disable all peripherals clock. */
    RCC->APB1ENR = 0;
    RCC->APB2ENR = 0;

    /* Switch to default cpu clock. */
    RCC->CFGR = 0;

    /* Disable MPU */
    //MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;

    /* disable and clean up all interrupts. */
    {
        int i;
        for(i = 0; i < 8; i++)
        {
            /* disable interrupts. */
            NVIC->ICER[i] = 0xFFFFFFFF;

            /* clean up interrupts flags. */
            NVIC->ICPR[i] = 0xFFFFFFFF;
        }

        /* Set new vector table pointer */
        SCB->VTOR = app_address;

        /* reset register values */
        __set_BASEPRI(0);
        __set_FAULTMASK(0);

        /* set up MSP and switch to it */
        __set_PSP(*(uint32_t*)app_address);
        __set_CONTROL(0);
        __set_MSP(*(uint32_t*)app_address);

        /* ensure what we have done could take effect */
        __ISB();
    }

    TIM_DeInit(TIM3);  
    TIM_Cmd(TIM3, DISABLE);  
    USART_DeInit(USART1);  
    USART_DeInit(USART2);  
    USART_Cmd(USART1,DISABLE);  
    RCC_RTCCLKCmd(DISABLE);  

    __disable_irq();
    
    /* never return */
    ((_func)(*(uint32_t*)(app_address + 4)))();
}


RET_TYPE go_addr(uint8_t argc, uint8_t *argv[])
{
    
    if(argc==2)
    {
        uint32_t addr = 0;
        sscanf((const char*)argv[1],"%x",&addr);
        jump_to_app(addr);
    }
    else
    {
    
    DBG_I("go_addr param is error!!");
    }
    return RET_OK;
}

REG_SHELL_CMD(go,0,go_addr,"go to addr and run!", "go to addr and run!  eg go 0x2000000 \r\n ");




