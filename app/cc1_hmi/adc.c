////////////////////////////////////////////////////////////////////////////////
#define _ADC_C_
////////////////////////////////////////////////////////////////////////////////

#include "types.h"
#include "mm32.h"

#include "common.h"

#include "bsp.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include "hal_dma.h"
#include "hal_tim.h"

#include "adc.h"
#include "tim.h"

////////////////////////////////////////////////////////////////////////////////
void adcTick()
{
    adcVol += ADC1Value[0];
    if(adcTickCnt ++ > 100) {
        adcTickCnt = 0;
        adcVolt = ((adcVol / 100) * 40 + adcVolt * 60) / 100;
        if(adcVolt > 4096) adcVolt = 4095;
        adcVol = 0;
    }
    u16 lcdPwm = (adcVolt * 999) / 4096;
    TIM_SetCompare4(TIM4, lcdPwm);
}

////////////////////////////////////////////////////////////////////////////////
void initGPIO_ADC()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	COMMON_EnableIpClock(emCLOCK_GPIOA);
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
void initADC()
{
    COMMON_EnableIpClock(emCLOCK_ADC1);
    COMMON_EnableIpClock(emCLOCK_DMA1);

    initGPIO_ADC();

    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    DMA_DeInit(DMA1_ch1);
    DMA_InitStructure.PeripheralBaseAddr = (u32)&(ADC1->DR);
    DMA_InitStructure.MemoryBaseAddr = (u32)&ADC1Value[0];
    DMA_InitStructure.DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.BufferSize = 2;
    DMA_InitStructure.PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.MemoryInc = DMA_MemoryInc_Enable;						//DMA_MemoryInc_Disable;
    DMA_InitStructure.PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.MemoryDataSize = DMA_MemoryDataSize_Word;
    DMA_InitStructure.Mode = DMA_Mode_Circular;
    DMA_InitStructure.Priority = DMA_Priority_High;
    DMA_InitStructure.M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_ch1, &DMA_InitStructure);
    DMA_Cmd(DMA1_ch1, ENABLE);

    ADC_InitStructure.ADC_Mode = ADC_CR_CONTINUE;
    ADC_InitStructure.ADC_PRESCARE = ADC_PCLK2_PRESCARE_16;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC1_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_Init(ADC1, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_4,           1, ADC_Samctl_1_5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor,  2, ADC_Samctl_1_5);

	ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
	ADC1->CFGR |= 0x04;

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
