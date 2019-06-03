////////////////////////////////////////////////////////////////////////////////
/// @file     2_4G.C
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE PROVIDES ALL THE 2.4GHZ EXAMPLE.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIA2.4GHZ FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT 2018-2019 MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion  --------------------------------------
#define _2_4G_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "types.h"
#include "system_mm32.h"

#include "drv.h"

#include "HAL_spi.h"
#include "HAL_exti.h"

#include "wl_hw.h"
#include "wl_2_4g_cfg.h"

#include "2_4g.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup EXAMPLE_2.4GHZ
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup 2.4GHZ_Exported_Functions
/// @{

void wl_2_4g_tick_task()
{
    counter++;
    
    if(counter % 20 == 0) {
        if(rx_buf[0] == 0x02) {
            tx_buf[0] = 0x82;
            rx_buf[0] = 0x00;
            wl_tx_flag = true;
        }
        
        if(rx_buf[0] == 0x82 ) {
            wl_tx_success_flag = true;
            rx_buf[0] = 0x00;
        }
    }
}

u32 temp;

void wl_2_4_para_init()
{
    counter = 0;
    wl_tx_flag = false;
    wl_tx_success_flag = false;

#ifdef WL_ADDR_TX
    //u8 rx_addr[WL_ADDR_WIDTH] = {0xAA, 0xBB, 0x00, 0x11, 0x22};
    //u8 tx_addr[WL_ADDR_WIDTH] = {0xCC, 0xDD, 0x99, 0x77, 0x55};
    u8 rx_addr[WL_ADDR_WIDTH] = {0xFF, 0x00, 0x00, 0x00, 0xFF};
    u8 tx_addr[WL_ADDR_WIDTH] = {0xFF, 0x00, 0x00, 0x00, 0xFF};
#else
    //u8 rx_addr[WL_ADDR_WIDTH] = {0xCC, 0xDD, 0x99, 0x77, 0x55};
    //u8 tx_addr[WL_ADDR_WIDTH] = {0xAA, 0xBB, 0x00, 0x11, 0x22};
    u8 rx_addr[WL_ADDR_WIDTH] = {0xFF, 0x00, 0x00, 0x00, 0xFF};
    u8 tx_addr[WL_ADDR_WIDTH] = {0xFF, 0x00, 0x00, 0x00, 0xFF};
#endif 
    
    memcpy(wl_tx_addr, tx_addr, WL_ADDR_WIDTH);
    memcpy(wl_rx_addr, rx_addr, WL_ADDR_WIDTH);
}

void wl_2_4g_mode()
{
    wl_2_4_para_init();
    
    wl_spi_init();
    wl_irq_it_init();
    
    wl_2_4g_init();
    wl_2_4g_rx_mode();
}

void wl_2_4g_task()
{
    if(wl_tx_flag) {
        if(tx_buf[0] == 0x00)
            tx_buf[0] = 0x02;
        
        wl_2_4g_tx(tx_buf, TX_PLOAD_WIDTH);
        memset(tx_buf, 0, TX_PLOAD_WIDTH);
        wl_tx_flag = false;
    }
}
/// @}

/// @}

/// @}
