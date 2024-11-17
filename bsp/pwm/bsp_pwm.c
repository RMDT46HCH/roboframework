#include "bsp_pwm.h"
#include "memory.h"
#include "stdlib.h"

static PWM_Instance_t *pwm_instance[PWM_DEVICE_CNT]={NULL};
static int16_t idx=0;
static uint32_t PWMSelectTclk(TIM_HandleTypeDef *htim);

PWM_Instance_t *PWMRegister(PWM_Init_Instance_t *config)
{
    if(idx>=PWM_DEVICE_CNT)
    {
        while (1)
        {
            ;
        }
    }
    PWM_Instance_t *instance= (PWM_Instance_t* ) malloc(sizeof(PWM_Instance_t));
    memset(instance,0,sizeof(PWM_Instance_t));

    instance->tim_handle=config->tim_handle;
    instance->pwm_module_callback=config->pwm_module_callback;
    instance->channel=config->channel;
    instance->dutyratio=config->dutyratio;
    instance->period=config->period;
    instance->id=config->id;
    instance->tclk=PWMSelectTclk(instance->tim_handle);
    HAL_TIM_PWM_Start(instance->tim_handle,instance->channel);
    pwm_instance[idx++]=instance;
    return instance;
}

void PWMSetOn(PWM_Instance_t *instance)
{
    HAL_TIM_Base_Start(instance->tim_handle);
}

void PWMSetOff(PWM_Instance_t *instance)
{
    HAL_TIM_Base_Stop(instance->tim_handle);
}

static uint32_t PWMSelectTclk(TIM_HandleTypeDef *htim)
{
    uintptr_t tclk_temp=(uintptr_t)((htim)->Instance);
    if (tclk_temp<=(APB1PERIPH_BASE+0x2000UL)&&(tclk_temp>=APB1PERIPH_BASE))
    {
        return (HAL_RCC_GetPCLK1Freq()*(APBPrescTable[(RCC->CFGR&RCC_CFGR_PPRE1)>>RCC_CFGR_PPRE1_Pos]==0?1:2));
    }
    else if((tclk_temp<=(APB2PERIPH_BASE+0x0400UL))&&(tclk_temp>=(APB2PERIPH_BASE))||(tclk_temp<=(APB2PERIPH_BASE+0x4800UL))&&(tclk_temp>=(APB2PERIPH_BASE+0x4000UL)))
    {
        return (HAL_RCC_GetPCLK2Freq()*(APBPrescTable[(RCC->CFGR&RCC_CFGR_PPRE1)>>RCC_CFGR_PPRE1_Pos]==0?1:2));
    }
    return 0;
}

void PWMSetPeriod(PWM_Instance_t *instance,float period)
{
    __HAL_TIM_SetAutoreload(instance->tim_handle,period*(instance->tclk)/(instance->tim_handle->Init.Prescaler+1));           
}   

void PWMSetDutyRatio(PWM_Instance_t *instance,float dutyratio)
{
    __HAL_TIM_SetCompare(instance->tim_handle,instance->channel,  dutyratio*(instance->tim_handle->Instance->ARR));           
}   

void PWMStartDMA(PWM_Instance_t *instance,uint32_t *pdata,uint32_t size)
{
    HAL_TIM_PWM_Start_DMA(instance->tim_handle,instance->channel,pdata,size);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    for(size_t i=0;i<idx;++i)
    {
        if(htim==pwm_instance[i]->tim_handle&&htim->Channel==(1<<(pwm_instance[i]->channel/4)))
        {                
            if(pwm_instance[i]->pwm_module_callback!=NULL)
            {
                pwm_instance[i]->pwm_module_callback(pwm_instance[i]);
                return;

            }
        }
    }
}