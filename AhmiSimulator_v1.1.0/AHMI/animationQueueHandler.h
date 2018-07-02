////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     DynamicPage.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    task to handle the animation queue
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef ANIMATIONQUEUEHANDLER__H
#define ANIMATIONQUEUEHANDLER__H
#include "publicDefine.h"
#ifdef AHMI_CORE
void animationQueueTask(void*);
funcStatus sendToAnimationQueue(
	u8           A_RefreshType,  //元件类型   
	ElementPtr   A_New_ElementPtr, //新元件指针
	ElementPtr   A_Old_ElementPtr  //旧元件指针
	);
#endif

#endif
