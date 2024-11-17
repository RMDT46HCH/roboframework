#include "can_comm.h"
#include "stdlib.h"
#include "memory.h"
#include "crc8.h"
#include "bsp_dwt.h"

static void BoardcommResetRx(BOARD_CANcomm_INSTANCE *instance)
{
    memset(instance->raw_recv_buff,0,sizeof(BOARD_CANcomm_INSTANCE));
    instance->recv_state=0;
    instance->current_recv_len=0;
}

//帧头+数据长度+数据帧+校验位+帧尾
BOARD_CANcomm_INSTANCE *board_comm_init(BOARD_INIT_CANcomm_INSTANCE *config)
{
    BOARD_CANcomm_INSTANCE *board_comm= (BOARD_CANcomm_INSTANCE* ) malloc(sizeof(BOARD_CANcomm_INSTANCE));
    memset(board_comm,0,sizeof(BOARD_CANcomm_INSTANCE));

    board_comm->recv_buff_len=config->recv_buff_len;
    board_comm->send_buff_len=config->send_buff_len;
    board_comm->recv_data_len=config->recv_data_len;
    board_comm->send_data_len=config->send_data_len;
    board_comm->raw_send_buff[0]=CAN_COMM_HEADER;
    board_comm->raw_send_buff[1]=config->send_data_len;
    board_comm->raw_send_buff[config->send_data_len+CAN_COMM_OFFSET_BYTES-1]=CAN_COMM_TAIL;
    config->can_config.id =board_comm;
    board_comm->board_can_comm=Can_Register(&config->can_config);
    return board_comm;
}

static void BoardCANcommRxCallback(CAN_INSTANCE_t *instance)
{
    BOARD_CANcomm_INSTANCE *comm=(BOARD_CANcomm_INSTANCE *)instance->id;
    if(instance->rx_buff[0]==CAN_COMM_HEADER&&comm->recv_state==0)
    {
        if(instance->rx_buff[1]==comm->recv_data_len)
        {
            comm->recv_state=1;
        }
        else
        {
            return;
        }
    }
    if(comm->recv_state)
    {
        if(comm->current_recv_len+instance->rx_len>comm->recv_buff_len)
        {
            BoardcommResetRx(comm);
            return;
        }
        memcpy(comm->raw_recv_buff+comm->current_recv_len,instance->rx_buff,instance->rx_len);
        comm->current_recv_len+=instance->rx_len;
        if(comm->current_recv_len==comm->recv_buff_len)
        {
            if(comm->raw_recv_buff[comm->recv_buff_len-1]==CAN_COMM_TAIL)
            {
                if(comm->raw_recv_buff[comm->recv_buff_len-2]==crc_8(comm->raw_recv_buff+2,comm->recv_buff_len))
                {
                    memcpy(comm->unpacked_recv_buff,comm->raw_recv_buff+2,comm->recv_data_len);
                    comm->update_flag=1;
                }
            }
        }
        BoardcommResetRx(comm);
        return;
    }
}

void BOARD_can_send(BOARD_CANcomm_INSTANCE *instance,uint8_t *data)
{   
    static uint8_t crc8;
    static uint8_t send_len;

    memcpy(instance->raw_send_buff+ 2 ,data,instance->send_data_len);
    crc8= crc_8(data,instance->send_data_len);
    instance->raw_send_buff[2+instance->send_data_len]=crc8;
    for(size_t i=0;i<instance->send_buff_len;i+=8)
    {
        send_len=instance->send_buff_len-i>=8?8:instance->send_buff_len-i;
        CANSetDLC(instance->board_can_comm,send_len);
        memcpy(instance->board_can_comm->tx_buff,instance->raw_send_buff+i,send_len);
        CANTransmit(instance->board_can_comm,1);
    }
}

void *Board_CAN_Get(BOARD_CANcomm_INSTANCE *instance)
{
    instance->update_flag=0;
    return instance->unpacked_recv_buff;
}


