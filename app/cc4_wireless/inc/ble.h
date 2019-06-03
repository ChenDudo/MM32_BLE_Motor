////////////////////////////////////////////////////////////////////////////////
/// @file     BLE.H
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE BLE
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
#ifndef __BLE_H
#define __BLE_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_BLE
/// @brief BLE example modules
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @defgroup BLE_Exported_Constants
/// @{

/// @}


////////////////////////////////////////////////////////////////////////////////
/// @defgroup BLE_Exported_Variables
/// @{
#ifdef _BLE_C_
#define GLOBAL


#else
#define GLOBAL extern
#endif

GLOBAL u32      SysTick_Count;
GLOBAL uint8_t* ble_mac_addr;
GLOBAL bool 	connected_flag;

#undef GLOBAL

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup BLE_Exported_Functions
/// @{

void wl_ble_mode();
void wl_ble_task();
void wl_ble_tick_task();

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* BLE_H */
////////////////////////////////////////////////////////////////////////////////
