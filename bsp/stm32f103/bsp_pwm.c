#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_pwm.h"

typedef void (*pf_TIM_OCx_init)(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
typedef void (*pf_RCC_ClockCmd)(uint32_t RCC_AHBPeriph, FunctionalState NewState);
typedef void (*pf_TIM_SetComparex)(TIM_TypeDef* TIMx, uint16_t Compare);
typedef struct
{
    GPIO_TypeDef*       port;
    uint32_t            RCC_port;
    uint16_t            pin;
    TIM_TypeDef*        Timer;
    uint16_t            pwm_mod;
    uint32_t            RCC_TIM;
    pf_RCC_ClockCmd     RCC_clk_cmd;
    pf_TIM_OCx_init     TIM_OCx_init ;   
    pf_TIM_SetComparex  TIM_SetComparex ;
    uint32_t            max_cnt;
    uint32_t            div_cnt;
    uint32_t            is_remap;
    uint32_t            remap_gpio;
}pwm_cfg_t;


#define SYS_CLK_FEQ  72000000

#define ADD_PWM_ITEM(port,pin,RCC_TIM,pwm_mod,timer_num,ch,period,div,is_remap,remap_num)  \
                        {   port,                           \
                            RCC_APB2Periph_##port,           \
                            GPIO_Pin_##pin,                 \
                            TIM##timer_num,                 \
                            pwm_mod,                         \
                            RCC_##RCC_TIM##Periph_##TIM##timer_num, \
                            RCC_##RCC_TIM##PeriphClockCmd,          \
                            TIM_OC##ch##Init,                       \
                            TIM_SetCompare##ch,                     \
                            period,                              \
                            div,                                 \
                            is_remap, \
                            remap_num,\
                         }

const pwm_cfg_t pwm_tab[]=
{
    //            port  pin               timer  CH
    //for relay control
//    ADD_PWM_ITEM(GPIOB, 7,APB1,TIM_OCMode_PWM1,4,2,3600,0,0,0),    //20k
//    ADD_PWM_ITEM(GPIOB, 6,APB1,TIM_OCMode_PWM1,4,1,3600,0,0,0),
//    ADD_PWM_ITEM(GPIOB, 5,APB1,TIM_OCMode_PWM1,3,2,3600,0,1,GPIO_PartialRemap_TIM3),    //20k
//    ADD_PWM_ITEM(GPIOB, 4,APB1,TIM_OCMode_PWM1,3,1,3600,0,1,GPIO_PartialRemap_TIM3),
    ADD_PWM_ITEM(GPIOB, 9,APB1,TIM_OCMode_PWM1,4,4,3600,0,0,0),    //20k
    ADD_PWM_ITEM(GPIOB, 8,APB1,TIM_OCMode_PWM1,4,3,3600,0,0,0),        
};

void bsp_pwm_init(void)
{
    uint32_t i = 0;
    GPIO_InitTypeDef            GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    TIM_OCInitTypeDef           TIM_OCInitStructure;
    
    //--------------
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;//����ģʽ
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;//�����ٶȣ������50MHz
    
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up; //���ϼ��� 
    TIM_TimeBaseStructure.TIM_ClockDivision     = 0;//ʱ�ӷָ�ֵ
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    
    //���ģʽ����
    
    TIM_OCInitStructure.TIM_OutputState     = TIM_OutputState_Enable;//ʹ������Ƚ�״̬
    TIM_OCInitStructure.TIM_OutputNState    = TIM_OutputNState_Disable;//ʧ������Ƚ�N״̬
    TIM_OCInitStructure.TIM_OCPolarity      = TIM_OCPolarity_High;//TIM����Ƚϼ��Ը�
    TIM_OCInitStructure.TIM_OCIdleState     = TIM_OCIdleState_Reset;//��MOE=0ʱ������TIM����ȽϿ���״̬

    //special handle 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    //for PB4 pwm

    for(i=0;i<ARRY_ITEMS_NUM(pwm_tab);i++)
    {
        if(pwm_tab[i].is_remap)
        {
            GPIO_PinRemapConfig(pwm_tab[i].remap_gpio, ENABLE);
        }
        
        //------gpio init
        GPIO_InitStructure.GPIO_Pin = pwm_tab[i].pin;//����ʹ�õĹܽ�
        RCC_APB2PeriphClockCmd(pwm_tab[i].RCC_port, ENABLE);//����A�˿�ʱ��ʹ��
        GPIO_Init(pwm_tab[i].port, &GPIO_InitStructure);//����һ��GPIO_InitTypeDef���͵Ľṹ
        
        //-------timer init
        TIM_OCInitStructure.TIM_OCMode              = pwm_tab[i].pwm_mod;//TIM�����ȵ���ģʽ2
        TIM_TimeBaseStructure.TIM_Prescaler         = pwm_tab[i].div_cnt;//ʱ��Ƶ�ʵ�Ԥ��Ƶֵ  1000k
        TIM_TimeBaseStructure.TIM_Period            = pwm_tab[i].max_cnt-1;//�Զ���װ�ؼĴ�������ֵ 1000k/1000 = 1k ��Ƶ��
        TIM_OCInitStructure.TIM_Pulse               = 0; //pwm_tab[i].max_cnt/10;//10%�����
        pwm_tab[i].RCC_clk_cmd(pwm_tab[i].RCC_TIM,ENABLE);
        TIM_TimeBaseInit(pwm_tab[i].Timer, &TIM_TimeBaseStructure);//��ʼ��TIMx��ʱ���������
        pwm_tab[i].TIM_OCx_init(pwm_tab[i].Timer, &TIM_OCInitStructure);
        TIM_Cmd(pwm_tab[i].Timer, ENABLE);//��ʱ��xʹ��
        
        //-------pwm output
        TIM_CtrlPWMOutputs(pwm_tab[i].Timer, ENABLE);//PWM����ͨ��TIMx����
    }
   
}

int32_t bsp_pwm_write(uint8_t pwm_ch,uint32_t val)
{
    uint32_t val_in = 0;
    if( pwm_ch<ARRY_ITEMS_NUM(pwm_tab) )
    {
         val_in = ((val* pwm_tab[pwm_ch].max_cnt)/10000); //the max pwm duty is 10000
         pwm_tab[pwm_ch].TIM_SetComparex(pwm_tab[pwm_ch].Timer,(uint16_t)val_in); 
         //DBG_I_LIMIT(1000,"bsp_pwm_write[%d]: %d  max:%d  se:%d",pwm_ch,val_in,pwm_tab[pwm_ch].max_cnt,val);
    }
//    DBG_I();
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
        //DBG_I("set_pwm index:%d val:%d success!!",ch,val);
    }
    else
    {
        DBG_W("warn the set_pwm param is error: eg:set_pwm ch val");
    }
    return RET_OK;
}

REG_SHELL_CMD(set_pwm,0,set_pwm,"set_pwm", "eg:set_pwm ch val\r\n ");
#endif

