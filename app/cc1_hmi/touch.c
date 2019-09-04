// Define to prevent recursive inclusion  --------------------------------------
#define _TOUCH_C_

// Includes  -------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include "system_mm32.h"

#include "drv.h"
#include "touch.h"
#include "resource.h"

#include "drv.h"
#include "common.h"

#include "hal_tim.h"
#include "hal_gpio.h"
#include "drv_tim.h"

#include "queue.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief  set delay time
/// @param  ms: delay time
/// @retval bool: true: delay ms time
////////////////////////////////////////////////////////////////////////////////
void delay_blocking(u16 ms)						//阻塞延迟
{
	u16 time;
	u16 changetime;
	tickFlag = false;
	time = tickCnt;
	while (1) {								
		if (tickFlag) {
			changetime = 500 - time + tickCnt;
		}
		else {
			changetime = tickCnt - time;
		}
		if (changetime == ms) break;			
	}
}

bool delay_nonblocking(u16 ms)					//非阻塞延迟
{
    static u16 cnt = 0;
    if (cnt == 0) {
        cnt = ms;
    }
	else if (sysTickFlag1mS) {
        sysTickFlag1mS = false;
        if (cnt > 0)
            cnt--;
        if (cnt == 0)
            return true;
    }
    return false;
}


//初始化触摸按键
//获得空载的时候触摸按键的取值.
//返回值:0,初始化成功;1,2,3初始化失败
////////////////////////////////////////////////////////////////////////////////
u8 TPAD_Init(u8 psc)
{
    u16 buf2[10];
    u16 buf3[10];
    u16 buf4[10];
    u16 temp;
    u8 j,i;
    TIM3_CH2_3_4_Cap_Init(TPAD_ARR_MAX_VAL, psc - 1);                           //以1Mhz的频率计数
    for (i = 0;i < 10; i++)                                                     //连续读取10次
    {
        buf2[i] = TPAD_Get_Val2();
        buf3[i] = TPAD_Get_Val3();
        buf4[i] = TPAD_Get_Val4();
    }
    for (i = 0; i < 9; i++){
        for (j = i + 1; j < 10; j++) {
            if (buf2[i] > buf2[j]) {
                temp    = buf2[i];
                buf2[i] = buf2[j];
                buf2[j] = temp;
            }
            if (buf3[i] > buf3[j]) {
                temp    = buf3[i];
                buf3[i] = buf3[j];
                buf3[j] = temp;
            }
            if (buf4[i] > buf4[j]) {
                temp    = buf4[i];
                buf4[i] = buf4[j];
                buf4[j] = temp;
            }
        }
    }
    temp = 0;
    for (i = 2; i < 8; i++) temp += buf2[i]; 
    tpad_default_val2 = temp / 6;
    if (tpad_default_val2 > 0x120) return 1; 

    temp = 0;
    for (i = 2; i < 8; i++) temp += buf3[i];
    tpad_default_val3 = temp / 6;
    if (tpad_default_val3 > 0x120) return 2;
    temp = 0;
    for (i = 2; i < 8; i++) temp += buf4[i];
    tpad_default_val4 = temp / 6;
    if (tpad_default_val4 > 0x120) return 3;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
void TPAD_Reset2(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    COMMON_EnableIpClock(emCLOCK_GPIOA);

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_20MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOA,GPIO_Pin_7);                                           //输出0,放电

    TIM_SetCounter(TIM3,0);
    TIM_ClearFlag(TIM3, (TIMFLAG_Typedef)(TIM_FLAG_CC2 | TIM_FLAG_Update));

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;                          //浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
void TPAD_Reset3(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    COMMON_EnableIpClock(emCLOCK_GPIOB);

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_20MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB,GPIO_Pin_0);

    TIM_SetCounter(TIM3,0);
    TIM_ClearFlag(TIM3, (TIMFLAG_Typedef)(TIM_FLAG_CC3 | TIM_FLAG_Update));
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
void TPAD_Reset4(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    COMMON_EnableIpClock(emCLOCK_GPIOB);

    GPIO_InitStructure.GPIO_Pin     = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_20MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_ResetBits(GPIOB,GPIO_Pin_1);

    TIM_SetCounter(TIM3,0);
    TIM_ClearFlag(TIM3, (TIMFLAG_Typedef)(TIM_FLAG_CC4 | TIM_FLAG_Update));

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//得到定时器捕获值
//如果超时,则直接返回定时器的计数值.
////////////////////////////////////////////////////////////////////////////////
u16 TPAD_Get_Val2(void)
{
    TPAD_Reset2();
    while(TIM_GetFlagStatus(TIM3, TIM_FLAG_CC2) == RESET)                       //等待捕获上升沿
    {
        if(TIM_GetCounter(TIM3) > TPAD_ARR_MAX_VAL - 500)
            return TIM_GetCounter(TIM3);                                        //超时了,直接返回CNT的值
    };
    return TIM_GetCapture2(TIM3);
}

////////////////////////////////////////////////////////////////////////////////
u16 TPAD_Get_Val3(void)
{
    TPAD_Reset3();
    while(TIM_GetFlagStatus(TIM3, TIM_FLAG_CC3) == RESET)
    {
        if(TIM_GetCounter(TIM3) > TPAD_ARR_MAX_VAL - 500)
            return TIM_GetCounter(TIM3);
    };
    return TIM_GetCapture3(TIM3);
}
////////////////////////////////////////////////////////////////////////////////
u16 TPAD_Get_Val4(void)
{
    TPAD_Reset4();
    while(TIM_GetFlagStatus(TIM3, TIM_FLAG_CC4) == RESET)
    {
        if(TIM_GetCounter(TIM3) > TPAD_ARR_MAX_VAL - 500) return TIM_GetCounter(TIM3);
    };
    return TIM_GetCapture4(TIM3);
}

////////////////////////////////////////////////////////////////////////////////
u16 TPAD_Get_MaxVal2(u8 n)
{
    u16 temp = 0;
    u16 res = 0;
    while(n--)
    {
        temp = TPAD_Get_Val2();
        if (temp > res) res = temp;
    };

    return res;
}

////////////////////////////////////////////////////////////////////////////////
u16 TPAD_Get_MaxVal3(u8 n)
{
    u16 temp = 0;
    u16 res = 0;
    while(n--)
    {
        temp = TPAD_Get_Val3();
        if (temp > res) res = temp;
    };

    return res;
}

////////////////////////////////////////////////////////////////////////////////
u16 TPAD_Get_MaxVal4(u8 n)
{
    u16 temp = 0;
    u16 res = 0;
    while(n--)
    {
        temp = TPAD_Get_Val4();
        if (temp > res) res = temp;
    };
    return res;
}


u16 rval; 
////////////////////////////////////////////////////////////////////////////////
u8 TPAD_Scan0(u8 mode) 
{ 
    u8 res = 0; 
	u8 sample = 3;								
	u16 rval; 
	if(mode) {
		sample = 3;	
		keyen2 = 0;
	} 
	rval = TPAD_Get_MaxVal2(sample);  
	if(rval > (tpad_default_val2 + startJudge_GATE)) {	
	  	rval = TPAD_Get_MaxVal2(sample);	  
		if((keyen2 == 0) && (rval > (tpad_default_val2 + TPAD_GATE_VAL))) { 
			res = 1; 
		}	    
		keyen2 = 5;	
	} 
	else  { 
    	if(keyen2 > 2) keyen2 = 2; 	
	} 
    if(keyen2) { 
    	keyen2--;	   	   	     	    	    
	} 
    return res; 
}

////////////////////////////////////////////////////////////////////////////////
void adjustDefaultFunc(void)
{
	u32 sum = 0;
	for (u16 i = 0; i < QUEUE_SIZE; i++) {
		sum += TPADdefaultQueue.MsgQueue[i];
//		TPADQueue.MsgQueue[i] = 0;
	}
	sum /= QUEUE_SIZE;
	tpad_default_val2 = (u16)((float)tpad_default_val2 * 0.8 + (float)sum * 0.2);
}

////////////////////////////////////////////////////////////////////////////////
void adjust_GATE_VAL2(MsgQueue_Def* queue)
{
	u32 sum = 0;
	for (u16 i = 0; i < QUEUE_SIZE; i++) {
		sum += queue->MsgQueue[i];
		queue->MsgQueue[i] = 0;
	}
	sum = sum / QUEUE_SIZE;
	TPAD_GATE_VAL2 = (u16)(sum * 0.60);
	queue->MsgQueueFlag = QUEUE_EMPTY;
}

////////////////////////////////////////////////////////////////////////////////
u8 TPAD_Scan1(u8 mode) 
{ 
	u8 res = 0; 
	u8 sample = 3;								
	u16 rval; 
	if(mode) {
		sample = 3;	
		keyen3 = 0;
	} 
	rval = TPAD_Get_MaxVal3(sample);  
	if(rval > (tpad_default_val3 + startJudge_GATE)) {	
	  	rval = TPAD_Get_MaxVal3(sample);	  
		if((keyen3 == 0) && (rval > (tpad_default_val3 + TPAD_GATE_VAL))) { 
			res = 1; 
		}	    
		keyen3 = 5;	
	} 
	else  { 
    	if(keyen3 > 2) keyen3 = 2; 	
	} 
    if(keyen3) { 
    	keyen3--;	   	   	     	    	    
	} 
    return res; 
}

////////////////////////////////////////////////////////////////////////////////
u8 TPAD_Scan2(u8 mode)
{
   	u8 res = 0; 
	u8 sample = 3;								
	u16 rval; 
	if(mode) {
		sample = 3;	
		keyen4 = 0;
	} 
	rval = TPAD_Get_MaxVal4(sample);  
	if(rval > (tpad_default_val4 + startJudge_GATE)) {	
	  	rval = TPAD_Get_MaxVal4(sample);	  
		if((keyen4 == 0) && (rval > (tpad_default_val4 + TPAD_GATE_VAL))) { 
			res = 1; 
		}	    
		keyen4 = 5;	
	} 
	else  { 
    	if(keyen4 > 2) keyen4 = 2; 	
	} 
    if(keyen4) { 
    	keyen4--;	   	   	     	    	    
	} 
    return res; 
}

////////////////////////////////////////////////////////////////////////////////
void TIM3_CH2_3_4_Cap_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef  TIM3_ICInitStructure;

    COMMON_EnableIpClock(emCLOCK_TIM3);
    COMMON_EnableIpClock(emCLOCK_GPIOA);
    COMMON_EnableIpClock(emCLOCK_GPIOB);

    GPIO_PinRemapConfig(0x00000000, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_20MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_20MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM3_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM3_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM3_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM3_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM3_ICInitStructure.TIM_ICFilter = 0x03;
    TIM_ICInit(TIM3, &TIM3_ICInitStructure);
    TIM3_ICInitStructure.TIM_Channel = TIM_Channel_3;
    TIM_ICInit(TIM3, &TIM3_ICInitStructure);
    TIM3_ICInitStructure.TIM_Channel = TIM_Channel_4;
    TIM_ICInit(TIM3, &TIM3_ICInitStructure);

    TIM_Cmd(TIM3,ENABLE );
}
