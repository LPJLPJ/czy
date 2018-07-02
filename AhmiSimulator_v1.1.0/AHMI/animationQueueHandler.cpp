////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     DynamicPageClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
// Additional Comments:
//    handle the animation queue
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "animationDefine.h"

#ifdef AHMI_CORE

extern QueueHandle_t  AnimationTaskQueue;///////////////////������У��洢������Ϣ�б������Ҫˢ�µ���Ϣλ����Ϣ
//extern AnimationMsg  AnimationData[AnimationMsg_Maxnum];///////////��Ϣ�б�
extern QueueHandle_t   RefreshQueueWithoutDoubleBuffer;
extern AnimationClass  gAnimationClass;


//-----------------------------
// �������� sendToAnimationQueue
// ��������Ϣ���͵���������
// �����б�
// ��ע(�����汾֮����޸�):
//   
//-----------------------------
funcStatus sendToAnimationQueue(
	u8           A_RefreshType,  //Ԫ������   
	ElementPtr   A_New_ElementPtr, //��Ԫ��ָ��
	ElementPtr   A_Old_ElementPtr  //��Ԫ��ָ��
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
// �������� animationQueueTask
// �������д�������
// �����б�
// ��ע(�����汾֮����޸�):
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
