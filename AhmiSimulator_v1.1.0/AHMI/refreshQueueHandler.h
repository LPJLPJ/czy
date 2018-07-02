////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     drawImmediately_cd.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicType.h"
#include "refreshQueueHandler_c1.h"
#include "refreshQueueHandler_cd.h"

#ifndef REFRESHQUEUEHANDLER__H
#define REFRESHQUEUEHANDLER__H

#ifdef __cplusplus
 extern "C" {
#endif

#include "publicDefine.h"
#ifdef AHMI_CORE
union ElementPtr
{
    DynamicPageClassPtr pageptr;
	WidgetClassPtr      wptr;
	CanvasClassPtr      cptr; 
	SubCanvasClassPtr   scptr;
	TextureClassPtr     tptr;  
};

typedef struct Animation    //////////��Ϣ�б�Ľṹ��
{
	u8           RefreshType;    //Ԫ������   
	ElementPtr   New_ElementPtr; //��Ԫ��ָ��
	ElementPtr   Old_ElementPtr; //��Ԫ��ָ��
	u8           Now_Frame;          //��ǰ֡�� 
	u8           Total_Frame;    //��֡��
	u8           Now_time  ;      //��ǰʱ��
	u8           Interval_time;   //֡ˢ��ʱ����
}AnimationMsg;

typedef AnimationMsg* AnimationMsgPtr;

typedef struct Refresh    //////////��Ϣ�б�Ľṹ��
{
	u8           mElementType;  //Ԫ������   
	ElementPtr   mElementPtr;	//Ԫ��ָ��
}RefreshMsg;

typedef RefreshMsg* RefreshMsgPtr;

typedef u8 animationTaskQueueMsg;

//funcStatus sendToRefreshQueue(AnimationMsg* pAnimationMsg);
funcStatus sendToRefreshQueue(RefreshMsg* refreshMsg);
funcStatus computingTileBoxFromAnimationMsg(AnimationMsg* pAnimationMsg, TileBoxClassPtr SourceBox);

funcStatus drawimmediately(RefreshMsg*);
funcStatus refreshDrawingTileBox(RefreshMsg* refreshMsg, TileBoxClassPtr pSourceBox);

void WidgetRefreshTask(void* pvParameters);

void AHMI_draw(u32 * sourceshift);

funcStatus judgeDoubleBuffer(u32* sourceshift, u8* stm32ptraddr, u8* doubleBufferEn);
#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
void swtichDispBufferTask(void* pvParameters);
#endif
#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
void screenUpdateTask(void* pvParameters);
#endif

#endif

#ifdef __cplusplus
 }
#endif

#endif
