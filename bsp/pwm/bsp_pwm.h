#ifndef BSP_PWM_H
#define BSP_PWM_H

#include "tim.h"
#include "stdint.h"
#include "main.h"
#define PWM_DEVICE_CNT 16

typedef struct PWM
{
    TIM_HandleTypeDef *tim_handle;
    uint32_t channel;
    uint32_t tclk;
    float period;
    float dutyratio;
    void (*pwm_module_callback)(struct PWM*);
    void *id;
}PWM_Instance_t;

typedef struct 
{
    TIM_HandleTypeDef *tim_handle;
    uint32_t channel;
    float period;
    float dutyratio;
    void (*pwm_module_callback)(PWM_Instance_t*);
    void *id;
}PWM_Init_Instance_t;

PWM_Instance_t *PWMRegister(PWM_Init_Instance_t *config);
void PWMSetOn(PWM_Instance_t *instance);
void PWMSetOff(PWM_Instance_t *instance);
void PWMSetPeriod(PWM_Instance_t *instance,float period);
void PWMSetDutyRatio(PWM_Instance_t *instance,float dutyratio);
void PWMStartDMA(PWM_Instance_t *instance,uint32_t *pdata,uint32_t size);
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);







#endif // !BSP_PWM_H
