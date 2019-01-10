//#define DEF_DBG_I
//#define DEF_DBG_W
//#define DEF_DBG_E
//#include "debug.h"
//#include "x_frame.h"
//#include "math_lib.h"

//#define MAX_CTRL_RANG       10000
//#define MIN_CTRL_RANG       (-MAX_CTRL_RANG)
//#define MAX_ANG_SPEED_LIMIT 180
//#define MAX_ANG_SPEED_ACC   3
//#define MIN_ANG_SPEED_LIMIT (-MAX_ANG_SPEED_LIMIT)
//#define ANGLE_CHANGE_THR    0.3f    //deg
//#define TIME_DELT           0.01f   //10ms
//#define OUT_PID_CNT         5

//static pid_obj_t angle_pid;
//static pid_obj_t angle_speed_pid;
//static float lock_angle;
//    
//static void pid_init(void)
//{
//    angle_pid.kp = 10;
//    angle_pid.ki = 0.1;
//    angle_pid.kd = 0.5;
//    angle_pid.I_lower_limit = MIN_ANG_SPEED_LIMIT;
//    angle_pid.I_uper_limit = MAX_ANG_SPEED_LIMIT;
//    angle_pid.output_lower_limit = MIN_ANG_SPEED_LIMIT;
//    angle_pid.output_uper_limit = MAX_ANG_SPEED_LIMIT;
//    
//    angle_speed_pid.kp = 60;
//    angle_speed_pid.ki = 1;
//    angle_speed_pid.kd = 0.5;
//    angle_speed_pid.I_lower_limit = MIN_CTRL_RANG;
//    angle_speed_pid.I_uper_limit = MAX_CTRL_RANG;
//    angle_speed_pid.output_lower_limit = MIN_CTRL_RANG;
//    angle_speed_pid.output_uper_limit = MAX_CTRL_RANG;
//}
////REG_INIT_MOD(pid_init);

//static void pid_clear(void)
//{
//    memset(&angle_pid, 0, sizeof angle_pid);
//    memset(&angle_speed_pid, 0, sizeof angle_speed_pid);
//}

//float angle_err_handle(float err)
//{
//    if (err > 100){
//        return -(angle_pid.fdbk + 360 - angle_pid.tag);
//    }else if (err < -100){
//        return angle_pid.tag + 360 - angle_pid.fdbk;
//    }
//    return err;
//}

///*
//                         -360 0 -1 angle
//   angle_speed           +----+----+
//+ <---+   +---> -        |    |    |
//      |   |              |    +----+ -90
//      +   +              |         |
//                         +---------+
//*/
//void yaw_ctrl(int16_t yaw_in, int16_t* yaw_out)
//{
//    static int angle_speed_des;
//    static int angle_speed_cur;
//    
//    static float angle_speed_pid_in;
//    pid_init();
//    angle_speed_des = remap(yaw_in, MIN_CTRL_RANG, MAX_CTRL_RANG, MIN_ANG_SPEED_LIMIT, MAX_ANG_SPEED_LIMIT);
//    if (abs(angle_speed_des - angle_speed_cur) > MAX_ANG_SPEED_ACC){
//        if (angle_speed_des > angle_speed_cur){
//            angle_speed_cur += MAX_ANG_SPEED_ACC;
//        }else if(angle_speed_des < angle_speed_cur){
//            angle_speed_cur -= MAX_ANG_SPEED_ACC;
//        }
//    }else{
//        angle_speed_cur = angle_speed_des;
//    }
//    
//    if (abs(yaw_in) > 0){
//        //update the lock_angle
//        update_lock_angle();
//        angle_speed_pid_in = angle_speed_cur;
//    }else{
//        static int angle_pid_cnt = 0;
//        if (++angle_pid_cnt >= OUT_PID_CNT){
//            float cal_result;
//            angle_pid_cnt = 0;
//            if (fabs(lock_angle - g_imu_info.angle_z) > ANGLE_CHANGE_THR){
//                cal_result = pid_calc(&angle_pid, 
//                                    NULL,	//MAKE_MOVE_AVG_HANDLE(yaw_ang),
//                                    lock_angle, 
//                                    -g_imu_info.angle_z, 
//                                    TIME_DELT * OUT_PID_CNT,
//                                    angle_err_handle);
//                angle_speed_pid_in = cal_result;
//            }else{
//                angle_speed_pid_in = 0;
//            }
//        }
//    }
//    
//    *yaw_out = pid_calc(&angle_speed_pid, 
//                            NULL,	//MAKE_MOVE_AVG_HANDLE(yaw_ang),
//                            angle_speed_pid_in, 
//                            -g_imu_info.angle_speed_z, 
//                            TIME_DELT,
//                            NULL
//                        );
//}

//void yaw_ctrl_clear(void)
//{
//    pid_clear();
//}

//void update_lock_angle(void)
//{
//    lock_angle = -g_imu_info.angle_z;
//}




