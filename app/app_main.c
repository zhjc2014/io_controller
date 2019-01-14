#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"
#include "math.h"
#include "param_cfg.h"
#include "math_lib.h"
#include "move_ctrl.h"
#include "bsp_can.h"

enum{
    INDEX_0 = 0,
    INDEX_1,
    INDEX_2,
    INDEX_3,
    INDEX_MAX
};

uint32_t flag_enable_cpf = 0;
const uint32_t debug_id = DEFAULT_DEBUG_DEV;
//the car will request for addr asignment after start up.
uint32_t can_lost_cnt = 0;

uint32_t can_addr = 0x00;
uint32_t g_index = INDEX_0;
REG_DEBUG(debug_id);

// ADD_LINK(PC_LINK_ID, PC_LINK_PRIO, PC_LINK_DEV, PC_LINK_PROTOCOL, 512, 1, 0);
// ADD_LINK(RF_LINK_0_ID, RF_LINK_0_PRIO, RF_LINK_0_DEV, RF_LINK_0_PROTOCOL, 64, 1, 0);

void set_can_filter(void)
{
    const int can_addr_bit_num = 4;
    uint32_t gpio_state;
    int i;
    for (i = 0; i < can_addr_bit_num; i++){
        hal_app_read(DRV_GPIO, (uint8_t *)&gpio_state, sizeof(uint32_t), gpio_canaddr_bit0 + i, NULL);
        if (gpio_state){
            can_addr |= 1<<i;
        }
    }
    can_addr += IO_BOARD_ADDR;
    hal_app_ctrl(DRV_CAN, CAN_SET_FILTER, (uint8_t*)&can_addr, sizeof(can_addr), NULL);
}
REG_INIT_MOD(set_can_filter);

void status_led_trig(void)
{
    static uint32_t led_state = 0;
    led_state = led_state ^ 0x01;
    hal_app_write(DRV_GPIO, (uint8_t *)&led_state, sizeof(uint32_t), gpio_led, NULL);
}

static void set_io_status(int index)
{
    int i;
    const int max_magician_per_line = 9;
    uint32_t io1_status;
    uint32_t io2_status;
    switch(index){
        case INDEX_0:
        io1_status = 1;
        io2_status = 1;
        for (i = 0; i < max_magician_per_line; i++){
            hal_app_write(DRV_GPIO, (uint8_t *)&io1_status, sizeof(uint32_t), gpio_magicain1_io1 + 2 * i, NULL);
            hal_app_write(DRV_GPIO, (uint8_t *)&io2_status, sizeof(uint32_t), gpio_magicain1_io2 + 2 * i, NULL);
        }
        break;
        case INDEX_1:    
        io1_status = 0;
        io2_status = 0;
        for (i = 0; i < max_magician_per_line; i++){
            hal_app_write(DRV_GPIO, (uint8_t *)&io1_status, sizeof(uint32_t), gpio_magicain1_io1 + 2 * i, NULL);
            hal_app_write(DRV_GPIO, (uint8_t *)&io2_status, sizeof(uint32_t), gpio_magicain1_io2 + 2 * i, NULL);
        }
        break;
        case INDEX_2:
        io1_status = 1;
        io2_status = 0;
        for (i = 0; i < max_magician_per_line; i++){
            hal_app_write(DRV_GPIO, (uint8_t *)&io1_status, sizeof(uint32_t), gpio_magicain1_io1 + 2 * i, NULL);
            hal_app_write(DRV_GPIO, (uint8_t *)&io2_status, sizeof(uint32_t), gpio_magicain1_io2 + 2 * i, NULL);
        }
        break;
        case INDEX_3:
        io1_status = 0;
        io2_status = 1;
        for (i = 0; i < max_magician_per_line; i++){
            hal_app_write(DRV_GPIO, (uint8_t *)&io1_status, sizeof(uint32_t), gpio_magicain1_io1 + 2 * i, NULL);
            hal_app_write(DRV_GPIO, (uint8_t *)&io2_status, sizeof(uint32_t), gpio_magicain1_io2 + 2 * i, NULL);
        }
        break;
        default:
        break;
    }
}

static void can_info_read(void)
{
    int i;
    int len;
    uint32_t index;
    static uint32_t last_index = INDEX_0;
    //can info read write
    can_data_t rx_data[CAN_FIFO_SIZE];
    //clear can rx data buffer
    len = hal_app_read(DRV_CAN, (uint8_t *)rx_data, 0, 0, NULL);
    for (i = 0; i < len && i < CAN_FIFO_SIZE; i++)
    {  
        can_lost_cnt = 0;
        if (rx_data[i].data[0] == 0xAA && rx_data[i].data[1] == 0xBB){
            index = rx_data[i].data[2];
            if (last_index != index && index < INDEX_MAX){
                // set_io_status(index);
                g_index = index;
                last_index = index;
            }
        }
        DBG_I("received data is ");
        DBG_I("g_index is %d", g_index);
        show_buf(rx_data[i].data, 3);
    }
}

// static void can_info_write(void)
// {
//     int i;
//     for (i = 0; i < 10; i++)
//     {
//         can_data_t tx_data;
//         memset(&tx_data, 0, sizeof tx_data);
//         //contrl setting data
//         tx_data.id = can_addr;
//         tx_data.len = 8;
//         tx_data.data[0] = 0xAA;
//         tx_data.data[1] = 0xBB;
//         tx_data.data[2] = i;        
//         hal_app_write(DRV_CAN, (uint8_t *)&tx_data, 0, 0, NULL);
//     }
//     hal_app_ctrl(DRV_CAN, CAN_START_SEND, NULL, 0, NULL);
// }

static void can_warning_clear(void)
{
//     int i = 0;
//     for (i = 0; i < MAX_MOTOR_NUM; i++)
//     {
//         can_data_t tx_data;
//         memset(&tx_data, 0, sizeof tx_data);
//         //clear the warning
//         if (motor_error_code_clear_flag[i])
//         {
//             tx_data.id = HOST_GADDR + i / 2;
//             tx_data.len = 8;
//             tx_data.data[0] = CAN_WARNING_CLEAR;
//             tx_data.data[1] = i % 2;
//             hal_app_write(DRV_CAN, (uint8_t *)&tx_data, 0, 0, NULL);
//         }
//     }
//     hal_app_ctrl(DRV_CAN, CAN_START_SEND, NULL, 0, NULL);
// }
}

static void ctrl_task(void)
{ 
    const int max_lost_cnt = 200;
    const int led_lost_div = 100;
    const int led_conn_div = 500;

    static int led_flash_div = led_lost_div;
   
    RUN_BY_LIMIT_BLOCK(10, can_lost_cnt++);
    if (can_lost_cnt > max_lost_cnt) 
    {
        led_flash_div = led_lost_div;
    }
    else
    {
        led_flash_div = led_conn_div;
    }
    RUN_BY_LIMIT_BLOCK(led_flash_div, status_led_trig());
    RUN_BY_LIMIT_BLOCK(12, can_info_read());
    // RUN_BY_LIMIT_BLOCK(14, can_info_write());
    RUN_BY_LIMIT_BLOCK(16, can_warning_clear());
    RUN_BY_LIMIT_BLOCK(20, set_io_status(g_index));
    //watch dog reload
    RUN_BY_LIMIT_BLOCK(50, hal_app_write(DRV_IWDG, NULL, 0, 0, NULL));
}
REG_TASK(ID_TASK, 2, ctrl_task);
