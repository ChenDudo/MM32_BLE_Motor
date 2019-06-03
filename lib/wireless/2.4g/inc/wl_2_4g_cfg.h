////////////////////////////////////////////////////////////////////////////////
/// @file     WL_2_4G_CFG.H
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE WL_2_4G_CFG
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
#ifndef __WL_2_4G_CFG_H
#define __WL_2_4G_CFG_H

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
/// @defgroup WL_2_4G_CFG_Exported_Types
/// @{

#define WL_ADDR_WIDTH   5
#define WL_ADDR_WIDTH_BYTE WL_REG_SETUP_AW_5BYTES
#define WL_RF_CH_VAL    40

#define TX_PLOAD_WIDTH 20
#define RX_PLOAD_WIDTH 20

#define TX_BUFFER_SIZE 20
#define RX_BUFFER_SIZE 60 

typedef enum {
	emWL_2_4G_NONE,
	emWL_2_4G_RX,
	emWL_2_4G_TX
} EM_WL_2_4G_STATUS;

#define WL_REG_CONFIG                   0x00
#define WL_REG_CONFIG_MASK_RX_DR_Pos    6                                       // Mask interrupt caused by RX_DR
#define WL_REG_CONFIG_MASK_RX_DR        (0x01 << WL_REG_CONFIG_MASK_RX_DR_Pos)  // 1: Disable 0: Enable
#define WL_REG_CONFIG_MASK_RX_DR_EN     0
#define WL_REG_CONFIG_MASK_RX_DR_DI     WL_REG_CONFIG_MASK_RX_DR
                                        
#define WL_REG_CONFIG_MASK_TX_DS_Pos    5                                       // Mask interrupt caused by TX_DS
#define WL_REG_CONFIG_MASK_TX_DS        (0x01 << WL_REG_CONFIG_MASK_TX_DS_Pos)  // 1: Disable 0: Enable
#define WL_REG_CONFIG_MASK_TX_DS_EN     0
#define WL_REG_CONFIG_MASK_TX_DS_DI     WL_REG_CONFIG_MASK_TX_DS

#define WL_REG_CONFIG_MASK_MAX_RT_Pos   4                                       // Mask interrupt caused by MAX_RT
#define WL_REG_CONFIG_MASK_MAX_RT       (0x01 << WL_REG_CONFIG_MASK_MAX_RT_Pos) // 1: Disable 0: Enable
#define WL_REG_CONFIG_MASK_MAX_RT_EN    0
#define WL_REG_CONFIG_MASK_MAX_RT_DI    WL_REG_CONFIG_MASK_MAX_RT

#define WL_REG_CONFIG_CRC_Pos           3                                       // Enable CRC. Forced high if one of the bits in the EN_AA is high                                   
#define WL_REG_CONFIG_CRC               (0x01 << WL_REG_CONFIG_CRC_Pos)         // 1: Enable 0: Disable 
#define WL_REG_CONFIG_CRC_EN            WL_REG_CONFIG_CRC
#define WL_REG_CONFIG_CRC_DI            0

#define WL_REG_CONFIG_CRC_BIT_Pos       2                                       // CRC encoding scheme
#define WL_REG_CONFIG_CRC_BIT           (0x01 << WL_REG_CONFIG_CRC_BIT_Pos)     // 1: 16 Bits 0: 8 Bits
#define WL_REG_CONFIG_CRC_16BIT         WL_REG_CONFIG_CRC_BIT
#define WL_REG_CONFIG_CRC_8BIT          0

#define WL_REG_CONFIG_PWR_Pos           1                                       
#define WL_REG_CONFIG_PWR               (0x01 << WL_REG_CONFIG_PWR_Pos)         // 1:Power up 0:Power down 
#define WL_REG_CONFIG_PWR_UP            WL_REG_CONFIG_PWR
#define WL_REG_CONFIG_PWR_DOWN          0

#define WL_REG_CONFIG_PRIM_Pos          0
#define WL_REG_CONFIG_PRIM              (0x01 << WL_REG_CONFIG_PRIM_Pos)        // 1:Rx 0:Tx
#define WL_REG_CONFIG_PRIM_RX           WL_REG_CONFIG_PRIM
#define WL_REG_CONFIG_PRIM_TX           0

#define WL_REG_EN_AA                    0x01                                    // Enable ¡®Auto Acknowledgment¡¯ Function
#define WL_REG_EN_AA_P0                 0x01                                    // Enable ¡®Auto Acknowledgment¡¯ Function
#define WL_REG_EN_AA_P1                 0x02                                    // Enable ¡®Auto Acknowledgment¡¯ Function
#define WL_REG_EN_AA_P2                 0x04                                    // Enable ¡®Auto Acknowledgment¡¯ Function
#define WL_REG_EN_AA_P3                 0x08                                    // Enable ¡®Auto Acknowledgment¡¯ Function
#define WL_REG_EN_AA_P4                 0x10                                    // Enable ¡®Auto Acknowledgment¡¯ Function
#define WL_REG_EN_AA_P5                 0x20                                    // Enable ¡®Auto Acknowledgment¡¯ Function

#define WL_REG_EN_RXADDR                0x02                                    // Enabled RX Addresses

#define WL_REG_SETUP_AW                 0x03                                    // Setup of Address Widths (common for all data pipes)
#define WL_REG_SETUP_AW_ILLEGAL         0x00                                    
#define WL_REG_SETUP_AW_3BYTES          0x01
#define WL_REG_SETUP_AW_4BYTES          0x02
#define WL_REG_SETUP_AW_5BYTES          0x03

#define WL_REG_SETUP_RETR               0x04                                    // Setup of Automatic Retransmission
#define WL_REG_RF_CH                    0x05                                    // RF Channel
#define WL_REG_RF_SETUP                 0x06                                    // RF Setup Register
#define WL_REG_RF_SETUP_RF_PWR_0DBM		0x06
#define WL_REG_RF_SETUP_RF_PWR_6DBM		0x04
#define WL_REG_RF_SETUP_RF_PWR_12DBM	0x02
#define WL_REG_RF_SETUP_RF_PWR_18DBM	0x00
#define WL_REG_RF_SETUP_RF_DR_250K      0x20
#define WL_REG_RF_SETUP_RF_DR_2M    	0x08
#define WL_REG_RF_SETUP_RF_DR_1M    	0x00

#define WL_REG_STATUS                   0x07                                    // Status Register (In parallel to the SPI command word applied on the MOSI pin, the STATUS register is shifted serially out on the MISO pin)
#define WL_REG_OBSERVE_TX               0x08                                    // Transmit observe register
#define WL_REG_CD                       0x09                                    // Received Power Detector. This register is called CD (Carrier Detect)
#define WL_REG_RX_ADDR_P0               0x0A                                    // Receive address data pipe 0. 5 Bytes maximum length.
#define WL_REG_RX_ADDR_P1               0x0B                                    // Receive address data pipe 1. 5 Bytes maximum length.
#define WL_REG_RX_ADDR_P2               0x0C                                    // Receive address data pipe 2. 5 Bytes maximum length.
#define WL_REG_RX_ADDR_P3               0x0D                                    // Receive address data pipe 3. 5 Bytes maximum length.
#define WL_REG_RX_ADDR_P4               0x0E                                    // Receive address data pipe 4. 5 Bytes maximum length.
#define WL_REG_RX_ADDR_P5               0x0F                                    // Receive address data pipe 5. 5 Bytes maximum length.
#define WL_REG_TX_ADDR                  0x10                                    // Transmit address. Used for a PTX device only.
#define WL_REG_RX_PW_P0                 0x11                                    // Number of bytes in RX payload in data pipe 0 (1 to 32 bytes).
#define WL_REG_RX_PW_P1                 0x12                                    // Number of bytes in RX payload in data pipe 1 (1 to 32 bytes).
#define WL_REG_RX_PW_P2                 0x13                                    // Number of bytes in RX payload in data pipe 2 (1 to 32 bytes).
#define WL_REG_RX_PW_P3                 0x14                                    // Number of bytes in RX payload in data pipe 3 (1 to 32 bytes).
#define WL_REG_RX_PW_P4                 0x15                                    // Number of bytes in RX payload in data pipe 4 (1 to 32 bytes).
#define WL_REG_RX_PW_P5                 0x16                                    // Number of bytes in RX payload in data pipe 5 (1 to 32 bytes).

#define WL_REG_FIFO_STATUS              0x17                                    // FIFO Status Register
#define WL_REG_FIFO_STATUS_TX_REUSE_Pos 6                                       // Pulse the rfce high for at least 10¦Ìs to Reuse last transmitted payload.
#define WL_REG_FIFO_STATUS_TX_REUSE     (1 << WL_REG_FIFO_STATUS_TX_REUSE_Pos)  // 
#define WL_REG_FIFO_STATUS_TX_FULL_Pos  5                                       // TX FIFO full flag.
#define WL_REG_FIFO_STATUS_TX_FULL      (1 << WL_REG_FIFO_STATUS_TX_FULL_Pos)   // 1: TX FIFO full. 0: Available locations in TX FIFO.
#define WL_REG_FIFO_STATUS_TX_EMPTY_Pos 4                                       // TX FIFO empty flag.
#define WL_REG_FIFO_STATUS_TX_EMPTY     (1 << WL_REG_FIFO_STATUS_TX_EMPTY_Pos)  // 1: TX FIFO empty. 0: Data in TX FIFO.
#define WL_REG_FIFO_STATUS_RX_FULL_Pos  1                                       // RX FIFO full flag.
#define WL_REG_FIFO_STATUS_RX_FULL      (1 << WL_REG_FIFO_STATUS_RX_FULL_Pos)   // 1: RX FIFO full. 0: Available locations in RX FIFO.
#define WL_REG_FIFO_STATUS_RX_EMPTY_Pos 0                                       // RX FIFO empty flag.
#define WL_REG_FIFO_STATUS_RX_EMPTY     (1 << WL_REG_FIFO_STATUS_RX_EMPTY_Pos)  // 1: RX FIFO empty. 0: Data in RX FIFO.

#define WL_REG_DYNPD	                0x1C                                    // Enable dynamic payload length
#define WL_REG_DYNPD_DPL_P5_Pos         5                                       // Enable dynamic payload length data pipe 5.
#define WL_REG_DYNPD_DPL_P5             (1 << WL_REG_DYNPD_DPL_P5_Pos)
#define WL_REG_DYNPD_DPL_P4_Pos         4                                       // Enable dynamic payload length data pipe 4.
#define WL_REG_DYNPD_DPL_P4             (1 << WL_REG_DYNPD_DPL_P4_Pos)
#define WL_REG_DYNPD_DPL_P3_Pos         3                                       // Enable dynamic payload length data pipe 3.
#define WL_REG_DYNPD_DPL_P3             (1 << WL_REG_DYNPD_DPL_P3_Pos)
#define WL_REG_DYNPD_DPL_P2_Pos         2                                       // Enable dynamic payload length data pipe 2.
#define WL_REG_DYNPD_DPL_P2             (1 << WL_REG_DYNPD_DPL_P2_Pos)
#define WL_REG_DYNPD_DPL_P1_Pos         1                                       // Enable dynamic payload length data pipe 1.
#define WL_REG_DYNPD_DPL_P1             (1 << WL_REG_DYNPD_DPL_P1_Pos)
#define WL_REG_DYNPD_DPL_P0_Pos         0                                       // Enable dynamic payload length data pipe 0.
#define WL_REG_DYNPD_DPL_P0             (1 << WL_REG_DYNPD_DPL_P0_Pos)

#define WL_REG_FEATURE	                0x1D                                    // Feature Register
#define WL_REG_FEATURE_EN_DPL_Pos       2                                       // Enables Dynamic Payload Length
#define WL_REG_FEATURE_EN_DPL           (1 << WL_REG_FEATURE_EN_DPL_Pos)
#define WL_REG_FEATURE_EN_ACK_PAY_Pos   1                                       // Enables Payload with ACK
#define WL_REG_FEATURE_EN_ACK_PAY       (1 << WL_REG_FEATURE_EN_ACK_PAY_Pos)
#define WL_REG_FEATURE_EN_DYN_ACK_Pos   0                                       // Enables the W_TX_PAYLOAD_NOACK command
#define WL_REG_FEATURE_EN_DYN_ACK       (1 << WL_REG_FEATURE_EN_DYN_ACK_Pos)

#define WL_REG_IC_VER                   0x1E
#define WL_REG_CHIP_OK                  0x1F

// Command
#define WL_CMD_R_RX_PL_WID              0x60
#define WL_CMD_RX_PAYLOAD               0x61
#define WL_CMD_TX_PAYLOAD               0xA0
#define WL_CMD_W_TX_PAYLOAD_NOACK       0xB0


#define WL_CMD_FLUSH_TX                 0xE1
#define WL_CMD_FLUSH_RX                 0xE2
#define WL_CMD_REUSE_TX_PL              0xE3

#define WL_CMD_NOP                      0xFF

// Flag
#define WL_FLAG_TX_FULL                 0x01                                    // TX FIFO full flag. 1: TX FIFO full. 0: Available locations in TX FIFO.
#define WL_FLAG_RX_P_NO                 0x07                                    // Data pipe number for the payload available for reading from RX_FIFO 
#define WL_FLAG_MAX_RT                  0x10                                    // Maximum number of TX retransmits interrupt Write 1 to clear bit.
#define WL_FLAG_TX_DS                   0x20                                    // Data Sent TX FIFO interrupt.
#define WL_FLAG_RX_DR                   0x40                                    // Data Ready RX FIFO interrupt.

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WL_2_4G_CFG_Exported_Variables
/// @{

#ifdef _WL_2_4G_CFG_C_
#define GLOBAL
#else
#define GLOBAL extern
#endif

GLOBAL bool wl_tx_flag;
GLOBAL bool wl_tx_success_flag;
GLOBAL bool wl_rx_flag;

GLOBAL EM_WL_2_4G_STATUS wl_2_4g_status;
GLOBAL u8 tx_buf[TX_PLOAD_WIDTH];
GLOBAL u8 rx_buf[RX_PLOAD_WIDTH];

GLOBAL u8 wl_tx_addr[WL_ADDR_WIDTH];
GLOBAL u8 wl_rx_addr[WL_ADDR_WIDTH];

#undef GLOBAL

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WL_2_4G_CFG_Exported_Functions
/// @{

void wl_2_4g_ce_high();
void wl_2_4g_ce_low();

void wl_2_4g_power_down();
void wl_2_4g_power_up();

void wl_2_4g_configuration();
void wl_2_4g_calibration();

void wl_2_4g_init();

void wl_2_4g_tx_mode();
void wl_2_4g_rx_mode();

void wl_2_4g_tx(u8* data, u8 len);

void wl_2_4g_irq_handler();

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /*_WL_2_4G_CFG_H */
////////////////////////////////////////////////////////////////////////////////
