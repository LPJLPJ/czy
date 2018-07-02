////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     TimerClass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.01 - modify timer function by 程泽雨
// Revision 2.04 - C++, 20160321 by 于春营
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef  TIMERCLASS__H
#define  TIMERCLASS__H

#include "FreeRTOS.h"
#include "publicType.h"
#include "timers.h"
#include "AHMIBasicStructure.h"
#include "publicDefine.h"

#ifdef AHMI_CORE

#define TimerStop                   0x0
#define TimerStart                  0x1
#define TimerSingleRun              0x0
#define TimerRoundRun               0x2
#define TimerIncrement              0x0
#define TimerDecrement              0x4

#define TimerMode    mUserVariables[0]
#define startValue   mUserVariables[1]
#define StopValue    mUserVariables[2]
#define StepValue    mUserVariables[3]
#define IntervalTime mUserVariables[4]

class TimerClass
{
public:
	u32 mUserVariables[5];
	u32 mCurValue;
	u16 mTagID;
	TimerHandle_t hTimer;
	TimerClass();
	~TimerClass();
	void TimerSetValue(u8 ID, u32 v);
	void initTimer(u16 TimerID);//TAG中的最开始为指向Timer的tag
	void curTimerStop();
};

typedef  TimerClass* TimerClassPtr;

void TimerCallbackFunc( TimerHandle_t pxTimer );
void AnimationTimerCallbackFunc( TimerHandle_t pxTimer );

//void TimerSetValue(TimerClassPtr Ptr, ActionTriggerClass* tagtrigger);

void TimerProcessTask(void* pvParameters);
void AnimationTimerProcessTask(void* pvParameters);
void TimerProcess(TimerHandle_t pxTimer);


#endif

#endif

