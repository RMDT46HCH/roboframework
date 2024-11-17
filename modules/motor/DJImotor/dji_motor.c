#include "dji_Motor.h"
#include "stdlib.h"
#include "bsp_can.h"
static uint8_t idx=0;
static DJMotor_INSTANCE_t *djmotor_instance[DJMotor_num]={NULL};

//C610(M2006)/C620(M3508):0X1FF,0X200
//GM6020:0X1FF,0X2FF
//rx_id=GM6020:0X204+ID,C610/C620:0X200+ID
//CAN1:0X1FF,0X200,0X2FF
//CAN2:0X1FF,0X200,0X2FF
static CAN_INSTANCE_t send_to_can[6]={
   [0]={.can_handle=&hcan1,.txconf.StdId=0x1ff,.txconf.IDE=CAN_ID_STD,.txconf.RTR=CAN_RTR_DATA,.txconf.DLC=0X08,.tx_buff={0}}, 
   [1]={.can_handle=&hcan1,.txconf.StdId=0x200,.txconf.IDE=CAN_ID_STD,.txconf.RTR=CAN_RTR_DATA,.txconf.DLC=0X08,.tx_buff={0}}, 
   [2]={.can_handle=&hcan1,.txconf.StdId=0x2FF,.txconf.IDE=CAN_ID_STD,.txconf.RTR=CAN_RTR_DATA,.txconf.DLC=0X08,.tx_buff={0}}, 
   [3]={.can_handle=&hcan2,.txconf.StdId=0x1ff,.txconf.IDE=CAN_ID_STD,.txconf.RTR=CAN_RTR_DATA,.txconf.DLC=0X08,.tx_buff={0}}, 
   [4]={.can_handle=&hcan2,.txconf.StdId=0x200,.txconf.IDE=CAN_ID_STD,.txconf.RTR=CAN_RTR_DATA,.txconf.DLC=0X08,.tx_buff={0}}, 
   [5]={.can_handle=&hcan2,.txconf.StdId=0x2ff,.txconf.IDE=CAN_ID_STD,.txconf.RTR=CAN_RTR_DATA,.txconf.DLC=0X08,.tx_buff={0}}, 
};

static  uint8_t sender_enable_flag[6]={0};

static void SenderGrouping(CAN_INIT_INSTANCE_t *config,DJMotor_INSTANCE_t *instance)
{
    uint8_t motor_send_num;
    uint8_t motor_grouping;

    switch (instance->motor_type)
    {
    case M2006:
    case M3508:
        if(config->tx_id<=4)
        {
            motor_send_num=config->tx_id-1;
            motor_grouping=config->can_handle==&hcan1?1:4;
        }
        else
        {
            motor_send_num=config->tx_id-5;
            motor_grouping=config->can_handle==&hcan1?0:3;
        }
        config->rx_id=0x200+config->tx_id;
        sender_enable_flag[motor_grouping]=1;
        instance->message_num=motor_send_num;
        instance->sender_group=motor_grouping;

        for(size_t i=0;i<idx;++i)
        {
            if(djmotor_instance[i]->motor_can_instance->can_handle==config->can_handle&&djmotor_instance[i]->motor_can_instance->rx_id==config->rx_id)
            {
                while (1)
                {
                   ;
                }
                
            }
        }
        break;
        case GM6020:
        if(config->tx_id<=4)
        {
            motor_send_num=config->tx_id-1;
            motor_grouping=config->can_handle==&hcan1?0:3;
        }
        else
        {
            motor_send_num=config->tx_id-5;
            motor_grouping=config->can_handle==&hcan1?2:5;
        }
        config->rx_id=0x204+config->tx_id;
        sender_enable_flag[motor_grouping]=1;
        instance->message_num=motor_send_num;
        instance->sender_group=motor_grouping;

        for(size_t i=0;i<idx;++i)
        {
            if(djmotor_instance[i]->motor_can_instance->can_handle==config->can_handle&&djmotor_instance[i]->motor_can_instance->rx_id==config->rx_id)
            {
                while (1)
                {
                   ;
                }
                
            }
        }
        break;
    default:
        break;
    }
}

void DJMotor_Stop(DJMotor_INSTANCE_t *motor)
{
    motor->motor_setting.motor_enable_flag=MOTOR_STOP;
}

void DJMotor_Enanble(DJMotor_INSTANCE_t *motor)
{
    motor->motor_setting.motor_enable_flag=MOTOR_ENABLE;
}

void DJMotor_outerloop(DJMotor_INSTANCE_t *motor,close_loop_type_e outer_loop)
{
    motor->motor_setting.outer_loop_type=outer_loop;
}

void DJMotor_Changefeed(DJMotor_INSTANCE_t *motor,feedback_source_e source,close_loop_type_e loop)
{
    if(loop==ANGLE_LOOP)
    motor->motor_setting.angle_feedback_source=source;
    else if(loop==SPEED_LOOP)
    motor->motor_setting.speed_feedback_source=source;    
}

void DJMotor_set(DJMotor_INSTANCE_t *motor,float set)
{
    motor->motor_controller.set=set;
}

void Decode_DJMotor(CAN_INSTANCE_t *instance)
{
    uint8_t *rxbuff=instance->rx_buff;
    DJMotor_INSTANCE_t *motor = (DJMotor_INSTANCE_t *)instance->id;
    DJMotor_Measure_t *measure=&motor->measure;
    measure->ecd=(uint16_t)(rxbuff[0]<<8)|rxbuff[1];
    measure->last_ecd=measure->ecd;
    measure->angle_round=measure->ecd*ECD_TO_ANGLE;
    measure->speed_aps=(1-SPEED_SMOOTH_COEF)*(float)((uint16_t)rxbuff[2]<<8|rxbuff[3])+SPEED_SMOOTH_COEF*(measure->speed_aps);   
    measure->real_current=(1-CURRENT_SMOOTH_COEF)*(float)((uint16_t)rxbuff[4]<<8|rxbuff[5])+CURRENT_SMOOTH_COEF*(measure->real_current);   
    measure->temperature=rxbuff[6];
    if( measure->ecd- measure->last_ecd<-4096)
    measure->angle_round++;
    else if( measure->ecd- measure->last_ecd>4096)
    measure->angle_round--;
    measure->total_angle=measure->angle_round*360+measure->angle_round;
}

DJMotor_INSTANCE_t *DJMotorInit(motor_init_instance_t *config)
{
    DJMotor_INSTANCE_t *instance= (DJMotor_INSTANCE_t* ) malloc(sizeof(DJMotor_INSTANCE_t));
    memset(instance,0,sizeof(DJMotor_INSTANCE_t));

    instance->motor_type=config->motor_type;
    instance->motor_setting.motor_reverse_flag=config->motor_setting_config.motor_reverse_flag;

    PIDInit(&instance->motor_controller.speed_pid,&config->pid_init_config.speed_pid);
    PIDInit(&instance->motor_controller.angle_pid,&config->pid_init_config.angle_pid);
    PIDInit(&instance->motor_controller.current_pid,&config->pid_init_config.current_pid);

    instance->motor_controller.other_angle_feedback_ptr=config->pid_init_config.other_angle_feedback_ptr;
    instance->motor_controller.other_angle_feedback_ptr=config->pid_init_config.other_speed_feedback_ptr;

    config->can_init_config.id=instance;
    instance->motor_can_instance=Can_Register(&config->can_init_config);
    config->can_init_config.can_module_callback=Decode_DJMotor;
    SenderGrouping(&config->can_init_config,instance);
    DJMotor_Enanble(instance);
    djmotor_instance[idx++]=instance;
    return instance;
}

void DJMotor_control()
{
    DJMotor_INSTANCE_t *motor;
    motor_setting_t *motor_setting;
    DJMotor_Measure_t *measure;
    pid_controller_t *motor_pid;
    float pid_set,pid_measure;
    int16_t set;
    for(size_t i=0;i<idx;++i)
    {
        motor=djmotor_instance[i];
        motor_setting=&motor->motor_setting;
        measure=&motor->measure;
        motor_pid=&motor->motor_controller;
        pid_set=motor_pid->set;

        if(motor_setting->motor_reverse_flag==MOTOR_REVERSE)
        pid_set*=-1;

        if((motor_setting->close_loop_type&ANGLE_LOOP)&&(motor_setting->outer_loop_type==ANGLE_LOOP))
        {
            if(motor_setting->angle_feedback_source==OTHER_FEED)
                pid_measure=*motor_pid->other_angle_feedback_ptr;
            else
                pid_measure=measure->angle_round;
            pid_set=PIDCalculate(&motor_pid->angle_pid,pid_set,pid_measure);
        }
        if((motor_setting->close_loop_type&SPEED_LOOP)&&(motor_setting->outer_loop_type==ANGLE_LOOP|SPEED_LOOP))
        {
            if(motor_setting->angle_feedback_source==OTHER_FEED)
                pid_measure=*motor_pid->other_speed_feedback_ptr;
            else
                pid_measure=measure->speed_aps;
            pid_set=PIDCalculate(&motor_pid->speed_pid,pid_set,pid_measure);
        }
        if(motor_setting->close_loop_type&CURRENT_LOOP)
        {
            pid_set=PIDCalculate(&motor_pid->current_pid,pid_set,pid_measure);
        }
            set=(int16_t)pid_set;
    }
    send_to_can[0].tx_buff[2*motor->message_num]=(uint8_t)(set>>8);
    send_to_can[0].tx_buff[2*motor->message_num+1]=(uint8_t)(set&0x00ff);

    if(motor->motor_setting.motor_enable_flag==MOTOR_STOP)
    {
        memset(send_to_can[motor->sender_group].tx_buff+2*motor->message_num,0,16u);
    }
    for(size_t i=0;i<6;++i)
    {
        CANTransmit(&send_to_can[i],1);
    }
}