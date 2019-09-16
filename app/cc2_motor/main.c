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
#include "flash.h"

u32  ledTick;
bool ledFlag;

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AppTaskTick()
{
    if (ledTick++ >= 249) {
        ledTick  = 0;
        ledFlag = true;
    }
    adcTick();
    syncTick();
    decodeTick();
    exdecodeTick();
    changeSecMax();
    motorTick();
    if(!(--uartTimeOut)) {
        isFirstRx = true;
    }
    if(!(--exuartTimeOut)) {
        exisFirstRx = true;
    }
    flashTick();
    encodeTick();
    exencodeTick();
}

////////////////////////////////////////////////////////////////////////////////
void initPara()
{
    ledTick     = 0;
    bufLen      = 0;
    ledFlag     = false;
    syncFlag    = false;
    recFlag     = false;
    txSendFlag  = false;
    isFirstRx   = true;
    isFirstOpen = true;
    adcVolFlag  = false;
    adcTempFlag = false;

    ///////////////
    exrecFlag       = false;
    extxSendFlag    = false;
    exisFirstRx     = true;
    exuartTimeOut   = 2;
    exbufLen        = 0;
    exSendStaTick   = 0;

    adcVolTick  = 0;
    adcTempTick = 0;
    adcSendTick = 0;
    uartTimeOut = 2;
    flashTickCnt= 0;

    //
    gDelayTime = 1;
    pwmListCnt = 0;
    gDelaynum  = 0;
    breathFlag  = false;
    breathValue = 0;
    pwmSetFlag  = false;
    pwmSetValue = PWM_DEFAULT_NUM;       //190916 chend: default = 50
    finalPWMValueMax = 0;
    finalBreathLen   = 999;

    dcHandle.dcPulseMax     = 100;
    dcHandle.dc1Sta         = emDC_Run;            //190916 chend: default = run
    dcHandle.dc1Dir         = 0;
    dcHandle.dc1PulseWidth  = 0;
    dcHandle.dc2Sta         = emDC_Stop;
    dcHandle.dc2Dir         = 0;
    dcHandle.dc2PulseWidth  = 0;

    securMax = (u16)(dcHandle.dcPulseMax * 0.9);    //190916 chend: default = 90%

    memset(uartTxBuf, 0x00, sizeof(uartTxBuf));
    memset(uartRxBuf, 0x00, sizeof(uartRxBuf));
    memset(exuartTxBuf, 0x00, sizeof(exuartTxBuf));
    memset(exuartRxBuf, 0x00, sizeof(exuartRxBuf));

    ptrUart = uartRxBuf;
}

////////////////////////////////////////////////////////////////////////////////
void initPeripheral()
{
    initDCmotor();
    initADC();
    initUART(COMx);
    initUART(exCOMx);
    initSyncPin_Slave();
    initTimer16();
}

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

    initPara();
    initPeripheral();

    while (1) {
        //----------- user code -----------


        //----------- key && led ----------
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
            (vdLED & 0x02) ? (vdLED &= ~0x02) : (vdLED |= 0x02);
            ledFlag = false;
            (dcHandle.dc1Sta == emDC_Run) ? (vdLED |= 0x01) : (vdLED &= ~0x01);
        }
        SysDisplay((u8*)&vdLED);
    }
}

/// @}

/// @}

/// @}
