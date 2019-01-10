#ifndef __MATH_LIB_H__
#define __MATH_LIB_H__

#include "x_frame.h"
#include "math.h"


typedef struct
{
    float       *pavg_buf;
    uint32_t    buf_deep;
    uint32_t    cur_write_index;
    uint8_t     flag_is_full;
}move_avg_obj_t;

typedef struct
{
    //cfg-----------------
    float        gain;                                //用于设置输入的感�?
    
    float         kp;
    float       ki;
    float        kd;


    float         input_dead_zone_min;
    float        input_dead_zone_max;

    float        i_output_min;
    float        i_output_max;

    float         input_min;
    float        input_max;

    float        output_min;
    float        output_max;

    float        incomplete_diff_const;                //不完全微分的一阶惯性常�?

    uint8_t     en_integral_separation;                //使能�?分分�?
    uint8_t        en_incompletely_differentiated;        //使能不完全微�?
    uint8_t        en_dead_zone_control;                //使能死区控制
    uint8_t        is_mode_inc_pid;


    //------------计算�?间变�?

    float        tag;
    float        fbk;
    float        dt;
    

    float        err;
    float        last_err;
    float        d_err;
    float        last_d_err;                         //位置PID需要用�?

    float        cur_p_err;                            //实际参与计算的perr
    float        cur_i_err;                            //实际参与计算的ierr
    float        cur_d_err;                            //实际参与计算的到derr
    float        last_cur_d_err;                        //用于做不完全�?分�?�算

    float        kp_output;
    float        ki_output;
    float        kd_output;

    float        tag_per;
    float         fbk_per;
    float        output_per;
    float        output;

    //interface-----------


    //core   func---------


    //depend -------------

    
}new_pid_obj_t;

typedef struct {
    float    tag;
    float    fdbk;
    float    err;
    float    last_err;
    float    last_last_err;
    float   I_err;
    float    D_err;
    float    D_move_avg_err;
    float     D_err_last;

    //---------------pid的配�?
    uint8_t enable_incomplet_d;
    uint8_t enable_d_filter;
    float    incomplete_T;
    float   incomplete_Kd_max;
    float   incomplete_Kd_min;
    float   incomplete_err_max;
    float   incomplete_err_min;

    //-------在启动不完全�?分的时候，Kd采用线性映�?
    

    // p i d 三个参数�?
    float    kp;
    float    ki;
    float     kd;
    float     delta_t;

    // I 限幅 参数�?
    float     I_lower_limit;
    float     I_uper_limit;

    
    // P I D 三项的�?�算结果
    float     P_result        ;
    float     I_result        ;
    float     D_result        ;

    //最终结�?
    float   pid_output_per;
    float   pid_output;

    //pid的输出为-100%--100%，所以需要映射到最大输�?
    //output 限幅
    float     output_lower_limit;
    float     output_uper_limit;
} pid_obj_t;

typedef struct{
    int min;
    int max;
    int tag_min;
    int tag_max;
}intmap_limit_val_t;

#define REG_MOVE_AVG_OBJ(name,deep) float avg_buf_##name[deep]={0}; move_avg_obj_t move_avg_obj_##name={avg_buf_##name,deep,0,0};
#define MAKE_MOVE_AVG_HANDLE(name)  &move_avg_obj_##name  

typedef float (*pf_err_handle)(float err);

float        max_val_sonar(float val1,float val2);
float        min_val_sonar(float val1,float val2);

int32_t     remap_int           (int32_t val, intmap_limit_val_t*);
int32_t     line_remap_int      (int32_t val,int32_t min,int32_t mid,int32_t max,int32_t tag_min,int32_t tag_max);
float       limit_val_f         (float val,float min,float max);
float       remap               (float val,float min,float max,float tag_min,float tag_max);
float       To_180_deg          (float deg);
float       pid_calc            (pid_obj_t * pid_obj, move_avg_obj_t* p_move_avg_obj,float tag, float fdbk,float T,pf_err_handle err_handle);
float       move_average_run(move_avg_obj_t* move_obj,float push_data);
float       ToRad               (float   degree);


float   new_pid_calc(new_pid_obj_t* p_pid_obj,float tag,float fbk,float    dt);
void    clear_pid_inter(new_pid_obj_t* p_pid_obj);
void    update_lock_angle(void);
#endif //__MATH_LIB_H__

