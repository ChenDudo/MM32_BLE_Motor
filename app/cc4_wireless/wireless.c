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
//#include "wt2031.h"

#include "wl_2_4g_cfg.h"
#include "HAL_exti.h"

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
}

////////////////////////////////////////////////////////////////////////////////
#include "hal_uart.h"
#include "hal_gpio.h"
#include "bsp.h"
#include "common.h"
void initUART(UART_TypeDef* UARTx)
{
    UART_InitTypeDef UART_InitStructure;

    UART_InitStructure.BaudRate = 115200;
    UART_InitStructure.WordLength = UART_WordLength_8b;
    UART_InitStructure.StopBits = UART_StopBits_1;
    UART_InitStructure.Parity = UART_Parity_No;
    UART_InitStructure.HWFlowControl = UART_HWFlowControl_RTS_CTS;
    UART_InitStructure.Mode = UART_Mode_Rx | UART_Mode_Tx;

    if (UARTx == UART1) {
        COMMON_EnableIpClock(emCLOCK_UART1);
        COMMON_EnableIpClock(emCLOCK_GPIOB);

        GPIO_Mode_AF_PP_20MHz_Init(GPIOB, GPIO_Pin_6, EXTI_MAPR_UART1, 	GPIO_AF_0);
        GPIO_Mode_IPU_Init		  (GPIOB, GPIO_Pin_7, EXTI_MAPR_UART1, 	GPIO_AF_0);
    }
    if (UARTx == UART2) {
        COMMON_EnableIpClock(emCLOCK_UART2);
        COMMON_EnableIpClock(emCLOCK_GPIOA);

        GPIO_Mode_AF_PP_20MHz_Init(GPIOA, GPIO_Pin_2, NO_REMAP, 		GPIO_AF_1);
        GPIO_Mode_IPU_Init		  (GPIOA, GPIO_Pin_3, NO_REMAP, 		GPIO_AF_1);
    }

    UART_Init(UARTx, &UART_InitStructure);
    //UART_ITConfig(UARTx, UART_IT_RXIEN, ENABLE);
    UART_Cmd(UARTx, ENABLE);
}

#define COMx    UART1

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

    initUART(COMx);
    u16 len = 0;
    memset(ble_rx_buf, 0x00, sizeof(ble_rx_buf));

    while (1) {

        if (bleWriteFlag) {
            len = strlen(ble_rx_buf);
            u8* ptr = ble_rx_buf;
            while (len--) {
                UART_SendData(COMx, (u8)*ptr++);
                while (UART_GetFlagStatus(COMx, UART_IT_TXIEN) == RESET);
            }
            memset(ble_rx_buf, 0x00, sizeof(ble_rx_buf));
            bleWriteFlag = false;
        }







        //        if(!WT2031_READY()) {
        //            wt2031_rw();
        //        }

//        if (wl_mode == emWL_BLE)    wl_ble_task();
//        if (wl_mode == emWL_2_4G)   wl_2_4g_task();
    }
}

