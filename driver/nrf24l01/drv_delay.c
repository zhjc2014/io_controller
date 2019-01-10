/**
  ******************************************************************************
  * @author  泽耀科技 ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   DELAY配置C文件
  ******************************************************************************
  * @attention
  *
  * 官网	:	http://www.ashining.com
  * 淘宝	:	https://shop105912646.taobao.com
  * 阿里巴巴:	https://cdzeyao.1688.com
  ******************************************************************************
  */



#include "drv_delay.h"


/**
  * @brief :延时初始化
  * @param :无
  * @note  :定时器默认初始化成us级计数
  * @retval:无
  */
void drv_delay_init( void )
{
	TIM_TimeBaseInitTypeDef	 TimerInitStructer;
	RCC_ClocksTypeDef RCC_ClocksStatus;
	
	//获取系统时钟
	RCC_GetClocksFreq(&RCC_ClocksStatus);
	
	//使能TIM1时钟
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
  * @brief :延时(us)
  * @param :
*			@Us:延时的us数
  * @note  :不超过65535
  * @retval:无
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
  * @brief :延时(ms)
  * @param :
  *			@Ms:延时的Ms数
  * @note  :不超过65
  * @retval:无
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
  * @brief :延时(500Ms)
  * @param :
*			@Ms:延时的500Ms倍数
  * @note  :不超过255
  * @retval:无
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
  * @brief :自由延时
  * @param :无
  * @note  :无
  * @retval:无
  */
void drv_delay_free( uint32_t Delay_Time )
{
	while( Delay_Time-- )
	{
	
	}
}
