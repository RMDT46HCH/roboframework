#ifndef REOMOTE_CONTROL_H
#define REOMOTE_CONTROL_H
#include "bsp_usart.h"
#include "stdint.h"

#define LAST 1
#define TEMP 0

#define KEY_PRESS 0
#define KEY_PRESS_WITH_CTRL 1
#define KEY_PRESS_WITH_SHIFT 2

#define RC_SW_UP ((uint16_t)1)
#define RC_SW_MID ((uint16_t)3)
#define RC_SW_DOWN ((uint16_t)2)

#define switch_is_up(s) (s==RC_SW_UP)
#define switch_is_mid(s) (s==RC_SW_MID)
#define switch_is_down(s) (s==RC_SW_DOWN)

#define RC_CH_MIN 364
#define RC_CH_OFFSET 1024
#define RC_CH_MAX 1684

typedef union 
{
    struct
    {
        uint16_t w:1;
        uint16_t s:1;
        uint16_t a:1;
        uint16_t d:1;        
        uint16_t q:1;
        uint16_t e:1;        
        uint16_t r:1;
        uint16_t g:1;        
        uint16_t ctrl:1;
        uint16_t shift:1;        
    };
    uint16_t keys;
}KEY_t;

typedef struct
{   struct 
    {
        int16_t rocker_left_x;
        int16_t rocker_left_y;
        int16_t rocker_right_x;
        int16_t rocker_right_y;

        uint8_t switch_left;
        uint8_t switch_right;
        uint16_t dial;
    }rc;
    struct
    {
       int16_t x;
       int16_t y;
       uint8_t press_l;
       uint8_t press_r;
    }mouse;
    KEY_t key[3];
    uint8_t key_count[3][16];
}rc_ctrl_t;

rc_ctrl_t *RemoteControlInit(UART_HandleTypeDef *rc_usart_handle);


#endif // !REOMOTE_CONTROL_H