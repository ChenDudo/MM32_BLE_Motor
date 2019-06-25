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


////////////////////////////////////////////////////////////////////////////////
/// @brief  This function handles App SysTick Handler.
/// @param  None.
/// @retval None.
////////////////////////////////////////////////////////////////////////////////
void adcTick()
{
    
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
    
    initGPIO_ADC();
    
    
    
}
