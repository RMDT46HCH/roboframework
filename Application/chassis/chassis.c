#include "robot_def.h"
#include "chassis.h"
#include "math.h"
#include "dji_motor.h"
#include "can_comm.h"

static chassis_ctrl_t chassis_cmd_recv;
static chassis_feedback_t chassis_feedback_data;
static DJMotor_INSTANCE_t *motor_lf,*motor_rf,*motor_lb,*motor_rb;
static float v_lf,v_lb,v_rf,v_rb,cosa,sina;
static float chassis_vx,chassis_vy;
static BOARD_CANcomm_INSTANCE *chassis_cmd;


void Chassis_Init()
{
    motor_init_instance_t chassis_motor_config={
        .can_init_config={
            .can_handle=&hcan1,
        },
        .pid_init_config={
            .speed_pid={
                .Kp=6,
                .Ki=0,
                .Kd=0,
                .MaxOut=12000,
            },
            .angle_pid={
                .Kp=0.5,
                .Ki=0,
                .Kd=0,
                .MaxOut=15000,
            },
        },
        .motor_setting_config={
            .angle_feedback_source=MOTOR_FEED,
            .speed_feedback_source=MOTOR_FEED,
        },
        .motor_type=M3508,
    };
    chassis_motor_config.can_init_config.tx_id=0x201;
    chassis_motor_config.motor_setting_config.motor_reverse_flag=MOTOR_NORMAL;
    motor_lf=DJMotorInit(&chassis_motor_config);

    chassis_motor_config.can_init_config.tx_id=0x202;
    chassis_motor_config.motor_setting_config.motor_reverse_flag=MOTOR_NORMAL;
    motor_lb=DJMotorInit(&chassis_motor_config);

    chassis_motor_config.can_init_config.tx_id=0x203;
    chassis_motor_config.motor_setting_config.motor_reverse_flag=MOTOR_REVERSE;
    motor_rf=DJMotorInit(&chassis_motor_config);

    chassis_motor_config.can_init_config.tx_id=0x204;
    chassis_motor_config.motor_setting_config.motor_reverse_flag=MOTOR_REVERSE;
    motor_rb=DJMotorInit(&chassis_motor_config);

    BOARD_INIT_CANcomm_INSTANCE comm_conf=
    {
        
        .can_config={
            .can_handle=&hcan2,
            .tx_id=0x209,
            .rx_id=0x200,
        },
        .send_data_len=sizeof(chassis_feedback_t),
        .recv_data_len=sizeof(chassis_ctrl_t),    
    };
    chassis_cmd= board_comm_init(&comm_conf);
}



static void chassis_motor_state_set()
{
    if(chassis_cmd_recv.chassis_mode=CHASSIS_NONE_FORCE)
    {
        DJMotor_Stop(motor_lf);
        DJMotor_Stop(motor_lb);
        DJMotor_Stop(motor_rf);
        DJMotor_Stop(motor_rb);
    }
    else
    {
        DJMotor_Enanble(motor_lf);
        DJMotor_Enanble(motor_lb);
        DJMotor_Enanble(motor_rf);
        DJMotor_Enanble(motor_rb);
    }
}

static void rotate_speed_set()
{
    float wt=DWT_GetTimeline_s();
    switch(chassis_cmd_recv.chassis_mode)
    {
        case CHASSIS_ROTATE:
        chassis_cmd_recv.wz=1200+100*sin(wt);
        break;
        case CHASSIS_NORMAL:
        chassis_cmd_recv.wz=-3*chassis_cmd_recv.offset_angle*chassis_cmd_recv.offset_angle;
        break;
        default:
        chassis_cmd_recv.wz=0;
        break;
    }
}

static void malun_cal()
{
    cosa=cos(chassis_cmd_recv.offset_angle);
    sina=sin(chassis_cmd_recv.offset_angle);

    chassis_vx=chassis_cmd_recv.vx*cosa+chassis_cmd_recv.vy*sina;
    chassis_vy=-chassis_cmd_recv.vx*sina+chassis_cmd_recv.vy*cosa;

    v_lf=chassis_vx-chassis_vy+chassis_cmd_recv.wz;
    v_lb=chassis_vx+chassis_vy+chassis_cmd_recv.wz;
    v_rf=chassis_vx+chassis_vy-chassis_cmd_recv.wz;
    v_rb=chassis_vx-chassis_vy-chassis_cmd_recv.wz;
    
    DJMotor_set(motor_lf,v_lf);
    DJMotor_set(motor_lb,v_lb);
    DJMotor_set(motor_rf,v_rf);
    DJMotor_set(motor_rb,v_rb);   
}

static void feedback_to_odom()
{
    chassis_feedback_data.real_vx= motor_lf->measure.speed_aps+motor_rf->measure.speed_aps+motor_lb->measure.speed_aps+motor_rb->measure.speed_aps;
    chassis_feedback_data.real_vy=-motor_lf->measure.speed_aps+motor_rf->measure.speed_aps+motor_lb->measure.speed_aps-motor_rb->measure.speed_aps;
    chassis_feedback_data.real_wz=motor_lf->measure.speed_aps-motor_rf->measure.speed_aps+motor_lb->measure.speed_aps-motor_rb->measure.speed_aps;

}


void Chassis_task()
{
    chassis_cmd_recv=*(chassis_ctrl_t*)Board_CAN_Get(chassis_cmd);
    chassis_motor_state_set();
    rotate_speed_set();
    malun_cal();
    feedback_to_odom();
    BOARD_can_send(chassis_cmd,(void*)&chassis_feedback_data);
}