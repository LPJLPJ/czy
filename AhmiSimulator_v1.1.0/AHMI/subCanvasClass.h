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
	//����
	matrixClass mSubCanvasMatrix; //����
	u16 NumOfWidget;      //
	u16 StartAddrOfWidget;//��ʼ��widget�������±�
	u16 NumofTouchWidget;
	u16 StartAddrOfTouchWidget; //��ʼ��touchwidget�����±�
	u8 attachCanvas;
	u8 startAddrOfSubCanAction; //��ʼ��ָ���ַ
	u8 mATag;
	

	//����
	SubCanvasClass(void);
	~SubCanvasClass(void);

	funcStatus initSubCanvas(void);

	//set animation tag
	funcStatus setAtag(u8 value);
};

typedef SubCanvasClass* SubCanvasClassPtr;

#endif

#endif
