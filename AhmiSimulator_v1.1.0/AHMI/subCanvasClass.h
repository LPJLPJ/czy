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
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef SUBCANVASCLASS__H
#define SUBCANVASCLASS__H

#include "publicType.h"
#include "publicDefine.h"

#ifdef AHMI_CORE

class CanvasClass;
class  matrixClass;
typedef CanvasClass* CanvasClassPtr;

class SubCanvasClass
{
public:
	//变量
	matrixClass mSubCanvasMatrix; //矩阵
	u16 NumOfWidget;      //
	u16 StartAddrOfWidget;//起始的widget的数组下标
	u16 NumofTouchWidget;
	u16 StartAddrOfTouchWidget; //起始的touchwidget数组下标
	u8 attachCanvas;
	u8 startAddrOfSubCanAction; //起始的指令地址
	u8 mATag;
	

	//函数
	SubCanvasClass(void);
	~SubCanvasClass(void);

	funcStatus initSubCanvas(void);

	//set animation tag
	funcStatus setAtag(u8 value);
};

typedef SubCanvasClass* SubCanvasClassPtr;

#endif

#endif
