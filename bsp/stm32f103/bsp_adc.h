#ifndef __BSP_ADC_H__
#define __BSP_ADC_H__
#include <stdint.h>

void      bsp_adc_init  (void);
uint8_t   bsp_adc_write (uint32_t item, uint8_t val);
uint16_t  bsp_adc_read  (uint32_t item);

#endif //__BSP_ADC_H__
