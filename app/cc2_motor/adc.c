// Define to prevent recursive inclusion  --------------------------------------
#define _ADC_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include "system_mm32.h"

#include "common.h"
#include "bsp.h"
#include "adc.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include "hal_dma.h"

#include "tb6612.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void adcTick()
{
    // 25deg temp reference value at 0x1FFF_F7F4
	static float temp;

	u16 *p;
	p = (u16*)0x1FFFF7F4;
	
	temp =  ((*p == 0xffff) || (*p == 0x0000) || (*p < 1700) || (*p > 1900)) 
		?  1800  : *p;	
		
	adcVol += ADC1Value[0];
	adcTmp += ADC1Value[1];
	
	if (adcVolTick ++ >= 80) {
		adcVolTick = 0;
		adcVolt = ((adcVol / 80) * 30 + adcVolt * 70) / 100;  
		if (adcVolt > 4096)  adcVolt = 0;
		adcVol = 0;
		adcVolFlag = true;
	} 
	
	if (adcTempTick ++ >= 400) {
		adcTempTick = 0;
		temp = ((adcTmp / 400) * 10 + temp * 90) / 100;  
		adcTemp = (10 * (temp - 1800) / 43) + 25;
		adcTmp = 0;
		adcTempFlag = true;
    } 
}

////////////////////////////////////////////////////////////////////////////////
void changeSecMax()
{
    if (adcVolFlag) {
        securMax = (u16)(adcVolt * 20 / 4096  + 70);    //range (70 : 90)
        adcVolFlag = false;
    }
}

////////////////////////////////////////////////////////////////////////////////
void initGPIO_ADC()
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	COMMON_EnableIpClock(emCLOCK_GPIOA);
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_1;
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
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1,           1, ADC_Samctl_1_5);    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor,  2, ADC_Samctl_1_5);    
    
	ADC_ITConfig(ADC1, ADC_IT_EOC, DISABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_DMACmd(ADC1, ENABLE);
	ADC1->CFGR |= 0x04;
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
