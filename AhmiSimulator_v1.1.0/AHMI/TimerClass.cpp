////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     TimerClass.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
//          2.01 - 修改Timer绑定的Tag编号 by XT 20151120
// Revision 2.04 - C++, 20160321 by 于春营
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"

#ifdef AHMI_CORE

extern TagClassPtr   TagPtr;
extern TimerClassPtr   TimerPtr;
extern QueueHandle_t   TimerCmdQueue;
extern QueueHandle_t   AnimationTimerCmdQueue;
extern  ConfigInfoClass		ConfigData;
extern AnimationClass  gAnimationClass;

#ifdef EMBEDDED
extern uint8_t runningControl;
#endif

TimerClass::TimerClass(void)
{}

TimerClass::~TimerClass(void)
{}

//-----------------------------
// 函数名： InitTimer
// 初始化timer
// 参数列表：
//   @param1 u16 TimerID  timer的标号
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void TimerClass::initTimer(u16 TimerID)
{
	hTimer =  xTimerCreate
		          (  /* Just a text name, not used by the RTOS kernel. */
                     "Timer",
                     /* The timer period in ticks. */
                     10,
                     /* The timers will auto-reload themselves when they expire. */
                     pdTRUE,
                     /* Assign each timer a unique id equal to its array index. */
                     ( void * ) TimerID,
                     /* Each timer calls the same callback when it expires. */
                     TimerCallbackFunc
                   );
	mTagID = NUM_OF_SYSTEM_TAG/*TagID从8号开始，0号tag不绑定任何数据 by XT 20151120*/ 
		         + TimerID;//现在timer只有一个tag
	//TagPtr[mTagID].bindTimer(TimerID);
}

//-----------------------------
// 函数名： TimerSetValue
// 给timer的某个变量赋值
// 参数列表：
//   @param1 TimerClassPtr Ptr timer指针
//   @param2 u8  ID            标号
//   @param3 u16 v             设置的参数值
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void TimerClass::TimerSetValue(u8 ID, u32 v)
{
	//if(v < 0)
	//	return;
	mUserVariables[ID] = v;
	if(ID == 0 && ((v & TimerStart) !=0))//start timer
	{
		mCurValue = startValue;
		if( xTimerIsTimerActive( hTimer ) != pdFALSE )
			xTimerStop(hTimer,0);
		xTimerChangePeriod(hTimer,IntervalTime,0);
		xTimerStart(hTimer,0);//启动Timer
		TagPtr[mTagID].setValue(mCurValue, mTagID);
	}
	else if (ID == 0 && ((v & TimerStart) ==0))//stop timer
	{
		xTimerStop(hTimer,0);
	}
}

//-----------------------------
// 函数名： curTimerStop
// 给timer的某个变量赋值
// 参数列表：
//   @param1 TimerClassPtr Ptr timer指针
//   @param2 u8  ID            标号
//   @param3 u16 v             设置的参数值
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void TimerClass::curTimerStop()
{
	this->TimerMode &= ~TimerStart;
}


//-----------------------------
// 函数名： TimerCallbackFunc
// timer回调函数
// 参数列表：
//   @param1 TimerHandle_t pxTimer  timer的句柄 
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void TimerCallbackFunc( TimerHandle_t pxTimer )
{
	u32 TimerID;
	if(NULL == pxTimer)
		return;
	TimerID = (u32 )pvTimerGetTimerID( pxTimer );
	if(TimerID == ConfigData.NumofTimers)
		xQueueSendToBack(AnimationTimerCmdQueue,&pxTimer,0);
	else 
		xQueueSendToBack(TimerCmdQueue,&pxTimer,0);
}


//-----------------------------
// 函数名： TimerProcess
// 执行timer的参数变化
// 参数列表：
//    @param1 TimerHandle_t  pxTimer     timer句柄
// 备注(各个版本之间的修改):
//-----------------------------
void TimerProcess(TimerHandle_t pxTimer)
{
	u8  TimerAdd ;
	u16 TimerID;
//	u16 newtagvalue;
	TimerClassPtr pCurTimer;
	TagClassPtr pBindingTag;

	if(NULL == pxTimer)
		return;

	TimerID = (u32 )pvTimerGetTimerID( pxTimer );//获取Timer ID
	
	if(TimerID == ConfigData.NumofTimers) //indicate that this is animation timer
	{
		gAnimationClass.Animation_maintain(4);
		return;
	}

	pCurTimer = &TimerPtr[TimerID]; //find current timer
	pBindingTag = &TagPtr[pCurTimer->mTagID]; //find current tag

	TimerAdd = TimerPtr[TimerID].TimerMode & TimerDecrement;
	if(TimerAdd == TimerDecrement)//timer自减
	{
		if( pBindingTag->mValue >=TimerPtr[TimerID].StopValue && pBindingTag->mValue <= TimerPtr[TimerID].startValue && (s32)pBindingTag->mValue >= 0) //in the duration
		{
			TimerPtr[TimerID].mCurValue = pBindingTag->mValue;
		}
		else if(TimerPtr[TimerID].TimerMode & TimerRoundRun && (pBindingTag->mValue < TimerPtr[TimerID].StopValue || (s32)pBindingTag->mValue <= 0) )//round run
		{
			TimerPtr[TimerID].mCurValue = TimerPtr[TimerID].startValue;
			TagPtr[TimerPtr[TimerID].mTagID].setValue(TimerPtr[TimerID].mCurValue, TimerPtr[TimerID].mTagID);
			return;
		}
		else if(pBindingTag->mValue < TimerPtr[TimerID].StopValue || pBindingTag->mValue > TimerPtr[TimerID].startValue || (s32)pBindingTag->mValue < 0)//stop timer
		{
			TimerPtr[TimerID].mCurValue = pBindingTag->mValue;
			xTimerStop(pxTimer,0);
			TimerPtr[TimerID].curTimerStop();
			return;
		}
	}
	else
	{
		if( pBindingTag->mValue >=TimerPtr[TimerID].startValue && pBindingTag->mValue <= TimerPtr[TimerID].StopValue && (s32)pBindingTag->mValue >= 0) //in the duration
		{
			TimerPtr[TimerID].mCurValue = pBindingTag->mValue;
		}
		else if(TimerPtr[TimerID].TimerMode & TimerRoundRun && pBindingTag->mValue > TimerPtr[TimerID].StopValue && (s32)pBindingTag->mValue >= 0)//round run
		{
			TimerPtr[TimerID].mCurValue = TimerPtr[TimerID].startValue;
			TagPtr[TimerPtr[TimerID].mTagID].setValue(TimerPtr[TimerID].mCurValue, TimerPtr[TimerID].mTagID);
			return;
		}
		else if(pBindingTag->mValue < TimerPtr[TimerID].startValue || pBindingTag->mValue > TimerPtr[TimerID].StopValue || (s32)pBindingTag->mValue < 0)//stop timer
		{
			TimerPtr[TimerID].mCurValue = pBindingTag->mValue;
			xTimerStop(pxTimer,0);
			TimerPtr[TimerID].curTimerStop();
			return;
		}
	}

	
	if(TimerAdd == TimerDecrement)//timer自减
	{
		TimerPtr[TimerID].mCurValue -=  TimerPtr[TimerID].StepValue;
		if( ( (TimerPtr[TimerID].mCurValue < TimerPtr[TimerID].StopValue) || (s32)TimerPtr[TimerID].mCurValue < 0 ) && !(TimerPtr[TimerID].TimerMode & TimerRoundRun)) //stop
		{
			xTimerStop(pxTimer,0);
			TimerPtr[TimerID].curTimerStop();
			return;
		}
		else if( (TimerPtr[TimerID].mCurValue == TimerPtr[TimerID].StopValue) && !(TimerPtr[TimerID].TimerMode & TimerRoundRun)) //??stop value
		{
				xTimerStop(pxTimer,0);
				TimerPtr[TimerID].curTimerStop();
				//newtagvalue = TimerPtr[TimerID].TimerMode & (~TimerStart); //关闭Timer
				//TagPtr[TimerID*6].setValue(newtagvalue, TimerID*6);
				
		}
		else if(TimerPtr[TimerID].mCurValue < TimerPtr[TimerID].StopValue || (s32)TimerPtr[TimerID].mCurValue < 0)//round run
		{
			TimerPtr[TimerID].mCurValue = TimerPtr[TimerID].startValue;
		}
		TagPtr[TimerPtr[TimerID].mTagID].setValue(TimerPtr[TimerID].mCurValue, TimerPtr[TimerID].mTagID);		
	}
	else 
	{
		TimerPtr[TimerID].mCurValue +=  TimerPtr[TimerID].StepValue;
		if( (TimerPtr[TimerID].mCurValue > TimerPtr[TimerID].StopValue) && !(TimerPtr[TimerID].TimerMode & TimerRoundRun) ) 
		{
				xTimerStop(pxTimer,0);
				TimerPtr[TimerID].curTimerStop();
				return;
				//newtagvalue = TimerPtr[TimerID].TimerMode & (~TimerStart); //关闭Timer
				//TagPtr[TimerID*6].setValue(newtagvalue, TimerID*6);
		}
		else if( (TimerPtr[TimerID].mCurValue == TimerPtr[TimerID].StopValue) && !(TimerPtr[TimerID].TimerMode & TimerRoundRun)) //stop
		{
			xTimerStop(pxTimer,0);
			TimerPtr[TimerID].curTimerStop();
		}
		else if(TimerPtr[TimerID].mCurValue > TimerPtr[TimerID].StopValue)//round run
		{
			TimerPtr[TimerID].mCurValue = TimerPtr[TimerID].startValue;
		}		
		TagPtr[TimerPtr[TimerID].mTagID].setValue(TimerPtr[TimerID].mCurValue, TimerPtr[TimerID].mTagID);
	}

	

	//2ms时钟调用动画列表维护任务
	//if(TimerID == 0)
	//	Animation_maintain(2);
}

//-----------------------------
// 函数名： TimerProcessTask
// 执行timer指令队列
// 参数列表：
//    @param1 void*  pvParameters     队列参数
// 备注(各个版本之间的修改):
//-----------------------------
void TimerProcessTask(void* pvParameters)
{
 	BaseType_t xStatus;
	TimerHandle_t pxTimer;
 	for(;;)
 	{
 		xStatus=xQueueReceive(TimerCmdQueue,
			                  (void*)&pxTimer,
							  portMAX_DELAY
			                   );
 		if(xStatus!=pdPASS)
 		{
 			return;
 		}
		TimerProcess(pxTimer);
  		taskYIELD();
 	}
}


//-----------------------------
// 函数名： AnimationTimerProcessTask
// 执行timer指令队列
// 参数列表：
//    @param1 void*  pvParameters     队列参数
// 备注(各个版本之间的修改):
//-----------------------------
void AnimationTimerProcessTask(void* pvParameters)
{
 	BaseType_t xStatus;
	TimerHandle_t pxTimer;
 	for(;;)
 	{
 		xStatus=xQueueReceive(AnimationTimerCmdQueue,
			                  (void*)&pxTimer,
							  portMAX_DELAY
			                   );
 		if(xStatus!=pdPASS)
 		{
 			return;
 		}
		TimerProcess(pxTimer);
  		taskYIELD();
 	}
}




#endif
