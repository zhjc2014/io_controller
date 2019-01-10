#ifndef __APP_DRIVER_CFG_H__
#define __APP_DRIVER_CFG_H__


enum
{
    DRV_SYSTICK  = 0,   /* sys tick 必须为0*/  //系统时钟是必须的一个驱动设备，必须注册
    // DRV_UART1,
    // DRV_UART2,
    // DRV_UART3,
    // DRV_WL_UART,
    // DRV_USB_VCP,
    // DRV_USB_HID,
    DRV_CAN,
    DRV_RTT,
    // DRV_FALSH,
    DRV_LED,
    DRV_SYS,
    // DRV_KEY,
    // DRV_ADC,
    DRV_GPIO,
    DRV_IWDG,
    // DRV_PWM,
    // DRV_BAT,
    // DRV_RANDOM,
    // DRV_RF24L01_0,
    // DRV_RF24L01_1,
    // DRV_RF24L01_2,
    // DRV_RF24L01_3,
    // DRV_RF24L01_4,
    // DRV_RF24L01_5,
};

enum
{
    gpio_led    = 0,
};

enum
{
    PWM_RED_LED = 0,
    PWM_BLUE_LED,    
};

//是否开启对应驱动
#define ENABLE_SYS_TICK_DRV         1 
#if (DEFAULT_PC_LINK_DEV == DRV_UART1)
    // #define ENABLE_UART_1_DRV           1 
    // #define ENABLE_USB_VCP_DRV          0
    #define ENABLE_CAN                  1
#else
    #define ENABLE_UART_1_DRV           0 
    #define ENABLE_USB_VCP_DRV          1
    #define ENABLE_CAN                  0
#endif
// #define ENABLE_UART_2_DRV           0 
// #define ENABLE_UART_3_DRV           0 
// #define ENABLE_USB_HID_DRV          0
#define ENABLE_RTT_DRV              1
// #define ENABLE_FALSH_DRV            1
#define ENABLE_SYS_DRV              1
#define ENABLE_LED_DRV              1
// #define ENABLE_KEY_DRV              0
// #define ENABLE_ADC_DRV              0
#define ENABLE_GPIO_DRV             1
#define ENABLE_IWDG_DRV             1
// #define ENABLE_PWM_DRV              1
// #define ENABLE_BAT_DRV              0
// #define ENABLE_RANDOM_DRV           1
// #define ENABLE_RF24L01_0_DRV        1
// #define ENABLE_RF24L01_1_DRV        1
// #define ENABLE_RF24L01_2_DRV        1
// #define ENABLE_RF24L01_3_DRV        1
// #define ENABLE_RF24L01_4_DRV        1
// #define ENABLE_RF24L01_5_DRV        1


#define FLASH_START_ADDRESS         (0x8000000)
#define FLASH_BASE_ADDRESS          (36*1024)   //loader size 32K
#define FLASH_LIMIT_MAX_SIZE        (128*1024)  //flash limit max
#define FLASH_BLOCK_SIZE            (2048)      //2K

//0x8010000

#define MIN_FLASH_ADDRESS = FLASH_START_ADDRESS + FLASH_BASE_ADDRESS;
#define MAX_FLASH_ADDRESS = FLASH_START_ADDRESS + FLASH_LIMIT_MAX_SIZE;

/*
 
    +------------------------------ 128K  FLASH -----------------------------------+
   /                                                                                \
  +-----------------+----+----+----+------------------------------------+------------+
  |       32K       | 2K | 2K | 2K |              80K                   |    10K     |
  +-------+---------+-+--+--+-+--+-+------------+-----------------------+-----+------+
          |          /      |     \             |                             |
     bootloader     /       |      \           APP                            |
                   /    bar_code    \                                      recorder
     boot_param --+                  +---app_param
      
*/


#define FLASH_BOOT_PARAM_SIZE       (FLASH_BLOCK_SIZE)    //loader-param size 2K
#define FLASH_BAR_CODE_SIZE         (FLASH_BLOCK_SIZE)    //loader-param size 2K
#define FLASH_APP_PARAM_SIZE        (FLASH_BLOCK_SIZE)    //loader-param size 2K
#define FLASH_APP_SIZE              (38*FLASH_BLOCK_SIZE) //loader-param size 80K
#define FLASH_REC_SIZE              (5*FLASH_BLOCK_SIZE)  //loader-param size 10K

#define FLASH_BOOT_PARAM_ADDR       (FLASH_START_ADDRESS   + FLASH_BASE_ADDRESS)
#define FLASH_BAR_CODE_ADDR         (FLASH_BOOT_PARAM_ADDR + FLASH_BOOT_PARAM_SIZE)
#define FLASH_APP_PARAM_ADDR        (FLASH_BAR_CODE_ADDR   + FLASH_BAR_CODE_SIZE)
#define FLASH_APP_ADDR              (FLASH_APP_PARAM_ADDR  + FLASH_APP_PARAM_SIZE)
#define FLASH_REC_ADDR              (FLASH_APP_ADDR        + FLASH_APP_SIZE)

#define ERASE_FLASH_CMD         0x01

#define SYS_RESET_CMD           0x01
#define SYS_JUMP_CMD            0x02
#define SYS_IAP_APP_INIT_CMD    0x03

#define SET_NRF_FREQ            0x01
#define SET_NRF_CHANNEL         0x02
#define SET_NRF_RXMODE          0x03
#define FLUSH_RX_BUFF           0x04

#define CAN_START_SEND          0x01

#endif //__APP_DRIVER_CFG_H__
