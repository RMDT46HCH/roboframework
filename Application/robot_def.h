#ifndef ROBOT_DEF_H
#define ROBOT_DEF_H

#include "ins_task.h"
#include "stdint.h"

//#define GIMBAL_BOARD
#define CHASSIS_BOARD
//底盘参数(MM)
#define R_WHEEL 50.0f
#define REDUCTION_RATIO_WHEEL 19.0f
#define YAW_ALIGN_ANGLE 1000
//云台参数
#define PITCH_MAX_ANGLE 20
#define PITCH_MIN_ANGLE -10


typedef enum
{
    ROBOT_STOP=0,
    ROBOT_READY,
}robot_mode_e;

typedef enum
{
    CHASSIS_NONE_FORCE=0,
    CHASSIS_ROTATE,
    CHASSIS_NORMAL,
}chassis_mode_e;

typedef struct 
{
    float vx;
    float vy;
    float wz;
    chassis_mode_e chassis_mode;
    float offset_angle;
}chassis_ctrl_t;

typedef struct 
{
    float real_vx;
    float real_vy;
    float real_wz;
}chassis_feedback_t;

typedef enum
{
    GIMBAL_NONE_FORCE=0,
    GIMBAL_NORMAL,
}gimbal_mode_e;

typedef struct 
{
    gimbal_mode_e gimbal_mode;
    float yaw;
    float pitch;
}gimbal_ctrl_t;

typedef struct
{
    uint16_t yaw_motor_single_round_angle;
}gimbal_feedback_t;

typedef enum
{
    SHOOT_OFF=0,
    SHOOT_ON,
}shoot_mode_e;

typedef enum
{
    FRI_OFF=0,
    FRI_ON,
}fri_mode_e;

typedef enum
{
    LOADER_OFF=0,
    LOADER_NORMAL,
    LOADER_REVERSE,
}loader_mode_e;

typedef enum
{
    BULLET_15=15,
    BULLET_18=18,
    BULLET_30=30,
}bullet_speed_e;

typedef struct
{
    shoot_mode_e shoot_mode;
    fri_mode_e fri_mode;
    loader_mode_e loader_mode;
    bullet_speed_e bullet_speed;
}shoot_ctrl_t;

typedef struct
{
    float loader_speed;
}shoot_feedback_t;







#endif // !ROBOT_DEF_H 