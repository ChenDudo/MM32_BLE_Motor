////////////////////////////////////////////////////////////////////////////////
/// @file     MAIN.C
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
#define _MAIN_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"
#include "mm32.h"

#include "drv.h"
#include "resource.h"
#include "motor.h"
#include "tb6612.h"
#include "adc.h"
#include "uart.h"
#include "sync.h"

u32  ledCnt;
bool ledFlag;

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AppTaskTick()
{
    if (ledCnt++ >= 500) {
        ledCnt  = 0;
        ledFlag = true;
    }
    adcTick();
    uartTick();
    syncTick();
    motorTick();

}

////////////////////////////////////////////////////////////////////////////////
void initPara()
{
    ledCnt = 0;
    ledFlag = false;
    memset(uartTxBuf, 0x00, sizeof(uartTxBuf));
    memset(uartRxBuf, 0x00, sizeof(uartRxBuf));

}

#define COMx    UART2
////////////////////////////////////////////////////////////////////////////////
void initPeripheral()
{
    initDCmotor();
    initADC();
    initUART(COMx);
    initSyncPin_Slave();

}

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

    initPara();
    initPeripheral();

    while (1) {

        if (SysKeyboard(&vkKey)) {
            switch  (vkKey) {
                case  VK_K0:
                dcHandle.dc1Sta = emDC_Run;
                KeyProcess_Key0();
                break;
                case  VK_K1:
                dcHandle.dc1Sta = emDC_Stop;
                KeyProcess_Key1();
                break;
                case  VK_K2:
                dcHandle.dc1PulseWidth += 5;
                KeyProcess_Key2();
                break;
                case  VK_K3:
                dcHandle.dc1PulseWidth -= 5;
                KeyProcess_Key3();
                break;
                default:
                break;
            }
        }

        if (ledFlag) {
            (vdLED & 0x02) ? (vdLED &= ~0x02) : (vdLED |= 0x02); // toggle LD3
            ledFlag = false;
        }
        SysDisplay((u8*)&vdLED);
    }
}

/// @}

/// @}

/// @}