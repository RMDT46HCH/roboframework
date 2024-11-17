#include "super_cap.h"
#include "memory.h"
#include "stdlib.h"

static SuperCap_Instance_t *supercap_instance=NULL;

static void SuperCapRXCallback(CAN_INSTANCE_t *instance)
{
    uint8_t *rxbuff;
    rxbuff=instance->rx_buff;
    SuperCap_Msg *Msg;
    Msg=&supercap_instance->Msg;
    Msg->current=(uint16_t)((rxbuff[2]<<8)|rxbuff[3]);
    Msg->vol=(uint16_t)((rxbuff[0]<<8)|rxbuff[1]);
    Msg->power=(uint16_t)((rxbuff[4]<<8)|rxbuff[5]);
}

SuperCap_Instance_t *SuperCapInit(SuperCap_Init_Instance_t *config)
{
    supercap_instance=(SuperCap_Instance_t*)malloc(sizeof(SuperCap_Instance_t));
    memset(supercap_instance,0,sizeof(supercap_instance));
    config->config.can_module_callback=SuperCapRXCallback;
    supercap_instance->supercap_can=Can_Register(&config->config);
    return supercap_instance;
}

void SuperCapSend(SuperCap_Instance_t *instance,uint8_t *data)
{
    memcpy(instance->supercap_can->tx_buff,data,8);
    CANTransmit(instance->supercap_can,1);
}

SuperCap_Msg SuperCapGet(SuperCap_Instance_t *instance)
{
    return instance->Msg;
}