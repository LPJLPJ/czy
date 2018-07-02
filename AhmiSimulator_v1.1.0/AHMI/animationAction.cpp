////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     DynamicPageClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/12/27 by Zhou Yuzhi(������)
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
// �������� triggerAnimation
// trigger the element's current animation
// �����б�
//  @param1 ElementPtr curElementPtr ���ָ��
//  @param2 u8 elememtType           �������
// ��ע(�����汾֮����޸�):
// ��
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
