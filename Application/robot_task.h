#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

#include "robot.h"
#include "ins_task.h"
#include "dji_motor.h"
#include "buzzer.h"

osThreadId insTaskHandle;
osThreadId robotTaskHandle;
osThreadId motorTaskHandle;

void StartInsTask(void const *argument);
void StartMotorTask(void const *argument);
void StartRobotTask(void const *argument);

void osTaskInit()
{
    osThreadDef(instask,StartInsTask,osPriorityAboveNormal,0,1024);
    insTaskHandle=osThreadCreate(osThread(instask),NULL);

    osThreadDef(motortask,StartMotorTask,osPriorityNormal,0,256);
    insTaskHandle=osThreadCreate(osThread(motortask),NULL);    
    
    osThreadDef(robottask,StartInsTask,osPriorityNormal,0,1024);
    insTaskHandle=osThreadCreate(osThread(robottask),NULL);
}

 __attribute__((noreturn)) void StartInsTask(void const *argument)
{
    static float ins_start;
    static float ins_dt;
    INS_Init();
    for(;;)
    {
        ins_start =DWT_GetTimeline_ms();
        INS_Task();
        ins_dt=DWT_GetTimeline_ms()-ins_start;
        if(ins_dt>1)
        ;
        osDelay(1);
    }

}
 __attribute__((noreturn)) void StartMotorTask(void const *argument)
{
    static float motor_start;
    static float motor_dt;
    for(;;)
    {
        motor_start =DWT_GetTimeline_ms();
        DJMotor_control();
        motor_dt=DWT_GetTimeline_ms()-motor_start;
        if(motor_dt>1)
        ;
        osDelay(1); 
    }
}

 __attribute__((noreturn))void StartRobotTask(void const *argument)
{
    static float robot_start;
    static float robot_dt;
    for(;;)
    {
        robot_start =DWT_GetTimeline_ms();
        DJMotor_control();
        robot_dt=DWT_GetTimeline_ms()-robot_start;
        if(robot_dt>10)
        ;
        osDelay(5); 
    }
}