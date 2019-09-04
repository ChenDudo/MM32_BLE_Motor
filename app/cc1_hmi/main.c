////////////////////////////////////////////////////////////////////////////////
/// @file     MAIN.C
/// @author   D CHEN
/// @version  2.0.0 Beta4
/// @date     2019-07-02
/// @brief    THIS FILE PROVIDES ALL THE SPI_DMA EXAMPLE.
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

////////////////////////////////////////////////////////////////////////////////
#define _MAIN_C_
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "system_mm32.h"
#include "mm32.h"

#include "drv.h"
#include "resource.h"


#include "rtc.h"
#include "datetime.h"
#include "lcd.h"
#include "uart.h"
#include "tim.h"


#include "queue.h"
#include "touch.h"

////////////////////////////////////////////////////////////////////////////////
void AppTaskTick()
{
    if (tickCnt++ >= 250) {
        tickCnt  = 0;
        tickFlag = true;
    }
    decodeTick();
    setTimTick();
    beepTick();
    AlarmTick();
    encodeTick();
    
}

////////////////////////////////////////////////////////////////////////////////
void initPara()
{
    recFlag     = false;
    txSendFlag  = false;
    beepEn      = false;
    isFirstRx   = true;
    beepFlag    = false;
    autoModeFlag= false;
    rev017Sta   = 0;
    ledCmd      = 0;
    timTick     = 0;
    uartSendTick = 0;
    beepMode    = biNone;
}


////////////////////////////////////////////////////////////////////////////////
void initPeripheral()
{
    lcd_init();
    initUART(COMx);
    initBeepTimer();
    initLcdTimer();
    while(initRTC()){};
    while(TPAD_Init(6));
    QueueConfig();
}

u8 trueLed;
////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);
    
    initPara();
    initPeripheral();
    vdLED = 0x01;
    
    while(1){
        lcdTick();
        
        if((SysKeyboard(&vkKey) && (vkKey == VK_K0)) || TPAD_Scan0(0)){
            beepMode = bi;
            autoModeFlag ? (autoModeFlag = false) : (autoModeFlag = true);
        }
        if((SysKeyboard(&vkKey) && (vkKey == VK_K1)) || TPAD_Scan1(0)){
            beepEn ? (beepEn = false) : (beepEn = true);
            beepMode = bi;
        }
        if((SysKeyboard(&vkKey) && (vkKey == VK_K2)) || TPAD_Scan2(0)){
            beepMode = bi;
            (ledCmd != 1) ? (ledCmd = 1) : (ledCmd = 2);
        }
        if (tickFlag) {                
            vdLED = vdLED << 1;
            if(vdLED > 8)
                vdLED = 1;
            tickFlag = false;
            trueLed = ~vdLED;
        }
        SysDisplay((u8*)&trueLed);
        
        /*  This is old version without touch : 20190904
        //        if (SysKeyboard(&vkKey)) {
        //            switch  (vkKey) {
        //                case  VK_K0:
        //                beepMode = bi;
        //                (vdLED & 0x01) ? (vdLED &= ~0x01) : (vdLED |= 0x01); // toggle LD1
        //                autoModeFlag ? (autoModeFlag = false) : (autoModeFlag = true);
        //                KeyProcess_Key0();
        //                break;
        //                case  VK_K1:
        //                beepEn ? (beepEn = false) : (beepEn = true);
        //                beepMode = bi;
        //                (vdLED & 0x02) ? (vdLED &= ~0x02) : (vdLED |= 0x02); // toggle LD1
        //                KeyProcess_Key1();
        //                break;
        //                case  VK_K2:
        //                beepMode = bi;
        //                (ledCmd != 1) ? (ledCmd = 1) : (ledCmd = 2);
        //                KeyProcess_Key2();
        //                break;
        //                default:
        //                break;
        //            }
        //        }
        */
        
        
    }
    
}
