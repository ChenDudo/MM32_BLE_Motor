////////////////////////////////////////////////////////////////////////////////
/// @file     STEP_MOTOR.C
/// @author   D Chen
/// @version  v2.0.0
/// @date     2019-05-21
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
#define _STEP_MOTOR_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "drv.h"
#include "resource.h"
#include "motor.h"
#include "tb6612.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AppTaskTick()
{
    if (tickCnt++ >= 500) {
        tickCnt  = 0;
        tickFlag = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  set delay time
/// @param  ms: delay time
/// @retval bool: true: delay ms time
////////////////////////////////////////////////////////////////////////////////
bool delay(u16 ms)
{
    static u16 cnt = 0;
    if (cnt == 0) {
        cnt = ms;
    }
	else if (sysTickFlag1mS) {
        sysTickFlag1mS = false;
        if (cnt > 0)
            cnt--;
        if (cnt == 0)
            return true;
    }
    return false;
}

void (*fpStep)(void);
typedef void (*fp)();
fp stepInterface[4] = {
    step1of4,
    step2of4,
    step3of4,
    step4of4
};
emStepper_handle stepHandle;
emMotor_handle motor1;
u32 stepFreqMax = 1300;
u8  S_Flexible = 5;
u16 S_StartCnt = 0;
u16 S_Len = 1000;
u16 stepLen = 10000;
#pragma location=0x0800f800
static const u16 sModeData[1000] __attribute__((at(0X800F000))) = {
    0



};

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

    stepHandle.stepStart      = true       ;
    stepHandle.stepDir        = 0          ;
    stepHandle.stepIdx        = 0          ;
    stepHandle.stepPulseMax   = 100        ;
    stepHandle.stepPulseWidth = 100        ;
    stepHandle.stepSum        = stepLen      ;
    stepHandle.stepFreq       = stepFreqMax;

    motor1.motorType  = emMotor_Step     ;
    motor1.motorSta   = emMotor_Standby  ;
    motor1.handle     = (int*)&stepHandle;

    initMotor(motor1);

    bool S_Start    = true;


    while (1) {

//        if (sysTickFlag1mS) {
//            if(S_Start) {
//                stepHandle.stepPulseWidth = (u16)(stepHandle.stepPulseMax * 1);
//                if (++S_StartCnt <= S_Len) {
//                    stepHandle.stepFreq = (u16)S_curve(S_Len, 400, stepFreqMax, S_Flexible, S_StartCnt);
//                }
//                else
//                    S_Start = false;
//            }
//            sysTickFlag1mS = false;
//        }

        //stepperRun(&stepHandle);
        if (stepRunFlag) {
            motorStandby(false);
            if (!stepHandle.stepStart) {
                stepHandle.stepPulseWidth = (u16)(stepHandle.stepPulseMax * 0.05);
                stepHandle.stepIdx = 0;
                motorStandby(true);
            }
            else {
                if(S_Start) {
                    stepHandle.stepPulseWidth = (u16)(stepHandle.stepPulseMax * 1);
                    if (++S_StartCnt <= S_Len) {
                        //stepHandle.stepFreq = (u16)S_curve(S_Len, 500, stepFreqMax, S_Flexible, S_StartCnt);
                        stepHandle.stepFreq = sModeData[S_StartCnt - 1];
                    }
                    else
                        S_Start = false;
                        stepHandle.stepPulseWidth = (u16)(stepHandle.stepPulseMax * 0.8);
                }
                if (stepHandle.stepDir) {
                    if (++stepHandle.stepIdx == 4)
                        stepHandle.stepIdx = 0;
                }
                else {
                    if (--stepHandle.stepIdx == -1)
                        stepHandle.stepIdx = 3;
                }
                if(--(stepHandle.stepSum) == 0)
                    stepHandle.stepStart = false;

            }
            writeStepPulseWidth(stepHandle.stepPulseWidth);
            writeStepFreq(stepHandle.stepFreq);
            fpStep = stepInterface[stepHandle.stepIdx];
            fpStep();

            stepRunFlag = false;
        }

        if (SysKeyboard(&vkKey)) {
            switch  (vkKey) {
                case  VK_K0:
                (vdLED & 0x01) ? (vdLED &= ~0x01) : (vdLED |= 0x01); // toggle LD3
                KeyProcess_Key0();
                break;
                case  VK_K1:
                (stepHandle.stepDir) ?  (stepHandle.stepDir = 0) : (stepHandle.stepDir = 1);
                KeyProcess_Key1();
                break;
                case  VK_K2:
                S_Start = true;
                S_StartCnt = 0;
                stepHandle.stepStart = true;
                stepHandle.stepSum   = stepLen;
                KeyProcess_Key2();
                break;
                case  VK_K3:
                stepHandle.stepStart = false;
                stepHandle.stepSum   = 0;
                KeyProcess_Key3();
                break;
                default:
                break;
            }
        }

        if (tickFlag) {
            (vdLED & 0x02) ? (vdLED &= ~0x02) : (vdLED |= 0x02); // toggle LD3
            tickFlag = false;
        }
        SysDisplay((u8*)&vdLED);
    }
}

/// @}

/// @}

/// @}
