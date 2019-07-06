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

////////////////////////////////////////////////////////////////////////////////
void AppTaskTick()
{
    if (tickCnt++ >= 500) {
        tickCnt  = 0;
        tickFlag = true;
    }
    //rtcTick();
    
}

////////////////////////////////////////////////////////////////////////////////
void initPara()
{
    
    
}


////////////////////////////////////////////////////////////////////////////////
void initPeripheral()
{
    lcd_init();
    while(initRTC()){};
    
}

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);
    
    initPara();
    initPeripheral();
    
    while(1){
        lcdTick();
        
        if (SysKeyboard(&vkKey)) {
            switch  (vkKey) {
                case  VK_K0:
                (vdLED & 0x01) ? (vdLED &= ~0x01) : (vdLED |= 0x01); // toggle LD1
                KeyProcess_Key0();
                break;
                case  VK_K1:
                (vdLED & 0x01) ? (vdLED &= ~0x01) : (vdLED |= 0x01); // toggle LD1
                KeyProcess_Key1();
                break;
                case  VK_K2:
                (vdLED & 0x01) ? (vdLED &= ~0x01) : (vdLED |= 0x01); // toggle LD1
                KeyProcess_Key2();
                break;
                case  VK_K3:
                (vdLED & 0x01) ? (vdLED &= ~0x01) : (vdLED |= 0x01); // toggle LD1
                KeyProcess_Key3();
                break;
                default:
                break;
            }
        }
        if (tickFlag) {
            
            (vdLED & 0x02) ? (vdLED &= ~0x02) : (vdLED |= 0x02); // toggle LD2
            
            tickFlag = false;
        }
        SysDisplay((u8*)&vdLED);
    }
    
}
