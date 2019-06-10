////////////////////////////////////////////////////////////////////////////////
/// @file     SPI_POLLING.C
/// @author   Z Yan
/// @version  2.0.0 Beta4
/// @date     2019-02-10
/// @brief    THIS FILE PROVIDES ALL THE SPI_DMA EXAMPLE.
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

////////////////////////////////////////////////////////////////////////////////
#define _SPI_FLASH_C_
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "types.h"
#include "system_mm32.h"

#include "drv.h"
#include "spi_flash.h"
#include "drv_display.h"
#include "drv_keyboard.h"
#include "bsp.h"
#include "spi_lcd.h"
#include "hal_tim.h"
#include "hal_rcc.h"
#include "hal_i2c.h"
#include "bsp_led.h"
#include "touch.h"

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup MM32_Example_Layer
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup EXAMPLE_SPI
/// @{

////////////////////////////////////////////////////////////////////////////////
/// @addtogroup SPI_Exported_Functions
/// @{


u8 readIDBuffer[3];
#define BUFFER_SIZE 200
u8 newTxBuffer[BUFFER_SIZE];
u8 newRxBuffer[BUFFER_SIZE];

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  : None.
/// @retval : None.
////////////////////////////////////////////////////////////////////////////////
void AppTaskTick()
{
    if (tickCnt++ >= 500) {
        tickCnt  = 0;
        tickFlag = true;
    }
    DRV_KEY_GetKeyValue();
}


////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles compare.
/// @param  pBuffer: Pointer points to a buffer.
/// @param  pBuffer: Pointer points to another buffer.
/// @param  BufferLength: buffer length.
/// @retval : status.
////////////////////////////////////////////////////////////////////////////////
u8 Buffercmp(const u8* pBuffer, u8* pBuffer1, u16 BufferLength)
{
    u16 i = 0;

    for(i = 0; i < BufferLength; i++){
        if(*pBuffer != *pBuffer1){
            return 1;
        }
        pBuffer++;
        pBuffer1++;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

u16 color = 0;
u8 flag = 0;
u16 pwmValue = 0;
u32 adc_temp = 0;

////////////////////////////////////////////////////////////////////////////////
void beep()
{
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_EXTI, ENABLE);

    GPIOB_ClockEnable();
    GPIO_Mode_OUT_OD_20MHz_Init(GPIOB, GPIO_Pin_3,  EXTI_MAPR_SWJ_JTAGDISABLE, GPIO_AF_0);
    GPIO_Mode_AF_PP_20MHz_Init(GPIOB, GPIO_Pin_3, EXTI_MAPR_TIM2_PARTIAL1, GPIO_AF_2);

    TIM_TimeBaseInitTypeDef pBase = {
        .TIM_Prescaler         = 71,
        .TIM_Period            = 999,
        .TIM_ClockDivision     = TIM_CKD_DIV1,
        .TIM_CounterMode       = TIM_CounterMode_Up,
        .TIM_RepetitionCounter = 0
    };
    TIM_TimeBaseInit(TIM2, &pBase);

    TIM_OCInitTypeDef pOC = {
        .TIM_Pulse        = 10,
        .TIM_OCMode       = TIM_OCMode_PWM1,
        .TIM_OutputState  = TIM_OutputState_Enable,
        .TIM_OutputNState = TIM_OutputNState_Disable,
        .TIM_OCPolarity   = TIM_OCPolarity_High,
        .TIM_OCNPolarity  = TIM_OCNPolarity_High,
        .TIM_OCIdleState  = TIM_OCIdleState_Reset,
        .TIM_OCNIdleState = TIM_OCNIdleState_Reset
    };
    TIM_OC2Init(TIM2, &pOC);

    TIM_CCPreloadControl(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);

    TIM_Cmd(TIM2, ENABLE);

    for(u32 i = 0; i < 1000; i++)
        for(u32 j = 0; j < 2000; j++);
    TIM_Cmd(TIM2, DISABLE);
}

void I2CTXByte(I2C_TypeDef *I2Cx,unsigned char temp)
{
    I2C_SendData(I2Cx, temp);
    while(I2C_GetFlagStatus(I2Cx, I2C_STATUS_FLAG_TFE)==0);

}
void I2CRXGroup(I2C_TypeDef *I2Cx,u16 rx_count,u8 *data_buf)
{
    int i,data_cnt = 0;
    char flag = 0;
    for(i=0;i<rx_count;i++)
    {
        while(1)
        {
            if((I2C_GetFlagStatus(I2Cx, I2C_STATUS_FLAG_TFNF) == 1) && (flag == 0))//RX FIFO未满就写入发送指令
            {
                I2C_ReadCmd(I2Cx);
                data_cnt++;
                if(data_cnt == rx_count)
                    flag = 1;
            }
            if(I2C_GetFlagStatus(I2Cx, I2C_STATUS_FLAG_RFNE))
            {
                data_buf[i] = I2C_ReceiveData(I2Cx);
                break;
            }
        }
    }
}

void I2CMasterWrite(I2C_TypeDef *I2Cx,unsigned short mem_byte_addr, unsigned short tx_count, unsigned char *tx_data )
{
    unsigned short i;
    unsigned char *p;
    p = tx_data;

    I2CTXByte(I2Cx,mem_byte_addr);
    for(i=0;i<tx_count;i++)
    {
        I2CTXByte(I2Cx,*p); //发送数据
        p++;
    }

    I2C_GenerateSTOP( I2Cx, ENABLE);
    while((I2C_GetITStatus(I2Cx, I2C_IT_STOP_DET))==0);
}

void I2CMasterRead(I2C_TypeDef *I2Cx,unsigned short mem_byte_addr, unsigned short rx_count, unsigned char *rx_data )
{
    I2CMasterWrite(I2Cx, mem_byte_addr, 0, rx_data);//写设备地址
    I2CRXGroup(I2Cx,rx_count,rx_data);

    I2C_GenerateSTOP( I2Cx, ENABLE);
    while((I2C_GetITStatus(I2Cx, I2C_IT_STOP_DET))==0);
}

u8 testEEPROM()
{
    u32 j = 80000;
    u8 txBuffer[8] = {0};
    u8 rxBuffer[8] = {0};

    GPIOB_ClockEnable();
    GPIO_Mode_AF_OD_20MHz_Init(GPIOB, GPIO_Pin_10, NO_REMAP, GPIO_AF_1);
    GPIO_Mode_AF_OD_20MHz_Init(GPIOB, GPIO_Pin_11, NO_REMAP, GPIO_AF_1);

    RCC_APB1PeriphClockCmd(RCC_APB1ENR_I2C2, ENABLE);

    I2C_InitTypeDef InitStructure;

    InitStructure.Mode  = true;
    InitStructure.Speed = false;
    InitStructure.OwnAddress = 0x20;
    InitStructure.ClockSpeed = 100000;

    I2C_Init(I2C2, &InitStructure);

    I2C_Cmd(I2C2, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    I2C_Cmd(I2C2, DISABLE);
    I2C2->TAR = 0xA0 >> 1;

    I2C_Cmd(I2C2, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    for(u8 i = 0; i < 8 ; i++){
        txBuffer[i] = i;
    }

    I2CMasterWrite(I2C2,0, 8, txBuffer);
    while(j--);
    I2CMasterRead(I2C2,0, 8, rxBuffer);

    return Buffercmp(txBuffer, rxBuffer, 8);
}

int main(void)
{
    u8 keynum[4] = {0};
    u8 checkSPIFLASH = 0;
    u8 checkEEPROM = 1;

    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);

    //beep();

    checkEEPROM = testEEPROM();
    checkSPIFLASH = spiFlashInit();

    SPI_DeInit(SPI2);

    HANDLE hTIM = CreateFile(emIP_TIM);
    if (hTIM == NULL)       while(1);

    tAPP_TIM_DCB dcb = {
        .hSub       = emFILE_TIM4,
        .mode       = emTIM_PWM,
        .cntFreq    = 1000000,
        .period     = 1000,
        .ch         = emTIM_CH4,
        .pulse      = 500,
        .remapEn    = false,
        .remapIdx   = 0,
    };

    if (!OpenFile(hTIM, (void*)&dcb))       while(1);
    pwmValue = dcb.pulse;

    HANDLE hADC = CreateFile(emIP_ADC);
    if (hADC == NULL)       while(1);

    tAPP_ADC_DCB dcb2 = {
        .hSub   = emFILE_ADC1,
        .type   = emTYPE_DMA,
        .mode   = emADC_Scan,
        .sync   = emTYPE_ASync,
        .trig   = emTRIGGER_Software,
        .chs    = LEFT_SHIFT_BIT(4),
        .temp   = false,
        .vRef   = false,
        .cb     = NULL,
    };

    if (!OpenFile(hADC, (void*)&dcb2))      while(1);

    lcd_init();
    lcd_clear(WHITE);
    lcd_drawLine(10, 80, 200, 100);
    lcd_drawLine(10, 80, 200, 200);
    lcd_drawRectangle(10 ,30, 40, 70);
    lcd_drawCircle(100, 40, 30);

    POINT_COLOR = RED;
    BACK_COLOR = WHITE;

    if(checkSPIFLASH){
        lcd_showString(0, 100, 240, 32, 32, "SPIFLASH OK");
    }
    else{
        lcd_showString(0, 100, 240, 32, 32, "SPIFLASH FAIL");
    }
    if(checkEEPROM){
        lcd_showString(0, 100, 240, 32, 32, "EEPROM FAIL");
    }
    else{
        lcd_showString(0, 50, 240, 32, 32, "EEPROM OK");
    }

    lcd_showString(0, 140, 240, 24, 24, "TESTing Key");

    POINT_COLOR = WHITE;
    BACK_COLOR = BLUE;
    lcd_showString(0, 170, 240, 16, 16, "Mindmotion");
    lcd_showString(0, 200, 240, 12, 12, "2019/05/08");

    for(u8 i = 0; i < 4; i++){
        LED.status[i] = true;
    }

    while(1){
        if (KEY.event[0]) {
            KEY.event[0]  = false;
            keynum[0]++;
            LED.status[0] = !LED.status[0];
            LED.status[0] ? LD1_on() : LD1_off();
        }
        if (KEY.event[1]) {
            KEY.event[1]  = false;
            keynum[1]++;
            LED.status[1] = !LED.status[1];
            LED.status[1] ? LD2_on() : LD2_off();

        }
        if (KEY.event[2]) {
            KEY.event[2]  = false;
            keynum[2]++;
            LED.status[2] = !LED.status[2];
            LED.status[2] ? LD3_on() : LD3_off();
        }

        if(keynum[0] == 5){
            break;
        }

        WriteFile(hADC, emFILE_ADC1, 0, 0);
        ReadFile(hADC, emFILE_ADC1, (u8*)&adc_temp, 16);

        pwmValue = (u16)((double)adc_temp / 4096 * 1000);

        WriteFile(hTIM, emFILE_TIM4, (u8*)&pwmValue, 1);
    }

    lcd_showString(0, 140, 240, 24, 24, "TESTing Capacitance");
    while(TPAD_Init(6));                    //Initialize the touch button, if there is an error, the LD6 is bright

    while (1) {
        if(TPAD_Scan2(0))   {
            LED.status[0] = !LED.status[0];
            LED.status[0] ? LD1_on() : LD1_off();
        }
        if(TPAD_Scan3(0))   {
            LED.status[1] = !LED.status[1];
            LED.status[1] ? LD2_on() : LD2_off();
        }
        if(TPAD_Scan4(0))   {
            LED.status[2] = !LED.status[2];
            LED.status[2] ? LD3_on() : LD3_off();
        }

        WriteFile(hADC, emFILE_ADC1, 0, 0);
        ReadFile(hADC, emFILE_ADC1, (u8*)&adc_temp, 16);

        pwmValue = (u16)((double)adc_temp / 4096 * 1000);

        WriteFile(hTIM, emFILE_TIM4, (u8*)&pwmValue, 1);

    }
}

////////////////////////////////////////////////////////////////////////////////
//void main()
//{
//    MCUID = SetSystemClock(emSYSTICK_On, (u32*)&AppTaskTick);
//
////    SPI_DeInit(SPI2);
//    HANDLE hTIM = CreateFile(emIP_TIM);
//    if (hTIM == NULL)       while(1);
//
//    tAPP_TIM_DCB dcb = {
//        .hSub       = emFILE_TIM4,
//        .mode       = emTIM_PWM,
//        .cntFreq    = 1000000,
//        .period     = 1000,
//        .ch         = emTIM_CH4,
//        .pulse      = 500,
//        .remapEn    = false,
//        .remapIdx   = 0,
//    };
//
//    if (!OpenFile(hTIM, (void*)&dcb))       while(1);
//
//    HANDLE hADC = CreateFile(emIP_ADC);
//    if (hADC == NULL)       while(1);
//
//    tAPP_ADC_DCB dcb2 = {
//        .hSub   = emFILE_ADC1,
//        .type   = emTYPE_DMA,
//        .mode   = emADC_Scan,
//        .sync   = emTYPE_ASync,
//        .trig   = emTRIGGER_Software,
//        .chs    = LEFT_SHIFT_BIT(4),
//        .temp   = false,
//        .vRef   = false,
//        .cb     = NULL,
//    };
//
//    if (!OpenFile(hADC, (void*)&dcb2))      while(1);
//
//    lcd_init();
//
//    POINT_COLOR = BLUE;
//    BACK_COLOR = WHITE;
//    lcd_showString(20, 5, 240, 24, 24, "BlueTooth Display");
//
//    POINT_COLOR = GREEN;
//    lcd_drawRectangle(10 ,40, 230, 220);
//    lcd_drawLine(10, 60, 230, 60);
//    lcd_drawLine(10, 80, 230, 80);
//    lcd_drawLine(10,100, 230,100);
//    lcd_drawLine(10,120, 230,120);
//    lcd_drawLine(10,140, 230,140);
//    lcd_drawLine(10,160, 230,160);
//    lcd_drawLine(10,180, 230,180);
//    lcd_drawLine(10,200, 230,200);
//
//
//    while(1){
//        WriteFile(hADC, emFILE_ADC1, 0, 0);
//        ReadFile(hADC, emFILE_ADC1, (u8*)&adc_temp, 16);
//        pwmValue = (u16)((double)adc_temp / 4096 * 1000);
//        WriteFile(hTIM, emFILE_TIM4, (u8*)&pwmValue, 1);
//
//
//    }
//
//
//}

