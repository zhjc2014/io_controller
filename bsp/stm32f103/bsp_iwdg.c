#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "stm32f10x_iwdg.h"

void bsp_iwdg_init(void)
{
    IWDG_SetPrescaler(IWDG_Prescaler_4);
    IWDG_SetReload(10000);
    IWDG_Enable();
}

void bsp_iwdg_write(void)
{
    IWDG_ReloadCounter();
}

int bsp_iwdg_read(uint16_t IWDG_FLAG)
{
    return IWDG_GetFlagStatus(IWDG_FLAG);
}

