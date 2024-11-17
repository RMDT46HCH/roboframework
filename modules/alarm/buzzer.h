#ifndef BUZZER_H
#define BUZZER_H
#include "bsp_pwm.h"

#define BUZZER_DEVICE_CNT 5
#define DOFreq 523
#define REFreq 587
#define MIFreq 659
#define FAFreq 698
#define SOFreq 784
#define LAFreq 880
#define SIFreq 988

typedef enum
{
    OCTAVE_1=0,
    OCTAVE_2,
    OCTAVE_3,
    OCTAVE_4,
    OCTAVE_5,
    OCTAVE_6,
    OCTAVE_7,
}octave_e;

typedef enum
{
    ALARM_LEVEL_HIGH=0,
    ALARM_LEVEL_ABOVE_MEDIUM,
    ALARM_LEVEL_MEDIUM,
    ALARM_LEVEL_BELOW_MEDIUM,
    ALARM_LEVEL_LOW,
}AlarmLevel_e;

typedef enum
{
    ALARM_OFF=0,
    ALARM_ON,
}AlarmState_e;

typedef struct
{
    AlarmLevel_e alarmlevel;
    AlarmState_e AlarmState;
    octave_e octave;
    float loudess;
}Buzzer_instance_t;

typedef struct
{
    AlarmLevel_e alarmlevel;
    octave_e octave;
    float loudess;
}Buzzer_Init_Instance_t;

#endif // !BUZZER_H
