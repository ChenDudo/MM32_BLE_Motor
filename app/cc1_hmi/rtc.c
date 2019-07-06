////////////////////////////////////////////////////////////////////////////////
#define _RTC_C_
////////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include "types.h"

#include "hal_nvic.h"
#include "hal_rtc.h"
#include "hal_bkp.h"
#include "hal_rcc.h"
#include "hal_pwr.h"

#include "datetime.h"
#include "rtc.h" 

////////////////////////////////////////////////////////////////////////////////
void NVIC_RTC()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    
    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

////////////////////////////////////////////////////////////////////////////////
void RTC_IRQHandler(void)
{		 
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {							
        RTC_GetTime(&gtp);  
    }
    if(RTC_GetITStatus(RTC_IT_ALR) != RESET) { //�����ж�
        RTC_ClearITPendingBit(RTC_IT_ALR);		//�������ж�	  	   
    } 				  								 
    RTC_ClearITPendingBit(RTC_IT_SEC);		//�������ж�
    RTC_WaitForLastTask();	  	    						 	   	 
}

////////////////////////////////////////////////////////////////////////////////
u8 initRTC()
{
//        PWR_BackupAccessCmd(ENABLE);
//        BKP_DeInit();
//        RCC_LSEConfig(RCC_LSE_ON);
//        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
//        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
//        RCC_RTCCLKCmd(ENABLE);
//        RTC_WaitForSynchro();
//        RTC_WaitForLastTask();
//        RTC_ITConfig(RTC_IT_SEC, ENABLE);
//        RTC_WaitForLastTask();
//        RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
//        RTC_WaitForLastTask();
//        //NVIC_RTC();
//        return 0;
    
    u8 temp = 0;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);  
    PWR_BackupAccessCmd(ENABLE);
    
    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5050) { 
        BKP_DeInit();
        RCC_LSEConfig(RCC_LSE_ON);
        
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {
            temp++;
            //delay_ms(10);
        }
        if (temp >= 250) 
            return 1;
        
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		//����RTCʱ��(RTCCLK),ѡ��LSE��ΪRTCʱ��    
        RCC_RTCCLKCmd(ENABLE);	                        //ʹ��RTCʱ��  
        RTC_WaitForLastTask();	                        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        RTC_WaitForSynchro();		                //�ȴ�RTC�Ĵ���ͬ��  
        RTC_ITConfig(RTC_IT_SEC, ENABLE);		//ʹ��RTC���ж�
        RTC_WaitForLastTask();	                        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        RTC_EnterConfigMode();                          /// ��������	
        RTC_SetPrescaler(32767);                        //����RTCԤ��Ƶ��ֵ
        RTC_WaitForLastTask();	                        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        //RTC_Set(2016,1,1,0,0,0);                    //����ʱ��	
        RTC_ExitConfigMode();                           //�˳�����ģʽ  
        BKP_WriteBackupRegister(BKP_DR1, 0X5050);	//��ָ���ĺ󱸼Ĵ�����д���û���������
    }
    else  {                                             //ϵͳ������ʱ
        RTC_WaitForSynchro();	                        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
        RTC_ITConfig(RTC_IT_SEC, ENABLE);	        //ʹ��RTC���ж�
        RTC_WaitForLastTask();	                        //�ȴ����һ�ζ�RTC�Ĵ�����д�������
    }
    //NVIC_RTC();		    				     
    RTC_GetTime(&gtp);	
    return 0; //ok
}


////////////////////////////////////////////////////////////////////////////////
void checkRTC()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    
    //	PWR_BackupAccessCmd(DISABLE);
    PWR_BackupAccessCmd(ENABLE);
    
    if (BKP_ReadBackupRegister(BKP_DR1) != 0x5AA5)  {
        BKP_WriteBackupRegister(BKP_DR1, 0x5AA5);
        
        PWR_BackupAccessCmd(DISABLE);
        
        initRTC();
    }
    else {
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
            RTC_WaitForSynchro(); 
        
        RTC_ITConfig(RTC_IT_SEC, ENABLE);
        RTC_WaitForLastTask();
    }
    BKP_TamperPinCmd(DISABLE); 
    BKP_ITConfig(DISABLE);
    
    BKP_RTCOutputConfig(BKP_RTCOutputSource_CalibClock);
    RCC_ClearFlag();
    
    timeSel = 0;
    
}


////////////////////////////////////////////////////////////////////////////////
