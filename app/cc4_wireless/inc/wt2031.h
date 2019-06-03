////////////////////////////////////////////////////////////////////////////////
/// @file     WT2031.H
/// @author   YQ Liu
/// @version  v2.0.0
/// @date     2019-02-18
/// @brief    THIS FILE CONTAINS ALL THE FUNCTIONS PROTOTYPES FOR THE WT2031
///           EXAMPLES.
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
#ifndef __WT2031_H
#define __WT2031_H

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @defgroup EXAMPLE_WT2031
/// @brief WT2031 example modules
/// @{


////////////////////////////////////////////////////////////////////////////////
/// @defgroup WT2031_Exported_Constants
/// @{

#define WT2031_ACRST_LOW()      GPIO_ResetBits(GPIOD, GPIO_Pin_1)
#define WT2031_ACRST_HIGH()     GPIO_SetBits(GPIOD, GPIO_Pin_1)
#define WT2031_STRB_LOW()       GPIO_ResetBits(GPIOD, GPIO_Pin_3)
#define WT2031_STRB_HIGH()      GPIO_SetBits(GPIOD, GPIO_Pin_3)

#define WT2031_SCLK_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define WT2031_SCLK_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define WT2031_TX_DATA()        GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)
#define WT2031_RX_DATA_LOW()    GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define WT2031_RX_DATA_HIGH()   GPIO_SetBits(GPIOA, GPIO_Pin_7)

#define WT2031_READY()          GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_4)


typedef struct {
    u16 header;                         // Always 0x13ec
    u8  power_control_id;
    u8  control_word_1;
    u16 rate_info[5];
    u16 res[3];
    u16 dtmf_ctl;
    u16 control_word_2;
    u16 data[12];    
} frame_input;

typedef struct {
    u16 header;                         // Always 0x13ec
    u8  power_control_id;               
    u8  control_word_1;
    u16 rate_info[5];                   
    u16 bit_error_rate;
    u16 soft_decision_distance;
    u16 detected_bit_errors;
    u16 dtmf_ctl;
    u16 control_word_2;
    u16 data[12];    
} frame_output;

/// @}


////////////////////////////////////////////////////////////////////////////////
/// @defgroup WT2031_Exported_Variables
/// @{
#ifdef _WT2031_C_
#define GLOBAL


#else
#define GLOBAL extern
#endif

GLOBAL u8 wt2031_rx_data[48];
GLOBAL u8 wt2031_tx_data[48];
GLOBAL frame_input wt2031_rx_data_str;
#undef GLOBAL

/// @}

////////////////////////////////////////////////////////////////////////////////
/// @defgroup WT2031_Exported_Functions
/// @{

void wt2031_init();
void wt2031_reset();
void wt2031_rw();
void wt2031_read();
void wt2031_write(u8* data);

/// @}

/// @}

/// @}

////////////////////////////////////////////////////////////////////////////////
#endif /* WT2031_H */
////////////////////////////////////////////////////////////////////////////////
