////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     DynamicPageClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    animation list create and refresh
//    2016/12/21 modify the structure and add the animation timer
// 
////////////////////////////////////////////////////////////////////////////////
#include "string.h"
#include "publicType.h"
#include "publicInclude.h"
#include "animationDefine.h"
#include "trace.h"

extern u8 animationDuration;
extern u8 animationExist;
extern xTaskHandle MouseTouchTaskHandle;
extern xTaskHandle KeyBoardInputTaskHandle;
extern xTaskHandle InterruptTaskHandle;
extern xTaskHandle WidgetRefreshTaskHandle;
extern xTaskHandle ActionProcessorTaskHandle;
extern xTaskHandle TimerProcessTaskHandle;
extern xTaskHandle AnimationTimerProcessTaskHandle;
extern xTaskHandle TagUpdateTaskHandle;
extern QueueHandle_t   ActionInstructionQueue;
extern  ConfigInfoClass		ConfigData;

#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
extern xTaskHandle switchBufferTaskHandle;
extern xTaskHandle screenUpdateTaskHandle;
#endif


extern DynamicPageClassPtr  gPagePtr;
extern u16             WorkingPageID;

extern QueueHandle_t   TimerCmdQueue;

#ifdef AHMI_CORE

//AnimationMsg  AnimationData[AnimationMsg_Maxnum];///////////消息列表

extern QueueHandle_t  AnimationTaskQueue;///////////////////任务队列，存储的是消息列表的中需要刷新的信息位置信息


//-----------------------------
// 函数名： initAnimationMsg
// 初始化队列
// 参数列表：
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus AnimationClass::initAnimationMsg()
{
	this->pAnimationMsgPtr = (AnimationMsgPtr)pvPortMalloc(sizeof(AnimationMsg)* AnimationMsg_Maxnum);
	if(pAnimationMsgPtr == NULL )
	{
		ERROR_PRINT("animation Class Initial failed.\r\n");
		return AHMI_FUNC_FAILURE;
	}
	
	memset(this->pAnimationMsgPtr, 0 , sizeof(AnimationMsg)* AnimationMsg_Maxnum);
	//initAnimationTimer();
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： initAnimationTimer
// 初始化队列
// 参数列表：
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus AnimationClass::initAnimationTimer(u16 TimerID)
{
#ifdef PC_SIM //in pc, 1 tick implies 10 ms
	aTimer =  xTimerCreate
		          (  /* Just a text name, not used by the RTOS kernel. */
                     "aTimer",
                     /* The timer period in ticks. */
                     10,
                     /* The timers will auto-reload themselves when they expire. */
                     pdTRUE,
                     /* Assign each timer a unique id equal to its array index. */
                     ( void * ) TimerID,
                     /* Each timer calls the same callback when it expires. */
                     TimerCallbackFunc
                   );
#endif
#ifdef EMBEDDED
	aTimer =  xTimerCreate
		          (  /* Just a text name, not used by the RTOS kernel. */
                     "aTimer",
                     /* The timer period in ticks. */
                     10,
                     /* The timers will auto-reload themselves when they expire. */
                     pdTRUE,
                     /* Assign each timer a unique id equal to its array index. */
                     ( void * ) TimerID,
                     /* Each timer calls the same callback when it expires. */
                     TimerCallbackFunc
                   );
#endif
//	xTimerStart(aTimer,0);
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： ahmi::TexRaster(
// 纹理光栅化
// 参数列表：
// @param1 u8 refreshTime      刷新时间
// 备注(各个版本之间的修改):
//   无
//-----------------------------
void AnimationClass::Animation_maintain(u8 refreshTime)
{
	animationTaskQueueMsg i;
#ifdef STATIC_BUFFER_EN
//	RefreshMsg refreshMsg;
#endif
//	u16 j;
//	u32 addr;
	for(i=0;i<AnimationMsg_Maxnum;i++)
	{
	  if(pAnimationMsgPtr[i].RefreshType !=ANIMATION_REFRESH_NULL)
	  {
		if(pAnimationMsgPtr[i].Now_Frame>=pAnimationMsgPtr[i].Total_Frame) //当前动画已经刷完，从列表中删除。
		{

			if(pAnimationMsgPtr[i].RefreshType == ANIMATION_REFRESH_PAGE)
			{
				animationDuration = 0;
//				xQueueReset(TimerCmdQueue);
//				vTaskResume(MouseTouchTaskHandle);
//				vTaskResume(TagUpdateTaskHandle);
//				vTaskResume(KeyBoardInputTaskHandle);
//				vTaskResume(InterruptTaskHandle);
//				vTaskResume(ActionProcessorTaskHandle);
//				vTaskResume(TimerProcessTaskHandle);
//#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
//				vTaskResume(screenUpdateTaskHandle);
//#endif
				//send refresh message 
#ifdef WHOLE_TRIBLE_BUFFER
#endif
				//send the action after animation
				//for(j=0;j != gPagePtr[WorkingPageID].mNumOfPageAction; j++)
				//{
				//	addr = (u32)(gPagePtr[WorkingPageID].pActionStartADDR[j] + gPagePtr[WorkingPageID].pActionInstructions);
				//	xQueueSendToBack(ActionInstructionQueue,&addr,portMAX_DELAY);
				//}
				
			}

			if(pAnimationMsgPtr[i].RefreshType == ANIMATION_REFRESH_PAGE || pAnimationMsgPtr[i].RefreshType == ANIMATION_REFRESH_CANVAS || pAnimationMsgPtr[i].RefreshType == ANIMATION_REFRESH_SUBCANVAS)//widget animation donot need to change animation exist
			{
				if(animationExist == 0)
				{
#ifdef AHMI_DEBUG
					ERROR_PRINT("error: trying to minus animationExist when it is 0");
#endif
				}
				else 
				{
					animationExist --;
#ifdef AHMI_DEBUG
					char text[100];
					sprintf(text,"existing animation number : %d",animationExist);
					ERROR_PRINT(text);
#endif
#ifdef STATIC_BUFFER_EN
					if(animationExist == 0)
					{
					
		#ifdef AHMI_DEBUG
						ERROR_PRINT("sending the refresh static buffer cmd");
		#endif
//						refreshMsg.mElementType = ANIMAITON_REFRESH_STATIC_BUFFER;
//						refreshMsg.mElementPtr.pageptr = gPagePtr+WorkingPageID;
//						sendToRefreshQueue(&refreshMsg);
	
					}
#endif
				}
			}


			pAnimationMsgPtr[i].RefreshType=ANIMATION_REFRESH_NULL;
			


		}
		else if(pAnimationMsgPtr[i].Now_time>=pAnimationMsgPtr[i].Interval_time)//当前画面，要开始画新的一帧。
		{
			animationTaskQueueHandle(i);////列表下标，传入队列中。处理该队列任务为  绘图队列处理任务
			pAnimationMsgPtr[i].Now_time=0;                          //清除，重新计时
		}
		else 
		{
			pAnimationMsgPtr[i].Now_time += refreshTime;
		}
					
	}
 }
}


//-----------------------------
// 函数名： Create_AnimatianData
// 纹理光栅化
// 参数列表：
// @param1 u8 A_RefreshType      元件类型，宏定义见"animationDefine.h"
// @prarm2 ElementPtr A_New_ElementPtr 新绘制元件指针
// @parma3  ElementPtr   A_Old_ElementPtr  旧元件指针
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus AnimationClass::Create_AnimatianData(	
	u8           A_RefreshType,  //元件类型   
	ElementPtr   A_New_ElementPtr, //新元件指针
	ElementPtr   A_Old_ElementPtr  //旧元件指针
						  ) 
{
	u8 i;
	u16 widgetType;
	taskENTER_CRITICAL();
	if(animationDuration == 1)
	{
		taskEXIT_CRITICAL();
		return AHMI_FUNC_SUCCESS;
	}
	if( xTimerIsTimerActive( aTimer ) == pdFALSE )
		xTimerStart(aTimer, 0);

	if(A_RefreshType==ANIMATION_REFRESH_WIDGET)
		{
			//if this is meter widget, the new animation should cover the old one
			widgetType = (A_New_ElementPtr.wptr->WidgetAttr) & 0x1f;
			if( widgetType == METER_TEX)
			{
				for(i=0;i<AnimationMsg_Maxnum;i++) // scan all the animation message
				{
					if(pAnimationMsgPtr[i].RefreshType == A_RefreshType && A_New_ElementPtr.wptr == pAnimationMsgPtr[i].New_ElementPtr.wptr && A_Old_ElementPtr.wptr == pAnimationMsgPtr[i].Old_ElementPtr.wptr) //the same refresh element
					{
						//replace it
						pAnimationMsgPtr[i].RefreshType = ANIMATION_REFRESH_WIDGET;
						pAnimationMsgPtr[i].New_ElementPtr.wptr = A_New_ElementPtr.wptr;
						pAnimationMsgPtr[i].Old_ElementPtr.wptr = A_Old_ElementPtr.wptr ;
						pAnimationMsgPtr[i].Total_Frame = ( (A_New_ElementPtr.wptr->TOTALFRAME_AND_NOWFRAME) >> 8 );
						pAnimationMsgPtr[i].Now_Frame =0;
						pAnimationMsgPtr[i].Now_time = 0;
						pAnimationMsgPtr[i].Interval_time =Animation_Time;
						A_New_ElementPtr.wptr->START_TAG_L = A_New_ElementPtr.wptr->OldValueL;
						A_New_ElementPtr.wptr->START_TAG_H = A_New_ElementPtr.wptr->OldValueH;
						animationTaskQueueHandle(i);////列表下标，传入队列中。开始第1帧绘制
						taskEXIT_CRITICAL();
#ifdef AHMI_DEBUG
						ERROR_PRINT("create canvas animation successfully");
#endif
						return AHMI_FUNC_SUCCESS;//创建成功
					}
				}
			}
		}


	for(i=0;i<AnimationMsg_Maxnum;i++)
	{
	  if(pAnimationMsgPtr[i].RefreshType ==ANIMATION_REFRESH_NULL)
	  {
			 if(A_RefreshType==ANIMATION_REFRESH_PAGE)
				 { //when refreshing page, need to suspend all the other task
					 //SuspendOtherTask();
//					 vTaskSuspend(MouseTouchTaskHandle);
//					 vTaskSuspend(TagUpdateTaskHandle);
//					 vTaskSuspend(KeyBoardInputTaskHandle);
//					 vTaskSuspend(InterruptTaskHandle);
//					 vTaskSuspend(ActionProcessorTaskHandle);
//					 vTaskSuspend(TimerProcessTaskHandle);
//#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
//					 vTaskSuspend(screenUpdateTaskHandle);
//#endif
#ifdef AHMI_DEBUG
				   ERROR_PRINT("create page animation successfully");
#endif
				   //check the data information
				   //null or out of boundary
				   if(A_New_ElementPtr.pageptr == NULL || (A_New_ElementPtr.pageptr - gPagePtr) >=  ConfigData.NumofPages || 
					   A_Old_ElementPtr.pageptr == NULL || (A_Old_ElementPtr.pageptr - gPagePtr) >=  ConfigData.NumofPages)
				   {
					   ERROR_PRINT("error when creating page animation: page pointer null or out of boundary");
					   return AHMI_FUNC_FAILURE;
				   }
					 animationDuration = 1;
					 pAnimationMsgPtr[i].RefreshType = ANIMATION_REFRESH_PAGE;
					 pAnimationMsgPtr[i].New_ElementPtr.pageptr=A_New_ElementPtr.pageptr ;
				     pAnimationMsgPtr[i].Old_ElementPtr.pageptr=A_Old_ElementPtr.pageptr ;
					 pAnimationMsgPtr[i].Total_Frame = pAnimationMsgPtr[i].New_ElementPtr.pageptr->mtotoalFrame;
					 animationExist ++;
				  }
			  else if(A_RefreshType==ANIMATION_REFRESH_CANVAS)
			   { 
				    if(A_New_ElementPtr.cptr == NULL || (A_New_ElementPtr.cptr - gPagePtr[WorkingPageID].pCanvasList) >=  gPagePtr[WorkingPageID].mNumOfCanvas || 
					   A_Old_ElementPtr.cptr == NULL || (A_Old_ElementPtr.cptr - gPagePtr[WorkingPageID].pCanvasList) >=  gPagePtr[WorkingPageID].mNumOfCanvas)
					{
						ERROR_PRINT("error when creating canvas animation: canvas pointer null or out of boundary");
					   return AHMI_FUNC_FAILURE;
					}
#ifdef AHMI_DEBUG
				   ERROR_PRINT("create canvas animation successfully");
#endif
				   pAnimationMsgPtr[i].RefreshType = ANIMATION_REFRESH_CANVAS;
				   pAnimationMsgPtr[i].New_ElementPtr = A_New_ElementPtr;
				   pAnimationMsgPtr[i].Old_ElementPtr = A_Old_ElementPtr ;
				   pAnimationMsgPtr[i].Total_Frame = gPagePtr[WorkingPageID].pAnimationActionList[ A_New_ElementPtr.cptr->curCustomAnimationPtr].mTotalFrame;
				   animationExist ++;
				}
			  else if(A_RefreshType==ANIMATION_REFRESH_SUBCANVAS)
				{ 
					 if(A_New_ElementPtr.scptr == NULL || (A_New_ElementPtr.scptr - gPagePtr[WorkingPageID].pSubCanvasList) >=  gPagePtr[WorkingPageID].mNumOfSubCanvas || 
					   A_Old_ElementPtr.scptr == NULL || (A_Old_ElementPtr.scptr - gPagePtr[WorkingPageID].pSubCanvasList) >=  gPagePtr[WorkingPageID].mNumOfSubCanvas)
					{
						ERROR_PRINT("error when creating subcanvas animation: subcanvas pointer null or out of boundary");
					   return AHMI_FUNC_FAILURE;
					}
#ifdef AHMI_DEBUG
					ERROR_PRINT("create subcanvas animation successfully");
#endif
				   pAnimationMsgPtr[i].RefreshType = ANIMATION_REFRESH_SUBCANVAS;
				   pAnimationMsgPtr[i].New_ElementPtr.scptr = A_New_ElementPtr.scptr;
				   pAnimationMsgPtr[i].Old_ElementPtr.scptr = A_Old_ElementPtr.scptr ;
				   pAnimationMsgPtr[i].Total_Frame = gPagePtr[WorkingPageID].pCanvasList[A_New_ElementPtr.scptr->attachCanvas].subcanvasAnimationFrame;
				   animationExist ++;
				  }
			  else if(A_RefreshType == ANIMATION_REFRESH_WIDGET)
			  {
				   if(A_New_ElementPtr.wptr == NULL || (A_New_ElementPtr.wptr - gPagePtr[WorkingPageID].pWidgetList) >=  gPagePtr[WorkingPageID].mNumOfWidget || 
					   A_Old_ElementPtr.wptr == NULL || (A_Old_ElementPtr.wptr - gPagePtr[WorkingPageID].pWidgetList) >=  gPagePtr[WorkingPageID].mNumOfWidget)
					{
						ERROR_PRINT("error when creating widget animation: widget pointer null or out of boundary");
					   return AHMI_FUNC_FAILURE;
					}
#ifdef AHMI_DEBUG
				ERROR_PRINT("create widget animation successfully");
#endif
				pAnimationMsgPtr[i].RefreshType = ANIMATION_REFRESH_WIDGET;
				pAnimationMsgPtr[i].New_ElementPtr.wptr = A_New_ElementPtr.wptr;
				pAnimationMsgPtr[i].Old_ElementPtr.wptr = A_Old_ElementPtr.wptr ;
				pAnimationMsgPtr[i].Total_Frame = ( (A_New_ElementPtr.wptr->TOTALFRAME_AND_NOWFRAME) >> 8 );
			  }
			  else
			 {
				 taskEXIT_CRITICAL();
				 return AHMI_FUNC_FAILURE; //不是这三种类型，创建失败
			  }

			  pAnimationMsgPtr[i].Now_Frame =0;
			  pAnimationMsgPtr[i].Now_time = 0;
			 
			  pAnimationMsgPtr[i].Interval_time =Animation_Time;

			  animationTaskQueueHandle(i);////列表下标，传入队列中。开始第1帧绘制
		
			  
		taskEXIT_CRITICAL();
		return AHMI_FUNC_SUCCESS;//创建成功
	  }
	}

#ifdef AHMI_DEBUG
		ERROR_PRINT("create animation failure: no enough place for animation member");
#endif
	 taskEXIT_CRITICAL();
	return AHMI_FUNC_FAILURE; //创建失败
}

//-----------------------------
// 函数名： animationTaskQueueHandle
// 动画队列处理任务
// 参数列表：
//     @param1 animationTaskQueueMsg i 队列消息
// 备注(各个版本之间的修改):
//   
//-----------------------------
funcStatus AnimationClass::animationTaskQueueHandle(animationTaskQueueMsg i)
{
	AnimationMsgPtr curAnimationData = &pAnimationMsgPtr[i];
	if(curAnimationData == NULL || i >= AnimationMsg_Maxnum)
	{
		ERROR_PRINT("error in animationTaskQueueHandler: connot read the pointer");
		return AHMI_FUNC_FAILURE;
	}
	curAnimationData->Now_Frame++;


	switch(curAnimationData->RefreshType)
	{
	case(ANIMATION_REFRESH_PAGE): //页面动画
		//set aTag of page
		taskENTER_CRITICAL();
		curAnimationData->New_ElementPtr.pageptr->setATag(curAnimationData->Now_Frame); 
		taskEXIT_CRITICAL();
		break;
	case(ANIMATION_REFRESH_CANVAS): //canvas动画
		taskENTER_CRITICAL();
		curAnimationData->New_ElementPtr.cptr->setATag(curAnimationData->Now_Frame); 
		taskEXIT_CRITICAL();
		break;
	case(ANIMATION_REFRESH_SUBCANVAS): //subcanvas animation
		taskENTER_CRITICAL();
		curAnimationData->New_ElementPtr.scptr->setAtag(curAnimationData->Now_Frame);
		taskEXIT_CRITICAL();
		break;
	case(ANIMATION_REFRESH_WIDGET): //widget动画
		taskENTER_CRITICAL();
		curAnimationData->New_ElementPtr.wptr->setATag(curAnimationData->Now_Frame);
		taskEXIT_CRITICAL();
		break;
	default:
		ERROR_PRINT("error in animationTaskQueueHandler: current type of animation donnot exist");
		return AHMI_FUNC_FAILURE;
	}

	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// 函数名： animationQueueReset
// 动画队列清空
// 参数列表：
//     void
// 备注(各个版本之间的修改):
//   
//-----------------------------
funcStatus AnimationClass::animationQueueReset(void)
{
	u8 i;
	for(i=0;i<AnimationMsg_Maxnum;i++)
	{
		pAnimationMsgPtr[i].RefreshType = ANIMATION_REFRESH_NULL;
	}
	animationDuration = 0;
	animationExist = 0;
	return AHMI_FUNC_SUCCESS;
}

#endif
