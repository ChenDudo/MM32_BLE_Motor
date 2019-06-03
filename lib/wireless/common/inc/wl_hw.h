////////////////////////////////////////////////////////////////////////////////
/// @file     WL_HW.H
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE WL_HW
///           FIRMWARE LIBRARY.
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
#ifndef __WL_HW_H
#define __WL_HW_H

// Files includes  -------------------------------------------------------------
#include "types.h"
#include "MM32.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup MM32_Wireless
/// @brief Wireless
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WL_HW_Exported_Types
/// @{

typedef enum {
    emWL_SPI_RX,
    emWL_SPI_TX
} EM_WL_SPI_DIR;

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WL_HW_Exported_Variables
/// @{

#ifdef _WL_HW_C_
#define GLOBAL

#else
#define GLOBAL extern
#endif

#undef GLOBAL

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WL_HW_Exported_Functions
/// @{


void wl_spi_init();

void wl_irq_init();
void wl_irq_it_init();
bool wl_irq_status();

void wl_activate(u8 data);
void wl_write_reg(u8 addr, u8 data);
u8 wl_read_reg(u8 addr);
void wl_write_buf(u8 addr,u8* buf, u8 len);
void wl_read_buf(u8 addr, u8* buf, u8 len);

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /*_WL_HW_H */
////////////////////////////////////////////////////////////////////////////////
