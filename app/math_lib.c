#define DEF_DBG_I
#define DEF_DBG_W
#define DEF_DBG_E
#include "debug.h"

#include "math_lib.h"

int32_t remap_int(int32_t val, intmap_limit_val_t *limit_val)
{
    int32_t val_tag;
    int32_t val_lim;
    float delt = (limit_val->tag_max - limit_val->tag_min) * 1.0 / (limit_val->max - limit_val->min);
    val_lim = limit_val_f(val, limit_val->min, limit_val->max);
    val_tag = (val_lim - limit_val->min) * delt + limit_val->tag_min;
//    DBG_I("%10d%10d%10d%10d%10d%10d%10d%10.2f", \
//            val, limit_val->min, limit_val->max, \
//            limit_val->tag_min, limit_val->tag_max, val_lim, val_tag, delt);
    return val_tag;
}

int32_t line_remap_int(int32_t val, int32_t min, int32_t mid, int32_t max, int32_t tag_min, int32_t tag_max)
{
    float delt_tag ;
    float delt_src ;
    float delt_neg ;
    float delt_pos ;
    float tag_val  ;
    //int32_t input_val = val;

    val = limit_val_f(val, min, max);

    delt_tag = (tag_max - 0);
    delt_src = (max - mid);
    delt_pos = delt_tag / delt_src;

    delt_tag = (0 - tag_min);
    delt_src = (mid - min);
    delt_neg = delt_tag / delt_src;

    if(val > mid) {
        tag_val = (val - mid) * delt_pos + 0;
    } else        {
        tag_val = 0 - (mid - val) * delt_neg;
    }

    return (int32_t)tag_val;
}

// �?速开方倒数
//-----------------------------------
float invsqrt (float x)
{
    float xhalf = 0.5f * x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float*)&i;
    x = x * (1.5f - xhalf * x * x);
    return x;
}

float min_val_sonar(float val1, float val2)
{
    if (val1 > val2) {
        if (val2 < 1e6) {
            return val1;
        } else {
            return val2;
        }

    } else {
        return val1;
    }
}

float max_val_sonar(float val1, float val2)
{
    if (val1 > val2) {
        return val1;
    } else {
        return val2;
    }
}

float limit_val_f(float val, float min, float max)
{
    return (val > max) ? (max) : ((val < min) ? (min) : (val));
}

float remap(float val, float min, float max, float tag_min, float tag_max)
{
    float delt = (tag_max - tag_min) / (max - min);
    val = limit_val_f(val, min, max);
    return (val - min) * delt + tag_min;
}

float To_180_deg(float deg)
{
    if      (deg > 180.0f)    {
        return (deg - 360.0f);
    } else if (deg < -180.0f)   {
        return (deg + 360.0f);
    } else                    {
        return deg;
    }
}



float move_average_run(move_avg_obj_t* move_obj, float push_data)
{
    float ret = 0;
    uint32_t i = 0;
    uint32_t deep = 0;
    move_obj->pavg_buf[move_obj->cur_write_index] = push_data;
    move_obj->cur_write_index++;
    if(move_obj->cur_write_index >= move_obj->buf_deep) {
        move_obj->cur_write_index = 0;
        move_obj->flag_is_full = 1;
    }

    if(move_obj->flag_is_full) {
        deep = move_obj->buf_deep;
    } else {
        deep = move_obj->cur_write_index;
    }

    for(i = 0; i < deep; i++) {
        ret += move_obj->pavg_buf[i];
    }
    return ret / deep;
}


#define pi 3.141592653f

float ToRad(float   degree)
{
    return (degree * pi / 180.0);
}

/*位置试PID*/
float pid_calc (pid_obj_t * pid_obj,  move_avg_obj_t* p_move_avg_obj, float tag, float fdbk, float T, pf_err_handle err_handle)
{
    float cur_d_res = 0;
    float cur_d_gain = 0;
    pid_obj->tag                 = tag;
    pid_obj->fdbk                 = fdbk;
    pid_obj->delta_t            = T;
    pid_obj->err                 = pid_obj->tag - pid_obj->fdbk;
    if(err_handle != NULL) {
        pid_obj->err = err_handle(pid_obj->err);
    }

    pid_obj->P_result             = pid_obj->kp * pid_obj->err;
    pid_obj->I_err                += (pid_obj->err * pid_obj->delta_t);
    pid_obj->I_err                 = limit_val_f(pid_obj->I_err, pid_obj->I_lower_limit / pid_obj->ki, pid_obj->I_uper_limit / pid_obj->ki);
    pid_obj->D_err               = pid_obj->err - pid_obj->last_err;
    //�?分环节看�?否需要�?�几级，做个平滑
    if(p_move_avg_obj != NULL) {
        pid_obj->D_move_avg_err     = move_average_run(p_move_avg_obj, pid_obj->D_err);
        pid_obj->D_err              = pid_obj->D_move_avg_err;
    }

    pid_obj->last_err           = pid_obj->err;

    pid_obj->I_result            =  pid_obj->ki * pid_obj->I_err;
    pid_obj->I_result           =  limit_val_f(pid_obj->I_result, pid_obj->I_lower_limit, pid_obj->I_uper_limit);

    if(pid_obj->enable_incomplet_d) {
        cur_d_res                    = (pid_obj->incomplete_T) * pid_obj->D_err + (1.0f - pid_obj->incomplete_T) * pid_obj->D_err_last;
        cur_d_gain                    = remap(fabs(pid_obj->err), pid_obj->incomplete_err_min, pid_obj->incomplete_err_max, pid_obj->incomplete_Kd_min, pid_obj->incomplete_Kd_max);

    } else {
        cur_d_res                    = pid_obj->D_err;
        cur_d_gain                     = pid_obj->kd;
    }



    pid_obj->D_err_last        = pid_obj->D_err;


    pid_obj->D_result           =  cur_d_gain * cur_d_res;
    pid_obj->pid_output         =  pid_obj->P_result + pid_obj->I_result + pid_obj->D_result;

//    if( fabs(pid_obj->D_move_avg_err)> 0.5f)
//    {
//        float scale  = remap(fabs(pid_obj->D_move_avg_err),0.5,4.0f,0,1);
//        float total = (pid_obj->output_uper_limit - pid_obj->output_lower_limit)/2.0;
//
//        if(pid_obj->D_move_avg_err)
//        {
//            pid_obj->pid_output = pid_obj->pid_output + (total* scale*0.05);  //前�?��?�大
//        }
//        else
//        {
//            pid_obj->pid_output = pid_obj->pid_output - (total* scale*0.05);  //前�?��?�大
//        }
//    }





    /*limit ctrl output*/
    pid_obj->pid_output         = limit_val_f(pid_obj->pid_output, pid_obj->output_lower_limit, pid_obj->output_uper_limit);

    /*return the output*/
    return pid_obj->pid_output;
}



void clear_pid_inter(new_pid_obj_t* p_pid_obj)
{
    p_pid_obj->cur_i_err = 0;
}


float  new_pid_calc(new_pid_obj_t* p_pid_obj, float tag, float fbk, float    dt)
{
    float cur_d_err = 0;
#define MIN_PER  -100.0f
#define MAX_PER  +100.0f
    //参数归一�?

    p_pid_obj->tag       = tag;
    p_pid_obj->fbk       = fbk;


    p_pid_obj->tag_per      = remap(p_pid_obj->tag, p_pid_obj->input_min, p_pid_obj->input_max, MIN_PER, MAX_PER);
    p_pid_obj->fbk_per   = remap(p_pid_obj->fbk, p_pid_obj->input_min, p_pid_obj->input_max, MIN_PER, MAX_PER);

    p_pid_obj->err       = p_pid_obj->tag_per  - p_pid_obj->fbk_per;

    //-----------------------死区控制
    if(p_pid_obj->en_dead_zone_control) {
        if( (p_pid_obj->err < p_pid_obj->input_dead_zone_max) && (p_pid_obj->err > p_pid_obj->input_dead_zone_min)) {
            p_pid_obj->err  = 0.0f;
        }
    }


    p_pid_obj->d_err     = p_pid_obj->err - p_pid_obj->last_err ;

    p_pid_obj->last_err  = p_pid_obj->err   ;
    p_pid_obj->last_d_err = p_pid_obj->d_err ;


    //---------------------------位置和�?�量PID
    if(p_pid_obj->is_mode_inc_pid) {
        p_pid_obj->cur_p_err  =  p_pid_obj->d_err;
        p_pid_obj->cur_i_err  =  p_pid_obj->err;
        p_pid_obj->cur_d_err  =  p_pid_obj->d_err - p_pid_obj->last_d_err;

    } else {
        p_pid_obj->cur_p_err  =  p_pid_obj->err;
        p_pid_obj->cur_i_err +=  p_pid_obj->err * dt;
        p_pid_obj->cur_i_err  = limit_val_f(p_pid_obj->cur_i_err, p_pid_obj->i_output_min / p_pid_obj->ki, p_pid_obj->i_output_max / p_pid_obj->ki);

        p_pid_obj->cur_d_err  =  p_pid_obj->d_err;
    }

    cur_d_err = p_pid_obj->cur_d_err ;

    //-----------------------�?分分�?
    if(p_pid_obj->en_integral_separation) {

    }

    //-----------------------不完全积�?
    if(p_pid_obj->en_incompletely_differentiated) {
        p_pid_obj->cur_d_err = (p_pid_obj->incomplete_diff_const) * cur_d_err + (1.0f - p_pid_obj->incomplete_diff_const) * (p_pid_obj->last_cur_d_err );
        p_pid_obj->last_cur_d_err  = cur_d_err;
    }



    p_pid_obj->kp_output      = p_pid_obj->kp * p_pid_obj->cur_p_err * p_pid_obj->gain ;
    p_pid_obj->ki_output      = p_pid_obj->ki * p_pid_obj->cur_i_err * p_pid_obj->gain ;
    p_pid_obj->kd_output      = p_pid_obj->kd * p_pid_obj->cur_d_err * p_pid_obj->gain ;

    p_pid_obj->ki_output  = limit_val_f(p_pid_obj->ki_output, p_pid_obj->i_output_min, p_pid_obj->i_output_max);


    p_pid_obj->output_per     = p_pid_obj->kp_output + p_pid_obj->ki_output + p_pid_obj->kd_output;


    //输出重映�?
    p_pid_obj->output          = remap(p_pid_obj->output_per, MIN_PER, MAX_PER, p_pid_obj->output_min, p_pid_obj->output_max);


    //DBG_I("tag:%0.1f  tagper:%0.1f  fbk:%0.1f",p_pid_obj->tag,p_pid_obj->tag_per,p_pid_obj->fbk_per);

    return p_pid_obj->output;
}

