#ifndef __BSP_DMP_H
#define __BSP_DMP_H

#include <stdint.h>

typedef struct{
    short acc[3];
    float pitchRowYaw[3];
}ST_DMP_DATA;

int dmp_init(uint8_t mpuNum);
int read_dmp(ST_DMP_DATA *dmpData);

#endif

