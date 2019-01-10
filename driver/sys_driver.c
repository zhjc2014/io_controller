#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "x_frame.h"

#if ENABLE_SYS_DRV

static int32_t drv_sys_init		(void)
{
	return 0;
}

static void system_reset(void)
{
    __disable_irq();
    NVIC_SystemReset();  //唤醒系统后应自动重启初始化
}



typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
#define APPLICATION_ADDRESS     ((uint32_t)0x0800f000)


static void jump(uint32_t addr)
{
    uint32_t JumpAddress;
    int i;
    
    __disable_irq();
    
    RCC_DeInit();
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE); 
    //NVIC_init();
    
    /* reset systick */
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    /* disable all peripherals clock. */
    RCC->APB1ENR = 0;
    RCC->APB2ENR = 0;
    /* Switch to default cpu clock. */
    RCC->CFGR = 0;
    RCC_RTCCLKCmd(DISABLE);  
    
    for(i = 0; i < 8; i++)
    {
        /* disable interrupts. */
        NVIC->ICER[i] = 0xFFFFFFFF;
        /* clean up interrupts flags. */
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    

    /* Jump to user application */
    JumpAddress = *(__IO uint32_t*) (addr + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    
    __set_PSP(*(uint32_t*)addr);
    __set_CONTROL(0);
    __set_MSP(*(uint32_t*)addr);

    __set_CONTROL(0);

    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*) addr);
    
    __ISB();


    /* Jump to application */
    Jump_To_Application();
}


#ifdef __IAP__
__IO uint32_t VectorTable[48] __attribute__((at(0x20000000)));  

void IAP_Set(void)
{
    uint32_t i = 0;

    for(i = 0; i < 48; i++)  
    {  
        VectorTable[i] = *(__IO uint32_t*)(APPLICATION_ADDRESS + (i<<2));  
    } 
  
    /* Enable the SYSCFG peripheral clock*/ 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SYSCFG, ENABLE);     
    //RCC_APB2PeriphReset(RCC_APB2Periph_SYSCFG, ENABLE) ;
    /* Remap SRAM at 0x00000000 */
    SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
    __enable_irq();
    
}	
#endif



int32_t drv_sys_ctrl (uint32_t ctrl_cmd ,uint8_t* pDest,uint32_t len ,uint32_t * real_c_bytes)
{
    DBG_I("drv_sys_ctrl ctrl_cmd:%d",ctrl_cmd);
    switch(ctrl_cmd)
    {
        case SYS_RESET_CMD:
            {
                system_reset();
            }
            break;
        case SYS_JUMP_CMD:
            {
                jump((uint32_t)pDest);
            }
            break;
            
        #ifdef __IAP__
        case SYS_IAP_APP_INIT_CMD:
            {
                IAP_Set();
            }
            break;
        #endif
            
        default: break;
    }
    
    return 0;
}
        
REG_DEV(    
            DRV_SYS,                        /*driver id         */
            DEV_AUTO_INIT,                  /*driver auto init  */
            drv_sys_init ,                  /*driver init func  */
            NULL,                           /*driver write func */
            NULL,                           /*driver read  func */
            drv_sys_ctrl,                   /*driver ctrl  func */
            NULL                            /*driver close func */
        );  

#endif 


#if 1
static int32_t go(uint8_t argc, uint8_t *argv[])
{
    if(argc==2)
    {
        uint32_t addr = 0x0;
        sscanf((const char*)argv[1],"0x%x",&addr);
        //DBG_I("this will go to address:0x%x val:0x%x",addr);
        jump(addr);
    }
    else
    {
        //DBG_E("go cmd param is error! eg: go address");
    }
	return RET_OK;
}

REG_SHELL_CMD(go,0,go,"go address!!", "eg: go address!\r\n ");
#endif

#if 1
static int32_t reset(uint8_t argc, uint8_t *argv[])
{
    system_reset();	
	return RET_OK;
}

REG_SHELL_CMD(reset,0,reset,"reset!!", "eg: reset!\r\n ");
#endif


