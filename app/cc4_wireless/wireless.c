// Define to prevent recursive inclusion  --------------------------------------
#define _WIRELESS_C_

// Includes  -------------------------------------------------------------------
#include "types.h"
#include "mm32.h"
#include <string.h>
#include "system_mm32.h"

#include "drv.h"
#include "wireless.h"
#include "resource.h"

#include "HAL_gpio.h"

#include "wl_ble_services.h"
#include "ble.h"
#include "2_4g.h"
#include "wt2031.h"

#include "wl_2_4g_cfg.h"
#include "HAL_exti.h"

void EXTI0_1_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_Line1)) {
        EXTI_ClearITPendingBit(EXTI_Line1);
        if (wl_mode == emWL_2_4G)
            wl_2_4g_irq_handler();
    }
}

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

    if (wl_mode == emWL_BLE)
        wl_ble_tick_task();
    else if (wl_mode == emWL_2_4G)
        wl_2_4g_tick_task();

    if(SysKeyboard(&vkKey) && (vkKey == VK_K1)) {
        wl_tx_flag = true;
    }

    if (wl_mode == emWL_2_4G && !(vdLED & 0x01)) {
        vdLED |= 0x01;
        vdLED &=~ 0x02;
        SysDisplay(&vdLED);
    }
    else if (wl_mode == emWL_BLE  && !(vdLED & 0x02)) {
        vdLED |= 0x02;
        vdLED &=~ 0x01;
        SysDisplay(&vdLED);
    }
    else if (wl_mode == emWL_NULL) {
        vdLED &=~ 0x03;
        SysDisplay(&vdLED);
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  main function.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);
    wl_mode = emWL_BLE;

    //wt2031_init();

    if (wl_mode == emWL_BLE)    wl_ble_mode();
    if (wl_mode == emWL_2_4G)   wl_2_4g_mode();

    while (1) {

//        if(!WT2031_READY()) {
//            wt2031_rw();
//        }

        if (wl_mode == emWL_BLE)    wl_ble_task();
        if (wl_mode == emWL_2_4G)   wl_2_4g_task();
    }
}

