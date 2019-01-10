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
    
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;//����ģʽ
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;//�����ٶȣ������50MHz
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;//�������Ϊ�������
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP ;//����������
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_13| GPIO_Pin_14;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);//����A�˿�ʱ��ʹ��
    GPIO_Init(GPIOA, &GPIO_InitStructure);//����һ��GPIO_InitTypeDef���͵Ľṹ
    
    
    //--------------
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF;//����ģʽ
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;//�����ٶȣ������50MHz
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;//�������Ϊ�������
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP ;//����������
    
    
    
    
    TIM_TimeBaseStructure.TIM_Prescaler         = 960;//ʱ��Ƶ�ʵ�Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up; //���ϼ��� 
    TIM_TimeBaseStructure.TIM_Period            = 1000;//�Զ���װ�ؼĴ�������ֵ
    TIM_TimeBaseStructure.TIM_ClockDivision     = 0;//ʱ�ӷָ�ֵ
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    
    //���ģʽ����
    TIM_OCInitStructure.TIM_OCMode          = TIM_OCMode_PWM2;//TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;//ʹ������Ƚ�״̬
    TIM_OCInitStructure.TIM_OutputNState    = TIM_OutputNState_Disable;//ʧ������Ƚ�N״̬
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;//TIM����Ƚϼ��Ը�
    TIM_OCInitStructure.TIM_OCIdleState     = TIM_OCIdleState_Set;//��MOE=0ʱ������TIM����ȽϿ���״̬
    TIM_OCInitStructure.TIM_Pulse           = 10;//ռ�ձȸ�ֵ
    

    for(i=0;i<ARRY_ITEMS_NUM(pwm_tab);i++)
    {
        //------gpio init
        GPIO_InitStructure.GPIO_Pin = pwm_tab[i].pin;//����ʹ�õĹܽ�
        RCC_AHBPeriphClockCmd(pwm_tab[i].RCC_port, ENABLE);//����A�˿�ʱ��ʹ��
        GPIO_Init(pwm_tab[i].port, &GPIO_InitStructure);//����һ��GPIO_InitTypeDef���͵Ľṹ
        GPIO_PinAFConfig(pwm_tab[i].port, pwm_tab[i].pin_source, pwm_tab[i].pin_AF);//���ù��ܴ�
        
        
        //-------timer init
        pwm_tab[i].RCC_clk_cmd(pwm_tab[i].RCC_TIM,ENABLE);
        TIM_TimeBaseInit(pwm_tab[i].Timer, &TIM_TimeBaseStructure);//��ʼ��TIM3��ʱ���������
        pwm_tab[i].TIM_OCx_init(pwm_tab[i].Timer, &TIM_OCInitStructure);
        TIM_Cmd(pwm_tab[i].Timer, ENABLE);//��ʱ��3ʹ��
        
        //-------pwm output
        TIM_CtrlPWMOutputs(pwm_tab[i].Timer, ENABLE);//PWM����ͨ��TIM3����
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
    TIM_ClearITPendingBit (TIM3, TIM_IT_Update);    //����Ҫ����жϱ�־λ
}
