#include "remote_control.h"
#include "memory.h"
rc_ctrl_t rc_ctrl[2];

static USART_Instance_t *rc_usart_instance;


static void sbus_to_rc(uint8_t *sbus_buff)
{
    rc_ctrl[TEMP].rc.rocker_right_x=((sbus_buff[0]|sbus_buff[1]<<8)&0x07FF)-RC_CH_OFFSET;
    rc_ctrl[TEMP].rc.rocker_right_y=((sbus_buff[1]>>3|sbus_buff[2]<<5)&0x07FF)-RC_CH_OFFSET;
    rc_ctrl[TEMP].rc.rocker_left_x=((sbus_buff[2]>>6|sbus_buff[3]<<2|sbus_buff[4]<<10)&0x07FF)-RC_CH_OFFSET;
    rc_ctrl[TEMP].rc.rocker_left_y=((sbus_buff[4]>>1|sbus_buff[5]<<7)&0x07FF)-RC_CH_OFFSET; 
    rc_ctrl[TEMP].rc.switch_left=(sbus_buff[5]>>4)&0x0003;
    rc_ctrl[TEMP].rc.switch_right=((sbus_buff[5]>>4)&0x000C)>>2;
    rc_ctrl[TEMP].rc.dial=((sbus_buff[16]|sbus_buff[1]<<8)&0x07FF)-RC_CH_OFFSET;
    rc_ctrl[TEMP].mouse.x=(sbus_buff[6]|(sbus_buff[7]<<8));
    rc_ctrl[TEMP].mouse.y=(sbus_buff[8]|(sbus_buff[9]<<8));
    rc_ctrl[TEMP].mouse.press_l=(sbus_buff[12]);
    rc_ctrl[TEMP].mouse.press_r=(sbus_buff[13]);
    *(uint16_t*)&rc_ctrl[TEMP].key[KEY_PRESS]=(uint16_t)(sbus_buff[14]|(sbus_buff[15]<<8));
    
    if(rc_ctrl[TEMP].key[KEY_PRESS].ctrl)
    {
        rc_ctrl[TEMP].key[KEY_PRESS_WITH_CTRL]=rc_ctrl[TEMP].key[KEY_PRESS];
    }
    else
    {
        memset(& rc_ctrl[TEMP].key[KEY_PRESS_WITH_CTRL],0,sizeof(KEY_t));
    }
    if(rc_ctrl[TEMP].key[KEY_PRESS].shift)
    {
        rc_ctrl[TEMP].key[KEY_PRESS_WITH_SHIFT]=rc_ctrl[TEMP].key[KEY_PRESS];
    }
    else
    {
        memset(&rc_ctrl[TEMP].key[KEY_PRESS_WITH_SHIFT],0,sizeof(KEY_t));
    }

    uint16_t key_now=rc_ctrl[TEMP].key[KEY_PRESS].keys;
    uint16_t key_last=rc_ctrl[LAST].key[KEY_PRESS].keys;
    uint16_t key_with_ctrl=rc_ctrl[TEMP].key[KEY_PRESS_WITH_CTRL].keys;
    uint16_t key_with_shift=rc_ctrl[TEMP].key[KEY_PRESS_WITH_SHIFT].keys;
    uint16_t key_with_ctrl_last=rc_ctrl[LAST].key[KEY_PRESS_WITH_CTRL].keys;
    uint16_t key_with_shift_last=rc_ctrl[LAST].key[KEY_PRESS_WITH_SHIFT].keys;
        
    for(uint16_t i=0,j=0x1;i<16;j<<=1,i++)
    {
        if(i==9||i==10)
        {
            continue;
        }
        if((key_now&j)&&!(key_last&j)&&!(key_with_ctrl&j)&&!(key_with_shift&j))
        rc_ctrl[TEMP].key_count[KEY_PRESS][i]++;
        if((key_with_ctrl&j)&&!(key_with_ctrl_last&j))
        rc_ctrl[TEMP].key_count[KEY_PRESS_WITH_CTRL][i]++;
        if((key_with_shift&j)&&!(key_with_shift_last&j))
        rc_ctrl[TEMP].key_count[KEY_PRESS_WITH_SHIFT][i]++;
    }
    memcpy(&rc_ctrl[LAST],&rc_ctrl[TEMP],sizeof(rc_ctrl_t));
}

void RemoteControlCallback()
{
    sbus_to_rc(rc_usart_instance->recv_buff);
}

rc_ctrl_t *RemoteControlInit(UART_HandleTypeDef *rc_usart_handle)
{
    USART_Init_Instance_t config;
    config.usart_handle=rc_usart_handle;
    config.usart_module_callback=RemoteControlCallback;
    //接收机每隔14ms通过DBUS发送一帧18字节数据.
    config.recv_buff_size=18u;
    rc_usart_instance=USARTRegister(&config);
    return rc_ctrl;
}