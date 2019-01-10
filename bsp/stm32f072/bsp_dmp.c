#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include <math.h>
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include "mpu6500.h"
#include "bsp_dmp.h"

#define PRINT_ACCEL         (0x01)
#define PRINT_GYRO          (0x02)
#define PRINT_QUAT          (0x04)
#define ACCEL_ON            (0x01)
#define GYRO_ON             (0x02)
#define MOTION              (0)
#define NO_MOTION           (1)
#define DEFAULT_MPU_HZ      (200)
#define FLASH_SIZE          (512)
#define FLASH_MEM_START     ((void*)0x1800)
#define q30                 1073741824.0f

static signed char gyro_orientation[9] = {-1, 0, 0,
                                          0, -1, 0,
                                          0, 0, 1
                                         };

static  unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}


static  unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar;
    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;
    return scalar;
}

static void run_self_test(void)
{
    int result;
    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result == 0x7) {
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        unsigned short accel_sens;
        mpu_get_gyro_sens(&sens);
        gyro[0] = (long)(gyro[0] * sens);
        gyro[1] = (long)(gyro[1] * sens);
        gyro[2] = (long)(gyro[2] * sens);
        dmp_set_gyro_bias(gyro);
        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        dmp_set_accel_bias(accel);
        printf("setting bias succesfully ......\r\n");
    }
}

int dmp_init(uint8_t mpuNum)
{
    if (whoamI_check(mpuNum)) return -1;
    if(!mpu_init()) {
        if(!mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL)){
            DBG_I("mpu_set_sensor complete ......\r\n");
            return -1;
        }
        if(!mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL)){
            DBG_I("mpu_configure_fifo complete ......\r\n");
            return -1;
        }
        if(!mpu_set_sample_rate(DEFAULT_MPU_HZ)){
            DBG_I("mpu_set_sample_rate complete ......\r\n");
            return -1;
        }
        if(!dmp_load_motion_driver_firmware()){
            DBG_I("dmp_load_motion_driver_firmware complete ......\r\n");
            return -1;
        }
        if(!dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation))){
            DBG_I("dmp_set_orientation complete ......\r\n");
            return -1;
        }
        if(!dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
                               DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
                               DMP_FEATURE_GYRO_CAL)){
            DBG_I("dmp_enable_feature complete ......\r\n");
            return -1;
        }
        if(!dmp_set_fifo_rate(DEFAULT_MPU_HZ)){
            DBG_I("dmp_set_fifo_rate complete ......\r\n");
            return -1;
        }
        run_self_test();
        if(!mpu_set_dmp_state(1)){
            DBG_I("mpu_set_dmp_state complete ......\r\n");
            return -1;
        }
        return 0;
    }else{
        return -1;
    }
}

int read_dmp(ST_DMP_DATA *dmpData)
{
    short gyro[3], accel[3], sensors;
    float q0 = 1.0f, q1 = 0.0f, q2 = 0.0f, q3 = 0.0f;
    unsigned long sensor_timestamp;
    unsigned char more;
    long quat[4];
    if(dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more)) {
        return -1;
    }
    
    memcpy(dmpData->acc, accel, sizeof accel);
    
    if (sensors & INV_WXYZ_QUAT) {
        q0 = quat[0] / q30;
        q1 = quat[1] / q30;
        q2 = quat[2] / q30;
        q3 = quat[3] / q30;
        dmpData->pitchRowYaw[0] = (float)asin(-2 * q1 * q3 + 2 * q0 * q2) * 57.3;
        dmpData->pitchRowYaw[1] = (float)atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * 57.3; // roll
        dmpData->pitchRowYaw[2] = (float)atan2(2 * (q1 * q2 + q0 * q3), q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.3;
        return 0;
    } else{
        return -1;
    }
}

