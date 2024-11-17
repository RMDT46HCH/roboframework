#include "buzzer.h"
#include "string.h"
#include "bsp_dwt.h"

static Buzzer_instance_t *buzzer_instance[BUZZER_DEVICE_CNT]={NULL};
static int16_t idx=0;
static PWM_Instance_t *buzzer;

void BuzzerInit()
{
    PWM_Init_Instance_t buzzer_config={
        .tim_handle=&htim4,
        .channel=TIM_CHANNEL_3,
        .dutyratio=0,
        .period=0.001,
    };
    buzzer=PWMRegister(&buzzer_config);
}

Buzzer_instance_t *BuzzerRegister(Buzzer_Init_Instance_t *config)
{
    if(idx>=BUZZER_DEVICE_CNT)
    {
        while (1)
        {
            ;
        }
    }
    Buzzer_instance_t *instance= (Buzzer_instance_t* ) malloc(sizeof(Buzzer_instance_t));
    memset(instance,0,sizeof(Buzzer_instance_t));

    instance->AlarmState=ALARM_OFF;
    instance->alarmlevel=config->alarmlevel;
    instance->loudess=config->loudess;
    instance->octave=config->octave;

    buzzer_instance[idx++]=instance;
    return instance;
}

void AlarmSetStatus(Buzzer_instance_t *buzzer,AlarmState_e state)
{
    buzzer->AlarmState=state;
}

void BuzzerTask()
{
    for(size_t i=0;i<BUZZER_DEVICE_CNT;++i)
    {
        if(buzzer_instance[i]->alarmlevel>ALARM_LEVEL_LOW)
        {
            continue;
        }
        if(buzzer_instance[i]->AlarmState=ALARM_OFF)
        {
            PWMSetDutyRatio(buzzer,0)
        }
        else
        {
            PWMSetDutyRatio(buzzer,buzzer_instance[i]->loudess)
            switch (buzzer_instance[i]->octave)
            {
                case OCTAVE_1:
                PWMSetPeriod(buzzer,(float)1/DOFreq);
                break;
                case OCTAVE_2:
                PWMSetPeriod(buzzer,(float)1/REFreq);
                break;
                case OCTAVE_3:
                PWMSetPeriod(buzzer,(float)1/MIFreq);
                break;
                case OCTAVE_4:
                PWMSetPeriod(buzzer,(float)1/FAFreq);
                break;
                case OCTAVE_5:
                PWMSetPeriod(buzzer,(float)1/SOFreq);
                break;
                case OCTAVE_6:
                PWMSetPeriod(buzzer,(float)1/LAFreq);
                break;
                case OCTAVE_7:
                PWMSetPeriod(buzzer,(float)1/SIFreq);
                break;
            default:
                break;
            }
        }
    }
}