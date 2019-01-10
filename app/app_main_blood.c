#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "x_frame.h"
#include "math.h"
#include "param_cfg.h"
#include "bsp_ht1621b.h"
#include "bsp_dmp.h"

uint32_t flag_enable_cpf = 0;
const uint32_t debug_id = DEFAULT_DEBUG_DEV;
float g_blood_percent = 100;
int HOST_ADDR = BLOOD_ADDR;         //fixed private addr
int HOST_GADDR = NO_GADDR;

REG_DEBUG(debug_id);

ADD_LINK(PC_LINK_ID, PC_LINK_PRIO, PC_LINK_DEV, PC_LINK_PROTOCOL, 256, 0, 0);
ADD_LINK(SUB_LINK_ID, SUB_LINK_PRIO, SUB_LINK_DEV, SUB_LINK_PROTOCOL, 256, 0, 0);

float limit_val(float val, float min, float max)
{
    return (val > max) ? (max) : ((val < min) ? (min) : (val));
}

float remap(float val, float min, float max, float tag_min, float tag_max)
{
    float val_tag = val;
    float delt = (float) (tag_max - tag_min) / (max - min);

    val = limit_val(val, min, max);
    val_tag = (val - min) * delt + tag_min;
    return val_tag;
}

typedef struct {
    float bloodPercentage;
    float soc;
} ST_BLOOD_BAR_INFO;

int32_t p_send_func(uint8_t argc, uint8_t *argv[])
{
    hal_frame_info_t frame = {0};
//    char* str = "help\r\n";
    ST_BLOOD_BAR_INFO stBloodBarInfo;
    stBloodBarInfo.bloodPercentage = g_blood_percent;
    stBloodBarInfo.soc = 0x00;
    frame.link_id = SUB_LINK_ID;
    frame.cmd_func = CONTROL_CMD_SET;
    frame.cmd_id   = BLOOD_BAR_INFO;
    frame.src      = HOST_ADDR;
    frame.dest     = PC_ADDR;
    frame.pdata = (uint8_t*)&stBloodBarInfo;
    frame.data_len = sizeof stBloodBarInfo;
    frame.check_type = 0;
    protocol_send(&frame);
    return RET_OK;
}

void data_push_test(void)
{
    p_send_func(0, NULL);
}

REG_TASK(ID_TASK, 1000, data_push_test);

//REG_SHELL_CMD(p_send, 0, p_send_func, "protocol send!!", "protocol send \r\n ");

//int32_t         action_demo      (hal_frame_info_t* p_frame)
//{
//    DBG_I("action_demo!!");
//    return 0;
//}

//ADD_LINK_ACTION(SUB_LINK_ID, CONTROL_CMD_SET, BLOOD_BAR_INFO, action_demo);

//blood led show task
static void led_show_task(void)
{
    static int state = 0;
    static uint32_t blood = 0;
    switch (state) {
    case 0: //init state: blood led state dynamic increasing
        hal_app_write(DRV_BLOOD, (uint8_t*)&blood, sizeof(uint32_t), 0, NULL);
        if (++blood > LCD_SEG_BYTE_NUM) {
            state++;
        }
        break;
    case 1: //work state: blood led
        blood = LCD_SEG_BYTE_NUM * g_blood_percent / 100.0;
        hal_app_write(DRV_BLOOD, (uint8_t*)&blood, sizeof(uint32_t), 0, NULL);
        break;
    default:
        state = 0;
        break;
    }
}
REG_TASK(ID_TASK, 50, led_show_task);

static void mpu_read_task(void)
{
#define MPU_NUM_MAX    4
    enum {ACCX = 0, ACCY, ACCZ};
    int16_t acc[3] = {0};
    int i;
    char str[256] = {0};
    char _str[64] = {0};
    static int16_t lastAccz[MPU_NUM_MAX] = {0};
    static int16_t lastlastAccz[MPU_NUM_MAX] = {0};
    static const int MIN_HIT = 1000;
    static const int MAX_HIT = 3000;
    static const float MIN_DEC = 3;
    static const float MAX_DEC = 6;
    float bloodDec = 0;
    int16_t deta1[MPU_NUM_MAX];
    int16_t deta2[MPU_NUM_MAX];
    int16_t hit[MPU_NUM_MAX];
    int16_t maxHit;
    int16_t secondHit;
    int16_t detaHit;
    strcat(str, "mpu data: ");
    for (i = 0; i < MPU_NUM_MAX; i++) {
        hal_app_read(DRV_MPU6500, (uint8_t*)acc, sizeof(acc), i, NULL);
//        sprintf(_str, "%d %d %d ", acc[ACCX], acc[ACCY], acc[ACCZ]);
//        strcat(str, _str);

//        deta1 = abs(lastAccz[i] - lastlastAccz[i]);
//        deta2 = abs(lastAccz[i] - acc[2]);
//        hit = (deta1 + deta2) / 2;
//        sprintf(_str, "%d %d %d ", deta1, deta2, hit);
//        strcat(str, _str);
//        if (deta1 > MIN_HIT && deta2 > MIN_HIT){
//            if (g_blood_percent > 0){
//                bloodDec = remap(hit, MIN_HIT, MAX_HIT, MIN_DEC, MAX_DEC);
//                g_blood_percent -= bloodDec;
//            }
//        }
//        lastlastAccz[i] = lastAccz[i];
//        lastAccz[i] = acc[2];

        hit[i] = acc[ACCZ] - lastAccz[i];
        lastAccz[i] = acc[ACCZ];

//        sprintf(_str, "%d %d ", acc[ACCZ], hit[i]);
//        sprintf(_str, "%d ", hit[i]);
//        strcat(str, _str);
    }
    maxHit = 0;
    secondHit = 0;
    for (i = 0; i < MPU_NUM_MAX; i++) {
        if (hit[i] > maxHit) {
            maxHit = hit[i];
        }
    }
    for (i = 0; i < MPU_NUM_MAX; i++) {
        if (hit[i] > secondHit && hit[i] < maxHit) {
            secondHit = hit[i];
        }
    }
    detaHit = maxHit - secondHit;
    sprintf(_str, "%d %d %d ", maxHit, secondHit, detaHit);
    strcat(str, _str);

    if (detaHit > MIN_HIT ) {
        if (g_blood_percent > 0) {
            bloodDec = remap(detaHit, MIN_HIT, MAX_HIT, MIN_DEC, MAX_DEC);
            g_blood_percent -= bloodDec;
        }
    }
    DBG_I_STRP(str);
}
REG_TASK(ID_TASK, 20, mpu_read_task);


static void dmp_read_task(void)
{
    ST_DMP_DATA dmpData;
    hal_app_read(DRV_MPU6500_DMP, (uint8_t*)&dmpData, sizeof(dmpData), 0, NULL);
    DBG_I("%f, %f, %f", dmpData.pitchRowYaw[0], dmpData.pitchRowYaw[1], dmpData.pitchRowYaw[2]);
}
//REG_TASK(ID_TASK, 10, dmp_read_task);
