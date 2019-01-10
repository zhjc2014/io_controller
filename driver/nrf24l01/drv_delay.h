/**
  ******************************************************************************
  * @author  泽耀科技 ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   DELAY配置H文件
  ******************************************************************************
  * @attention
  *
  * 官网    :    http://www.ashining.com
  * 淘宝    :    https://shop105912646.taobao.com
  * 阿里巴巴:    https://cdzeyao.1688.com
  ******************************************************************************
  */


#ifndef __DRV_DELAY_H__
#define __DRV_DELAY_H__


#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"


/** 采用定时器做精确延时 */
//延时硬件定义
#define DELAY_TIME_BASE                    TIM1
#define DELAY_TIME_BASE_CLK                RCC_APB2Periph_TIM1


void drv_delay_init( void );
void drv_delay_us( uint16_t Us );
void drv_delay_ms( uint8_t Ms );
void drv_delay_500Ms( uint8_t Ms_500 );
void drv_delay_free( uint32_t Delay_Time );

#endif

