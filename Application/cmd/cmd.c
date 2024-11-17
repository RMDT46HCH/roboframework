#include "robot_def.h"
#include "cmd.h"
#include "remote_control.h"
#include "can_comm.h"
#include "message_center.h"
#include "bsp_dwt.h"
static chassis_ctrl_t chassis_cmd_send;
static chassis_feedback_t chassis_fetch_data;

static shoot_ctrl_t shoot_cmd_send;
static shoot_feedback_t shoot_fetch_data;

static rc_ctrl_t *rc_data;
static gimbal_ctrl_t gimbal_cmd_send;
static gimbal_feedback_t gimbal_fetch_data;

static Publisher_t *gimbal_cmd_pub;
static Subscriber_t *gimbal_cmd_sub;
static Publisher_t *shoot_cmd_pub;
static Subscriber_t *shoot_cmd_sub;

static BOARD_CANcomm_INSTANCE *cmd_can_comm;
void Robot_CMD_Init()
{
    rc_data=RemoteControlInit(&huart3);
    gimbal_cmd_sub=SubRegister("gimbal_feed",sizeof(gimbal_feedback_t));
    gimbal_cmd_pub=PubRegister("gimbal_cmd",sizeof(gimbal_ctrl_t));

    shoot_cmd_sub=SubRegister("shoot_feed",sizeof(shoot_feedback_t));
    shoot_cmd_pub=PubRegister("shoot_cmd",sizeof(shoot_ctrl_t));

    BOARD_INIT_CANcomm_INSTANCE comm_conf=
    {
        .can_config={
            .can_handle=&hcan2,
            .tx_id=0x200,
            .rx_id=0x209,
        },
        .send_data_len=sizeof(chassis_ctrl_t),
        .recv_data_len=sizeof(chassis_feedback_t),    
    };
    cmd_can_comm=   board_comm_init(&comm_conf);
    gimbal_cmd_send.pitch=0;
}

static void robot_stop()
{
    chassis_cmd_send.chassis_mode=CHASSIS_NONE_FORCE;
    gimbal_cmd_send.gimbal_mode=GIMBAL_NONE_FORCE;
    shoot_cmd_send.fri_mode=FRI_OFF;
    shoot_cmd_send.shoot_mode=SHOOT_OFF;
    shoot_cmd_send.loader_mode=LOADER_OFF;
}

static void basic_set()
{
    if(gimbal_cmd_send.pitch>PITCH_MAX_ANGLE)
    {
        gimbal_cmd_send.pitch=PITCH_MAX_ANGLE;
    }
    else if (gimbal_cmd_send.pitch<PITCH_MIN_ANGLE)
    {
        gimbal_cmd_send.pitch=PITCH_MIN_ANGLE;
    }
    else
    gimbal_cmd_send.pitch=gimbal_cmd_send.pitch;

    gimbal_cmd_send.gimbal_mode=GIMBAL_NORMAL;
    shoot_cmd_send.fri_mode=FRI_ON;
    shoot_cmd_send.shoot_mode=SHOOT_ON;
    shoot_cmd_send.bullet_speed=BULLET_30;
}

static void chassis_RC()
{
    if(switch_is_mid(rc_data->rc.switch_right))
    {
         chassis_cmd_send.chassis_mode=CHASSIS_NORMAL;
    }
    else if(switch_is_up(rc_data->rc.switch_right))
    {
        chassis_cmd_send.chassis_mode=CHASSIS_ROTATE;
    }   
    chassis_cmd_send.vx=10*rc_data->rc.rocker_left_y;
    chassis_cmd_send.vy=-10*rc_data->rc.rocker_left_x;
}

static void chassis_AC()
{
    chassis_cmd_send.chassis_mode=CHASSIS_ROTATE;
    //chassis_cmd_send.vx=10*MinipcData->rocker_left_y;
    //chassis_cmd_send.vy=-10*MinipcData->rocker_left_x;
}

static void gimbal_rc()
{
    gimbal_cmd_send.yaw+=0.001*rc_data->rc.rocker_right_x;    
    gimbal_cmd_send.pitch+=0.001*rc_data->rc.rocker_right_y;
}

static void gimbal_ac()
{
    // //if(minipc->deep!=0)
    // {
    //     //gimbal_cmd_send.yaw-=0.001*MinipcData->rocker_right_x;    
    // //gimbal_cmd_send.pitch-=0.001*MinipcData->rocker_right_y;
    // }
    //else
    float t_pitch= DWT_GetTimeline_ms();
    gimbal_cmd_send.yaw-=0.33;
    gimbal_cmd_send.pitch=20*abs(t_pitch);
}

static void remote_control_set()
{
    basic_set();
    chassis_AC();
    gimbal_rc();
}
static void auto_control_set()
{
    basic_set();
    gimbal_ac();
    chassis_AC();
}

static void calc_offset_angle()
{
    if (gimbal_fetch_data.yaw_motor_single_round_angle<YAW_ALIGN_ANGLE)
    {
        chassis_cmd_send.offset_angle=YAW_ALIGN_ANGLE-gimbal_fetch_data.yaw_motor_single_round_angle;
    }
    else 
    chassis_cmd_send.offset_angle=-(YAW_ALIGN_ANGLE-gimbal_fetch_data.yaw_motor_single_round_angle);
}
void cmd_task()
{
    chassis_fetch_data=*(chassis_feedback_t*)Board_CAN_Get(cmd_can_comm);
    SubGetMessage(gimbal_cmd_sub,&gimbal_fetch_data);
    SubGetMessage(shoot_cmd_sub,&shoot_fetch_data);

    if(switch_is_down(rc_data->rc.switch_right))
    robot_stop();
    else if(switch_is_mid(rc_data->rc.switch_right))
    remote_control_set();
    else if(switch_is_up(rc_data->rc.switch_right))
    auto_control_set();

    calc_offset_angle();
    PubPushMessage(gimbal_cmd_pub,(void*)&gimbal_cmd_send);
    PubPushMessage(shoot_cmd_pub,(void*)&shoot_cmd_send);
    BOARD_can_send(cmd_can_comm,(void*)&chassis_cmd_send);
}