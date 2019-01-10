#ifndef __PUBLIC_DEFINE_H
#define __PUBLIC_DEFINE_H
#include <stdint.h>

typedef struct {
    float       ax;
    float       ay;
    float       az;
    float       angle_speed_x;
    float       angle_speed_y;
    float       angle_speed_z;
    float       angle_x;
    float       angle_y;
    float       angle_z;
    float       temp;
} imu_info_t;

extern imu_info_t g_imu_info;


#endif

