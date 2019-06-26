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
    1,2,3,3,4,4,5,6,7,8,9,10,12,14,17,19,23,25,29,
    33,39,44,52,59,70,80,94,107,125,130,143,150,167,180,191,200,210,223,235,245,255,255,255,245,
    235,223,210,200,191,180,167,150,143,130,125,107,94,80,70,59,52,44,39,33,29,
    25,23,19,17,14,12,10,9,8,7,6,5,4,4,3,3,2,2,1
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
    TIM16->ARR = (u16)(breathValue * 200 + 1000);           //range 15 - 60
    gDelayTime = (u16)(breathValue + 1000);                // range 10 - 110
}

////////////////////////////////////////////////////////////////////////////////
void initTimer16()
{    
    COMMON_EnableIpClock(emCLOCK_TIM16);
    
    TIM_TimeBaseInitTypeDef pBase = {
        .TIM_Prescaler         = 0,                // 0 ~ 0xFFFF, counter_Freq = APBx_Freq / (timPsc + 1)
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
