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

#ifndef CANVASCLASS__H
#define CANVASCLASS__H

#include "publicType.h"
#include "publicDefine.h"
#include "canvasClass_c1.h"

#ifdef AHMI_CORE

class matrixClass;

class CanvasClass
{
public:
	//����
	matrixClass mCanvasMatrix;
	u16 mCanvaAttr; //reserved
	u16 mwidth;   //����
	u16 mheight;  //����
	s16 moffsetX; //����
	s16 moffsetY; //����
	u16 mStartAddrOfSubCanvas;
	u16 mNumOfCanvasAction;
	u16 bindTagID;   //�󶨵�tag���
	u8  mCanvasID;   //canvas���
	u8  mFocusedCanvas; //��ǰ��subcanvas
	u8  mNumOfSubCanvas;
	u8* pCanvasActionStartADDR;
	u8 animationType;                     //�ӻ����л��Ķ�������
	u8 subcanvasAnimationFrame;           //�ӻ����л�����ʱ��
	u8 mATag;                            //animation tag
	u8 startAddrOfCanvasCustomAnimation; // the start address of canvas animation for custom.
	u8 numOfCanvasCustomAnimation;       // the number of the animation
	u8 curCustomAnimationPtr;             // the current runing custom animation, -1 indicating that there is no animation running
	

	//����
	CanvasClass(void);
	~CanvasClass(void);

	//�����µ��ӻ�������ʾ����
	funcStatus loadSubCanvasWithAnimation(
		SubCanvasClassPtr pCurSubCanvas,
		SubCanvasClassPtr pNextSubCanvas
		);

	//�����µ��ӻ���
	funcStatus focusNewSubCanvas(
		u8 subCanvasID
		);

	funcStatus drawCanvas(
		TileBoxClassPtr tileBox , //��Χ��
		u32 *sourceShift        , //sourcebufferָ��
		u8 pageEnable           ,  //�Ƿ�ˢ������ҳ��
		matrixClassPtr pPageMatrix, //���ӵľ���
		SubCanvasClassPtr scptr   , //�ӻ���
		u8 RefreshType            , //ˢ������
		u8 staticTextureEn        //�Ƿ���Ƶ���̬�洢�ռ�
		);

	funcStatus drawCanvasWithAnimation(
		TileBoxClassPtr tileBox,
		u32* pSourceShift
		);

	funcStatus initCanvas(void);

	//set canvas custom animation tag
	funcStatus setATag(u8 value);

	//compute the matrix of canvas
	funcStatus computeCanvasMatrix(matrixClassPtr pPageMatrix, matrixClassPtr curCanvasMatrix);

	//trigger canvas custom animation
	funcStatus triggerAnimation(u8 animationID);

};

#endif

#endif
