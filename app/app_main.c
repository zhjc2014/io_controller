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

#define EN_CONST_FREQ   1
#if EN_CONST_FREQ
#define CONST_FREQ      5
#endif

#define MAX_PWM 10000
#define MIN_PWM 0

#define MAX_DRIVER_NUM  4
#define MAX_MOTOR_NUM  (MAX_DRIVER_NUM*2)
//#define MOTOR_ALL_REVERSE (1 << 16)

#pragma pack(1)
typedef struct {
    uint16_t btnState;
    int16_t axisValue[6];
}game_controller_data_t;
#pragma pack()

 typedef struct{
    uint32_t second:5;
    uint32_t minute:6;
    uint32_t hour:5;
    uint32_t date:5;
    uint32_t month:4;
    uint32_t year:7; //1980+
}time_stamp_t;

typedef union{
    time_stamp_t time_stamp;
    uint32_t value;
}time_stamp_u;

typedef struct{
    time_stamp_u time_stamp_union;
    float blood_per;
    uint8_t startState;
    uint8_t soc;
}car_info_t;

typedef enum{
    COM_TYPE_OTHERS = 0,
    COM_TYPE_MAIN_BOARD,
    COM_TYPE_CENTER_BOARD,
    COM_TYPE_DRIVER_BOARD,
}com_type_e;

typedef struct{
    uint32_t com_type; 
    uint32_t car_type;
    uint32_t car_addr;
}pc_link_send_t;

typedef enum{
    red_push = 0,
    red_add,
    blue_push,
    blue_add, 
    motor5,
    motor6,
    motor7,
    motor8, 
}e_motor_number;

typedef enum{
    pwm_ctrl = 0,
    speed_ctrl,
    pos_ctrl,
}motor_ctrl_type_e;

//#pragma pack(1)
typedef struct{
    uint8_t channel;        //motor channel
    uint8_t mode;           //ctrl mode
    uint8_t paramIndex;     //motor param
    uint32_t value;         //control value
}motor_control_setting_t;
//#pragma pack()

car_info_t car_info;
uint32_t flag_enable_cpf = 0;
const uint32_t debug_id = DEFAULT_DEBUG_DEV;
const int HOST_ADDR = NO_ADDR;   //fixed private addr 
int HOST_GADDR = CB_ADDR;        //variable global addr, 
                                 //the car will request for addr asignment after start up.    
uint32_t nrf_lost_cnt = 0;
uint32_t rf_init_flag = 0;
uint32_t start_state_lost_cnt = 0;
static uint8_t freq_channel;
//static uint32_t motorControlState = 0;  
static motor_control_setting_t ctrl_setting[MAX_MOTOR_NUM];   //two channel, two motor
static uint16_t motor_error_code[MAX_DRIVER_NUM][2];          //two channel, two motor
static int motor_error_code_clear_flag[MAX_MOTOR_NUM] = {0x00};
static int motorControlState = 0;


REG_DEBUG(debug_id);

ADD_LINK(PC_LINK_ID,    PC_LINK_PRIO,   PC_LINK_DEV,  PC_LINK_PROTOCOL,     512,    1, 0);
ADD_LINK(RF_LINK_0_ID,  RF_LINK_0_PRIO, RF_LINK_0_DEV,  RF_LINK_0_PROTOCOL, 64,     1, 0);
ADD_LINK(RF_LINK_1_ID,  RF_LINK_1_PRIO, RF_LINK_1_DEV,  RF_LINK_1_PROTOCOL, 64,     1, 0);
ADD_LINK(RF_LINK_2_ID,  RF_LINK_2_PRIO, RF_LINK_2_DEV,  RF_LINK_2_PROTOCOL, 64,     1, 0);
ADD_LINK(RF_LINK_3_ID,  RF_LINK_3_PRIO, RF_LINK_3_DEV,  RF_LINK_3_PROTOCOL, 64,     1, 0);
ADD_LINK(RF_LINK_4_ID,  RF_LINK_4_PRIO, RF_LINK_4_DEV,  RF_LINK_4_PROTOCOL, 64,     1, 0);
ADD_LINK(RF_LINK_5_ID,  RF_LINK_5_PRIO, RF_LINK_5_DEV,  RF_LINK_5_PROTOCOL, 64,     1, 0);

static int32_t find_freqency_ack (hal_frame_info_t* p_frame_info)
{
    if (p_frame_info->need_ack){
        uint8_t data = 0;
        hal_frame_info_t frame = {0};
        frame.link_id = RF_LINK_0_ID;
        frame.data_len = sizeof data;
        frame.need_ack = 1;
        frame.check_type = CHECK_TYPE_16CRC;
        frame.src      = p_frame_info->dest;
        frame.dest     = p_frame_info->src;
        frame.cmd_func = COMM_CMD_SET;
        frame.cmd_id   = CMD_ID_HEART_BEAT;    
        frame.pdata = (uint8_t*)&data;
        protocol_send(&frame);
    }
    return 0;
}
ADD_LINK_ACTION(RF_LINK_0_ID, COMM_CMD_SET, CMD_ID_HEART_BEAT, find_freqency_ack);

//pc want to get frequency
static int32_t get_frequency_ack(hal_frame_info_t* p_frame_info)
{
    if (p_frame_info->need_ack){
        hal_frame_info_t frame = {0};
        frame.link_id = RF_LINK_0_ID;
        frame.data_len = sizeof freq_channel;
        frame.need_ack = 1;
        frame.check_type = CHECK_TYPE_16CRC;
        frame.src      = p_frame_info->dest;
        frame.dest     = p_frame_info->src;
        frame.cmd_func = p_frame_info->cmd_func;
        frame.cmd_id   = p_frame_info->cmd_id;    
        frame.pdata = (uint8_t*)&freq_channel;
        protocol_send(&frame);
    }
    DBG_I("%s", __FUNCTION__);
    return 0;
}
ADD_LINK_ACTION(PC_LINK_ID, CONTROL_CMD_SET, CTR_CMD_ID_GET_REQ, get_frequency_ack);

static int32_t joystick_data_get(hal_frame_info_t* p_frame_info)
{
    game_controller_data_t *raw_data;
    raw_data = (game_controller_data_t *)&p_frame_info->pdata[1];
    if (PC_ADDR == p_frame_info->src){
//		DBG_I("joystick data received, btn state is 0x%.4X", raw_data->btnState);
        if ((raw_data->btnState >> 8 & 0x03) == 0x03){
//	        DBG_I("two btn pressed");
//	        if (p_frame_info->pdata[0] == CAR1_ADDR || \
//	        	p_frame_info->pdata[0] == CAR3_ADDR){
            if (p_frame_info->pdata[0] == 0 ||
                p_frame_info->pdata[0] == 2){
//				DBG_I("car1 or car3 request relay work");
                motorControlState |= (1 << red_add);
            }else{
//				DBG_I("car2 or car4 request relay work");
                motorControlState |= (1 << blue_add);
            }
        }else{
//			DBG_I("not two btn pressed");
//			if (p_frame_info->pdata[0] == CAR1_ADDR || \
//				p_frame_info->pdata[0] == CAR3_ADDR){
            if (p_frame_info->pdata[0] == 0 ||
                p_frame_info->pdata[0] == 2){
                motorControlState &= ~(1 << red_add);
            }else{
                motorControlState &= ~(1 << blue_add);
            }
        }
    }
    return 0;
}
ADD_LINK_ACTION(PC_LINK_ID, CONTROL_CMD_SET, CTR_CMD_ID_JOYSTICK_DATA, joystick_data_get);

static int32_t get_start_state(hal_frame_info_t* p_frame_info)
{
    if (p_frame_info->src == PC_ADDR){
        int16_t pwm = 0;
        
        car_info_t *car_info_temp = (car_info_t*)(p_frame_info->pdata);
        //update the start state
        car_info.startState = car_info_temp->startState;
        start_state_lost_cnt = 0;
        if (car_info.startState){
            pwm = MAX_PWM;
            motorControlState |= (1 << red_push) | (1 << blue_push);
        }else{
            pwm = MIN_PWM;
            motorControlState &= ~((1 << red_push) | (1 << blue_push));
        }
        hal_app_write(DRV_PWM, (uint8_t*)&pwm, sizeof(pwm), PWM_RED_LED, NULL);     //battle field led ctrl
        hal_app_write(DRV_PWM, (uint8_t*)&pwm, sizeof(pwm), PWM_BLUE_LED, NULL);    //battle field led ctrl
    }
//    DBG_I("%s", __FUNCTION__);
    return 0;
}
ADD_LINK_ACTION(PC_LINK_ID, CONTROL_CMD_SET, CTR_CMD_ID_CAR_INFO_SET, get_start_state);

//for pc_link 
void pc_link_send(void)
{
    hal_frame_info_t frame = {0};
    pc_link_send_t data;
    data.car_addr = 0x00;
    data.car_type = 0x00;
    data.com_type = COM_TYPE_CENTER_BOARD;
    frame.link_id   = PC_LINK_ID;
    frame.cmd_func  = CONTROL_CMD_SET;
    frame.cmd_id    = CTR_CMD_ID_COM_TYPE_CHECK;
    frame.src       = CB_ADDR;
    frame.dest      = PC_ADDR;
    frame.pdata     = (uint8_t*)&data;
    frame.need_ack  = 0;
    frame.data_len  = sizeof(pc_link_send_t);
    frame.check_type = CHECK_TYPE_16CRC;
    protocol_send(&frame);
}
REG_TASK(LW_TASK, 100, pc_link_send);    //for test

static void change_nrf_frequency(void)
{
    #define MAX_FREQUENCY 125
    uint32_t random_num;
    if (!rf_init_flag){
        rf_init_flag = 1;
    #if !EN_CONST_FREQ
        hal_app_read(DRV_RANDOM, (uint8_t*)&random_num, sizeof(random_num), 0, NULL);
        freq_channel = random_num % (MAX_FREQUENCY + 1);
        DBG_I("freq_channel is %d", freq_channel);
        hal_app_ctrl(DRV_RF24L01_0, SET_NRF_FREQ, &freq_channel, sizeof(freq_channel), NULL);
        hal_app_ctrl(DRV_RF24L01_0, SET_NRF_RXMODE, NULL, 0, NULL);
        hal_app_ctrl(DRV_RF24L01_0, FLUSH_RX_BUFF, NULL, 0, NULL);
    #else
        freq_channel = CONST_FREQ; 
        hal_app_ctrl(DRV_RF24L01_0, SET_NRF_FREQ, &freq_channel, sizeof(freq_channel), NULL);
        hal_app_ctrl(DRV_RF24L01_0, SET_NRF_RXMODE, NULL, 0, NULL);
        hal_app_ctrl(DRV_RF24L01_0, FLUSH_RX_BUFF, NULL, 0, NULL);
    #endif        
    }
}
REG_TASK(ID_TASK, 500, change_nrf_frequency);

void status_led_trig(void)
{ 
    static uint32_t led_state = 0;
    led_state = led_state ^ 0x01;
    hal_app_write(DRV_GPIO, (uint8_t*)&led_state, sizeof(uint32_t), gpio_led, NULL);
}

static void can_info_read(void)
{
    int i;
    int len;
    //can info read write
    can_data_t rx_data[CAN_FIFO_SIZE];
    //clear can rx data buffer
    len = hal_app_read(DRV_CAN, (uint8_t *)rx_data, 0, 0, NULL);
    for (i = 0; i < len && i < CAN_FIFO_SIZE; i++){
        if (rx_data[i].id >= DRIVER0_ADDR + 0x00 && \
            rx_data[i].id  <= DRIVER0_ADDR + 0x03){
            uint8_t cmd = rx_data[i].data[0];
            if (CAN_WARNING_DATA == cmd){   //get motor warning state
                int index = rx_data[i].id - DRIVER0_ADDR;
                memcpy(motor_error_code[index], &rx_data[i].data[1], 4);
            }
        }
    }
}

static void can_info_write(void)
{
    //can info write 
    int i;
    for (i = 0; i < MAX_MOTOR_NUM; i++){
        can_data_t tx_data;
        memset(&tx_data, 0, sizeof tx_data);
        //contrl setting data
        tx_data.id = HOST_GADDR + i / 2;
        tx_data.len = 8;
        tx_data.data[0] = CAN_CTRL_DATA;
        tx_data.data[1] = ctrl_setting[i].channel;
        tx_data.data[2] = ctrl_setting[i].mode;
        tx_data.data[3] = ctrl_setting[i].paramIndex;
        memcpy(&tx_data.data[4], &ctrl_setting[i].value, sizeof ctrl_setting[i].value);
        hal_app_write(DRV_CAN, (uint8_t *)&tx_data, 0, 0, NULL);
    }
    hal_app_ctrl(DRV_CAN, CAN_START_SEND, NULL, 0, NULL);
}

static void can_warning_clear(void)
{
    int i = 0;
    for (i = 0; i < MAX_MOTOR_NUM; i++){
        can_data_t tx_data;
        memset(&tx_data, 0, sizeof tx_data);
        //clear the warning
        if (motor_error_code_clear_flag[i]){
            tx_data.id = HOST_GADDR + i / 2;
            tx_data.len = 8;
            tx_data.data[0] = CAN_WARNING_CLEAR;
            tx_data.data[1] = i % 2;
            hal_app_write(DRV_CAN, (uint8_t *)&tx_data, 0, 0, NULL);
        }
    }
    hal_app_ctrl(DRV_CAN, CAN_START_SEND, NULL, 0, NULL);
}

static void ctrl_setting_write(void)
{
    #define ERROR_CNT_THR   20
    #define ERROR_TIME_THR  3000
    static int state[MAX_MOTOR_NUM] = {0x00};
    static int cnt[MAX_MOTOR_NUM] = {0x00};
    static int lock[MAX_MOTOR_NUM] = {0x00};
    static int reverse_delay[MAX_MOTOR_NUM] = {0x00};
    
    static int stall_cnt[MAX_MOTOR_NUM] = {0x00};
    static int timer_cnt[MAX_MOTOR_NUM] = {0x00};
    static int error_flag[MAX_MOTOR_NUM] = {0x00};
    static int state_2[MAX_MOTOR_NUM] = {0x00};
    static const int expect_value[MAX_MOTOR_NUM][2] = {
        {1, -50},    //paramIndex & value
        {1, -90},
        {1, -50},
        {1, -90},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
    };
    int i = 0;
    //set motor ctrl setting value
    for(i = 0; i < MAX_MOTOR_NUM; i++){ 
        if (lock[i]) continue;
        if (motorControlState & (1 << i)){
            ctrl_setting[i].channel = i % 2;
            ctrl_setting[i].mode = speed_ctrl;
            ctrl_setting[i].paramIndex = expect_value[i][0];
            ctrl_setting[i].value = expect_value[i][1];
        }else{
            ctrl_setting[i].channel = i % 2;
            ctrl_setting[i].mode = pwm_ctrl;
            ctrl_setting[i].paramIndex = expect_value[i][0];
            ctrl_setting[i].value = 0;
        }
    }
    //stall check 
    for (i = 0; i < MAX_MOTOR_NUM; i++){
        //clear the error state
        if (!car_info.startState){
            timer_cnt[i] = 0;
            stall_cnt[i] = 0;
            state_2[i] = 0;
            lock[i] = 0;
            error_flag[i] = 0;
        }else{
        if (error_flag[i]) continue;
        switch (state[i]){
            case 0:
                if (0x0001 == motor_error_code[i/2][i%2]){
                    ctrl_setting[i].value = 0;  //prevent the stall happenning again
                    lock[i] = 1;
                    cnt[i] = 0;
                    stall_cnt[i]++;
                    if (stall_cnt[i] > ERROR_CNT_THR){
                        error_flag[i] = 1;
                    }else {
                        motor_error_code_clear_flag[i] = 1;
                        state[i]++;
                    }
                    DBG_I("stalled");
                }
                break;
            case 1:
                if (0x0000 == motor_error_code[i/2][i%2]){
                    //reverse speed 
                    motor_error_code_clear_flag[i] = 0;
                    ctrl_setting[i].value = -expect_value[i][1];
                    state[i]++;
                    cnt[i] = 0;
                    if (expect_value[i][1] != 0){
                        reverse_delay[i] = 20 * 30 * 1000.0 / (abs(expect_value[i][1]) * 60); //30 is deg 
                    }
                    DBG_I("stall cleared");
                }
                break;
            case 2:                                                                                                             
                if (++cnt[i] > reverse_delay[i] / 10){    //reverse delay
                    if ((0x0001 == motor_error_code[i/2][i%2])){    //if reverse stalled 
                        ctrl_setting[i].value = 0;                  //prevent the stall happenning again
                        motor_error_code_clear_flag[i] = 1;         //clear the stall flag
                        state[i]++;
                        if (blue_push == i){
                            DBG_I("reverse stalled");
                        }
                    }else{
                        //return normal state
                        lock[i] = 0;
                        state[i] = 0;
                        cnt[i] = 0;
                    }
                }
                break;
            case 3:
                if (0x0000 == motor_error_code[i/2][i%2]){
                    motor_error_code_clear_flag[i] = 0;
                    //return normal state
                    lock[i] = 0;
                    state[i] = 0;
                    cnt[i] = 0;
                    
                    if (blue_push == i){
                        DBG_I("reverse stalled cleared, return to normal mode");
                    }
                }
                break;
        }
        //stalled 10 times within 20sec 
        switch (state_2[i]){
            case 0:
                if (stall_cnt[i]){
                    timer_cnt[i] = 0;
                    state_2[i]++;
                }
                break;
            case 1:
                timer_cnt[i]++;
                if (timer_cnt[i] > ERROR_TIME_THR && \
                    stall_cnt[i] <=  ERROR_CNT_THR){
                    state_2[i] = 0;
                    timer_cnt[i] = 0;
                    stall_cnt[i] = 0;
                }
                break;
            
        }
        }
    }
}

static void start_state_check(void)
{
    if (++start_state_lost_cnt > 10){
        motorControlState &= ~((1 << red_push) | (1 << blue_push));
    }
}

static void ctrl_task(void)
{
    static int state = 0;
    static int cnt = 0;
    uint32_t led_flash_div = 100;
    nrf_lost_cnt++;
    if (nrf_lost_cnt > 200){
        led_flash_div = 100;
    }else{
        led_flash_div = 500;
    }
    RUN_BY_LIMIT_BLOCK(led_flash_div, status_led_trig());
    RUN_BY_LIMIT_BLOCK(12,  can_info_read());
    RUN_BY_LIMIT_BLOCK(14,  can_info_write());
    RUN_BY_LIMIT_BLOCK(16,  can_warning_clear());
    RUN_BY_LIMIT_BLOCK(10,  ctrl_setting_write());
    RUN_BY_LIMIT_BLOCK(200,  start_state_check());
    //watch dog reload
    hal_app_write(DRV_IWDG, NULL, 0, 0, NULL);
}
REG_TASK(ID_TASK, 2, ctrl_task);

