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
#include "adc.h"
#include "uart.h"

u32  ledTick;
bool ledFlag;

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AppTaskTick()
{
    if (ledTick++ >= 499) {
        ledTick  = 0;
        ledFlag = true;
    }
    adcTick();
    decodeTick();
    exdecodeTick();

    if(!(--uartTimeOut)) {
        isFirstRx = true;
    }
    if(!(--exuartTimeOut)) {
        exisFirstRx = true;
    }
    encodeTick();
    exencodeTick();
}

////////////////////////////////////////////////////////////////////////////////
void initPara()
{
    ledTick     = 0;
    bufLen      = 0;
    ledFlag     = false;
    recFlag     = false;
    txSendFlag  = false;
    isFirstRx   = true;
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

    memset(uartTxBuf, 0x00, sizeof(uartTxBuf));
    memset(uartRxBuf, 0x00, sizeof(uartRxBuf));
    memset(exuartTxBuf, 0x00, sizeof(exuartTxBuf));
    memset(exuartRxBuf, 0x00, sizeof(exuartRxBuf));

    ptrUart = uartRxBuf;
}

////////////////////////////////////////////////////////////////////////////////
void initPeripheral()
{
    initADC();
    initUART(COMx);
    initUART(exCOMx);

}

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

    initPara();
    initPeripheral();
    vdLED = 1;
    
    while (1) {
        //----------- user code -----------


        //----------- key && led ----------
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
        if (ledFlag) {                
            vdLED = vdLED << 1;
            if(vdLED > 8)
                vdLED = 1;
            ledFlag = false;
        }
        SysDisplay((u8*)&vdLED);
    }
}

/// @}

/// @}

/// @}
