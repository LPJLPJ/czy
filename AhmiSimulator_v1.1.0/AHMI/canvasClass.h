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
	//变量
	matrixClass mCanvasMatrix;
	u16 mCanvaAttr; //reserved
	u16 mwidth;   //整数
	u16 mheight;  //整数
	s16 moffsetX; //整数
	s16 moffsetY; //整数
	u16 mStartAddrOfSubCanvas;
	u16 mNumOfCanvasAction;
	u16 bindTagID;   //绑定的tag标号
	u8  mCanvasID;   //canvas标号
	u8  mFocusedCanvas; //当前的subcanvas
	u8  mNumOfSubCanvas;
	u8* pCanvasActionStartADDR;
	u8 animationType;                     //子画布切换的动画类型
	u8 subcanvasAnimationFrame;           //子画布切换动画时间
	u8 mATag;                            //animation tag
	u8 startAddrOfCanvasCustomAnimation; // the start address of canvas animation for custom.
	u8 numOfCanvasCustomAnimation;       // the number of the animation
	u8 curCustomAnimationPtr;             // the current runing custom animation, -1 indicating that there is no animation running
	

	//函数
	CanvasClass(void);
	~CanvasClass(void);

	//加载新的子画布并显示动画
	funcStatus loadSubCanvasWithAnimation(
		SubCanvasClassPtr pCurSubCanvas,
		SubCanvasClassPtr pNextSubCanvas
		);

	//加载新的子画布
	funcStatus focusNewSubCanvas(
		u8 subCanvasID
		);

	funcStatus drawCanvas(
		TileBoxClassPtr tileBox , //包围盒
		u32 *sourceShift        , //sourcebuffer指针
		u8 pageEnable           ,  //是否刷新整个页面
		matrixClassPtr pPageMatrix, //附加的矩阵
		SubCanvasClassPtr scptr   , //子画布
		u8 RefreshType            , //刷新类型
		u8 staticTextureEn        //是否绘制到静态存储空间
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
