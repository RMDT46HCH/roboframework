#include "bsp_can.h"
#include "bsp_dwt.h"
#include "main.h"
#include "memory.h"
#include "stdlib.h"
static CAN_INSTANCE_t *can_instance[CAN_MX_Register]={NULL};
static int16_t idx=0;
static void CANAddfilter(CAN_INSTANCE_t *instance)
{
    CAN_FilterTypeDef can_filter_st;
    static uint8_t can1_filter_idx=0,can2_filter_idx=14;
    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDLIST;
    can_filter_st.FilterScale = CAN_FILTERSCALE_16BIT;
    can_filter_st.FilterFIFOAssignment = (instance->tx_id&1)?CAN_FilterFIFO0:CAN_FilterFIFO1;
    can_filter_st.SlaveStartFilterBank = 14;
    can_filter_st.FilterIdLow = instance->rx_id<<5;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = (instance->can_handle==&hcan1)?(can1_filter_idx++):(can2_filter_idx++);    
    HAL_CAN_ConfigFilter(instance->can_handle, &can_filter_st);
}

void CANServiceInit()
{
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING);

    HAL_CAN_Start(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO1_MSG_PENDING);
}


CAN_INSTANCE_t *Can_Register(CAN_INIT_INSTANCE_t *config)
{
    if(!idx)
    {
        CANServiceInit();   
    }
    if(idx>=CAN_MX_Register)
    {
        while (1)
        {
            ;
        }
    }
    for(size_t i=0;i<=idx;i++)
    {
        if(can_instance[i]->can_handle==config->can_handle&&can_instance[i]->rx_id==config->rx_id)
        {
            while(1)
            {
                ;
            }
        }
    }
    CAN_INSTANCE_t *instance= (CAN_INSTANCE_t* ) malloc(sizeof(CAN_INSTANCE_t));
    memset(instance,0,sizeof(CAN_INSTANCE_t));
    instance->txconf.StdId=config->tx_id;
    instance->txconf.IDE=CAN_ID_STD;
    instance->txconf.RTR=CAN_RTR_DATA;
    instance->txconf.DLC=0x08;
    instance->can_module_callback=config->can_module_callback;
    instance->can_handle=config->can_handle;
    instance->tx_id=config->tx_id;
    instance->rx_id=config->rx_id;
    instance->id=config->id;
    CANAddfilter(instance);
    can_instance[idx++]=instance;
    return instance;
}

uint8_t CANTransmit(CAN_INSTANCE_t *instance,float timeout)
{
    float dwt_start=DWT_GetTimeline_ms();
    while(HAL_CAN_GetTxMailboxesFreeLevel(instance->can_handle)==0)
    {
        if(DWT_GetTimeline_ms()-dwt_start>timeout)
        return 0;
    }
    if(HAL_CAN_AddTxMessage(instance->can_handle,&instance->txconf,instance->tx_buff,&instance->tx_mailbox))
    {
        return 0;
    }
    return 1;
}

void CANSetDLC(CAN_INSTANCE_t *instance,uint8_t length)
{
    if(length==0||length>8)
    {
        while(1)
        {
            ;
        }
    }
    instance->txconf.DLC=length;
}

static void CANFIFOCallback(CAN_HandleTypeDef *hcan,uint32_t fifox)
{
    static CAN_RxHeaderTypeDef rxconf;
    uint8_t can_rx_conf[8];
    while(HAL_CAN_GetRxFifoFillLevel(hcan,fifox))
    {
        HAL_CAN_GetRxMessage(hcan,fifox,&rxconf,can_rx_conf);
        for(size_t i=0;i<idx;++i)
        {
            if(hcan==can_instance[i]->can_handle&&rxconf.StdId==can_instance[i]->tx_id)
            {                
                if(can_instance[i]->can_module_callback!=NULL)
                {
                    can_instance[i]->rx_len=rxconf.DLC;
                    memcpy(can_instance[i]->rx_buff,can_rx_conf,rxconf.DLC);
                    can_instance[i]->can_module_callback(can_instance[i]);
                }
            }
            return;
        }
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CANFIFOCallback(hcan,CAN_RX_FIFO0);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CANFIFOCallback(hcan,CAN_RX_FIFO1);
}
