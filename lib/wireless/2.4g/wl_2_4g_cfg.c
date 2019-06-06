////////////////////////////////////////////////////////////////////////////////
/// @file     WL_2_4G_CFG.C
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
#define _WL_2_4G_CFG_C_

#include "wl_hw.h"
#include "wl_2_4g_cfg.h"

void wl_2_4g_ce_high()
{
    wl_activate(0x53);
    wl_write_reg(0x16, 0x8E);
    wl_activate(0x51);
}

void wl_2_4g_ce_low()
{
    wl_activate(0x53);
    wl_write_reg(0x16, 0x8C);
    wl_activate(0x51);
}

void wl_2_4g_power_down()
{
    wl_2_4g_ce_low();

    // Config Reg
    wl_write_reg(WL_REG_CONFIG, WL_REG_CONFIG_MASK_RX_DR_DI   |
                                WL_REG_CONFIG_MASK_TX_DS_DI   |
                                WL_REG_CONFIG_MASK_MAX_RT_DI  |
                                WL_REG_CONFIG_CRC_DI     |
                                WL_REG_CONFIG_CRC_16BIT  |
                                WL_REG_CONFIG_PWR_DOWN   |
                                WL_REG_CONFIG_PRIM_RX);

    // Clear RX / TX FIFO
    wl_write_reg(WL_CMD_FLUSH_TX, WL_CMD_NOP);
    wl_write_reg(WL_CMD_FLUSH_RX, WL_CMD_NOP);
}

void wl_2_4g_power_up()
{
    wl_write_reg(WL_REG_CONFIG,	WL_REG_CONFIG_MASK_RX_DR_EN   |
                                WL_REG_CONFIG_MASK_TX_DS_EN   |
                                WL_REG_CONFIG_MASK_MAX_RT_EN  |
                                WL_REG_CONFIG_CRC_EN     |
                                WL_REG_CONFIG_CRC_16BIT  |
                                WL_REG_CONFIG_PWR_UP     |
                                WL_REG_CONFIG_PRIM_RX);

    wl_2_4g_ce_high();
}

void wl_2_4g_configuration()
{
    u8 buf[3] = {0};

    wl_activate(0x56);
    wl_write_reg(0x00, 0x00);
    wl_activate(0x51);

    wl_activate(0x53);

    // xocc ?
    buf[0] = 0x7F;
    buf[1] = 0x80;
    wl_write_buf(0x14, buf, 2);

    // RF Tx Power
    buf[0] = 0x02;
    buf[1] = 0x43;
    wl_write_buf(0x0F, buf, 2);

    // Tx Gain
    buf[1] = wl_read_reg(0x08);

    if(!buf[1])
        buf[1] = 0x11;

    buf[0] = 0xC0;
    buf[2] = 0x1D;
    wl_write_buf(0x04, buf, 3);

    wl_write_reg(0x01, 0x02);

    buf[0] = 0x10;
    buf[1] = 0x02;
    wl_write_buf(0x0A, buf, 2);

    buf[0] = 0x02;
    buf[1] = 0x12;
    wl_write_buf(0x0D, buf, 2);

    buf[0] = 0x01;
    buf[1] = 0x07;
    wl_write_buf(0x0E, buf, 2);
    wl_activate(0x51);
}

void wl_2_4g_calibration()
{
    u8 data = 0;

    wl_activate(0x53);

    wl_write_reg(WL_REG_CHIP_OK, 0x03);
    do {
        data = wl_read_reg(WL_REG_CHIP_OK);
    }while(data & 0x03);

    wl_write_reg(WL_REG_CHIP_OK, 0x03);
    do {
        data = wl_read_reg(WL_REG_CHIP_OK);
    }while(data & 0x03);

    wl_activate(0x51);
}

void wl_2_4g_init()
{
    wl_2_4g_power_down();

    wl_2_4g_configuration();

    wl_2_4g_calibration();

    wl_write_reg(WL_REG_EN_AA, WL_REG_EN_AA_P0);                                // RF set pipe0 auto ACK
    wl_write_reg(WL_REG_SETUP_AW, WL_ADDR_WIDTH_BYTE);                          // RX/TX Address field width
    wl_write_reg(WL_REG_RF_CH, WL_RF_CH_VAL);                                   // RF set channel 2400 + 64
    wl_write_reg(WL_REG_RF_SETUP, WL_REG_RF_SETUP_RF_PWR_0DBM | WL_REG_RF_SETUP_RF_DR_1M);

    // RX Mode Config
    wl_write_reg(WL_REG_EN_RXADDR, 0x01);                                       // Enabled RX Addresses
    wl_write_buf(WL_REG_RX_ADDR_P0, wl_rx_addr, WL_ADDR_WIDTH);                 // RX ADDR
    wl_write_reg(WL_REG_RX_PW_P0, RX_PLOAD_WIDTH);                              // Number of bytes in RX payload in data pipe 0
    wl_write_reg(WL_CMD_FLUSH_RX, WL_CMD_NOP);

    // TX Mode Config
    wl_write_buf(WL_REG_TX_ADDR,    wl_tx_addr, WL_ADDR_WIDTH);                 // TX ADDR
    wl_write_reg(WL_REG_SETUP_RETR, 0x34);                                      // Auto Retry time and times, high 4 bits: n * 250 us, low 4 bits: n times
    wl_write_reg(WL_CMD_FLUSH_TX, WL_CMD_NOP);

}

void wl_2_4g_tx_mode()
{
    // EN_CRC=1, CRCO=1, PWR_UP=1; PRIM_RX=0
    wl_write_reg(WL_REG_CONFIG, WL_REG_CONFIG_MASK_RX_DR_EN |
                                WL_REG_CONFIG_MASK_TX_DS_EN |
                                WL_REG_CONFIG_MASK_MAX_RT_EN |
                                WL_REG_CONFIG_CRC_EN |
                                WL_REG_CONFIG_CRC_16BIT |
                                WL_REG_CONFIG_PWR_UP |
                                WL_REG_CONFIG_PRIM_TX);

    wl_2_4g_status = emWL_2_4G_TX;
}

void wl_2_4g_tx(u8* data, u8 len)
{
    if (wl_2_4g_status != emWL_2_4G_TX) {
        wl_2_4g_tx_mode();
    }

    wl_2_4g_ce_low();

    // Write data
    wl_write_buf(WL_CMD_TX_PAYLOAD, data, len);

    wl_2_4g_ce_high();
}


void wl_2_4g_rx_mode()
{
    wl_write_reg(WL_REG_CONFIG, WL_REG_CONFIG_MASK_RX_DR_EN |
                            WL_REG_CONFIG_MASK_TX_DS_EN |
                            WL_REG_CONFIG_MASK_MAX_RT_EN |
                            WL_REG_CONFIG_CRC_EN |
                            WL_REG_CONFIG_CRC_16BIT |
                            WL_REG_CONFIG_PWR_UP |
                            WL_REG_CONFIG_PRIM_RX);

    wl_write_reg(WL_REG_STATUS, wl_read_reg(WL_REG_STATUS));
    wl_write_reg(WL_CMD_FLUSH_RX, WL_CMD_NOP);

    wl_2_4g_ce_high();

    wl_2_4g_status = emWL_2_4G_RX;
}



void wl_2_4g_irq_handler()
{
    u8 status = wl_read_reg(WL_REG_STATUS);                                     // Get status reg flag

    wl_write_reg(WL_REG_STATUS, status);                                        // Clear irq flag

    if(status & WL_FLAG_RX_DR) {                                                // Rx data
        wl_2_4g_ce_low();

        wl_read_buf(WL_CMD_RX_PAYLOAD, rx_buf, RX_PLOAD_WIDTH);

        wl_2_4g_ce_high();
    }

    if(status & WL_FLAG_TX_DS) {                                                // Tx complete
        if (wl_2_4g_status != emWL_2_4G_RX) {
            wl_2_4g_rx_mode();
        }

    }

    if(status & WL_FLAG_MAX_RT) {                                               // Over time
        wl_2_4g_rx_mode();
        wl_write_reg(WL_CMD_FLUSH_TX, WL_CMD_NOP);                              // Clear TX FIFO

    }

    if(status & WL_FLAG_TX_FULL) {                                              // Tx FIFO FULL

    }

}