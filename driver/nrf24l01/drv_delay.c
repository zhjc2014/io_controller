/**
  ******************************************************************************
  * @author  ��ҫ�Ƽ� ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   DELAY����C�ļ�
  ******************************************************************************
  * @attention
  *
  * ����	:	http://www.ashining.com
  * �Ա�	:	https://shop105912646.taobao.com
  * ����Ͱ�:	https://cdzeyao.1688.com
  ******************************************************************************
  */



#include "drv_delay.h"


/**
  * @brief :��ʱ��ʼ��
  * @param :��
  * @note  :��ʱ��Ĭ�ϳ�ʼ����us������
  * @retval:��
  */
void drv_delay_init( void )
{
	TIM_TimeBaseInitTypeDef	 TimerInitStructer;
	RCC_ClocksTypeDef RCC_ClocksStatus;
	
	//��ȡϵͳʱ��
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	
	//ʹ��TIM1ʱ��
	RCC_APB2PeriphClockCmd( DELAY_TIME_BASE_CLK, ENABLE );
	
	TimerInitStructer.TIM_ClockDivision = TIM_CKD_DIV1;
	TimerInitStructer.TIM_CounterMode = TIM_CounterMode_Up;
	TimerInitStructer.TIM_Period = 0x00FF;
	TimerInitStructer.TIM_RepetitionCounter = DISABLE;
	TimerInitStructer.TIM_Prescaler = ( RCC_ClocksStatus.PCLK1_Frequency / 1000000 ) - 1;
	TIM_TimeBaseInit( DELAY_TIME_BASE, &TimerInitStructer );
	
	TIM_ClearFlag( DELAY_TIME_BASE, TIM_FLAG_Update ); 
	TIM_SetCounter( DELAY_TIME_BASE, 0 );
	TIM_Cmd( DELAY_TIME_BASE, ENABLE );
	while( RESET == TIM_GetFlagStatus( DELAY_TIME_BASE, TIM_FLAG_Update ));
	TIM_Cmd( DELAY_TIME_BASE, DISABLE );
	TIM_SetCounter( DELAY_TIME_BASE, 0 );
	TIM_ClearFlag( DELAY_TIME_BASE, TIM_FLAG_Update ); 
}

/**
  * @brief :��ʱ(us)
  * @param :
*			@Us:��ʱ��us��
  * @note  :������65535
  * @retval:��
  */
void drv_delay_us( uint16_t Us )
{
	DELAY_TIME_BASE->ARR = Us;
	DELAY_TIME_BASE->CNT = 0;
	DELAY_TIME_BASE->CR1 |= (uint32_t)0x01;
	while( RESET == ( DELAY_TIME_BASE->SR & TIM_FLAG_Update ));
	DELAY_TIME_BASE->SR &= (uint32_t)( ~(uint32_t)TIM_FLAG_Update );
	DELAY_TIME_BASE->CR1 &= (uint32_t)( ~(uint32_t)0x01 );
}

/**
  * @brief :��ʱ(ms)
  * @param :
  *			@Ms:��ʱ��Ms��
  * @note  :������65
  * @retval:��
  */
void drv_delay_ms( uint8_t Ms )
{
	DELAY_TIME_BASE->ARR = Ms * 1000;
	DELAY_TIME_BASE->CNT = 0;
	DELAY_TIME_BASE->CR1 |= (uint32_t)0x01;
	while( RESET == ( DELAY_TIME_BASE->SR & TIM_FLAG_Update ));
	DELAY_TIME_BASE->SR &= (uint32_t)( ~(uint32_t)TIM_FLAG_Update );
	DELAY_TIME_BASE->CR1 &= (uint32_t)( ~(uint32_t)0x01 );
}

/**
  * @brief :��ʱ(500Ms)
  * @param :
*			@Ms:��ʱ��500Ms����
  * @note  :������255
  * @retval:��
  */
void drv_delay_500Ms( uint8_t Ms_500 )
{
	while( Ms_500 -- )
	{
		drv_delay_ms( 50 );		//1 * 50ms
		drv_delay_ms( 50 );		//2 * 50ms
		drv_delay_ms( 50 );		//3 * 50ms
		drv_delay_ms( 50 );		//4 * 50ms
		drv_delay_ms( 50 );		//5 * 50ms
		drv_delay_ms( 50 );		//6 * 50ms
		drv_delay_ms( 50 );		//7 * 50ms
		drv_delay_ms( 50 );		//8 * 50ms
		drv_delay_ms( 50 );		//9 * 50ms
		drv_delay_ms( 50 );		//10 * 50ms = 500ms
	}
}

/**
  * @brief :������ʱ
  * @param :��
  * @note  :��
  * @retval:��
  */
void drv_delay_free( uint32_t Delay_Time )
{
	while( Delay_Time-- )
	{
	
	}
}
