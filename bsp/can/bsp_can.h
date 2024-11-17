#ifndef BSP_CAN_H
#define BSP_CAN_H
#include <stdint.h>
#include "can.h"
#define CAN_MX_Register 4

typedef struct _
{
    CAN_HandleTypeDef *can_handle;
    CAN_TxHeaderTypeDef txconf;
    uint32_t tx_mailbox;
    void (*can_module_callback)(struct _*);
    uint16_t tx_id;
    uint16_t rx_id;
    uint8_t  tx_buff[8];
    uint8_t  rx_buff[8];
    uint16_t rx_len;
    void *id;
}CAN_INSTANCE_t;

typedef struct
{
    void (*can_module_callback)(CAN_INSTANCE_t *);
    CAN_HandleTypeDef *can_handle;
    uint16_t tx_id;
    uint16_t rx_id;
    void *id;
}CAN_INIT_INSTANCE_t;

CAN_INSTANCE_t *Can_Register(CAN_INIT_INSTANCE_t *config);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);
uint8_t CANTransmit(CAN_INSTANCE_t *instance,float timeout);
void CANSetDLC(CAN_INSTANCE_t *instance,uint8_t length);

#endif // !CAN_H







