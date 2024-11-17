#ifndef BSP_USART_H
#define BSP_USART_H
#include "main.h"
#include <stdint.h>
#include "usart.h"
#define USART_RXBUFF_LIMIT 256
#define USART_MX_REGISTER 3
typedef enum
{
    USART_TRANSFER_NONE=0,
    USART_TRANSFER_BLOCKING,
    USART_TRANSFER_DMA,
    USART_TRANSFER_IT,
}USART_TRANSFER_MODE;

typedef struct usart
{
    uint8_t recv_buff[USART_RXBUFF_LIMIT];
    uint8_t recv_buff_size;
    UART_HandleTypeDef *usart_handle;
    void (*usart_module_callback)(struct usart*);
}USART_Instance_t;

typedef struct
{
    uint8_t recv_buff_size;
    void (*usart_module_callback)(USART_Instance_t*);
    UART_HandleTypeDef *usart_handle;
}USART_Init_Instance_t;

USART_Instance_t *USARTRegister(USART_Init_Instance_t *config);
void USARTSend(USART_Instance_t *instance,uint8_t *send_buff,uint16_t send_size,USART_TRANSFER_MODE mode);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *usart,uint16_t size);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *usart);


#endif // 
