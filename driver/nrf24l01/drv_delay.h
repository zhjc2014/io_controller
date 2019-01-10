/**
  ******************************************************************************
  * @author  ��ҫ�Ƽ� ASHINING
  * @version V3.0
  * @date    2016-10-08
  * @brief   DELAY����H�ļ�
  ******************************************************************************
  * @attention
  *
  * ����    :    http://www.ashining.com
  * �Ա�    :    https://shop105912646.taobao.com
  * ����Ͱ�:    https://cdzeyao.1688.com
  ******************************************************************************
  */


#ifndef __DRV_DELAY_H__
#define __DRV_DELAY_H__


#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"


/** ���ö�ʱ������ȷ��ʱ */
//��ʱӲ������
#define DELAY_TIME_BASE                    TIM1
#define DELAY_TIME_BASE_CLK                RCC_APB2Periph_TIM1


void drv_delay_init( void );
void drv_delay_us( uint16_t Us );
void drv_delay_ms( uint8_t Ms );
void drv_delay_500Ms( uint8_t Ms_500 );
void drv_delay_free( uint32_t Delay_Time );

#endif

