#include "bsp_gpio.h"
#include "stdint.h"
#include "memory.h"
#include "stdlib.h"
static GPIO_Instance_t *gpio_instance[GPIO_MX_DEVICE_NUM]={NULL};
static int16_t idx=0;


GPIO_Instance_t *GPIORegister(GPIO_Init_Instance_t *config)
{
    if(idx>=GPIO_MX_DEVICE_NUM)
    {
        while (1)
        {
            ;
        }
    }
    GPIO_Instance_t *instance= (GPIO_Instance_t* ) malloc(sizeof(GPIO_Instance_t));
    memset(instance,0,sizeof(GPIO_Instance_t));

    instance->GPIOx=config->GPIOx;
    instance->exti_mode=config->exti_mode;
    instance->gpio_module_callback=config->gpio_module_callback;
    instance->GPIO_Pin=config->GPIO_Pin;
    instance->id=config->id;
    gpio_instance[idx++]=instance;
    return instance;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
{
    GPIO_Instance_t *gpio;
     for(size_t i=0;i<idx;++i)
    {
        if(gpio_instance[i]->GPIO_Pin==GPIO_PIN)
        {                
            if(gpio_instance[i]->gpio_module_callback!=NULL)
            {
                gpio_instance[i]->gpio_module_callback(gpio_instance[i]);
                return;

            }
        }
    }
}

void GPIOSet(GPIO_Instance_t *instance)
{
    HAL_GPIO_WritePin(instance->GPIOx,instance->GPIO_Pin,GPIO_PIN_SET);
}

void GPIOReset(GPIO_Instance_t *instance)
{
    HAL_GPIO_WritePin(instance->GPIOx,instance->GPIO_Pin,GPIO_PIN_RESET);
}

void GPIOToggle(GPIO_Instance_t *instance)
{
    HAL_GPIO_TogglePin(instance->GPIOx,instance->GPIO_Pin);
}

GPIO_PinState GPIO_ReadPin(GPIO_Instance_t *instance)
{
    return HAL_GPIO_ReadPin(instance->GPIOx,instance->GPIO_Pin);
}
