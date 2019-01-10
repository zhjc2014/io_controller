#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_gpio.h"

typedef struct
{
    GPIO_TypeDef*       port;
    uint32_t            RCC_port;
    uint16_t            pin;
    uint8_t             is_input;
}gpio_cfg_t;



#define ADD_GPIO_ITEM(port,pin,is_input)  \
                        {   port,                           \
                            RCC_AHBPeriph_##port,           \
                            GPIO_Pin_##pin,                 \
                            is_input,\
                         }

const gpio_cfg_t gpio_tab[]=
{
    //            port  pin               timer  CH
    
    ADD_GPIO_ITEM(GPIOB, 3, 0),
    ADD_GPIO_ITEM(GPIOB, 4, 0),
    ADD_GPIO_ITEM(GPIOB, 5, 0),
    ADD_GPIO_ITEM(GPIOB, 6, 0),
    ADD_GPIO_ITEM(GPIOB, 7, 0),
    ADD_GPIO_ITEM(GPIOB, 8, 0),
    ADD_GPIO_ITEM(GPIOB, 9, 0),

};




void bsp_gpio_init(void)
{
    uint32_t i = 0;
    GPIO_InitTypeDef            GPIO_InitStructure;
    //--------------
    
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;//定义速度，最高是50MHz

    for(i=0;i<ARRY_ITEMS_NUM(gpio_tab);i++)
    {

        //------gpio init
        GPIO_InitStructure.GPIO_Pin = gpio_tab[i].pin;//定义使用的管脚
        RCC_AHBPeriphClockCmd(gpio_tab[i].RCC_port, ENABLE);//设置A端口时钟使能
        
        if(gpio_tab[i].is_input)    {GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;}
        else                        {GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;}
        
        GPIO_Init(gpio_tab[i].port, &GPIO_InitStructure);//定义一个GPIO_InitTypeDef类型的结构
    }
   
}

int32_t bsp_gpio_write(uint8_t ch,uint32_t val)
{
    if( ch<ARRY_ITEMS_NUM(gpio_tab) )
    {
         if(val){  GPIO_SetBits(gpio_tab[ch].port, gpio_tab[ch].pin);}
         else   {GPIO_ResetBits(gpio_tab[ch].port, gpio_tab[ch].pin);}
    }
	return 0;
}

uint32_t bsp_gpio_read(uint8_t ch)
{
    if( ch<ARRY_ITEMS_NUM(gpio_tab) )
    {
        return GPIO_ReadInputDataBit(gpio_tab[ch].port, gpio_tab[ch].pin);
    }
	return 0;
}

#if 1
int32_t set_gpio(uint8_t argc, uint8_t *argv[])
{
    if(argc>=3)
    {
        uint32_t ch = 0;
        uint32_t val = 0;
        
        sscanf((const char*)argv[1],"%d",&ch);
        sscanf((const char*)argv[2],"%d",&val);    
        
        bsp_gpio_write((uint8_t)ch,val);
        DBG_I("bsp_gpio_write index:%d val:%d success!!",ch,val);
    }
    else
    {
        DBG_W("warn the bsp_gpio_write param is error: eg:bsp_gpio_write ch val");
    }
	return RET_OK;
}

REG_SHELL_CMD(set_gpio,0,set_gpio,"set_gpio", "eg:set_gpio ch val\r\n ");
#endif

