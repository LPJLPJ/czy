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
#ifndef ANIMATION_ACTION__H
#define ANIMATION_ACTION__H

#include "publicType.h"
//#include "refreshQueueHandler.h"
#include "animationDefine.h"
#include "trace.h"

union ElementPtr;

class AnimationActionClass
{
public:
	s16 mStartX;                   //start point x 11.4
	s16 mStartY;                   //start point y 11.4
	s16 mStopX;					   //stop point x  11.4
	s16 mStopY;                    //stop point y  11.4
	u16 mWidthBeforeAnimation;     //width before  6.10
	u16 mHeightBeforeAnimation;    //height before 6.10
	u16 mWidthAfterAnimation;      //width after   6.10
	u16 mHeightAfterAnimation;     //height after  6.10
	u8  mTotalFrame;               //animation frame
	u8  mAnimationDirection;       //direction, 0 is add and 1 is minus
	u8  mAnimationID ;             //animation ID, only when tag and id is correct will the animation be triggered

	funcStatus triggerAnimation(ElementPtr curElementPtr, u8 elememtType); //trigger the element's current animation
};

#endif