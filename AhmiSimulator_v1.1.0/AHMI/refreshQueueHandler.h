////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     drawImmediately_cd.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
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

typedef struct Animation    //////////信息列表的结构体
{
	u8           RefreshType;    //元件类型   
	ElementPtr   New_ElementPtr; //新元件指针
	ElementPtr   Old_ElementPtr; //旧元件指针
	u8           Now_Frame;          //当前帧数 
	u8           Total_Frame;    //总帧数
	u8           Now_time  ;      //当前时间
	u8           Interval_time;   //帧刷新时间间隔
}AnimationMsg;

typedef AnimationMsg* AnimationMsgPtr;

typedef struct Refresh    //////////信息列表的结构体
{
	u8           mElementType;  //元件类型   
	ElementPtr   mElementPtr;	//元件指针
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
