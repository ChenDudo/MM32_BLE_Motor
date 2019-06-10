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
#define _DC_MOTOR_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "drv.h"
#include "resource.h"
#include "motor.h"
#include "tb6612.h"

#include "hal_tim.h"
bool samSpeedFlag;
u32 speed1, speed2;
u32 samCnt = 0;


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
    if (samCnt++ >= 1000) {
        samCnt = 0;
        samSpeedFlag = true;
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

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

    emDC_handle dcHandle = {
        .dcPulseMax     = 100,

        .dc1Sta         = emDC_Run,
        .dc1Dir         = 0,
        .dc1PulseWidth  = 50,

        .dc2Sta         = emDC_Run,
        .dc2Dir         = 0,
        .dc2PulseWidth  = 50,
    };

    emMotor_handle motor1 = {
        .motorType  = emMotor_DC,
        .motorSta   = emMotor_Standby,
        .handle     = (int*)&dcHandle
    };

    initMotor(motor1);

    HANDLE hADC = CreateFile(emIP_ADC);
	if (hADC == NULL)		while(1);

    //--- adc ch1 --------------------------------------------------------------
    tAPP_ADC_DCB dcb = {
		.hSub	= emFILE_ADC1,					// EM_FILE_ADC
		.type	= emTYPE_DMA,    				// polling, interrupt, dma
		.mode	= emADC_Scan, 				    // Conversion mode: emADC_Imm,emADC_Scan,emADC_Continue
		.sync	= emTYPE_ASync,      			// emTYPE_Sync, emTYPE_ASync
		.trig 	= emTRIGGER_Software,           // Software Start & Trigger enum
		.chs    = LEFT_SHIFT_BIT(1),            // channels: ADC_CH_n
		.temp 	= false,            			// Temperature measurement:0(DISABLE),1(ENABLE)
		.vRef	= false,            			// Reference voltage:0(DISABLE),1(ENABLE)
		.cb		= NULL,   			            // adc callback
	};

    if (!OpenFile(hADC, (void*)&dcb))		while(1);
    u32 adc_temp = 0;
    u16 dcSpeed = 0;

    initECA();
    initECB();

    samSpeedFlag = false;
    speed1 = speed2 = 0;
    u8 begin;

    while (1) {

        if (samSpeedFlag) {
            if (begin++ == 0) {
                TIM_SetCounter(TIM1, 0);
                TIM_SetCounter(TIM3, 0);
            }
            else {
                speed1 = TIM_GetCounter(TIM1);
                speed2 = TIM_GetCounter(TIM3);
                begin = 0;
            }
            samSpeedFlag = false;
        }

        WriteFile(hADC, emFILE_ADC1, 0, 0);
        ReadFile(hADC, emFILE_ADC1, (u8*)&adc_temp, 16);
        dcSpeed = (u16)((double)adc_temp / 4096 * dcHandle.dcPulseMax);
        dcHandle.dc1PulseWidth = dcSpeed;
        dcHandle.dc2PulseWidth = dcSpeed;

        dcMotorRun(&dcHandle);

        if (SysKeyboard(&vkKey)) {
            switch  (vkKey) {
                case  VK_K0:
                KeyProcess_Key0();
                break;
                case  VK_K1:
                KeyProcess_Key1();
                break;
                case  VK_K2:
                KeyProcess_Key2();
                break;
                case  VK_K3:
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
