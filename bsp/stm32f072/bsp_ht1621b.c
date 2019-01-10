#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "bsp_ht1621b.h"

void delay_us(uint32_t nus)
{
//	uint32_t i = 0;
//	uint32_t j = 0;
//	for(i=0;i<nus;i++)
//	{
//		for(j=0;j<10;j++);
//	}
}

union _LCD_RAM_DATA lcd_ram;

void lcd_ram_set_all(uint8_t on_off)
{
    uint8_t i;
    for (i = 0; i < LCD_SEG_BYTE_NUM; i++) {
        lcd_ram.data_array[i] = on_off;
    }
}


void lcd_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE );

    GPIO_InitStructure.GPIO_Pin = LCD_CS_PIN | LCD_RD_PIN | LCD_WR_PIN | LCD_DATA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT ;   //ÍÆÍìÊä³ö
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_PIN_PORT, &GPIO_InitStructure);
}

void ht1621b_init(void)
{
    lcd_gpio_init();

    ht1621b_send_command(CMD_SYS_DIS);
    ht1621b_send_command(0x08);
    ht1621b_send_command(0x20); //set lcd 1/3 bias, 1/4 duty  CMD_BIAS3_COM4
    ht1621b_send_command(CMD_SYS_EN);
    ht1621b_send_command(CMD_LCD_ON);
    ht1621b_send_command(0xAf);

    lcd_ram_set_all(0x0);
    ht1621b_write_continue(0, lcd_ram.data_array, LCD_SEG_BYTE_NUM);
}

void ht1621b_write_continue(uint8_t addr, uint8_t *p_data, uint8_t cnt)
{
    uint8_t i;

    LCD_CS_L;

    ht1621b_send_high_bit(CMD_WRITE, 3); //write "101" bit
    ht1621b_send_high_bit(addr << 1, 7); //write 6 bits of addr

    for (i = 0; i < cnt; i++) {
        ht1621b_send_low_bit(*p_data, 8);
    }

    LCD_CS_H;
}


void ht1621b_write_half_byte(uint8_t addr, uint8_t w_data)
{
    LCD_CS_L;

    ht1621b_send_high_bit(CMD_WRITE, 3); //write "101" bit
    ht1621b_send_high_bit(addr << 1, 7); //write 6 bits of addr
    ht1621b_send_low_bit(w_data, 4); //write low 4 bits of data

    LCD_CS_H;
}

static void ht1621b_send_command(uint8_t command)
{
    LCD_CS_L;

    ht1621b_send_high_bit(0x80, 3); //write "100" bit
    ht1621b_send_high_bit(command, 9); //write command and last bit "X"

    LCD_CS_H;
}

static void ht1621b_send_high_bit(uint8_t w_data, uint8_t b_cnt)
{
    uint8_t i;

    for (i = 0; i < b_cnt; i++) {

        LCD_WR_L;

        if ((w_data & 0x80) == 0) {
            LCD_DA_L;
        } else {
            LCD_DA_H;
        }



        delay_us(10);

        LCD_WR_H;

        w_data <<= 1;

        delay_us(10);
    }
}

static void ht1621b_send_low_bit(uint8_t w_data, uint8_t b_cnt)
{
    uint8_t i;

    for (i = 0; i < b_cnt; i++) {
        if ((w_data & 0x01) == 0) {
            LCD_DA_L;
        } else {
            LCD_DA_H;
        }

        LCD_WR_L;

        delay_us(10);

        LCD_WR_H;

        w_data >>= 1;

        delay_us(10);

    }
}


//REG_INIT_MOD(ht1621b_init);




void show_led(uint8_t level)
{
    uint8_t cur_level = 0;
    uint8_t i = 0;
    uint8_t is_resv = 1;      //is blood bar reversed
    float delt = 30.0f / 100.0f;

    cur_level = level;
//    cur_level = (uint8_t)(level * delt);

//    if(cur_level < 10) {
//        ht1621b_send_command(0x09);  //ÉÁË¸£¬±¨¾¯
//    } else {
//        ht1621b_send_command(0x08);
//    }


    //if(level>100){cur_level = 30;}
    for(i = 0; i <= 30; i++) {
        uint8_t cur_val = 0;


        if(is_resv) {
            if(i <= cur_level) {
                if(i <= 10) {
                    cur_val = 0x04;
                } else if(i > 20) {
                    cur_val = 0x01;
                } else          {
                    cur_val = 0x02;
                }
            }
            ht1621b_write_half_byte(((30 - i) << 1), cur_val);
        } else {
            if(i <= cur_level) {
                if(i < 10) {
                    cur_val = 1;
                } else if(i >= 20) {
                    cur_val = 0x04;
                } else         {
                    cur_val = 0x02;
                }
            }
            ht1621b_write_half_byte((i << 1), cur_val);
        }


    }


}


int32_t lcd_w(uint8_t argc, uint8_t *argv[])
{
    if(argc >= 2) {
        uint32_t reg = 0;
        uint32_t read_len = 0;
        uint32_t addr = 0;
        uint8_t i = 0;

        uint8_t dat[4] = {0};
        sscanf((const char*)argv[1], "%d", &addr);
        sscanf((const char*)argv[2], "%x", &reg);

        //sscanf((const char*)argv[3],"%d",&read_len);

        DBG_I();
        lcd_ram.data_array[addr] = reg;
        ht1621b_write_continue(0, lcd_ram.data_array, LCD_SEG_BYTE_NUM);




        show_buf(lcd_ram.data_array, 30);

    } else {
        DBG_W("warn the lcd_w param is error: eg:lcd_w addr reg len");
    }
    return RET_OK;
}

REG_SHELL_CMD(lcd_w, 0, lcd_w, "lcd_w", "eg:lcd_w addr reg len\r\n ");


int32_t lcd_clr(uint8_t argc, uint8_t *argv[])
{
    if(argc >= 1) {
        uint32_t reg = 0;
        sscanf((const char*)argv[1], "%x", &reg);

        lcd_ram_set_all(reg);
        ht1621b_write_continue(0, lcd_ram.data_array, LCD_SEG_BYTE_NUM);


        DBG_W("set all  the lcd_clr :0x%x ", reg);

    } else {
        DBG_W("warn the lcd_clr param is error: eg:lcd_clr flag\r\n ");
    }
    return RET_OK;
}

REG_SHELL_CMD(lcd_clr, 0, lcd_clr, "lcd_clr", "eg:lcd_clr flag\r\n ");



int32_t lcd_wh(uint8_t argc, uint8_t *argv[])
{
    if(argc >= 1) {
        uint32_t addr = 0;
        uint32_t val = 0;
        sscanf((const char*)argv[1], "%x", &addr);
        sscanf((const char*)argv[2], "%x", &val);

        ht1621b_write_half_byte((uint8_t)addr, (uint8_t)val);


        DBG_W("ht1621b_write_half_byte addr:0x%x  val:0x%x ", addr, val);

    } else {
        DBG_W("warn the lcd_clr param is error: eg:lcd_clr flag\r\n ");
    }
    return RET_OK;
}

REG_SHELL_CMD(lcd_wh, 0, lcd_wh, "lcd_wh", "eg:lcd_wh addr val\r\n ");



int32_t led(uint8_t argc, uint8_t *argv[])
{
    if(argc >= 1) {
        uint32_t val = 0;
        sscanf((const char*)argv[1], "%d", &val);


        show_led(val);


        DBG_W("led   val:%d ", val);

    } else {
        DBG_W("warn the lcd_clr param is error: eg:lcd_clr flag\r\n ");
    }
    return RET_OK;
}

REG_SHELL_CMD(led, 0, led, "lcd_wh", "eg:lcd_wh addr val\r\n ");
