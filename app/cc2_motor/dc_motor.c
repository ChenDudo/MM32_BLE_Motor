////////////////////////////////////////////////////////////////////////////////
/// @file     STEP_MOTOR.C
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
#define _DC_MOTOR_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "drv.h"
#include "resource.h"
#include "motor.h"
#include "tb6612.h"

#include "hal_tim.h"
#include "hal_uart.h"
#include "hal_gpio.h"
#include "bsp.h"
#include "common.h"

bool samSpeedFlag;
u32 samCnt = 0;
u8 uartSynReceive[100];

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
    if (samCnt++ >= 500) {
        samCnt = 0;
        samSpeedFlag = true;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  set delay time
/// @param  ms: delay time
/// @retval bool: true: delay ms time
////////////////////////////////////////////////////////////////////////////////
bool delay(u16 ms)
{
    static u16 cnt = 0;
    if (cnt == 0) {
        cnt = ms;
    }
	else if (sysTickFlag1mS) {
        sysTickFlag1mS = false;
        if (cnt > 0)
            cnt--;
        if (cnt == 0)
            return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
#define SYNC_H  GPIO_SetBits  (GPIOC, GPIO_Pin_15)
#define SYNC_L  GPIO_ResetBits(GPIOC, GPIO_Pin_15)
void initSyncPin()
{
    COMMON_EnableIpClock(emCLOCK_GPIOC);

    GPIO_InitTypeDef 	GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

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
    //UART_ITConfig(UARTx, UART_IT_RXIEN, ENABLE);
    UART_Cmd(UARTx, ENABLE);
}


#define COMx    UART2

emDC_handle dcHandle;
////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

    emDC_handle dcHandle = {
        .dcPulseMax     = 1000,

        .dc1Sta         = emDC_Stop,
        .dc1Dir         = 0,
        .dc1PulseWidth  = 1000,

        .dc2Sta         = emDC_Stop,
        .dc2Dir         = 0,
        .dc2PulseWidth  = 1000,
    };

    emMotor_handle motor1 = {
        .motorType  = emMotor_DC,
        .motorSta   = emMotor_Standby,
        .handle     = (int*)&dcHandle
    };

    initMotor(motor1);
    initSyncPin();
    initUART(COMx);
    memset(uartSynReceive, 0x00, sizeof(uartSynReceive));
    u8* ptr = uartSynReceive;
    u8 entryCnt = 0;
    u8 reclen = 0;
    bool recFlag = false;

    while (1) {

        if (!(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15))){
            if (UART_GetITStatus(COMx, UART_IT_RXIEN) == SET) {
                UART_ClearITPendingBit(COMx, UART_IT_RXIEN);
                *ptr = COMx->RDR;
                ptr += 1;
                entryCnt ++;
            }
        }
        else {
            if (UART_GetITStatus(COMx, UART_IT_RXIEN) == SET) {
                UART_ClearITPendingBit(COMx, UART_IT_RXIEN);
                *ptr = COMx->RDR;
                ptr += 1;
            }
            if (entryCnt) {
                recFlag = true;
                entryCnt = 0;
            }
        }

        //        if (recFlag) {
        //            ptr = uartSynReceive;
        //            recFlag = false;
        //            memset(uartSynReceive, 0x00, sizeof(uartSynReceive));
        //            recFlag = false;
        //        }



        if (recFlag) {
            ptr = uartSynReceive;
            switch(*ptr) {
                case 0x01: {
                    //(*(ptr + 1)) ? (dcHandle.dc1Sta = emDC_Run) : (dcHandle.dc1Sta = emDC_Stop);
                    if(*(ptr + 1) == 0x01)
                        dcHandle.dc1Sta = emDC_Run;
                    if(*(ptr + 1) == 0x02)
                        dcHandle.dc1Sta = emDC_Stop;
                }
                break;
                case 0x02: {
                    u16 cnt = ptr[1] << 8;
                    cnt +=  ptr[2];
                    dcHandle.dc1PulseWidth = cnt;
                }
                break;
                case 0x03:
                break;
                default:
                break;
            }
            memset(uartSynReceive, 0x00, sizeof(uartSynReceive));
            recFlag = false;
        }

        //        if (recFlag) {
        //            ptr = uartSynReceive;
        //            if(*ptr == 0xcd) {
        //                switch(*(ptr + 1) & 0x0F) {
        //                    case 0x00: {
        //                        (*(ptr + 2)) ? (dcHandle.dc1Sta = emDC_Run) : (dcHandle.dc1Sta = emDC_Stop);
        //                    }
        //                    break;
        //                    case 0x01: {
        //                        u8 len = (u8)((*(ptr + 1) & 0xF0) >> 3) - 4;
        //                        dcHandle.dc1PulseWidth = *(ptr + len);
        //                    }
        //                    break;
        //                    case 0x02:
        //                    break;
        //                    default:
        //                    break;
        //                }
        //                memset(uartSynReceive, 0x00, sizeof(uartSynReceive));
        //                recFlag = false;
        //            }
        //        }

        dcMotorRun(&dcHandle);

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

        if (tickFlag) {
            (vdLED & 0x02) ? (vdLED &= ~0x02) : (vdLED |= 0x02); // toggle LD3
            tickFlag = false;
        }
        SysDisplay((u8*)&vdLED);
    }
}

/// @}

/// @}

/// @}
