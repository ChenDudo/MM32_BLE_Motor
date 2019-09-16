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

#include "sync.h"
#include "tb6612.h"
#include "adc.h"
#include "motor.h"
#include "flash.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void decodeTick()
{
    if (recFlag) {

        u8 *ptr = uartRxBuf;
        switch(*ptr) {
            case 0x01: {
                if(*(ptr + 1) == 0x01) {
                    isFirstOpen = true;             //190916 chend
                    dcHandle.dc1Sta = emDC_Run;
                }
                if(*(ptr + 1) == 0x02)
                    dcHandle.dc1Sta = emDC_Stop;
            }
            break;
            case 0x02: {
                //pwmSetFlag = true;
                pwmSetValue = (u16)(ptr[1] * 0.7 + 30);  //range (30 : 100)
            }
            break;
            case 0x03:
            break;
            case 0x04: {
                if(*(ptr + 1) == 0x01) {
                    breathFlag = true;
                    breathValue = 1;
                }
                if(*(ptr + 1) == 0x02)
                    breathFlag = false;
            }
            break;
            case 0x05:
                breathValue = ptr[1];
            break;
            case 0x06:
            case 0x07:
                extxSendFlag = true;
                memcpy(exuartTxBuf, uartRxBuf, sizeof(uartRxBuf));
            break;
            default:
            break;
        }

        memset(uartRxBuf, 0x00, sizeof(uartRxBuf));
        recFlag = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
void exdecodeTick()
{
    if (exrecFlag) {

        u8 *ptr = exuartRxBuf;
        switch(*ptr) {
            case 0x01: {
                if(*(ptr + 1) == 0x01) {
                    isFirstOpen = true;
                    dcHandle.dc1Sta = emDC_Run;
                }
                if(*(ptr + 1) == 0x02)
                    dcHandle.dc1Sta = emDC_Stop;
            }
            break;
            default:
            break;
        }

        memset(exuartRxBuf, 0x00, sizeof(exuartRxBuf));
        exrecFlag = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
void encodeTick()
{
    if (adcSendTick ++ > 10000) {
        adcSendTick = 0;
        if (adcTempFlag) {
            uartTxBuf[0] = 0x03;            //CMD 0x03: ADCTemp
            uartTxBuf[1] = (u8)adcTemp;
            uartTxBuf[2] = (u8)(((float)adcTemp - (u8)adcTemp) * 100);
            txSendFlag = true;
            adcTempFlag = false;
        }

        if (txSendFlag) {
            u16 len = strlen((char*)uartTxBuf);
            UART_SendData(COMx, (u16)len);
            while (UART_GetFlagStatus(COMx, UART_IT_TXIEN) == RESET);

            u8* ptr = uartTxBuf;
            while (len--) {
                UART_SendData(COMx, (u8)*ptr++);
                while (UART_GetFlagStatus(COMx, UART_IT_TXIEN) == RESET);
            }

            memset(uartTxBuf, 0x00, sizeof(uartTxBuf));
            txSendFlag  = false;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
void exencodeTick()
{
    if (extxSendFlag) {
        u16 len = strlen((char*)exuartTxBuf);
        UART_SendData(exCOMx, (u16)len);
        while (UART_GetFlagStatus(exCOMx, UART_IT_TXIEN) == RESET);

        u8* ptr = exuartTxBuf;
        while (len--) {
            UART_SendData(exCOMx, (u8)*ptr++);
            while (UART_GetFlagStatus(exCOMx, UART_IT_TXIEN) == RESET);
        }

        memset(exuartTxBuf, 0x00, sizeof(exuartTxBuf));
        extxSendFlag  = false;
    }
    
    if(!extxSendFlag){
        if(exSendStaTick++ > 2000 ){
            exSendStaTick = 0;
            exuartTxBuf[0] = 0x08;
            exuartTxBuf[1] = dcHandle.dc1Sta;
            u16 len = strlen((char*)exuartTxBuf);
            UART_SendData(exCOMx, (u16)len);
            u8* ptr = exuartTxBuf;
            while (len--) {
                UART_SendData(exCOMx, (u8)*ptr++);
                while (UART_GetFlagStatus(exCOMx, UART_IT_TXIEN) == RESET);
            }
        }
    }
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
        COMMON_EnableIpClock(emCLOCK_GPIOB);

        GPIO_Mode_AF_PP_20MHz_Init(GPIOB, GPIO_Pin_6, EXTI_MAPR_UART1, 	GPIO_AF_0);
        GPIO_Mode_IPU_Init		  (GPIOB, GPIO_Pin_7, EXTI_MAPR_UART1, 	GPIO_AF_0);

        COMMON_NVIC_Configure(UART1_IRQn, 1, 1);
    }
    if (UARTx == UART2) {
        COMMON_EnableIpClock(emCLOCK_UART2);
        COMMON_EnableIpClock(emCLOCK_GPIOA);

        GPIO_Mode_AF_PP_20MHz_Init(GPIOA, GPIO_Pin_2, NO_REMAP, 		GPIO_AF_1);
        GPIO_Mode_IPU_Init		  (GPIOA, GPIO_Pin_3, NO_REMAP, 		GPIO_AF_1);

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

        if (exisFirstRx) {
            exbufLen = (u16)UART1->RDR;
            exptrUart = exuartRxBuf;
            exisFirstRx = false;
            exuartTimeOut = 5;
        }
        else {
            if (exbufLen--) {
                *exptrUart = (u16)UART1->RDR;
                exptrUart ++;
                if (exbufLen == 0) {
                    exisFirstRx = true;
                    exrecFlag = true;
                }

            }
        }
        UART_ClearITPendingBit(UART1, UART_ICR_RXICLR);
    }
}

////////////////////////////////////////////////////////////////////////////////
void UART2_IRQHandler()
{
    if(UART_GetITStatus(UART2, UART_IT_RXIEN) != RESET) {

        if (isFirstRx) {
            bufLen = (u16)UART2->RDR;
            ptrUart = uartRxBuf;
            isFirstRx = false;
        }
        else {
            if (bufLen--) {
                *ptrUart = (u16)UART2->RDR;
                ptrUart ++;
                if (bufLen == 0) {
                    isFirstRx = true;
                    recFlag = true;
                }

            }
        }
        UART_ClearITPendingBit(UART2, UART_ICR_RXICLR);
    }
}
