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
void initSyncPin_Master()
{
    COMMON_EnableIpClock(emCLOCK_GPIOC);
    SYNC_H;
    GPIO_Mode_OUT_PP_Init(GPIOC, GPIO_Pin_15);
}

////////////////////////////////////////////////////////////////////////////////
void initSyncPin_Slave()
{
    COMMON_EnableIpClock(emCLOCK_GPIOC);

    GPIO_InitTypeDef 	GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

////////////////////////////////////////////////////////////////////////////////
bool readSync()
{
    return GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);
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
    //UART_ITConfig(UARTx, UART_IT_RXIEN, ENABLE);
    UART_Cmd(UARTx, ENABLE);
}


#define COMx    UART2

////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

    // ===== PWM output module =====
    emDC_handle dcHandle = {
        .dcPulseMax     = 100,

        .dc1Sta         = emDC_Stop,
        .dc1Dir         = 0,
        .dc1PulseWidth  = 50,

        .dc2Sta         = emDC_Stop,
        .dc2Dir         = 0,
        .dc2PulseWidth  = 50,
    };

    emMotor_handle motor1 = {
        .motorType  = emMotor_DC,
        .motorSta   = emMotor_Standby,
        .handle     = (int*)&dcHandle
    };

    // ===== ADC sample module =====
    HANDLE hADC = CreateFile(emIP_ADC);
	if (hADC == NULL)		while(1);
    tAPP_ADC_DCB dcb = {
		.hSub	= emFILE_ADC1,					// EM_FILE_ADC
		.type	= emTYPE_DMA,    				// polling, interrupt, dma
		.mode	= emADC_Scan, 				    // Conversion mode: emADC_Imm,emADC_Scan,emADC_Continue
		.sync	= emTYPE_ASync,      			// emTYPE_Sync, emTYPE_ASync
		.trig 	= emTRIGGER_Software,           // Software Start & Trigger enum
		.chs    = LEFT_SHIFT_BIT(1),            // channels: ADC_CH_n
		.temp 	= false,            			// Temperature measurement:0(DISABLE),1(ENABLE)
		.vRef	= false,            			// Reference voltage:0(DISABLE),1(ENABLE)
		.cb		= NULL,   			            // adc callback
	};
    if (!OpenFile(hADC, (void*)&dcb))		while(1);


    initMotor(motor1);
    initSyncPin_Slave();
    initUART(COMx);

    memset(uartSynReceive, 0x00, sizeof(uartSynReceive));
    u8* ptr = uartSynReceive;
    u8  entryCnt = 0;
    u16 securityPwmMax = (u16)(dcHandle.dcPulseMax * 0.8);
    u32 adc_temp = 0;

    bool recFlag = false;

    while (1) {

        if (samSpeedFlag) {
            WriteFile(hADC, emFILE_ADC1, 0, 0);
            ReadFile(hADC, emFILE_ADC1, (u8*)&adc_temp, 16);
            securityPwmMax = (u16)((double)adc_temp / 4096 * 0.5 * dcHandle.dcPulseMax + 0.5 * dcHandle.dcPulseMax);;
        }

        if (!readSync()){
            if (UART_GetITStatus(COMx, UART_IT_RXIEN) == SET) {
                *ptr = COMx->RDR;
                ptr += 1;
                entryCnt ++;
                UART_ClearITPendingBit(COMx, UART_IT_RXIEN);
            }
        }
        else {
            if (UART_GetITStatus(COMx, UART_IT_RXIEN) == SET) {
                *ptr = COMx->RDR;
                ptr += 1;
                UART_ClearITPendingBit(COMx, UART_IT_RXIEN);
            }
            if (entryCnt) {
                recFlag = true;
                entryCnt = 0;
            }
        }

        if (recFlag) {
            ptr = uartSynReceive;
            switch(*ptr) {
                case 0x01: {
                    if(*(ptr + 1) == 0x01)
                        dcHandle.dc1Sta = emDC_Run;
                    if(*(ptr + 1) == 0x02)
                        dcHandle.dc1Sta = emDC_Stop;
                }
                break;
                case 0x02: {
                    dcHandle.dc1PulseWidth = securityPwmMax * ptr[1] / 100;
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
