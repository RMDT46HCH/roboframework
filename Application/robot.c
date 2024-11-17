#include "robot.h"
#include "bsp_dwt.h"
#include "robot_def.h"
#include "robot_task.h"

#if defined(GIMBAL_BOARD)
#include "gimbal.h"
#include "shoot.h"
#include "cmd.h"
#endif

#if defined(CHASSIS_BOARD)
#include "chassis.h"
#endif

void RobotInit()
{
    __disable_irq();
    DWT_Init(168);
    osTaskInit();
    #if defined(GIMBAL_BOARD) 
    Robot_CMD_Init();
    gimbal_init();
    shoot_init();
    #endif 
    #if defined(CHASSIS_BOARD) 
    Chassis_Init();
    #endif 
    //OSTASKINIT();
    __enable_irq();
}

void RobotTask()
{
    #if defined(GIMBAL_BOARD) 
    cmd_task();
    gimbal_task();
    shoot_task();
    #endif

    #if defined(CHASSIS_BOARD) 
    Chassis_task();
    #endif 
}

