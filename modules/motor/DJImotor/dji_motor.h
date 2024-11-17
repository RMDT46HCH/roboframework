#ifndef DJMotor_H
#define DJMotor_H
#include "motor_def.h"
#include "stdint.h"
#include "bsp_can.h"
#define ECD_TO_ANGLE (360/8192)
#define SPEED_SMOOTH_COEF 0.05
#define CURRENT_SMOOTH_COEF 0.05
#define DJMotor_num 12
typedef struct 
{
    uint8_t ecd;
    uint8_t last_ecd;
    uint8_t angle_round;
    float speed_aps;
    float real_current;
    float temperature;
    float total_angle;
    float init_angle;
    int32_t total_round;
}DJMotor_Measure_t;

typedef struct
{
    DJMotor_Measure_t measure;
    motor_setting_t motor_setting;
    pid_controller_t motor_controller;
    CAN_INSTANCE_t *motor_can_instance;
    motor_type_e motor_type;

    uint8_t sender_group;
    uint8_t message_num;
}DJMotor_INSTANCE_t;
void DJMotor_Stop(DJMotor_INSTANCE_t *motor);
void DJMotor_Enanble(DJMotor_INSTANCE_t *motor);
void DJMotor_outerloop(DJMotor_INSTANCE_t *motor,close_loop_type_e outer_loop);
void DJMotor_Changefeed(DJMotor_INSTANCE_t *motor,feedback_source_e source,close_loop_type_e loop);
void DJMotor_set(DJMotor_INSTANCE_t *motor,float set);
void Decode_DJMotor(CAN_INSTANCE_t *instance);
DJMotor_INSTANCE_t *DJMotorInit(motor_init_instance_t *config);
void DJMotor_control();

#endif // !DJMotor_H