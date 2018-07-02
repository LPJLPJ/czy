////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     DynamicPageClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/12/27 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    animation action
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicType.h"
#include "publicInclude.h"
#include "animationDefine.h"
#include "trace.h"


extern AnimationClass  gAnimationClass;

//-----------------------------
// 函数名： triggerAnimation
// trigger the element's current animation
// 参数列表：
//  @param1 ElementPtr curElementPtr 组件指针
//  @param2 u8 elememtType           组件类型
// 备注(各个版本之间的修改):
// 无
//-----------------------------
funcStatus AnimationActionClass::triggerAnimation(ElementPtr curElementPtr, u8 elememtType) //trigger the element's current animation{
{
	if(elememtType == ANIMATION_REFRESH_CANVAS) //canvas action
	{
		curElementPtr.cptr->curCustomAnimationPtr = mAnimationID;
		return gAnimationClass.Create_AnimatianData(ANIMATION_REFRESH_CANVAS, curElementPtr, curElementPtr);
	}
	else 
		return AHMI_FUNC_FAILURE;
}
