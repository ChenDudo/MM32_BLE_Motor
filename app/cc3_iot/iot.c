// Define to prevent recursive inclusion  --------------------------------------
#define _IOT_C_

// Includes  -------------------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include "system_mm32.h"

#include "drv.h"
#include "iot.h"
#include "hts.h"
#include "lps.h"
#include "lsm.h"
#include "resource.h"
#include "hal_gpio.h"
#include "hal_uart.h"
#include "common.h"
#include "drv_uart.h"
#include "sensor_common.h"
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_uart.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void AppTaskTick()
{
    if (tickCnt++ >= 3600000) {
        tickCnt  = 0;
        tickFlag = true;
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

#if 1
void UART_Configure()
{
    UART_InitTypeDef InitStructure;

    InitStructure.BaudRate      = 9600;
    InitStructure.WordLength    = UART_WordLength_8b;
    InitStructure.StopBits      = UART_StopBits_1;
    InitStructure.Parity        = UART_Parity_No;
    InitStructure.HWFlowControl = UART_HWFlowControl_None;
    InitStructure.Mode          = (EM_UARTMODE)(UART_Mode_Rx | UART_Mode_Tx);
    InitStructure.Mode         &= ~UART_GCR_DMA;

    COMMON_EnableIpClock(emCLOCK_UART1);
    COMMON_EnableIpClock(emCLOCK_UART2);

    UART_Init(UART1, &InitStructure);
    UART_Init(UART2, &InitStructure);
    UART_Cmd(UART1, ENABLE);
    UART_Cmd(UART2, ENABLE);

    BSP_UART_GPIO_Configure(UART1, 0, 0);
    BSP_UART_GPIO_Configure(UART2, 0, 0);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  numbers transform to string
////////////////////////////////////////////////////////////////////////////////
void NumtoStr(u32 value, u8* str, u8 len)
{
    for (u8 i = 0;i < len;i++) {
        str[len - i - 1] = ((value % 16) > 9) ? (value % 16) + 55 : (value % 16) + 48;
        value >>= 4;
    }
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Combine two string together
////////////////////////////////////////////////////////////////////////////////
void stract(u8 *dest, u8 *src,u8 len)
{
    while (*dest)
        dest++;
    for(u8 i = 0;i < len;i++) {
        *dest = *src;
        dest++;
        src++;
    }
}

void hardwareCfg()
{
    BSP_LED_Configure();
    BSP_KEY_Configure();
    memset(&KEYB, 0x00, sizeof(KEYB));
    GPIO_SetBits(GPIOB, GPIO_Pin_0);

    LD3_on();
    LD4_on();
    LD5_on();
    LD6_on();
    CloseLED();

    if (HTS221_Init())  LD4_on();
    if (LPS22HB_Init()) LD5_on();
    if (LSM6DSL_Init()) LD6_on();

    HTS221_Get_Equation_Temp();
    HTS221_Get_Equation_Hum();
    LPS22HB_Config();
}

//bool checkCommond()
//{
//    u8 result[32];
//    u8 i = 0;
//    while(1) {
//        if(UART_GetITStatus(UART2, UART_IT_RXIEN)) {
//            result[i] = UART_ReceiveData(UART2);
//            UART_ClearITPendingBit(UART2, UART_IT_RXIEN);
//            if ((result[i] == 'K') && (result[i-1] == 'O'))
//                return true;
//            else if ((result[i] == 'R') && (result[i-1] == 'O') && (result[i-2] == 'R') && (result[i-3] == 'R') && (result[i-4] == 'E'))
//                return false;
//            i++;
//            if (i == 255) return false;
//        }
//    }
//}

//const AT_Commond ATCommond[] = {
//    ATCOMMOND("AT+CMEE=1\r\n", 11),
//    ATCOMMOND("AT+CFUN=0\r\n", 11),
//    ATCOMMOND("AT+CGSN=1\r\n", 11),
//    ATCOMMOND("AT+NCDP=180.101.147.115,5683\r\n", 30),
//    ATCOMMOND("AT+CFUN=1\r\n", 11),
//    ATCOMMOND("AT+CGATT=1\r\n", 12),
//    ATCOMMOND("AT+CGPADDR\r\n", 12)
//};
//
//bool connectResult[7];
//
//bool connectIoT()
//{
//    u8* str;
//    for (u8 i = 0;i < 7;i++) {
//        str = ATCommond[i].str;
//        for (u8 j = 0;j < ATCommond[i].len;j++) {
//            UART_SendData(UART2, *str++);
//            while (!UART_GetITStatus(UART2, UART_IT_TXIEN));
//        }
//        if(checkCommond())
//            connectResult[i] = true;
//        else return false;
//    }
//    return true;
//}

float avTemp;
float avHum;
double press;

u8 tempStr[2];
u8 humStr[2];
u8 pressStr[6];

u8 txCommond[12] = "AT+NMGS=6,00";
u8 rxCommond[22] = "AT+NMGS=5,";
u8 txBuffer[24];
u8 rxBuffer[22];

void getSensorInfo()
{
    avTemp = HTS221_Get_AVTemp();
    avHum = HTS221_Get_AVHum();
    press = LPS22HB_Get_Press() * 100;

    NumtoStr((u8)avTemp, tempStr, 2);
    NumtoStr((u8)avHum, humStr, 2);
    NumtoStr((u32)press, pressStr, 6);

    stract(txBuffer, txCommond, 12);
    stract(txBuffer, tempStr, 2);
    stract(txBuffer, humStr, 2);
    stract(txBuffer, pressStr, 6);
    txBuffer[22] = '\r';
    txBuffer[23] = '\n';
    for (u8 i = 0;i < 24;i++) {
        UART_SendData(UART2, txBuffer[i]);
        while (!UART_GetITStatus(UART2, UART_IT_TXIEN));
    }
}

int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

    UART_Configure();
    hardwareCfg();

//    connectIoT();

    u8 rxCnt = 0;
    u8 rxEnd = 0;
    u8 rxStart = 0;
    u8 revStr[32];
    u8 respStr[12];
    tickFlag = true;

    while(1) {
        //每小时发送一次sensor数据
        if (tickFlag) {
            tickFlag = false;
            memset(txBuffer, 0, 24);
            getSensorInfo();
        }

        //得到下发的命令的数组号
        if(UART_GetITStatus(UART2, UART_IT_RXIEN)) {
            revStr[rxCnt] = UART_ReceiveData(UART2);
            UART_ClearITPendingBit(UART2, UART_IT_RXIEN);
            if ((revStr[rxCnt] == ':') && (revStr[rxCnt - 1] == 'I')) {
                rxStart = rxCnt + 3;
                getCommond = true;
            }
            if ((getCommond) && (revStr[rxCnt] == '\n'))
                if (revStr[rxCnt] == '\n') rxEnd = rxCnt;
            rxCnt++;
        }

        //通过得到的下发命令产生对应的响应命令
        if(rxEnd) {
            respStr[0] = revStr[rxStart];
            respStr[1] = revStr[rxStart + 1] + 1;
            respStr[2] = revStr[rxStart + 2];
            respStr[3] = revStr[rxStart + 3];
            respStr[4] = revStr[rxStart + 4];
            respStr[5] = revStr[rxStart + 5];
            respStr[6] = '0';
            respStr[7] = '0';
            respStr[8] = revStr[rxStart + 6];
            respStr[9] = revStr[rxStart + 7];
            respStr[10] = '\r';
            respStr[11] = '\n';

            stract(rxBuffer, rxCommond, 12);
            stract(rxBuffer, respStr, 12);

            getCommond = false;
            rxReady = true;
            rxEnd = 0;
            rxCnt = 0;
            rxStart = 0;
        }

        //响应命令准备好，开始上发
        if(rxReady) {
            for (u8 i = 0;i < 22;i++) {
                UART_SendData(UART2, rxBuffer[i]);
                while (!UART_GetITStatus(UART2, UART_IT_TXIEN));
            }
            if (respStr[9] - 48) LD4_on();
            else LD4_off();
            rxReady = false;
            memset(rxBuffer, 0, 22);
            memset(revStr, 0, 22);
        }
    }
}
#endif




#if 0
float avTemp;
float avHum;
double press;
LSM6DSL_AXES angularRate;
LSM6DSL_AXES accRate;
////////////////////////////////////////////////////////////////////////////////
/// @brief  main function.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

//-----------------------------初始化LED和KEY-----------------------------------
    BSP_LED_Configure();
    BSP_KEY_Configure();
    memset(&KEYB, 0x00, sizeof(KEYB));
    GPIO_SetBits(GPIOB, GPIO_Pin_0);    //使用I2C必须将PB0置1

//---------------------------------测试LED--------------------------------------
    LD3_on();
    LD4_on();
    LD5_on();
    LD6_on();
    CloseLED();

//------------------------------初始化sensor------------------------------------
    if (HTS221_Init())  LD4_on();       //初始化成功LED5灯亮, htsRight置true
    if (LPS22HB_Init()) LD5_on();       //初始化成功LED6灯亮, lpsRight置true
    if (LSM6DSL_Init()) LD6_on();       //初始化成功LED7灯亮, lsmRight置true

//-------------------------------配置6轴传感器----------------------------------
    LSM6DSL_6DOrientation_Init();
    LSM6DSL_6DOrientation_Process();

//----------------------------得到温湿度的方程系数------------------------------
    HTS221_Get_Equation_Temp();
    HTS221_Get_Equation_Hum();

//-------------------------------配置压力传感器---------------------------------
    LPS22HB_Config();

    while (1) {
        avTemp = (u8)HTS221_Get_AVTemp();
        avHum = (u8)HTS221_Get_AVHum();
        press = (u32)LPS22HB_Get_Press();

//--------------------------------读取角速度------------------------------------
        Get_Angular_Rate(&angularRate);

//--------------------------------读取加速度------------------------------------
        Get_Acceleration_Rate(&accRate);

//---------------------------------测试KEY--------------------------------------
        if (!KEYB.event[0] && Key1()) {
            if ((delay(20))) {
                KEYB.event[0]  = true;
                if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)) LD3_on();
                else LD3_off();
            }
        }
        else if (!Key1()) {
            KEYB.event[0] = false;
        }

        if (!KEYB.event[1] && Key2()) {
            if ((delay(20))) {
                KEYB.event[1]  = true;
                if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7)) LD4_on();
                else LD4_off();
            }
        }
        else if (!Key2()) {
            KEYB.event[1] = false;
        }

        if (!KEYB.event[2] && Key3()) {
            if ((delay(20))) {
                KEYB.event[2]  = true;
                if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8)) LD5_on();
                else LD5_off();
            }
        }
        else if (!Key3()) {
            KEYB.event[2] = false;
        }

        if (!KEYB.event[3] && Key4()) {
            if ((delay(20))) {
                KEYB.event[3]  = true;
                if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9)) LD6_on();
                else LD6_off();
            }
        }
        else if (!Key4()) {
            KEYB.event[3] = false;
        }
    }
}
#endif
