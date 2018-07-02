////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/09/08
// File Name:     rtc.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/09/08 by Yu
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////
#include "ahmirtc.h"
#include "FreeRTOS.h"
#include "TagClass.h"
#include "task.h"
#include "FreeRTOS.h"
#include "ahmirtc.h"
//#include "AHMICfgDefinition.h"

#ifdef EMBEDDED
#include "RX8025.h"
#endif

extern TagClassPtr			TagPtr;
unsigned char RTCData[16];

#ifdef EMBEDDED
void RTC_init(uint8_t* pRTCData)
{
	Init_device(pRTCData);
}

void GetTimerAndDate(uint8_t* pRTCData, uint32_t* pTime, uint32_t* pDate)
{
	RX8025_Read(pRTCData, 7);
	*pTime = (pRTCData[2] << 16) + (pRTCData[1] << 8) + pRTCData[0];
	*pDate = 0x20000000 + (pRTCData[6] << 16) + (pRTCData[5] << 8) + pRTCData[4];
}

void ChangeRTC(void)
{
	uint32_t time;
	uint32_t date;
	
	time = TagPtr[SYSTEM_RTC_HOUR_MINUITE_TAG].getValue();
	date = TagPtr[SYSTEM_RTC_YEAR_TAG].getValue();
	
	RTCData[0] = time & 0x000000FF;
	RX8025_Write(RX8025_ADDR_SECONDS, RTCData[0]);
	RTCData[1] = (time >> 8) & 0x000000FF;
	RX8025_Write(RX8025_ADDR_MINUTES, RTCData[1]);
	RTCData[2] = (time >> 16) & 0x000000FF;
	RX8025_Write(RX8025_ADDR_HOURS, RTCData[2]);
	
	RTCData[4] = date & 0x000000FF;
	RX8025_Write(RX8025_ADDR_DATES, RTCData[4]);
	RTCData[5] = (date >> 8) & 0x000000FF;
	RX8025_Write(RX8025_ADDR_MONTH, RTCData[5]);
	RTCData[6] = (date >> 16) & 0x000000FF;
	RX8025_Write(RX8025_ADDR_YEARS, RTCData[6]);
}
#endif

void RTCReadTask(void* pvParameters)
{
#ifdef EMBEDDED
	uint32_t time;
	uint32_t date;
#endif
	
#ifdef EMBEDDED
	RTC_init(RTCData);
#endif

	for(;;)
	{
#ifdef EMBEDDED
		GetTimerAndDate(RTCData, &time, &date);
		TagPtr[SYSTEM_RTC_YEAR_TAG].setValue(date,SYSTEM_RTC_YEAR_TAG);
		TagPtr[SYSTEM_RTC_HOUR_MINUITE_TAG].setValue(time,SYSTEM_RTC_HOUR_MINUITE_TAG);
#endif
		vTaskDelay(200 / portTICK_RATE_MS);
	}
}

