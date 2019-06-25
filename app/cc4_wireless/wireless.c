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

#include "wl_ble_services.h"
#include "ble.h"
#include "2_4g.h"
//#include "wt2031.h"

#include "wl_2_4g_cfg.h"
#include "HAL_exti.h"

#include "uart.h"
#include "sync.h"

////////////////////////////////////////////////////////////////////////////////
void EXTI0_1_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_Line1)) {
        EXTI_ClearITPendingBit(EXTI_Line1);
        if (wl_mode == emWL_BLE)
            wl_ble_irq_handler();
        
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
    
    writeDataTick();
    readDataTick();
    
    if(!(--uartTimeOut)) {
        isFirstRx = true;        
    }
}

////////////////////////////////////////////////////////////////////////////////
void initPara()
{
    bufLen      = 0;
    syncFlag    = false;
    recFlag     = false;
    txSendFlag  = false;
    isFirstRx   = true;
    
    wl_mode = emWL_BLE;
    
    memset(uartTxBuf, 0x00, sizeof(uartTxBuf));
    memset(uartRxBuf, 0x00, sizeof(uartRxBuf));
    
    memset(ble_rx_buf, 0x00, sizeof(ble_rx_buf));
    memset(ble_tx_buf, 0x00, sizeof(ble_tx_buf));
    
    ptrUart = uartRxBuf;
}


////////////////////////////////////////////////////////////////////////////////
void initPeripheral()
{
    if (wl_mode == emWL_BLE)    wl_ble_mode();
    if (wl_mode == emWL_2_4G)   wl_2_4g_mode();
    
    initUART(COMx);
    //initSyncPin_Master();
    
    //wt2031_init();
}


#include "hal_uart.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief  main function.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
int main(void)
{
	MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);
    
    initPara();
    initPeripheral();
    
    while (1) {
        
        if(UART_GetITStatus(UART1, UART_IT_RXIEN) != RESET) {
            if (isFirstRx) {
                bufLen = (u16)UART1->RDR;
                ptrUart = uartRxBuf;
                isFirstRx = false;
                uartTimeOut = 2;
            }
            else {
                if (bufLen--) {
                    *ptrUart = (u16)UART1->RDR;
                    ptrUart ++;
                    if (bufLen == 0) {
                        isFirstRx = true;
                        recFlag = true;
                    }
                }
            }
            UART_ClearITPendingBit(UART1, UART_ICR_RXICLR);
        }
        
        
        //        if (wl_mode == emWL_BLE)    wl_ble_task();
        //        if (wl_mode == emWL_2_4G)   wl_2_4g_task();
    }
}

