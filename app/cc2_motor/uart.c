// Define to prevent recursive inclusion  --------------------------------------
#define _UART_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "common.h"
#include "bsp.h"
#include "uart.h"
#include "hal_uart.h"
#include "hal_gpio.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void uartTick()
{

}

////////////////////////////////////////////////////////////////////////////////
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
    UART_ClearITPendingBit(UARTx, UART_IT_RXIEN);
    UART_ITConfig(UARTx, UART_IT_RXIEN, ENABLE);
    UART_Cmd(UARTx, ENABLE);
}


////////////////////////////////////////////////////////////////////////////////
void UART1_IRQHandler()
{
    static u8 i = 0;
	if(UART_GetITStatus(UART1, UART_IT_RXIEN) != RESET) {
		UART_ClearITPendingBit(UART1, UART_IT_RXIEN);
        uartRxBuf[i++] = UART_ReceiveData(UART1);
	}
}

////////////////////////////////////////////////////////////////////////////////
void UART2_IRQHandler()
{
    static u8 i = 0;
	if(UART_GetITStatus(UART2, UART_IT_RXIEN) != RESET) {
		UART_ClearITPendingBit(UART2, UART_IT_RXIEN);
        uartRxBuf[i++] = UART_ReceiveData(UART2);
	}
}

//        if (recFlag) {
//            ptr = uartSynReceive;
//            switch(*ptr) {
//                case 0x01: {
//                    if(*(ptr + 1) == 0x01)
//                        dcHandle.dc1Sta = emDC_Run;
//                    if(*(ptr + 1) == 0x02)
//                        dcHandle.dc1Sta = emDC_Stop;
//                }
//                break;
//                case 0x02: {
//                    dcHandle.dc1PulseWidth = securityPwmMax * ptr[1] / 100;
//                }
//                break;
//                case 0x03:
//                break;
//                default:
//                break;
//            }
//            memset(uartSynReceive, 0x00, sizeof(uartSynReceive));
//            recFlag = false;
//        }