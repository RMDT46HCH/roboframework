#ifndef BSP_GPIO_h
#define BSP_GPIO_h
#include "gpio.h"
#include "stdint.h"
#define GPIO_MX_DEVICE_NUM 10

typedef enum
{
    GPIO_EXTI_MODE_RISING=0,
    GPIO_EXTI_MODE_FALLING,
    GPIO_EXTI_MODE_RISING_FALLING,
    GPIO_EXTI_MODE_NONE,
}GPIO_EXTI_MODE_e;

typedef struct GPIO
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    GPIO_PinState pin_state;
    GPIO_EXTI_MODE_e exti_mode;
    void(*gpio_module_callback)(struct GPIO*);
    void *id;
}GPIO_Instance_t;

typedef struct 
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
    GPIO_PinState pin_state;
    GPIO_EXTI_MODE_e exti_mode;
    void(*gpio_module_callback)(GPIO_Instance_t*);
    void *id;
}GPIO_Init_Instance_t;

GPIO_Instance_t *GPIORegister(GPIO_Init_Instance_t *config);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN);
void GPIOSet(GPIO_Instance_t *instance);
void GPIOReset(GPIO_Instance_t *instance);
void GPIOToggle(GPIO_Instance_t *instance);
GPIO_PinState GPIO_ReadPin(GPIO_Instance_t *instance);





#endif // !BSP_GPIO_h