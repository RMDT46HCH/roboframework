#include "bsp_usart.h"
#include "memory.h"
#include "stdlib.h"

static USART_Instance_t *usart_instance[USART_MX_REGISTER]={NULL};
static int16_t idx=0;

void USARTServiceInit(USART_Instance_t *instance)
{
    HAL_UARTEx_ReceiveToIdle_DMA(instance->usart_handle,instance->recv_buff,instance->recv_buff_size);
    __HAL_DMA_DISABLE_IT(instance->usart_handle->hdmarx,DMA_IT_HT);
}

USART_Instance_t *USARTRegister(USART_Init_Instance_t *config)
{

    if(idx>=USART_MX_REGISTER)
    {
        while (1)
        {
            ;
        }
    }
    for(size_t i=0;i<=idx;i++)
    {
        if(usart_instance[i]->usart_handle==config->usart_handle)
        {
            while(1)
            {
                ;
            }
        }
    }
    USART_Instance_t *instance= (USART_Instance_t* ) malloc(sizeof(USART_Instance_t));
    memset(instance,0,sizeof(USART_Instance_t));

    instance->usart_module_callback=config->usart_module_callback;
    instance->usart_handle=config->usart_handle;
    USARTServiceInit(instance);   
    usart_instance[idx++]=instance;

    return instance;
}
void USARTSend(USART_Instance_t *instance,uint8_t *send_buff,uint16_t send_size,USART_TRANSFER_MODE mode)
{
    switch (mode)
    {
    case USART_TRANSFER_BLOCKING:
    HAL_UART_Transmit(instance->usart_handle,send_buff,send_size,100);
        break;
        case USART_TRANSFER_IT:
    HAL_UART_Transmit_IT(instance->usart_handle,send_buff,send_size);
        break;   
         case USART_TRANSFER_DMA:
    HAL_UART_Transmit_DMA(instance->usart_handle,send_buff,send_size);
        break;
    default:
        while (1)
        {
            ;
        }
        break;
    }
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *usart,uint16_t size)
{
    for(size_t i=0;i<idx;++i)
    {
        if(usart==usart_instance[i]->usart_handle)
        {
            if(usart_instance[i]->usart_module_callback!=NULL)
            {
                usart_instance[i]->usart_module_callback(usart_instance[i]);
                memcpy(usart_instance[i]->recv_buff,0,size);
            }
                HAL_UARTEx_ReceiveToIdle_DMA(usart,usart_instance[i]->recv_buff,usart_instance[i]->recv_buff_size);
                __HAL_DMA_DISABLE_IT(usart->hdmarx,DMA_IT_HT);
            return;
        }
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *usart)
{
    for(size_t i=0;i<idx;++i)
        {
            if(usart==usart_instance[i]->usart_handle)
            {
                HAL_UARTEx_ReceiveToIdle_DMA(usart,usart_instance[i]->recv_buff,usart_instance[i]->recv_buff_size);
                __HAL_DMA_DISABLE_IT(usart->hdmarx,DMA_IT_HT);
                return;
            }
        }
}