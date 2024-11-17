#ifndef MOTOR_DEF_H
#define MOTOR_DEF_H
#include "controller.h"
#include "bsp_can.h"
typedef enum
{
    OPEN_LOOP=0b0000,
    SPEED_LOOP=0b0001,
    ANGLE_LOOP=0b0010,
    CURRENT_LOOP=0b0100,
    SPEED_ANGLE_LOOP=0b0011,
    SPEED_CURRENT_LOOP=0b0101,
    ANGLE_CURRENT_LOOP=0b0110,
    ALL_LOOP=0b0111,
}close_loop_type_e;

typedef enum
{
    MOTOR_FEED=0,
    OTHER_FEED,
}feedback_source_e;

typedef enum
{
    MOTOR_NORMAL=0,
    MOTOR_REVERSE,
}motor_reverse_flag_e;

typedef enum
{
    MOTOR_STOP=0,
    MOTOR_ENABLE,
}motor_enable_flag_e;

typedef enum
{
    NONE=0,
    GM6020,
    M3508,
    M2006,
}motor_type_e;

typedef struct 
{
    close_loop_type_e outer_loop_type;
    close_loop_type_e close_loop_type;
    feedback_source_e angle_feedback_source;
    feedback_source_e speed_feedback_source;
    motor_reverse_flag_e motor_reverse_flag;
    motor_enable_flag_e motor_enable_flag;
}motor_setting_t;

typedef struct 
{
    float *other_angle_feedback_ptr;
    float *other_speed_feedback_ptr;

    PIDInstance current_pid;
    PIDInstance speed_pid;
    PIDInstance angle_pid;

    float set;
}pid_controller_t;

typedef struct 
{
    float *other_angle_feedback_ptr;
    float *other_speed_feedback_ptr;

    PID_Init_Config_s current_pid;
    PID_Init_Config_s speed_pid;
    PID_Init_Config_s angle_pid;
}pid_init_controller_t;

typedef struct
{
    pid_init_controller_t pid_init_config; 
    motor_setting_t motor_setting_config;
    motor_type_e motor_type;
    CAN_INIT_INSTANCE_t can_init_config;
}motor_init_instance_t;



#endif // !MOTOR_DEF_H