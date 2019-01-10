#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"
#include "spi.h"
#include "mpu6500.h"

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} Vector3i;

typedef struct {
    float x;
    float y;
    float z;
} Vector3f;

#define INT16_VAL(v, idx) ((int16_t)(((uint16_t)v[2*idx] << 8) | v[2*idx+1]))

Vector3f g_MPU6500_accel_hor;
Vector3f g_MPU6500_gyro_hor;

#define MPU6500_SPI 1

#define MPU1_CS_PORT    GPIOB
#define MPU1_CS_PIN     GPIO_Pin_0

#define MPU2_CS_PORT    GPIOB
#define MPU2_CS_PIN     GPIO_Pin_2

#define MPU3_CS_PORT    GPIOB
#define MPU3_CS_PIN     GPIO_Pin_11

#define MPU4_CS_PORT    GPIOB
#define MPU4_CS_PIN     GPIO_Pin_13

#define MPU1_CS_H     GPIO_SetBits(MPU1_CS_PORT, MPU1_CS_PIN);
#define MPU1_CS_L   GPIO_ResetBits(MPU1_CS_PORT, MPU1_CS_PIN);

#define MPU2_CS_H     GPIO_SetBits(MPU2_CS_PORT, MPU2_CS_PIN);
#define MPU2_CS_L   GPIO_ResetBits(MPU2_CS_PORT, MPU2_CS_PIN);

#define MPU3_CS_H     GPIO_SetBits(MPU3_CS_PORT, MPU3_CS_PIN);
#define MPU3_CS_L   GPIO_ResetBits(MPU3_CS_PORT, MPU3_CS_PIN);

#define MPU4_CS_H     GPIO_SetBits(MPU4_CS_PORT, MPU4_CS_PIN);
#define MPU4_CS_L   GPIO_ResetBits(MPU4_CS_PORT, MPU4_CS_PIN);

#define MPU1_INT_PORT  GPIOB
#define MPU1_INT_PIN   GPIO_Pin_1
#define MPU1_INT_EXTI  EXTI_Line1


#define MPU2_INT_PORT  GPIOB
#define MPU2_INT_PIN   GPIO_Pin_10
#define MPU2_INT_EXTI  EXTI_Line10


#define MPU3_INT_PORT  GPIOB
#define MPU3_INT_PIN   GPIO_Pin_12
#define MPU3_INT_EXTI  EXTI_Line12


#define MPU4_INT_PORT  GPIOB
#define MPU4_INT_PIN   GPIO_Pin_14
#define MPU4_INT_EXTI  EXTI_Line14

typedef struct {
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
    uint32_t EXTI_Line;
} ST_GPIO_EXTI;

typedef struct {
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
} ST_GPIO;


static const ST_GPIO_EXTI stGpioInt[] = {
    {MPU1_INT_PORT, MPU1_INT_PIN,  MPU1_INT_EXTI  },
    {MPU2_INT_PORT, MPU2_INT_PIN,  MPU2_INT_EXTI  },
    {MPU3_INT_PORT, MPU3_INT_PIN,  MPU3_INT_EXTI  },
    {MPU4_INT_PORT, MPU4_INT_PIN,  MPU4_INT_EXTI  }
};

static const ST_GPIO stGpioSel[] = {
    {MPU1_CS_PORT, MPU1_CS_PIN  },
    {MPU2_CS_PORT, MPU2_CS_PIN  },
    {MPU3_CS_PORT, MPU3_CS_PIN  },
    {MPU4_CS_PORT, MPU4_CS_PIN  }
};

static uint8_t initFlag[] = {0, 0, 0, 0};

static void mpu_sel_enable(int mpuNum, int enable);

static void mpu_int_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef exti;
    int i;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    for (i = 0; i < MPU_NUM_MAX; i++) {
        GPIO_InitStructure.GPIO_Pin = stGpioInt[i].GPIO_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_Init(stGpioInt[i].GPIOx, &GPIO_InitStructure);
        RCC_APB2PeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
        exti.EXTI_Line = stGpioInt[i].EXTI_Line;
        exti.EXTI_Mode = EXTI_Mode_Interrupt;
        exti.EXTI_Trigger = EXTI_Trigger_Rising;
        exti.EXTI_LineCmd = ENABLE;
        EXTI_Init(&exti);
    }
}

static void mpu_sel_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    int i;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    for (i = 0; i < MPU_NUM_MAX; i++ ) {
        GPIO_InitStructure.GPIO_Pin =  stGpioSel[i].GPIO_Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_Init(stGpioSel[i].GPIOx, &GPIO_InitStructure);
        mpu_sel_enable(i, 0);
    }
}

static void mpu_sel_enable(int mpuNum, int enable)
{
    if (enable) {  
        if (0 == mpuNum) {
            MPU1_CS_L
        } else if (1 == mpuNum) {
            MPU2_CS_L
        } else if (2 == mpuNum) {
            MPU3_CS_L
        } else if (3 == mpuNum) {
            MPU4_CS_L
        }
    } else {
        if (0 == mpuNum) {
            MPU1_CS_H
        } else if (1 == mpuNum) {
            MPU2_CS_H
        } else if (2 == mpuNum) {
            MPU3_CS_H
        } else if (3 == mpuNum) {
            MPU4_CS_H
        }
    }
}

///////////////////////function used by dmp start////////////////
extern uint32_t gsysTick;

void get_ms(unsigned long *ms)
{
    *ms = gsysTick;
}
void delay_ms(uint32_t ms)
{
    uint32_t time;
    time = gsysTick + ms;
    while (gsysTick < time);
}
///////////////////////function used by dmp end////////////////

/**
  * @brief This function read 1byte data from the indicated register of MPU6500
    *    @param rAddr: the indicated address of register
    * @retval read value (uint8_t)
  */
static uint8_t MPU6500ReadRegister(int mpuNum, uint8_t rAddr)
{
    uint8_t t_ret = 0;
    mpu_sel_enable(mpuNum, 1);
    SPI_WriteReadByte(MPU6500_SPI, 0x80 | rAddr);
    t_ret = SPI_WriteReadByte(MPU6500_SPI, 0xFF);
    mpu_sel_enable(mpuNum, 0);
    return t_ret;
}

/**
  * @brief This function read several datas from the indicated registers of MPU6500
    *    @param rAddr: the indicated beginning address of registers
    * @param number: the number of register which will be read
    * @param *rBuf: the store buffer pointer
    * @retval None
  */
static void MPU6500ReadRegisters(int mpuNum, uint8_t rAddr, uint8_t number, uint8_t *rBuf)
{
    uint8_t i = 0;
    mpu_sel_enable(mpuNum, 1);

    SPI_WriteReadByte(MPU6500_SPI, 0x80 | rAddr);
    for(i = 0; i < number; i++) {
        rBuf[i] = SPI_WriteReadByte(MPU6500_SPI, 0xFF);
        delay_ms(5);
    }

    mpu_sel_enable(mpuNum, 0);
}

/**
  * @brief This function does write a uint8_t value into the indicated register
    *    @param wAddr: the indicated address of register
    * @param wData: the uint8_t data which will be wrote
    * @retval None
  */
static void MPU6500WriteRegister(int mpuNum, uint8_t wAddr, uint8_t wData)
{
    mpu_sel_enable(mpuNum, 1);

    SPI_WriteReadByte(MPU6500_SPI, wAddr);
    SPI_WriteReadByte(MPU6500_SPI, wData);

    mpu_sel_enable(mpuNum, 0);
}

/**
  * @brief This function does read accel gyro temperature raw datas from MPU6500
    *    @param *accelBuf: accelerator datas buffer
    * @param *gyroBuf: gyroscope datas buffer
    * @param *tempBuf: temperature data buffer
    * @retval None
  */
static void MPU6500ReadRawData(int mpuNum, Vector3i *accelBuf_hor, Vector3i *gyroBuf_hor, int16_t *tempBuf)
{
    uint8_t l_buffer[14];

    /*get raw datas from mpu6500*/
    MPU6500ReadRegisters(mpuNum, MPU6500_ACCEL_XOUT_H, sizeof(l_buffer), l_buffer);

    /*combine hign 8bit and low 8bit, then rotate the frame*/
    accelBuf_hor->x = -INT16_VAL(l_buffer, 2);
    accelBuf_hor->y = INT16_VAL(l_buffer, 1);
    accelBuf_hor->z = INT16_VAL(l_buffer, 0);

    gyroBuf_hor->x = -INT16_VAL(l_buffer, 6);
    gyroBuf_hor->y = INT16_VAL(l_buffer, 5);
    gyroBuf_hor->z = INT16_VAL(l_buffer, 4);

    *tempBuf =  INT16_VAL(l_buffer, 3);
}


///////////////////////function used by dmp start////////////////
int mympu_write(uint8_t addr, uint8_t wAddr, uint8_t number, uint8_t *wBuf)
{
    int i;
    for (i = 0; i < number; i++){
        uint8_t wData = wBuf[i];
        MPU6500WriteRegister(addr, wAddr + i, wData);
    }
    return 0;
}

int mympu_read(uint8_t addr, uint8_t rAddr, uint8_t number, uint8_t *rBuf)
{
    MPU6500ReadRegisters(addr, rAddr, number, rBuf);
    return 0;
}

int whoamI_check(uint8_t mpuNum)
{
    uint8_t readValue;
    MPU6500WriteRegister(mpuNum, MPU6500_PWR_MGMT_1, MPU6500_PWRMGMT_PLL_X_CLK);
    delay_ms(10);
    readValue = MPU6500ReadRegister(mpuNum, MPU6500_WHO_AM_I);            //reset value is 0x70
    if (readValue != 0x70) {                                            //do a check
        DBG_I("MPU6500 %i who am i error!!", mpuNum + 1);
        return -1;
    }
    return 0;
}
///////////////////////function used by dmp end////////////////



/**
  * @brief This function does initialize sensor MPU6500
    * @param None
    * @retval None
  */

int8_t MPU6500Init(void)
{
#if !ENABLE_DMP_DEV
    int i;
    uint8_t readValue;
    uint8_t writeValue;
    mpu_sel_init();
    SPI1_init();
    SPI_Set_speed(MPU6500_SPI, SPI_BaudRatePrescaler_256);
    delay_ms(5);
//    mpu_int_init();
    for (i = 0; i < MPU_NUM_MAX; i++) {
//        /*MPU6500 Initialization*/
//        MPU6500WriteRegister(i, MPU6500_PWR_MGMT_1, MPU6500_IMU_RST);//reset the imu, reset value is 1,
//        delay_ms(10);
//        readValue = MPU6500ReadRegister(i, MPU6500_PWR_MGMT_1);
//        if (readValue != 0x01){
//            DBG_I("MPU6500 %i reset pm1 error!! 0x%02X", i + 1, readValue);
//            continue;
//        }
//        readValue = MPU6500ReadRegister(i, MPU6500_WHO_AM_I);            //reset value is 0x70
//        if (readValue != 0x70) {                                            //do a check
//            DBG_I("MPU6500 %i read WHO AM I error!! 0x%02X", i + 1, readValue);
//            continue;
//        }
//        writeValue = 0x07;
//        MPU6500WriteRegister(i, MPU6500_PWR_MGMT_2, writeValue);      //disable xg yg zg
//        delay_ms(10);
//        readValue = MPU6500ReadRegister(i, MPU6500_PWR_MGMT_2);
//        if (readValue != writeValue){
//            DBG_I("MPU6500 %i write pm2 error!! 0x%02X", i + 1, readValue);
//            continue;
//        }
//        writeValue = (0x03) << 2; //¡À16g
//        MPU6500WriteRegister(i, MPU6500_ACCEL_CONFIG, writeValue);    //configure the accel scale, least sensitive
//        delay_ms(10);
//        readValue = MPU6500ReadRegister(i, MPU6500_ACCEL_CONFIG);
//        if (readValue != writeValue){
//            DBG_I("MPU6500 %i write acccfg error!! 0x%02X", i + 1, readValue);
//            continue;
//        }
//        writeValue = 0x01;
//        MPU6500WriteRegister(i, MPU6500_ACCEL_CONFIG2, writeValue);   //Set Accel LPF setting to 184 Hz Bandwidth
//        delay_ms(10);
//        readValue = MPU6500ReadRegister(i, MPU6500_ACCEL_CONFIG2);
//        if (readValue != writeValue){
//            DBG_I("MPU6500 %i write acccfg2 error!! 0x%02X", i + 1, readValue);
//            continue;
//        }
//        writeValue = 0x40;
//        MPU6500WriteRegister(i, MPU6500_INT_EN, writeValue);          //Enable Motion Interrupt
//        delay_ms(10);
//        readValue = MPU6500ReadRegister(i, MPU6500_INT_EN);
//        if (readValue != writeValue){
//            DBG_I("MPU6500 %i write inten error!! 0x%02X", i + 1, readValue);
//            continue;
//        }
//        writeValue = (0x03<<6);
//        MPU6500WriteRegister(i, MPU6500_MOT_DETECT_CTRL, writeValue); //Enable Accel Hardware Intelligence
//        delay_ms(10);
//        readValue = MPU6500ReadRegister(i, MPU6500_MOT_DETECT_CTRL);
//        if (readValue != writeValue){
//            DBG_I("MPU6500 %i write motdec error!! 0x%02X", i + 1, readValue);
//            continue;
//        }
//        writeValue = 200;
//        MPU6500WriteRegister(i, MPU6500_WAKEON_THR, writeValue);        //Set Motion Threshold(0~1020mg)
//        delay_ms(10);
//        readValue = MPU6500ReadRegister(i, MPU6500_WAKEON_THR);
//        if (readValue != writeValue){
//            DBG_I("MPU6500 %i write wakeon thr error!! 0x%02X", i + 1, readValue);
//            continue;
//        }
//        writeValue = 10;
//        MPU6500WriteRegister(i, MPU6500_LP_ACCEL_ODR, writeValue);      //Set Frequency of Wake-up 250Hz
//        delay_ms(10);
//        readValue = MPU6500ReadRegister(i, MPU6500_LP_ACCEL_ODR);
//        if (readValue != writeValue){
//            DBG_I("MPU6500 %i write lpacc error!! 0x%02X", i + 1, readValue);
//            continue;
//        }
//        writeValue = (0x01<<5);
//        MPU6500WriteRegister(i, MPU6500_PWR_MGMT_1, writeValue); //Enable Cycle Mode (Accel Low Power Mode)
//        delay_ms(10);
//        readValue = MPU6500ReadRegister(i, MPU6500_PWR_MGMT_1);
//        if (readValue != writeValue){
//            DBG_I("MPU6500 %i write pm1 error!! 0x%02X", i + 1, readValue);
//            continue;
//        }

        MPU6500WriteRegister(i, MPU6500_PWR_MGMT_1, MPU6500_PWRMGMT_PLL_X_CLK);
        delay_ms(10);

        readValue = MPU6500ReadRegister(i, MPU6500_WHO_AM_I);                //reset value is 0x70
        if (readValue != 0x70) {                                            //do a check
            /*id error*/
            DBG_I("MPU6500 %i Init error!!", i + 1);
            return -1;
        }
        
        writeValue = MPU6500_GYRO_SCALE_500;
        MPU6500WriteRegister(i, MPU6500_GYRO_CONFIG, writeValue); //configure the gyro scale, second most sensitive
        delay_ms(10);
        readValue = MPU6500ReadRegister(i, MPU6500_GYRO_CONFIG);
        if (readValue != writeValue){
            DBG_I("MPU6500 %i write gyrocfg error!! 0x%02X", i + 1, readValue);
            continue;
        }

        writeValue = MPU6500_GYRO_LOWPASS_41HZ;
        MPU6500WriteRegister(i, MPU6500_CONFIG, writeValue);   //configure the gyro low pass filter, 41 Hz
        delay_ms(10);
        readValue = MPU6500ReadRegister(i, MPU6500_CONFIG);
        if (readValue != writeValue){
            DBG_I("MPU6500 %i write cfg error!! 0x%02X", i + 1, readValue);
            continue;
        }

        writeValue = MPU6500_ACCEL_SCALE_16G;
        MPU6500WriteRegister(i, MPU6500_ACCEL_CONFIG, writeValue);//configure the accel scale, least sensitive
        delay_ms(10);
        readValue = MPU6500ReadRegister(i, MPU6500_ACCEL_CONFIG);
        if (readValue != writeValue){
            DBG_I("MPU6500 %i write acccfg error!! 0x%02X", i + 1, readValue);
            continue;
        }
        
        writeValue = MPU6500_ACCEL_LOWPASS_41HZ;
        MPU6500WriteRegister(i, MPU6500_ACCEL_CONFIG2, writeValue);//config the accel low pass filter
        delay_ms(10);
        readValue = MPU6500ReadRegister(i, MPU6500_ACCEL_CONFIG2);
        if (readValue != writeValue){
            DBG_I("MPU6500 %i write acccfg2 error!! 0x%02X", i + 1, readValue);
            continue;
        }
        
        writeValue = MPU6500_SMPLRT_1000HZ;
        MPU6500WriteRegister(i, MPU6500_SMPLRT_DIV, writeValue);   //configure the sampling frequence
        delay_ms(10);
        readValue = MPU6500ReadRegister(i, MPU6500_SMPLRT_DIV);
        if (readValue != writeValue){
            DBG_I("MPU6500 %i write smplrtdiv error!! 0x%02X", i + 1, readValue);
            continue;
        }
        
        writeValue = MPU6500_ENABLE_AG;
        MPU6500WriteRegister(i, MPU6500_PWR_MGMT_2, writeValue);            //enable accel gyro and temperature
        delay_ms(10);
        readValue = MPU6500ReadRegister(i, MPU6500_PWR_MGMT_2);
        if (readValue != writeValue){
            DBG_I("MPU6500 %i write pm2 error!! 0x%02X", i + 1, readValue);
            continue;
        }
        
        DBG_I("MPU6500 %i Init success!!", i + 1);
        initFlag[i] = 1;
    }
    SPI_Set_speed(MPU6500_SPI, SPI_BaudRatePrescaler_64);
#endif
    return 0;
}

//REG_INIT_MOD(mpu_init);

#define MAX_FILTER_BUF_SIZE 10
typedef struct {
    int16_t buff[MAX_FILTER_BUF_SIZE];
    int index;
    int count;
}ST_MPU_FILTER_BUF;



static ST_MPU_FILTER_BUF stMpuAccxFilterBuf[MPU_NUM_MAX] = {
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
};

static ST_MPU_FILTER_BUF stMpuAccyFilterBuf[MPU_NUM_MAX] = {
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
};

static ST_MPU_FILTER_BUF stMpuAcczFilterBuf[MPU_NUM_MAX] = {
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
    {{0}, 0},
};
static void mpu_pool(void)
{
    int i;
    #if  ACCX_READ_EN
    for (i = 0; i < MPU_NUM_MAX; i++){
        int16_t accx;
        int16_t accx_h;
        int16_t accx_l;
        accx_h = MPU6500ReadRegister(i, MPU6500_ACCEL_XOUT_H);
        accx_l = MPU6500ReadRegister(i, MPU6500_ACCEL_XOUT_H+1);
        accx = (accx_h << 8) + accx_l;
        stMpuAccxFilterBuf[i].buff[stMpuAccxFilterBuf[i].index] = abs(accx);
//        DBG_I("mpuNum is %d, abs(accz) is %d", i, stMpuAcczFilterBuf[i].buff[stMpuAcczFilterBuf[i].index]);
        stMpuAccxFilterBuf[i].index = (stMpuAccxFilterBuf[i].index + 1) & (MAX_FILTER_BUF_SIZE - 1);        
        stMpuAccxFilterBuf[i].count++;
        if (stMpuAccxFilterBuf[i].count > MAX_FILTER_BUF_SIZE){
            stMpuAccxFilterBuf[i].count = MAX_FILTER_BUF_SIZE;
        }
    }
    #endif
    #if  ACCY_READ_EN
    for (i = 0; i < MPU_NUM_MAX; i++){
        int16_t accy;
        int16_t accy_h;
        int16_t accy_l;
        accy_h = MPU6500ReadRegister(i, MPU6500_ACCEL_YOUT_H);
        accy_l = MPU6500ReadRegister(i, MPU6500_ACCEL_YOUT_H+1);
        accy = (accy_h << 8) + accy_l;
        stMpuAccyFilterBuf[i].buff[stMpuAccyFilterBuf[i].index] = abs(accy);
//        DBG_I("mpuNum is %d, abs(accz) is %d", i, stMpuAcczFilterBuf[i].buff[stMpuAcczFilterBuf[i].index]);
        stMpuAccyFilterBuf[i].index = (stMpuAccyFilterBuf[i].index + 1) & (MAX_FILTER_BUF_SIZE - 1);        
        stMpuAccyFilterBuf[i].count++;
        if (stMpuAccyFilterBuf[i].count > MAX_FILTER_BUF_SIZE){
            stMpuAccyFilterBuf[i].count = MAX_FILTER_BUF_SIZE;
        }
    }
    #endif
    #if  ACCZ_READ_EN
    for (i = 0; i < MPU_NUM_MAX; i++){
        int16_t accz;
        int16_t accz_h;
        int16_t accz_l;
        accz_h = MPU6500ReadRegister(i, MPU6500_ACCEL_ZOUT_H);
        accz_l = MPU6500ReadRegister(i, MPU6500_ACCEL_ZOUT_H+1);
        accz = (accz_h << 8) + accz_l;
        stMpuAcczFilterBuf[i].buff[stMpuAcczFilterBuf[i].index] = abs(accz);
//        DBG_I("mpuNum is %d, abs(accz) is %d", i, stMpuAcczFilterBuf[i].buff[stMpuAcczFilterBuf[i].index]);
        stMpuAcczFilterBuf[i].index = (stMpuAcczFilterBuf[i].index + 1) & (MAX_FILTER_BUF_SIZE - 1);        
        stMpuAcczFilterBuf[i].count++;
        if (stMpuAcczFilterBuf[i].count > MAX_FILTER_BUF_SIZE){
            stMpuAcczFilterBuf[i].count = MAX_FILTER_BUF_SIZE;
        }
    }
    #endif
}

#if !ENABLE_DMP_DEV
REG_TASK(VH_TASK, 2, mpu_pool);
#endif

int16_t mpu_accx_read(int mpuNum)
{
#if !ENABLE_DMP_DEV    
    int sum = 0;
    int i;
    int count;
    for (i = 0; i < stMpuAccxFilterBuf[mpuNum].count; i++){
        sum += stMpuAccxFilterBuf[mpuNum].buff[i];
    }
    count = stMpuAccxFilterBuf[mpuNum].count;
    if (count > 0){
        int16_t avgValue;
        avgValue = sum / count;
//        DBG_I("%s", __FUNCTION__);
//        DBG_I("mpuNum is %d, sum is %d, count is %d, avgValue is %d", mpuNum, sum, count, avgValue);
        return avgValue;
    }
#endif
    return 0;    
}

int16_t mpu_accy_read(int mpuNum)
{
#if !ENABLE_DMP_DEV      
    int sum = 0;
    int i;
    int count;
    for (i = 0; i < stMpuAccyFilterBuf[mpuNum].count; i++){
        sum += stMpuAccyFilterBuf[mpuNum].buff[i];
    }
    count = stMpuAccyFilterBuf[mpuNum].count;
    if (count > 0){
        int16_t avgValue;
        avgValue = sum / count;
//        DBG_I("%s", __FUNCTION__);
//        DBG_I("mpuNum is %d, sum is %d, count is %d, avgValue is %d", mpuNum, sum, count, avgValue);
        return avgValue;
    }
#endif
    return 0;
    
}

int16_t mpu_accz_read(int mpuNum)
{
#if !ENABLE_DMP_DEV  
    int sum = 0;
    int i;
    int count;
    for (i = 0; i < stMpuAcczFilterBuf[mpuNum].count; i++){
        sum += stMpuAcczFilterBuf[mpuNum].buff[i];
    }
    count = stMpuAcczFilterBuf[mpuNum].count;
//    memset(stMpuAcczFilterBuf, 0, sizeof stMpuAcczFilterBuf);
    if (count > 0){
        int16_t avgValue;
        avgValue = sum / count;
//        DBG_I("%s", __FUNCTION__);
//        DBG_I("mpuNum is %d, sum is %d, count is %d, avgValue is %d", mpuNum, sum, count, avgValue);
        return avgValue;
    }
#endif
    return 0;
}

//Vector3i acce_buf;
//Vector3i gyro_buf;
//int16_t  tempBuf;

//void EXTI0_1_IRQHandler(void)
//{
//    uint32_t line;
//    line = MPU1_INT_EXTI;
//    if (EXTI_GetITStatus(line) != RESET) {
//        EXTI_ClearITPendingBit(line);
//        MPU6500ReadRawData(0, &acce_buf, &gyro_buf, &tempBuf);
//        DBG_I("int0 acce z is: %d", acce_buf.z); 
//    }
//}

//void EXTI4_15_IRQHandler(void)
//{
//    uint32_t line;
//    line = MPU2_INT_EXTI;
//    if (EXTI_GetITStatus(line) != RESET) {
//        EXTI_ClearITPendingBit(line);
//        MPU6500ReadRawData(1, &acce_buf, &gyro_buf, &tempBuf);
//        DBG_I("int1 acce z is: %d", acce_buf.z);
//    }
//    line = MPU3_INT_EXTI;
//    if (EXTI_GetITStatus(line) != RESET) {
//        EXTI_ClearITPendingBit(line);
//        MPU6500ReadRawData(2, &acce_buf, &gyro_buf, &tempBuf);
//        DBG_I("int2 acce z is: %d", acce_buf.z);
//    }
//    line = MPU4_INT_EXTI;
//    if (EXTI_GetITStatus(line) != RESET) {
//        EXTI_ClearITPendingBit(line);
//        MPU6500ReadRawData(3, &acce_buf, &gyro_buf, &tempBuf);
//        DBG_I("int3 acce z is: %d", acce_buf.z);
//    }
//}
