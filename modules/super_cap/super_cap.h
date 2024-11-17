#ifndef SUPER_CAP_H
#define SUPER_CAP_H
#include "bsp_can.h"

typedef struct
{
    uint16_t vol;
    uint16_t current;
    uint16_t power;
}SuperCap_Msg;

typedef struct
{
    SuperCap_Msg Msg;
    CAN_INSTANCE_t *supercap_can;
}SuperCap_Instance_t;

typedef struct
{
    CAN_INIT_INSTANCE_t config;
}SuperCap_Init_Instance_t;

#endif // SUPER_CAP_H
