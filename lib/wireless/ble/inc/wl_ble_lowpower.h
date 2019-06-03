////////////////////////////////////////////////////////////////////////////////
/// @file     WL_BLE_LOWPOWER.H
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE BLE_VARIABLES
///           EXAMPLES.
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
#ifndef __WL_BLE_LOWPOWER_H
#define __WL_BLE_LOWPOWER_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_BLE_VARIABLES
/// @brief BLE_VARIABLES example modules
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @defgroup BLE_VARIABLES_Exported_Constants
/// @{

#define AT_CMD_BUF_SIZE 	52
#define AT_CMD_NAME_SIZE 	16
#define BUF_SIZE 			100

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup BLE_VARIABLES_Exported_Variables
/// @{
#ifdef _WL_BLE_LOWPOWER_C_
#define GLOBAL

#else
#define GLOBAL extern
#endif

GLOBAL u8 interrupt_diable_cnt;

GLOBAL u16 tx_timeout;
GLOBAL u16 rx_timeout;
GLOBAL u8  lowpower_mode;

#undef GLOBAL

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup BLE_VARIABLES_Exported_Functions
/// @{

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* BLE_VARIABLES_H */
////////////////////////////////////////////////////////////////////////////////
