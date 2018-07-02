////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:     TimerClass.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
//          2.01 - �޸�Timer�󶨵�Tag��� by XT 20151120
// Revision 2.04 - C++, 20160321 by �ڴ�Ӫ
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
// �������� InitTimer
// ��ʼ��timer
// �����б�
//   @param1 u16 TimerID  timer�ı��
// ��ע(�����汾֮����޸�):
//   ��
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
	mTagID = NUM_OF_SYSTEM_TAG/*TagID��8�ſ�ʼ��0��tag�����κ����� by XT 20151120*/ 
		         + TimerID;//����timerֻ��һ��tag
	//TagPtr[mTagID].bindTimer(TimerID);
}

//-----------------------------
// �������� TimerSetValue
// ��timer��ĳ��������ֵ
// �����б�
//   @param1 TimerClassPtr Ptr timerָ��
//   @param2 u8  ID            ���
//   @param3 u16 v             ���õĲ���ֵ
// ��ע(�����汾֮����޸�):
//   ��
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
		xTimerStart(hTimer,0);//����Timer
		TagPtr[mTagID].setValue(mCurValue, mTagID);
	}
	else if (ID == 0 && ((v & TimerStart) ==0))//stop timer
	{
		xTimerStop(hTimer,0);
	}
}

//-----------------------------
// �������� curTimerStop
// ��timer��ĳ��������ֵ
// �����б�
//   @param1 TimerClassPtr Ptr timerָ��
//   @param2 u8  ID            ���
//   @param3 u16 v             ���õĲ���ֵ
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
void TimerClass::curTimerStop()
{
	this->TimerMode &= ~TimerStart;
}


//-----------------------------
// �������� TimerCallbackFunc
// timer�ص�����
// �����б�
//   @param1 TimerHandle_t pxTimer  timer�ľ�� 
// ��ע(�����汾֮����޸�):
//   ��
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
// �������� TimerProcess
// ִ��timer�Ĳ����仯
// �����б�
//    @param1 TimerHandle_t  pxTimer     timer���
// ��ע(�����汾֮����޸�):
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

	TimerID = (u32 )pvTimerGetTimerID( pxTimer );//��ȡTimer ID
	
	if(TimerID == ConfigData.NumofTimers) //indicate that this is animation timer
	{
		gAnimationClass.Animation_maintain(4);
		return;
	}

	pCurTimer = &TimerPtr[TimerID]; //find current timer
	pBindingTag = &TagPtr[pCurTimer->mTagID]; //find current tag

	TimerAdd = TimerPtr[TimerID].TimerMode & TimerDecrement;
	if(TimerAdd == TimerDecrement)//timer�Լ�
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

	
	if(TimerAdd == TimerDecrement)//timer�Լ�
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
				//newtagvalue = TimerPtr[TimerID].TimerMode & (~TimerStart); //�ر�Timer
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
				//newtagvalue = TimerPtr[TimerID].TimerMode & (~TimerStart); //�ر�Timer
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

	

	//2msʱ�ӵ��ö����б�ά������
	//if(TimerID == 0)
	//	Animation_maintain(2);
}

//-----------------------------
// �������� TimerProcessTask
// ִ��timerָ�����
// �����б�
//    @param1 void*  pvParameters     ���в���
// ��ע(�����汾֮����޸�):
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
// �������� AnimationTimerProcessTask
// ִ��timerָ�����
// �����б�
//    @param1 void*  pvParameters     ���в���
// ��ע(�����汾֮����޸�):
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
