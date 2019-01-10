#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_pwm.h"

typedef void (*pf_TIM_OCx_init)(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
typedef void (*pf_RCC_ClockCmd)(uint32_t RCC_AHBPeriph, FunctionalState NewState);
typedef void (*pf_TIM_SetComparex)(TIM_TypeDef* TIMx, uint32_t Compare);
typedef struct
{
    GPIO_TypeDef*       port;
    uint32_t            RCC_port;
    uint16_t            pin;
    uint16_t            pin_source;
    uint8_t             pin_AF;
    TIM_TypeDef*        Timer;
    uint32_t            RCC_TIM;
    pf_RCC_ClockCmd     RCC_clk_cmd;
    pf_TIM_OCx_init     TIM_OCx_init ;   
    pf_TIM_SetComparex  TIM_SetComparex ;
    
}pwm_cfg_t;

#define ADD_PWM_ITEM(port,pin,AF,RCC_TIM,timer_num,ch)  \
                        {   port,                           \
                            RCC_AHBPeriph_##port,           \
                            GPIO_Pin_##pin,                 \
                            GPIO_PinSource##pin,            \
                            AF,                             \
                            TIM##timer_num,                 \
                            RCC_##RCC_TIM##Periph_##TIM##timer_num, \
                            RCC_##RCC_TIM##PeriphClockCmd,          \
                            TIM_OC##ch##Init,                       \
                            TIM_SetCompare##ch                     \
                         }

const pwm_cfg_t pwm_tab[]=
{
    ADD_PWM_ITEM(GPIOC,6,GPIO_AF_0,APB1,3,1),
//    ADD_PWM_ITEM(GPIOC,7,GPIO_AF_0,APB1,3,2),
//    ADD_PWM_ITEM(GPIOC,8,GPIO_AF_0,APB1,3,3),
//    ADD_PWM_ITEM(GPIOC,9,GPIO_AF_0,APB1,3,4),
};

void bsp_pwm_init(void)
{
    uint32_t i = 0;
    GPIO_InitTypeDef            GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    TIM_OCInitTypeDef           TIM_OCInitStructure;
    
    //----------------jlink swd
    
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;//复用模式
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;//定义速度，最高是50MHz
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;//输出类型为推挽输出
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP ;//带上拉电阻
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_13| GPIO_Pin_14;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);//设置A端口时钟使能
    GPIO_Init(GPIOA, &GPIO_InitStructure);//定义一个GPIO_InitTypeDef类型的结构
    
    
    //--------------
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;//复用模式
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;//定义速度，最高是50MHz
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;//输出类型为推挽输出
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP ;//带上拉电阻
    
    
    
    
    TIM_TimeBaseStructure.TIM_Prescaler         = 960;//时钟频率的预分频值
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up; //向上计数 
    TIM_TimeBaseStructure.TIM_Period            = 1000;//自动重装载寄存器周期值
    TIM_TimeBaseStructure.TIM_ClockDivision     = 0;//时钟分割值
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    
    //输出模式配置
    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM2;//TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;//使能输出比较状态
    TIM_OCInitStructure.TIM_OutputNState    = TIM_OutputNState_Disable;//失能输出比较N状态
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;//TIM输出比较极性高
    TIM_OCInitStructure.TIM_OCIdleState     = TIM_OCIdleState_Set;//当MOE=0时，设置TIM输出比较空闲状态
    TIM_OCInitStructure.TIM_Pulse           = 10;//占空比赋值
    

    for(i=0;i<ARRY_ITEMS_NUM(pwm_tab);i++)
    {
        //------gpio init
        GPIO_InitStructure.GPIO_Pin = pwm_tab[i].pin;//定义使用的管脚
        RCC_AHBPeriphClockCmd(pwm_tab[i].RCC_port, ENABLE);//设置A端口时钟使能
        GPIO_Init(pwm_tab[i].port, &GPIO_InitStructure);//定义一个GPIO_InitTypeDef类型的结构
        GPIO_PinAFConfig(pwm_tab[i].port, pwm_tab[i].pin_source, pwm_tab[i].pin_AF);//复用功能打开
        
        
        //-------timer init
        pwm_tab[i].RCC_clk_cmd(pwm_tab[i].RCC_TIM,ENABLE);
        TIM_TimeBaseInit(pwm_tab[i].Timer, &TIM_TimeBaseStructure);//初始化TIM3的时间基数数据
        pwm_tab[i].TIM_OCx_init(pwm_tab[i].Timer, &TIM_OCInitStructure);
        TIM_Cmd(pwm_tab[i].Timer, ENABLE);//定时器3使能
        
        //-------pwm output
        TIM_CtrlPWMOutputs(pwm_tab[i].Timer, ENABLE);//PWM波形通过TIM3产生
    }
   
}

int32_t bsp_pwm_write(uint8_t pwm_ch,uint32_t val)
{
    if( pwm_ch<ARRY_ITEMS_NUM(pwm_tab) )
    {
         pwm_tab[pwm_ch].TIM_SetComparex(pwm_tab[pwm_ch].Timer,val); 
    }
    return 0;
}

#if 1
int32_t set_pwm(uint8_t argc, uint8_t *argv[])
{
    if(argc>=3)
    {
        uint32_t ch = 0;
        uint32_t val = 0;
        
        sscanf((const char*)argv[1],"%d",&ch);
        sscanf((const char*)argv[2],"%d",&val);    
        
        bsp_pwm_write((uint8_t)ch,val);
        DBG_I("set_pwm index:%d val:%d success!!",ch,val);
    }
    else
    {
        DBG_W("warn the set_pwm param is error: eg:set_pwm ch val");
    }
    return RET_OK;
}

REG_SHELL_CMD(set_pwm,0,set_pwm,"set_pwm", "eg:set_pwm ch val\r\n ");
#endif

void TIM3_IRQHandler(void)
{    
    TIM_ClearITPendingBit (TIM3, TIM_IT_Update);    //必须要清除中断标志位
}
