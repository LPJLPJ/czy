////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     UartClass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef UART__H
#define UART__H

#include "publicType.h"

//#ifdef USE_UART
typedef struct Uart
{
	u8* UartBuffer;
	u8  LinkTag;
}UartClass;

typedef UartClass* UartClassPtr;


void UartDataReceiveTask(void* pvParameters);
void UartReceiveStateMachine(u8 data,u8 reset);
void CountDownTimerCallBackFunc(TimerHandle_t pxTimer);
void InitUartBuffer(u8 ID, u8 size, u8 TagID);
void UpdataTag(u8 ID, u16 v);

//#endif
	

#endif
