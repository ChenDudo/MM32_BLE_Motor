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

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MOTOR_Exported_Variables
/// @{
#ifdef _UART_C_
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL u8 ledCmd;

GLOBAL bool recFlag;
GLOBAL bool txSendFlag;
GLOBAL bool isFirstRx;

GLOBAL bool receiveTimFlag;
GLOBAL bool autoModeFlag;
GLOBAL u8 rev017Sta;

GLOBAL u8 uartTxBuf[20];
GLOBAL u8 uartRxBuf[20];

GLOBAL u8 timData[20];

GLOBAL u8* ptrUart;
GLOBAL u8  bufLen;

GLOBAL u16 uartTimeOut;
GLOBAL u16 uartSendTick;

#undef GLOBAL
/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MOTOR_Exported_Functions
/// @{

void decodeTick();
void encodeTick();
void initUART(UART_TypeDef* UARTx);


/// @}


/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif
////////////////////////////////////////////////////////////////////////////////
