////////////////////////////////////////////////////////////////////////////////
/// @file     WL_HW.C
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE PROVIDES ALL THE WIRELESS EXAMPLE.
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
#define _WL_HW_C_

#include "HAL_rcc.h"
#include "HAL_spi.h"
#include "HAL_gpio.h"
#include "HAL_exti.h"
#include "HAL_nvic.h"

#include "resource.h"
#include "wl_hw.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initialize wireless spi(SPI2).
///         SCK     -   PB15
///         MOSI    -   PB13
///         MISO    -   PB12
///         CSN     -   PB14
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void wl_spi_init()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    GPIO_SetBits(GPIOB, GPIO_Pin_15);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_4);                       // MISO  PB12
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_4);                       // MOSI  PB13
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_3);                       // SCK   PB14
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_1);                       // CSN   PB15

    GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;   		                        // SPI2_CSN
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);                                           // CSN High

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;   		                        // SPI_SCK
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;   		                        // SPI2_MOSI
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;  		                        // SPI2_MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataWidth = 8;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                                  // MODE0: SPI_CPOL_Low, SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = 6;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_Cmd(SPI2, ENABLE);                                                      //Enable SPI2

    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Tx);
    SPI_BiDirectionalLineConfig(SPI2, SPI_Direction_Rx);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Send spi(SPI2) data(8 bits).
/// @param  data.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void wl_spi_tx(u8 data)
{
    SPI_SendData(SPI2, data);
    while(!SPI_GetFlagStatus(SPI2, SPI_FLAG_TXEPT));
    SPI_ReceiveData(SPI2);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Receive spi(SPI2) data(8 bits).
/// @param  None.
/// @retval data(u8).
////////////////////////////////////////////////////////////////////////////////
u8 wl_spi_rx()
{
    SPI_SendData(SPI2, 0xFF);
    while(!SPI_GetFlagStatus(SPI2, SPI_FLAG_RXAVL));
    return SPI_ReceiveData(SPI2);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initialize the wireless module irq port(PB1).
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void wl_irq_init()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Initialize the wireless module IRQ port(PB1), interrupt mode.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void wl_irq_it_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBENR_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_EXTI, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    EXTI_LineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
    NVIC_Init(&NVIC_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Activate the wireless module.
/// @param  data.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void wl_activate(u8 data)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);

    wl_spi_tx(0x50);
    wl_spi_tx(data);

    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Write the register of the wireless module.
/// @param  addr: Register address,
///         data: Write data.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void wl_write_reg(u8 addr, u8 data)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);

    wl_spi_tx(addr | 0x20);
    wl_spi_tx(data);

    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Read the register of the wireless module.
/// @param  addr: Register address,
/// @retval data: Read data.
////////////////////////////////////////////////////////////////////////////////
u8 wl_read_reg(u8 addr)
{
    u8 data;

    GPIO_ResetBits(GPIOB, GPIO_Pin_15);

    wl_spi_tx(addr);
    data = wl_spi_rx();

    GPIO_SetBits(GPIOB, GPIO_Pin_15);

    return data;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Write a set of data to the registers of the wireless module.
/// @param  addr: Register address,
///         buf: Write data buffer pointer,
///         len: Buffer length.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void wl_write_buf(u8 addr,u8* buf, u8 len)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);

    wl_spi_tx(addr | 0x20);

    for(u8 i = 0; i < len; i++)
        wl_spi_tx(buf[i]);

    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Read a set of data to the registers of the wireless module.
/// @param  addr: Register address,
///         buf: Read data buffer pointer,
///         len: Buffer length.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void wl_read_buf(u8 addr, u8* buf, u8 len)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
    wl_spi_tx(addr);

    for(u8 i = 0; i < len; i++)
        buf[i] = wl_spi_rx();

    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Get wireless irq port(PB1) Status.
/// @param  None.
/// @retval status(bool).
////////////////////////////////////////////////////////////////////////////////
bool wl_irq_status()
{
    return !(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1));
}

// =============================================================================
// === HARDCODE ================================================================
// =============================================================================
////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI Write and Read.
///         Used by the Bluetooth protocol stack.
/// @param  data(8 bits),
///         dir: emWL_SPI_RX or emWL_SPI_TX.
/// @retval data(receive data, 8 bits).
////////////////////////////////////////////////////////////////////////////////
u8 SPI_WriteRead(u8 data, EM_WL_SPI_DIR dir) //porting api
{
	SPI2->TDR = data;
	while(!(SPI2->SR & SPI_FLAG_TXEPT));
	while(!(SPI2->SR & SPI_FLAG_RXAVL));
    return SPI2->RDR;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI CS enable.
///         Used by the Bluetooth protocol stack.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_CS_Enable_() //porting api
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  SPI CS disable.
///         Used by the Bluetooth protocol stack.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void SPI_CS_Disable_() //porting api
{
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

////////////////////////////////////////////////////////////////////////////////
/// @brief  Get wireless irq status.
///         Used by the Bluetooth protocol stack.
/// @param  None.
/// @retval status.
////////////////////////////////////////////////////////////////////////////////
char IsIrqEnabled(void) //porting api
{
    return (!(GPIO_ReadInputData(GPIOB) & GPIO_Pin_1));
}