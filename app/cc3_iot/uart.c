// Define to prevent recursive inclusion  --------------------------------------
#define _UART_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "common.h"
#include "bsp.h"
#include "uart.h"
#include "hal_uart.h"
#include "bsp_uart.h"

#include "adc.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void decodeTick()
{
   
}

////////////////////////////////////////////////////////////////////////////////
void exdecodeTick()
{
    
}

////////////////////////////////////////////////////////////////////////////////
void encodeTick()
{
    
}

////////////////////////////////////////////////////////////////////////////////
void exencodeTick()
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

    COMMON_EnableIpClock(emCLOCK_EXTI);

    if (UARTx == UART1) {
        COMMON_EnableIpClock(emCLOCK_UART1);
        COMMON_EnableIpClock(emCLOCK_GPIOA);

        BSP_UART_GPIO_Configure(UART1, 0, 0);
        //GPIO_Mode_AF_PP_20MHz_Init(GPIOA, GPIO_Pin_9, NO_REMAP, GPIO_AF_1);
        //GPIO_Mode_IPU_Init(GPIOA, GPIO_Pin_10, NO_REMAP, GPIO_AF_1);

        COMMON_NVIC_Configure(UART1_IRQn, 1, 1);
    }
    if (UARTx == UART2) {
        COMMON_EnableIpClock(emCLOCK_UART2);
        
        BSP_UART_GPIO_Configure(UART2, 0, 0);
        //GPIO_Mode_AF_PP_20MHz_Init(GPIOA, GPIO_Pin_2, NO_REMAP, GPIO_AF_1);
        //GPIO_Mode_IPU_Init(GPIOA, GPIO_Pin_3, NO_REMAP, GPIO_AF_1);

        COMMON_NVIC_Configure(UART2_IRQn, 0, 0);
    }
    UART_Init(UARTx, &UART_InitStructure);
    UART_ClearITPendingBit(UARTx, UART_IT_RXIEN);
    UART_ITConfig(UARTx, UART_IT_RXIEN, ENABLE);
    UART_Cmd(UARTx, ENABLE);
}


////////////////////////////////////////////////////////////////////////////////
void UART1_IRQHandler()
{
    if(UART_GetITStatus(UART1, UART_IT_RXIEN) != RESET) {
        UART2->TDR = UART1->RDR;
        UART_ClearITPendingBit(UART1, UART_ICR_RXICLR);
    }
}

////////////////////////////////////////////////////////////////////////////////
void UART2_IRQHandler()
{
    if(UART_GetITStatus(UART2, UART_IT_RXIEN) != RESET) {
        //UART2->TDR = UART2->RDR;
        UART_ClearITPendingBit(UART2, UART_ICR_RXICLR);
    }
}
