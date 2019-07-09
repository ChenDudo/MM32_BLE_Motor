////////////////////////////////////////////////////////////////////////////////
#define _DATETIME_C_
////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include "types.h"
#include "mm32.h"

#include "datetime.h"
#include "rtc.h"
#include "uart.h"
#include "tim.h"
#include "hal_rtc.h"

////////////////////////////////////////////////////////////////////////////////
void AlarmTick()
{
    
    if((gtp.hours  >= 9) && (gtp.hours  <= 18)){ 
        // 9:00 - 18:00 ���°����� (5s) �� ����
        if(gtp.minute == 0) {
            beepMode = bibi;
            if(gtp.second > 4)
                beepMode = biNone;
        }
        ledCmd = 1;     //open logo led
    }
    else {
        ledCmd = 2;     //close logo led
        
        if(gtp.hours == 11) {     
            if(gtp.minute == 40) {      // 11��40 ����ͬ����¥�÷� (3s)
                beepMode = bibi;
                if(gtp.second > 3)
                    beepMode = biNone;
            }
        }
        else {
            if(gtp.minute == 0) {       // �������� (2s)
                beepMode = bi;
                if(gtp.second > 2)
                    beepMode = biNone;
            }
            if(gtp.minute == 30) {      // ������� (1s)
                beepMode = bi;
                if(gtp.second > 1)
                    beepMode = biNone;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
u16 hex16Toint(u16 dat)
{
    u8 bit0 = dat % 16;
    u8 bit1 = (dat % 256) / 16;
    u8 bit2 = (dat % 4096) / 256;
    u8 bit3 = dat / 4096;
    
    return (bit3 * 10 + bit2 + bit1 * 1000 + bit0 * 100);
    
}

////////////////////////////////////////////////////////////////////////////////
u8 hex8Toint(u8 dat)
{
    u8 bit0 = dat % 16;
    u8 bit1 = dat / 16;
    
    return (bit1 * 10 + bit0); 
    
}

////////////////////////////////////////////////////////////////////////////////
void setTimTick()
{
    if (receiveTimFlag) {
        u8 *ptr = timData;
        
        u16 yearTemp  = *(u16*)(ptr + 1);
        u8  mouthTemp = (u8)*(ptr + 3);
        u8  dayTemp   = (u8)*(ptr + 4);
        u8  hourTemp  = (u8)*(ptr + 5);
        u8  minTemp   = (u8)*(ptr + 6);
        u8  secTemp   = (u8)*(ptr + 7);
        u8  weekTemp  = (u8)*(ptr + 8);
        
        tp1.year    = (u32)hex16Toint(yearTemp);
        tp1.month   = (u32)hex8Toint(mouthTemp);
        tp1.day     = (u32)hex8Toint(dayTemp  );
        tp1.hours   = (u32)hex8Toint(hourTemp );
        tp1.minute  = (u32)hex8Toint(minTemp  );
        tp1.second  = (u32)hex8Toint(secTemp  );
        tp1.week    = (u32)hex8Toint(weekTemp) - 1;
        RTC_SetTime(&tp1);
        
        receiveTimFlag = false;
    }
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
u32 DateTimeToSeconds(dateTimeDef *tp)
{
	u32 LeapY = (tp->year == 2000) ? 0 : (( tp->year - 2000 - 1) / 4 + 1);
	u32 ComY  = (tp->year - 2000) - LeapY;
	
	u32 Days =  ( tp->year % 4) ? 
		(LeapY * 366 + ComY * 365 + Month_Days_Accu_C[tp->month - 1] + (tp->day - 1)) : 
		(LeapY * 366 + ComY * 365 + Month_Days_Accu_L[tp->month - 1] + (tp->day - 1)); 
		
		
	return Days * SecsPerDay + tp->hours * 3600 + tp->minute * 60 + tp->second;
}

////////////////////////////////////////////////////////////////////////////////
u8 GetWeek(u32* sec)
{
	return  ((*sec / SecsPerDay) + 6) % 7;										//2000/1/1 week Saturday
}

////////////////////////////////////////////////////////////////////////////////
u16 GetYear(u32* sec)
{
	u8 i;
	u32 OffSec = *sec % SecsPerFourYear;
	for (i = 0; i < 4; i++){
		if (OffSec < Year_Secs_Accu[i + 1])
			break;
	}
	u16 year = 2000 + (*sec / SecsPerFourYear * 4) + i;
	*sec =  OffSec - Year_Secs_Accu[i];
	return year;
}

////////////////////////////////////////////////////////////////////////////////
u16 GetMouthItem(u32 *sec,  u32 *item)
{
	u8 i = 0;
	for (i = 0; i < 12; i++){
		if (*sec < *(item + i + 1)) {
			*sec = *sec - *(item + i);
			break;
		}
	}
	return i + 1;
}

////////////////////////////////////////////////////////////////////////////////
u16 GetMouth(u16 year, u32* sec)
{
	return (year % 4) ? GetMouthItem(sec,  &Month_Secs_Accu_C[0]) : 
		                GetMouthItem(sec,  &Month_Secs_Accu_L[0]);
}

////////////////////////////////////////////////////////////////////////////////
u16 GetDayItem(u16 day,  u16 *item)
{
	for (u8 i = 0; i < 12; i++){
		if (day < *(item + i + 1)) {
			day = day - *(item + i);
			break;
		}
	}
	return day + 1;
}

////////////////////////////////////////////////////////////////////////////////
u16 GetDay(u16 year, u32 *sec)
{
	u16 day = *sec / SecsPerDay;
	*sec = *sec % SecsPerDay;
	return (year % 4) ? GetDayItem(day,  &Month_Days_Accu_C[0]) : 
		                GetDayItem(day,  &Month_Days_Accu_L[0]);
}

////////////////////////////////////////////////////////////////////////////////
void RTC_GetTime(dateTimeDef *tp)
{ 
	u32 sec = RTC_GetCounter();
	
	tp->week = GetWeek(&sec);
	tp->year = GetYear(&sec);
	tp->month = GetMouth(tp->year ,&sec);
	tp->day = GetDay(tp->year ,&sec);
	
	tp->hours 	=  sec / 3600;
	tp->minute 	= (sec % 3600) / 60;
	tp->second 	= (sec % 3600) % 60;
}

////////////////////////////////////////////////////////////////////////////////
void RTC_SetTime(dateTimeDef *tp)
{
	RTC_SetCounter(DateTimeToSeconds(tp));
}

void adjYear()
{
//	if (Key3CntF)	{if (--gtp.year < 2016)	gtp.year = 2025;}
//	else 			{if (++gtp.year > 2025)	gtp.year = 2016;}
//	u8 day = Month_Days[gtp.month - 1];
//	if (gtp.year % 4)	day--; 
//	if (gtp.day > day) gtp.day = day;
}
void adjMonth()
{
//	if (Key3CntF)	{if (--gtp.month < 1) 	gtp.month = 12;}
//	else 			{if (++gtp.month > 12) 	gtp.month = 1;}
//	u8 day = Month_Days[gtp.month - 1];
//	if (gtp.year % 4)	day--; 
//	if (gtp.day > day) gtp.day = day;
}

void adjDay()
{
//	u8 day = Month_Days[gtp.month - 1];
//	if (gtp.year % 4)	day--; 
//	if (Key3CntF)	{if (--gtp.day < 1)	 	gtp.day = day;}
//	else 			{if (++gtp.day > day) 	gtp.day = 1;}
}

void adjHours()
{
//	if (Key3CntF)	{if (--gtp.hours == -1) gtp.hours = 23;}
//	else 			{if (++gtp.hours > 23) 	gtp.hours = 0;}
}		

void adjMinute()
{
//	if (Key3CntF)	{if (--gtp.minute == -1) gtp.minute = 59;}
//	else 			{if (++gtp.minute > 59) 	gtp.minute = 0;}
}

void adjSecond()
{
//	if (Key3CntF)	{if (--gtp.second == -1) gtp.second = 59;}
//	else 			{if (++gtp.second > 59) 	gtp.second = 0;}
}
////////////////////////////////////////////////////////////////////////////////
void modifyTime(u8 sel)
{
	switch (sel) {
	case 0:		adjYear();		break;
	case 1:		adjMonth();		break;
	case 2:		adjDay();		break;
	case 3:		adjHours();		break;
	case 4:		adjMinute();	break;
	case 5:		adjSecond();	break;
	default:	RTC_SetTime(&gtp);		break;
	}
}
