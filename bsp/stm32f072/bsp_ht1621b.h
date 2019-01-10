#ifndef __BSP_HT1621B_H__
#define __BSP_HT1621B_H__

#include "stm32f0xx.h"
#include "x_frame.h"

#define LCD_SEG_BYTE_NUM 30

#define CMD_READ  0xc0
#define CMD_WRITE 0xa0
#define CMD_READ_MODIFY_WRITE 0x05

#define CMD_SYS_DIS     0x00
#define CMD_SYS_EN      0x01
#define CMD_LCD_OFF     0x02
#define CMD_LCD_ON      0x03
#define CMD_TIMER_DIS   0x04
#define CMD_WDT_DIS     0x05
#define CMD_TIMER_EN    0x06
#define CMD_WDT_EN      0x07

#define CMD_BIAS2_COM2  0x20
#define CMD_BIAS2_COM3  0x24
#define CMD_BIAS2_COM4  0x28

#define CMD_BIAS3_COM2  0x21
#define CMD_BIAS3_COM3  0x25
#define CMD_BIAS3_COM4  0x29

#define LCD_CS_PIN      GPIO_Pin_4
#define LCD_WR_PIN      GPIO_Pin_6
#define LCD_RD_PIN      GPIO_Pin_5
#define LCD_DATA_PIN    GPIO_Pin_7

#define LCD_PIN_PORT    GPIOB

#define LCD_CS_H    GPIO_SetBits(LCD_PIN_PORT, LCD_CS_PIN);
#define LCD_CS_L  GPIO_ResetBits(LCD_PIN_PORT, LCD_CS_PIN);

#define LCD_RD_H    GPIO_SetBits(LCD_PIN_PORT, LCD_RD_PIN);
#define LCD_RD_L  GPIO_ResetBits(LCD_PIN_PORT, LCD_RD_PIN);

#define LCD_WR_H    GPIO_SetBits(LCD_PIN_PORT, LCD_WR_PIN);
#define LCD_WR_L  GPIO_ResetBits(LCD_PIN_PORT, LCD_WR_PIN);

#define LCD_DA_H    GPIO_SetBits(LCD_PIN_PORT, LCD_DATA_PIN);
#define LCD_DA_L  GPIO_ResetBits(LCD_PIN_PORT, LCD_DATA_PIN);

#define MASTER_CAPACITY_PERCENT_ADDR 0x04
#define SLAVE_CAPACITY_PERCENT_ADDR  0x08

#define DISCHARGE_REMAINING_TIME_ADDR 0x16
#define CHARGE_REMAINING_TIME_ADDR 0x12

#define POWER_ADDR 0x0c



union _LCD_RAM_DATA{
  uint8_t data_array[LCD_SEG_BYTE_NUM];  
}; //union _LCD_RAM_DATA



void ht1621b_init(void);
void lcd_ram_set_all(uint8_t on_off);
void ht1621b_write_continue(uint8_t addr, uint8_t *p_data, uint8_t cnt);
static void ht1621b_write_half_byte(uint8_t addr, uint8_t w_data);
static void ht1621b_send_high_bit(uint8_t w_data, uint8_t b_cnt);
static void ht1621b_send_low_bit(uint8_t w_data, uint8_t b_cnt);
static void ht1621b_send_command(uint8_t command);
static void lcd_gpio_config(void);

void show_led(uint8_t level);

#endif //__BSP_HT1621B_H__
