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

uint32_t flag_enable_cpf = 0;
const uint32_t debug_id = DEFAULT_DEBUG_DEV;
const int HOST_ADDR = NO_ADDR; //fixed private addr
int HOST_GADDR = CB_ADDR;      //variable global addr,
//the car will request for addr asignment after start up.
uint32_t nrf_lost_cnt = 0;

REG_DEBUG(debug_id);

// ADD_LINK(PC_LINK_ID, PC_LINK_PRIO, PC_LINK_DEV, PC_LINK_PROTOCOL, 512, 1, 0);
// ADD_LINK(RF_LINK_0_ID, RF_LINK_0_PRIO, RF_LINK_0_DEV, RF_LINK_0_PROTOCOL, 64, 1, 0);

 void status_led_trig(void)
 {
     static uint32_t led_state = 0;
     led_state = led_state ^ 0x01;
     hal_app_write(DRV_GPIO, (uint8_t *)&led_state, sizeof(uint32_t), gpio_led, NULL);
 }

static void can_info_read(void)
{
    int i;
    int len;
    //can info read write
    can_data_t rx_data[CAN_FIFO_SIZE];
    //clear can rx data buffer
    len = hal_app_read(DRV_CAN, (uint8_t *)rx_data, 0, 0, NULL);
    for (i = 0; i < len && i < CAN_FIFO_SIZE; i++)
    {
        if (rx_data[i].id >= DRIVER0_ADDR + 0x00 &&
                rx_data[i].id <= DRIVER0_ADDR + 0x03)
        {
            uint8_t cmd = rx_data[i].data[0];
            if (CAN_WARNING_DATA == cmd)
            {   //get motor warning state
                int index = rx_data[i].id - DRIVER0_ADDR;
//                memcpy(motor_error_code[index], &rx_data[i].data[1], 4);
            }
        }
    }
}

static void can_info_write(void)
{
    //can info write
    int i;
//    for (i = 0; i < MAX_MOTOR_NUM; i++)
//    {
//        can_data_t tx_data;
//        memset(&tx_data, 0, sizeof tx_data);
//        //contrl setting data
//        tx_data.id = HOST_GADDR + i / 2;
//        tx_data.len = 8;
//        tx_data.data[0] = CAN_CTRL_DATA;
//        tx_data.data[1] = ctrl_setting[i].channel;
//        tx_data.data[2] = ctrl_setting[i].mode;
//        tx_data.data[3] = ctrl_setting[i].paramIndex;
//        memcpy(&tx_data.data[4], &ctrl_setting[i].value, sizeof ctrl_setting[i].value);
//        hal_app_write(DRV_CAN, (uint8_t *)&tx_data, 0, 0, NULL);
//    }
    hal_app_ctrl(DRV_CAN, CAN_START_SEND, NULL, 0, NULL);
}

static void can_warning_clear(void)
{
    int i = 0;
//    for (i = 0; i < MAX_MOTOR_NUM; i++)
//    {
//        can_data_t tx_data;
//        memset(&tx_data, 0, sizeof tx_data);
//        //clear the warning
//        if (motor_error_code_clear_flag[i])
//        {
//            tx_data.id = HOST_GADDR + i / 2;
//            tx_data.len = 8;
//            tx_data.data[0] = CAN_WARNING_CLEAR;
//            tx_data.data[1] = i % 2;
//            hal_app_write(DRV_CAN, (uint8_t *)&tx_data, 0, 0, NULL);
//        }
//    }
    hal_app_ctrl(DRV_CAN, CAN_START_SEND, NULL, 0, NULL);
}

static void ctrl_task(void)
{
    static int state = 0;
    static int cnt = 0;
    static int led_flash_div = 100;
    nrf_lost_cnt++;
    if (nrf_lost_cnt > 200)
    {
        led_flash_div = 100;
    }
    else
    {
        led_flash_div = 500;
    }
    RUN_BY_LIMIT_BLOCK(led_flash_div, status_led_trig());
    RUN_BY_LIMIT_BLOCK(12, can_info_read());
    RUN_BY_LIMIT_BLOCK(14, can_info_write());
    RUN_BY_LIMIT_BLOCK(16, can_warning_clear());
    // RUN_BY_LIMIT_BLOCK(10, ctrl_setting_write());
    // RUN_BY_LIMIT_BLOCK(200, start_state_check());
    //watch dog reload
    hal_app_write(DRV_IWDG, NULL, 0, 0, NULL);
}
REG_TASK(ID_TASK, 2, ctrl_task);
