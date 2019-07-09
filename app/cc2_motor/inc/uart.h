// Define to prevent recursive inclusion  --------------------------------------
#ifndef __UART_H
#define __UART_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_MOTOR
/// @brief MOTOR example modules
/// @{

#define COMx    UART2
#define exCOMx  UART1

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MOTOR_Exported_Variables
/// @{
#ifdef _UART_C_
#define GLOBAL
#else
#define GLOBAL extern
#endif

//back to HMI board
GLOBAL bool extxSendFlag;
GLOBAL u8 exuartTxBuf[20];
GLOBAL u8 exuartRxBuf[20];
GLOBAL u8* exptrUart;
GLOBAL u8  exbufLen;
GLOBAL bool exrecFlag;
GLOBAL bool exisFirstRx;
GLOBAL u16 exuartTimeOut;

//
GLOBAL bool recFlag;
GLOBAL bool txSendFlag;
GLOBAL bool isFirstRx;

GLOBAL u8 uartTxBuf[20];
GLOBAL u8 uartRxBuf[20];

GLOBAL u32 adcSendTick;

GLOBAL u8* ptrUart;
GLOBAL u8  bufLen;

GLOBAL u16 uartTimeOut;

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MOTOR_Exported_Functions
/// @{

void decodeTick();
void encodeTick();
void initUART(UART_TypeDef* UARTx);

void exencodeTick();
void exdecodeTick();
////////////////////////////////////////////////////////////////////////////////

/// @}


/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
