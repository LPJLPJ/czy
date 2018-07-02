////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     animation.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef ANIMATION__H
#define ANIMATION__H
#include "publicType.h"
#include "publicDefine.h"
#include "animationDefine.h"
#include "refreshQueueHandler.h"

#ifdef AHMI_CORE

union ElementPtr;
struct Animation;
typedef void * TimerHandle_t;

class AnimationClass
{
	//变量
private:
	AnimationMsgPtr pAnimationMsgPtr;
	TimerHandle_t   aTimer;   //动画时钟

	//函数
public:
	//-----------------------------
	// 函数名： initAnimationMsg
	// 初始化队列
	// 参数列表：
	// 备注(各个版本之间的修改):
	//   无
	//-----------------------------
	funcStatus initAnimationMsg();


	//-----------------------------
	// 函数名： initAnimationTimer
	// 初始化队列
	// 参数列表：
	// 备注(各个版本之间的修改):
	//   无
	//-----------------------------
	funcStatus initAnimationTimer(u16 TimerID);

	//-----------------------------
	// 函数名： Create_AnimatianData
	// 创建动画列表
	// 参数列表：
	// @param1 u8 A_RefreshType      元件类型，宏定义见"animationDefine.h"
	// @prarm2 ElementPtr A_New_ElementPtr 新绘制元件指针
	// @parma3  ElementPtr   A_Old_ElementPtr  旧元件指针
	// 备注(各个版本之间的修改):
	//   无
	//-----------------------------
	funcStatus Create_AnimatianData(	
		u8           A_RefreshType,    //元件类型   
		ElementPtr   A_New_ElementPtr, //新元件指针
		ElementPtr   A_Old_ElementPtr  //旧元件指针
		); 

	//-----------------------------
	// 函数名： Animation_maintain
	// 维护动画列表
	// 参数列表：
	// @param1 u8 A_RefreshType      元件类型，宏定义见"animationDefine.h"
	// @prarm2 ElementPtr A_New_ElementPtr 新绘制元件指针
	// @parma3  ElementPtr   A_Old_ElementPtr  旧元件指针
	// 备注(各个版本之间的修改):
	//   无
	//-----------------------------
	void Animation_maintain(u8 refreshTime);

	//-----------------------------
	// 函数名： animationTaskQueueHandle
	// 动画队列处理任务
	// 参数列表：
	//     @param1 animationTaskQueueMsg i 队列消息
	// 备注(各个版本之间的修改):
	//   
	//-----------------------------
	funcStatus animationTaskQueueHandle(animationTaskQueueMsg i);

	//-----------------------------
	// 函数名： animationQueueReset
	// 动画队列清空
	// 参数列表：
	//     void
	// 备注(各个版本之间的修改):
	//   
	//-----------------------------
	funcStatus animationQueueReset(void);

};



#endif

#endif


