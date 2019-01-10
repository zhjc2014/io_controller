#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_systick.h"

volatile static uint32_t sys_tick = 0;

void bsp_sys_tick_init(void)
{
    SystemInit();
    if (SysTick_Config(SystemCoreClock / 1000))
    { 
        /* Capture error */ 
        while (1);
    }  

//    NVIC_SetPriorityGrouping(0);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    
    /* Configure the SysTick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x0); 
}

uint32_t bsp_read(void)
{
    return sys_tick;
}

void bsp_write(uint32_t tick)
{
    sys_tick = tick;
}

void SysTick_Handler(void)
{
    hal_dev_notify(DRV_ID_SYS_TICK,0,NULL,0);
    sys_tick++;
}


void HardFault_Handler(void)  
{  
  while (1)  
  {  
  }  
}  


typedef struct
{
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r12;
    unsigned int lr;
    unsigned int pc;
    unsigned int psr;
    unsigned int SHCSR;
    unsigned char MFSR;
    unsigned char BFSR;
    unsigned short int UFSR;
    unsigned int HFSR;
    unsigned int DFSR;
    unsigned int MMAR;
    unsigned int BFAR;
}hard_fault_data_t;

#define LOG_FAULT DBG_E

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
    //hal_app_ctrl(1,1,NULL,0,NULL);  //此时的中断系统已经不能使用，只能切换到查询模式等待串口发送完毕
    
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
