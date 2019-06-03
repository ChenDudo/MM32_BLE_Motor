////////////////////////////////////////////////////////////////////////////////
/// @file     WT2031.C
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE PROVIDES ALL THE WT2031 EXAMPLE.
////////////////////////////////////////////////////////////////////////////////
/// @attention
///
/// THE EXISTING FIRMWARE IS ONLY FOR REFERENCE, WHICH IS DESIGNED TO PROVIDE
/// CUSTOMERS WITH CODING INFORMATION ABOUT THEIR PRODUCTS SO THEY CAN SAVE
/// TIME. THEREFORE, MINDMOTION SHALL NOT BE LIAWT2031 FOR ANY DIRECT, INDIRECT OR
/// CONSEQUENTIAL DAMAGES ABOUT ANY CLAIMS ARISING OUT OF THE CONTENT OF SUCH
/// HARDWARE AND/OR THE USE OF THE CODING INFORMATION CONTAINED HEREIN IN
/// CONNECTION WITH PRODUCTS MADE BY CUSTOMERS.
///
/// <H2><CENTER>&COPY; COPYRIGHT 2018-2019 MINDMOTION </CENTER></H2>
////////////////////////////////////////////////////////////////////////////////

// Define to prevent recursive inclusion  --------------------------------------
#define _WT2031_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "types.h"
#include "HAL_rcc.h"
#include "HAL_spi.h"
#include "HAL_gpio.h"
#include "HAL_exti.h"
#include "HAL_nvic.h"

#include "drv.h"
#include "wt2031.h"

//|---------|---------|---------|---------|
//|SPI1_NSS |SPI1_SCK |SPI1_MISO|SPI1_MOSI|
//|---------|---------|---------|---------|
//| PA4/AF0 | PA5/AF0 | PA6/AF0 | PA7/AF0 |
//|---------|---------|---------|---------|

// WT2031_READY        PA4  Input       EPR Encoder Packet Ready 0:ready
// WT2031_SCLK         PA5  Output      SCK
// WT2031_TX_DATA      PA6  Input       MISO
// WT2031_RX_DATA      PA7  Output      MOSI
// WT2031_STRB         PD3  Output      nNSS
// WT2031_ACRST        PD1  Output      RESET

void wt2031_spi_init()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_SPI1 | RCC_APB2ENR_EXTI, ENABLE);
    
    
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_3);                       // Remove MOSI
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_0);                       // SCK       PA5
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_0);                       // MISO      PA6
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_0);                       // MOSI      PA7
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_1;   		
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_3;   		
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_5;           //  SCLK
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_7;   		// MOSI
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_4;   		// READY
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_6;   		// MISO
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataWidth = 8;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                                  // MODE0: SPI_CPOL_Low, SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                                
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = 6;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI1, &SPI_InitStructure);
    
    SPI_Cmd(SPI1, ENABLE);                                                      // Enable SPI1
    
    SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Tx);
    SPI_BiDirectionalLineConfig(SPI1, SPI_Direction_Rx);
}

void wt2031_reset()
{
    WT2031_ACRST_HIGH();
    WT2031_ACRST_LOW();
    WT2031_ACRST_HIGH();
}

void wt2031_init()
{
    memset(wt2031_rx_data, 0, 48);
    
    wt2031_spi_init();
    
    WT2031_STRB_LOW();
    
    wt2031_reset();
}

void wt2031_strb()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_5;  
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    WT2031_SCLK_HIGH();
    WT2031_SCLK_LOW();
    WT2031_STRB_HIGH();
    
    WT2031_SCLK_HIGH();
    WT2031_SCLK_LOW();
    WT2031_STRB_LOW();    
    
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

u8 wt2031_rw_byte(u8 data)
{
    WRITE_REG(SPI1->TDR, data);
    while(!SPI_GetFlagStatus(SPI1, SPI_FLAG_TXEPT));
    while(!SPI_GetFlagStatus(SPI1, SPI_FLAG_RXAVL));
    return READ_REG(SPI1->RDR);
}

void wt2031_rw() 
{
    u8   data[2] = {0};
    
    wt2031_strb();

    data[0] = wt2031_rw_byte(0x00);
    
    if(data[0] == 0x13) {
        data[1] = wt2031_rw_byte(0x00);
        
        if(data[1] == 0xEC) {
            *(u16*)wt2031_rx_data = 0xEC13;
            
            for(u8 i = 2; i < 48; i++) {
                if(i % 2 == 0)
                    wt2031_strb();

                if (i - 2 >= 14 && i - 2 <= 20)
                    wt2031_rx_data[i] = wt2031_rw_byte(0x00); // DTMF code
                else if (i - 2 == 21)
                    wt2031_rx_data[i] = wt2031_rw_byte(0xFF); // DTMF dbm
                else if (i - 2 == 22)
                    wt2031_rx_data[i] = wt2031_rw_byte(0x20); // volume
                else if (i - 2 == 23)
                    wt2031_rx_data[i] = wt2031_rw_byte(0x20);
                else
                    wt2031_rx_data[i] = wt2031_rw_byte(wt2031_rx_data[i - 2]);
            }
        }
    }
}

void wt2031_read()
{
    u8   data[2] = {0};
    
    wt2031_strb();

    data[0] = wt2031_rw_byte(0x00);
    
    if(data[0] == 0x13) {
        data[1] = wt2031_rw_byte(0x00);
        
        if(data[1] == 0xEC) {
            *(u16*)wt2031_rx_data = 0xEC13;
            
            for(u8 i = 2; i < 48; i++) {
                if(i % 2 == 0)
                    wt2031_strb();

                wt2031_rx_data[i] = wt2031_rw_byte(wt2031_rx_data[i - 2]);
            }
        }
    }
}

void wt2031_write(u8* data) 
{
    for (u8 i = 0; i < 48; i++) {
        if (i % 2 == 0)
            wt2031_strb();
        
        wt2031_rw_byte(data[i]);
    }
}