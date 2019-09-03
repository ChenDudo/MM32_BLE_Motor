////////////////////////////////////////////////////////////////////////////////
#define _TIM_C_
////////////////////////////////////////////////////////////////////////////////

#include "types.h"
#include "mm32.h"

#include "common.h"

#include "bsp.h"
#include "bsp_tim.h"
#include "hal_tim.h"
#include "hal_gpio.h"

#include "tim.h"

////////////////////////////////////////////////////////////////////////////////
void beepTick()
{
    if(beepEn){
        if (beepFlag) {
            switch(beepMode){
                case bi:
                timTick = 300;
                beepMode = biNone;
                break;
                case bibi:
                timTick = 500;
                beepMode = biNone;
                break;
                case bi_bi:
                timTick = 700;
                beepMode = biNone;
                break;
                default:
                beepOFF();
                break;
            }
        }
        
        if(timTick > 0) {
            beepFlag = false;
            
            if (timTick > 500)
                beepON();
            else if (timTick > 500)
                beepOFF();
            else if (timTick > 300)
                beepON();
            else if (timTick > 200)
                beepOFF();
            else
                beepON();
            
            timTick --;
        }
        else {
            beepOFF();
            beepFlag = true;
        }
    }
    
}

////////////////////////////////////////////////////////////////////////////////
void beepON()
{
    TIM_Cmd(TIM2, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void beepOFF()
{
    TIM_Cmd(TIM2, DISABLE);
}

////////////////////////////////////////////////////////////////////////////////
void initLcdTimer()
{
    BSP_TIM_CH4_GPIO_Configure(TIM4, 0, 0, 1);
    
    COMMON_EnableIpClock(emCLOCK_TIM4);
    
    TIM_TimeBaseInitTypeDef pBase = {
        .TIM_Prescaler         = 0,
        .TIM_Period            = 999,
        .TIM_ClockDivision     = TIM_CKD_DIV1,
        .TIM_CounterMode       = TIM_CounterMode_Up,
        .TIM_RepetitionCounter = 0
    };
    
    TIM_OCInitTypeDef pOC = {
        .TIM_Pulse        = 400,
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,
        .TIM_OCPolarity   = TIM_OCPolarity_High,
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Reset
    };
    TIM_TimeBaseInit(TIM4, &pBase);
    
    TIM_OC4Init(TIM4, &pOC);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
    
    TIM_CtrlPWMOutputs(TIM4, ENABLE);
    TIM_Cmd(TIM4, ENABLE);    
}

////////////////////////////////////////////////////////////////////////////////
void initBeepTimer()
{
    GPIO_Mode_OUT_OD_20MHz_Init(GPIOB, GPIO_Pin_3,  EXTI_MAPR_SWJ_JTAGDISABLE, GPIO_AF_0);
    BSP_TIM_CH2_GPIO_Configure(TIM2, 1, 2, 1);
    
    COMMON_EnableIpClock(emCLOCK_TIM2);
    
    TIM_TimeBaseInitTypeDef pBase = {
        .TIM_Prescaler         = 95,
        .TIM_Period            = 999,
        .TIM_ClockDivision     = TIM_CKD_DIV1,
        .TIM_CounterMode       = TIM_CounterMode_Up,
        .TIM_RepetitionCounter = 0
    };
    
    TIM_OCInitTypeDef pOC = {
        .TIM_Pulse        = 49,
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,
        .TIM_OCPolarity   = TIM_OCPolarity_High,
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Reset
    };
    TIM_TimeBaseInit(TIM2, &pBase);
    
    TIM_OC2Init(TIM2, &pOC);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    //TIM_Cmd(TIM2, ENABLE); 
    TIM_Cmd(TIM2, DISABLE); 
}
