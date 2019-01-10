#ifndef __MPU6500_H__
#define __MPU6500_H__
#include <stdint.h>

#define ACCX_READ_EN    0
#define ACCY_READ_EN    0
#define ACCZ_READ_EN    1


/*registers========================================*/
#define MPU6500_PWR_MGMT_1          ((uint8_t)0x6B)
#define MPU6500_PWR_MGMT_2           ((uint8_t)0x6C)
#define MPU6500_WHO_AM_I           ((uint8_t)0x75)
#define MPU6500_GYRO_CONFIG          ((uint8_t)0x1B)
#define MPU6500_ACCEL_CONFIG      ((uint8_t)0x1C)
#define MPU6500_ACCEL_CONFIG2      ((uint8_t)0x1D)
#define MPU6500_CONFIG              ((uint8_t)0x1A)
#define MPU6500_SMPLRT_DIV          ((uint8_t)0x19)
#define MPU6500_INT_STATUS          ((uint8_t)0x3A)
#define MPU6500_ACCEL_XOUT_H      ((uint8_t)0x3B)
#define MPU6500_ACCEL_YOUT_H      ((uint8_t)0x3D)
#define MPU6500_ACCEL_ZOUT_H      ((uint8_t)0x3F)
#define MPU6500_INT_EN            ((uint8_t)0x38)
#define MPU6500_MOT_DETECT_CTRL   ((uint8_t)0x69)
#define MPU6500_LP_ACCEL_ODR      ((uint8_t)0x1E)
#define MPU6500_WAKEON_THR        ((uint8_t)0x1F)

#define MPU6500_PWRMGMT_PLL_X_CLK    0X01

/*configure value===================================*/
#define MPU6500_ID                        ((uint8_t)0x70)
#define MPU6500_IMU_RST                  ((uint8_t)0X80)
#define MPU6500_IMU_RST_VAL                ((uint8_t)0x01)
#define MPU6500_CLK_COURSE               ((uint8_t)0X01)    //auto select the best availabe clock source

#define MPU6500_GYRO_SCALE_250            ((uint8_t)0x00)
#define MPU6500_GYRO_SCALE_500            ((uint8_t)0x08)
#define MPU6500_GYRO_SCALE_1000            ((uint8_t)0x10)
#define MPU6500_GYRO_SCALE_2000            ((uint8_t)0x18)

#define MPU6500_GYRO_LOWPASS_41HZ        ((uint8_t)0x03)

#define MPU6500_ACCEL_SCALE_2G             ((uint8_t)0x00)
#define    MPU6500_ACCEL_SCALE_4G            ((uint8_t)0x08)
#define MPU6500_ACCEL_SCALE_8G            ((uint8_t)0x10)
#define MPU6500_ACCEL_SCALE_16G            ((uint8_t)0x18)

#define MPU6500_ACCEL_LOWPASS_41HZ        ((uint8_t)0x03)
#define MPU6500_SMPLRT_1000HZ            ((uint8_t)0x00)
#define MPU6500_ENABLE_AG                ((uint8_t)0x00)

#define MPU_NUM_MAX    4

int8_t MPU6500Init(void);
int16_t mpu_accx_read(int mpuNum);
int16_t mpu_accy_read(int mpuNum);
int16_t mpu_accz_read(int npuNum);
/////////////////for dmp driver 

int whoamI_check(uint8_t mpuNum);
int mympu_write(uint8_t addr, uint8_t wAddr, uint8_t number, uint8_t *wBuf);
int mympu_read(uint8_t addr, uint8_t rAddr, uint8_t number, uint8_t *rBuf);
void get_ms(unsigned long *ms);
void delay_ms(uint32_t ms);

#endif //__BSP_PWM_H__
