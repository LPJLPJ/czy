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
//    handle the animation queue
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "animationDefine.h"

#ifdef AHMI_CORE

extern QueueHandle_t  AnimationTaskQueue;///////////////////任务队列，存储的是消息列表的中需要刷新的信息位置信息
//extern AnimationMsg  AnimationData[AnimationMsg_Maxnum];///////////消息列表
extern QueueHandle_t   RefreshQueueWithoutDoubleBuffer;
extern AnimationClass  gAnimationClass;


//-----------------------------
// 函数名： sendToAnimationQueue
// 将动画信息发送到动画队列
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
funcStatus sendToAnimationQueue(
	u8           A_RefreshType,  //元件类型   
	ElementPtr   A_New_ElementPtr, //新元件指针
	ElementPtr   A_Old_ElementPtr  //旧元件指针
	)
{
	AnimationMsg curAnimationMsg;
	curAnimationMsg.RefreshType = A_RefreshType;
	curAnimationMsg.New_ElementPtr = A_New_ElementPtr;
	curAnimationMsg.Old_ElementPtr = A_Old_ElementPtr;
	//xQueueSendToBack(AnimationTaskQueue,&curAnimationMsg,portMAX_DELAY);
	gAnimationClass.Create_AnimatianData(curAnimationMsg.RefreshType, curAnimationMsg.New_ElementPtr, curAnimationMsg.Old_ElementPtr);
	return AHMI_FUNC_SUCCESS;
}
//-----------------------------
// 函数名： animationQueueTask
// 动画队列处理任务
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void animationQueueTask(void* pvParameters)
{
	AnimationMsg curAnimationMsg;
	BaseType_t xStatus;
	xStatus=xQueueReceive(AnimationTaskQueue,
 								(void*)&curAnimationMsg,
 								portMAX_DELAY);
 	if(xStatus!=pdPASS)
 	{
 		return;
 	}
	gAnimationClass.Create_AnimatianData(curAnimationMsg.RefreshType, curAnimationMsg.New_ElementPtr, curAnimationMsg.Old_ElementPtr);
}


#endif
