#include "gimbal.h"
#include "robot_def.h"
#include "message_center.h"
#include "dji_motor.h"
#include "ins_task.h"
#include "bmi088.h"
static DJMotor_INSTANCE_t *yaw_motor,*pitch_motor;
static gimbal_ctrl_t gimbal_cmd_recv;
static gimbal_feedback_t gimbal_feedback_data;
static Publisher_t *gimbal_pub;
static Subscriber_t *gimbal_sub;
static attitude_t *gimbal_imu_data;
void gimbal_init()
{
    gimbal_imu_data=INS_Init();
    motor_init_instance_t yaw_config={
        .can_init_config={
            .can_handle=&hcan2,
            .tx_id=2,
        },
        .pid_init_config={
            .angle_pid={
                .Kp=3,
                .Ki=3,
                .Kd=0.3,
                .MaxOut=500,
            },
            .speed_pid={
                .Kp=260,
                .Ki=0,
                .Kd=0,
                .MaxOut=20000,
            },
        },
        .motor_setting_config={
            .angle_feedback_source=OTHER_FEED,
            .speed_feedback_source=OTHER_FEED,
            .close_loop_type=ANGLE_LOOP|SPEED_LOOP,
            .outer_loop_type=ANGLE_LOOP,
            .motor_reverse_flag=MOTOR_REVERSE,
        },
        .motor_type=GM6020
    };
    motor_init_instance_t pitch_config={
        .can_init_config={
            .tx_id=1,
        },
        .pid_init_config={
            .angle_pid={
                .Kp=3,
                .Ki=3,
                .Kd=0.3,
                .MaxOut=500,
            },
            .speed_pid={
                .Kp=260,
                .Ki=0,
                .Kd=0,
                .MaxOut=20000,
            },
        },
        .motor_setting_config={
            .angle_feedback_source=OTHER_FEED,
            .speed_feedback_source=OTHER_FEED,
            .close_loop_type=ANGLE_LOOP|SPEED_LOOP,
            .outer_loop_type=ANGLE_LOOP,
            .motor_reverse_flag=MOTOR_REVERSE,

        },
        .motor_type=GM6020
    };
    gimbal_sub=SubRegister("gimbal_cmd",sizeof(gimbal_ctrl_t));
    gimbal_pub=PubRegister("gimbal_feed",sizeof(gimbal_feedback_t));
    yaw_motor=DJMotorInit(&yaw_config);
    pitch_motor=DJMotorInit(&pitch_config);
}

static void gimbal_mode_set()
{
    switch (gimbal_cmd_recv.gimbal_mode)
    {
        case GIMBAL_NONE_FORCE:
            DJMotor_Stop(yaw_motor);
            DJMotor_Stop(pitch_motor);
            break;
        case GIMBAL_NORMAL:
            DJMotor_Enanble(yaw_motor);
            DJMotor_Enanble(pitch_motor);
            DJMotor_set(yaw_motor,gimbal_cmd_recv.yaw);
            DJMotor_set(pitch_motor,gimbal_cmd_recv.pitch);
            break;
        default:
            break;
    }
}

static void gimbal_feedback()
{
    gimbal_feedback_data.yaw_motor_single_round_angle=yaw_motor->measure.angle_round;
}

void gimbal_task()
{
    SubGetMessage(gimbal_sub,&gimbal_cmd_recv);
    gimbal_mode_set();
    gimbal_feedback();
    PubPushMessage(gimbal_pub,(void*)&gimbal_feedback_data);
}