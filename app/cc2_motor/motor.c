////////////////////////////////////////////////////////////////////////////////
/// @file     MOTOR.C
/// @author   D Chen
/// @version  v2.0.0
/// @date     2019-06-26
/// @brief    THIS FILE PROVIDES ALL THE MOTOR EXAMPLE.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT 2018-2019 MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion  --------------------------------------
#define _MOTOR_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "drv.h"
#include "resource.h"

#include "hal_tim.h"
#include "common.h"

#include "motor.h"
#include "tb6612.h"
#include "adc.h"
////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void motorTick()
{
    finalPWMValueMax = securMax * pwmSetValue / 100;
    finalBreathLen   = breathValue;
    if(breathFlag) {
        // breath mode
        writeBreathLen();
        OnTimer16();
    }
    else {
        // normal mode
        OffTimer16();
        dcHandle.dc1PulseWidth = finalPWMValueMax;
    }
    
    dcMotorRun(&dcHandle);
}

u8 pwmList[] = {
74,74,75,77,78,80,82,85,88,92,
96,101,106,112,119,127,135,144,153,162,
171,180,189,197,205,212,218,223,228,232,
236,239,242,244,246,247,249,250,250,251,
252,253,254,255,255,255,254,253,252,251,
250,250,249,247,246,244,242,239,236,232,
228,223,218,212,205,197,189,180,171,162,
153,144,135,127,119,112,106,101,96,92,
88,85,82,80,78,77,75,74,74,73
};

////////////////////////////////////////////////////////////////////////////////
void TIM16_IRQHandler()
{
    if (TIM_GetITStatus(TIM16, TIM_IT_Update) != RESET) {
        gDelaynum ++;
        if(gDelaynum >= gDelayTime){ 
            dcHandle.dc1PulseWidth = (u16)(pwmList[pwmListCnt] * finalPWMValueMax / 255);
            pwmListCnt ++;
            if (pwmListCnt >=  strlen((char*)pwmList)){                                 //Check the table to find the last one, redirected to the header
                pwmListCnt = 0;
            }
            gDelaynum = 0; 
        }
        TIM_ClearITPendingBit(TIM16, TIM_IT_Update);
    }
}

////////////////////////////////////////////////////////////////////////////////
void writeBreathLen()
{
    TIM16->ARR = (u16)(breathValue * 20 + 1000);           //range 1020 - 3000
    gDelayTime = (u16)(breathValue + 100);                // range 10 - 110
}

////////////////////////////////////////////////////////////////////////////////
void initTimer16()
{    
    COMMON_EnableIpClock(emCLOCK_TIM16);
    
    TIM_TimeBaseInitTypeDef pBase = {
        .TIM_Prescaler         = 95,                // 0 ~ 0xFFFF, counter_Freq = APBx_Freq / (timPsc + 1)
        .TIM_Period            = 999,                   // 0 ~ 0xFFFF(some TIMx support 0xFFFFFFF)
        .TIM_ClockDivision     = TIM_CKD_DIV1,
        .TIM_CounterMode       = TIM_CounterMode_Up,    /// if use \TIM_CounterMode_CenterAligned3, PWM period is A half
        .TIM_RepetitionCounter = 0
    };
    TIM_TimeBaseInit(TIM16, &pBase);
    
    TIM_ARRPreloadConfig(TIM16, ENABLE);
    
    COMMON_NVIC_Configure(TIM16_IRQn,  1,  1);
    TIM_ClearITPendingBit(TIM16, TIM_IT_Update);
    TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);
    
    OffTimer16();
}

////////////////////////////////////////////////////////////////////////////////
void OnTimer16()
{
    TIM_Cmd(TIM16, ENABLE);
}

////////////////////////////////////////////////////////////////////////////////
void OffTimer16()
{
    TIM_Cmd(TIM16, DISABLE);
}

////////////////////////////////////////////////////////////////////////////////
void initDCmotor(void)
{
    emMotor_handle motor1 = {
        .motorType  = emMotor_DC,
        .motorSta   = emMotor_Standby,
        .handle     = (int*)&dcHandle
    };
    
    initMotor(motor1);
    
    dcMotorRun(&dcHandle);
}


/// @}

/// @}

/// @}
