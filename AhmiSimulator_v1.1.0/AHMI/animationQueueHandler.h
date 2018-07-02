////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     DynamicPage.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
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
	u8           A_RefreshType,  //Ԫ������   
	ElementPtr   A_New_ElementPtr, //��Ԫ��ָ��
	ElementPtr   A_Old_ElementPtr  //��Ԫ��ָ��
	);
#endif

#endif
