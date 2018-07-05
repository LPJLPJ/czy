////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     DynamicPageClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    definition of subcanvas
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"

#ifdef AHMI_CORE

extern DynamicPageClassPtr  gPagePtr;
extern u16           WorkingPageID;
//extern QueueHandle_t   RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;
extern AnimationClass  gAnimationClass;

SubCanvasClass::SubCanvasClass()
{}

SubCanvasClass::~SubCanvasClass()
{}

funcStatus SubCanvasClass::initSubCanvas(void)
{
	this->mATag = 0;
	this->mSubCanvasMatrix.matrixInit();
	return this->mSubCanvasMatrix.matrixInit();
}

//-----------------------------
// 函数名：  setAtag
// set animation tag
// 参数列表：
// @param1 u8 value  this value is between 0 and totalframe
// 备注(各个版本之间的修改):
// ...
//-----------------------------
funcStatus SubCanvasClass::setAtag(u8 value)
{
	CanvasClassPtr focusedCanvas;
	s16 curSubCanvasOffsetX; //1.11.4 , relative to canvas                                                    
	s16 curSubCanvasOffsetY; //1.11.4
//	u16 scalerX;     //1.6.9
//	u16 scalerY;     //1.6.9
	u16 transitionParamIn; //0.10
	u16 transitionParamOut; //0.10
//	RefreshMsg refreshMsg;

#ifdef AHMI_DEBUG
	char text[100];
	sprintf(text,"set current subcanvas animation tag %d", value);
	ERROR_PRINT(text);
#endif
	
	focusedCanvas = &gPagePtr[WorkingPageID].pCanvasList[attachCanvas];
	if(focusedCanvas == NULL || focusedCanvas->subcanvasAnimationFrame == 0) //no animation
	{
		return AHMI_FUNC_FAILURE;
	}
	if(value == 0 || value > focusedCanvas->subcanvasAnimationFrame) //out of baundary
	{
		ERROR_PRINT("ERROR when setting subcanvas animation tag, value out of baundary");
		return AHMI_FUNC_FAILURE;
	}
	this->mATag = value;
	transitionParamIn = (mATag) * 1024 / focusedCanvas->subcanvasAnimationFrame;//0.10
	gPagePtr[WorkingPageID].inoutTransitioin(transitionParamIn, transitionParamOut);

	switch(focusedCanvas->animationType) 
	{
	case( SUBCANVAS_ANIMATION_SHIFTING_L ): //shift from left

		curSubCanvasOffsetX = (s16)( ( - (s32)focusedCanvas->mwidth ) * 16 + (s32)( (transitionParamOut * focusedCanvas->mwidth ) >> 6 )) ;
		curSubCanvasOffsetY = 0;

		this->mSubCanvasMatrix.A = (1 << 20);
		this->mSubCanvasMatrix.B = (0 << 20);
		this->mSubCanvasMatrix.C = (0 << 20);
		this->mSubCanvasMatrix.D = (1 << 20);
		this->mSubCanvasMatrix.E = -(curSubCanvasOffsetX << 9);
		this->mSubCanvasMatrix.F = -(curSubCanvasOffsetY << 9);

		//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
		refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
		refreshMsg.mElementPtr.pageptr = this;
		sendToRefreshQueue(&refreshMsg);	
#endif

		break;
	case( SUBCANVAS_ANIMATION_SHIFTING_R ): //shift from right

		curSubCanvasOffsetX = (s16) (((s32)focusedCanvas->mwidth ) * 16 - (s32)( (transitionParamOut * focusedCanvas->mwidth ) >> 6 )) ;
		curSubCanvasOffsetY = 0;

		this->mSubCanvasMatrix.A = (1 << 20);
		this->mSubCanvasMatrix.B = (0 << 20);
		this->mSubCanvasMatrix.C = (0 << 20);
		this->mSubCanvasMatrix.D = (1 << 20);
		this->mSubCanvasMatrix.E = -(curSubCanvasOffsetX << 9);
		this->mSubCanvasMatrix.F = -(curSubCanvasOffsetY << 9);
#ifndef WHOLE_TRIBLE_BUFFER
		refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
		refreshMsg.mElementPtr.pageptr = this;
		sendToRefreshQueue(&refreshMsg);	
#endif
		break;
	case( SUBCANVAS_ANIMATION_SHIFTING_UP ): //shift from up

		curSubCanvasOffsetX = 0 ;
		curSubCanvasOffsetY = (s16) (( -(s32)focusedCanvas->mheight ) * 16 + (s32)( (transitionParamOut * focusedCanvas->mheight ) >> 6 ));

		this->mSubCanvasMatrix.A = (1 << 20);
		this->mSubCanvasMatrix.B = (0 << 20);
		this->mSubCanvasMatrix.C = (0 << 20);
		this->mSubCanvasMatrix.D = (1 << 20);
		this->mSubCanvasMatrix.E = -(curSubCanvasOffsetX << 9);
		this->mSubCanvasMatrix.F = -(curSubCanvasOffsetY << 9);
#ifndef WHOLE_TRIBLE_BUFFER
		refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
		refreshMsg.mElementPtr.pageptr = this;
		sendToRefreshQueue(&refreshMsg);	
#endif
		break;
	case( SUBCANVAS_ANIMATION_SHIFTING_DOWN ): //shift from down

		curSubCanvasOffsetX = 0 ;
		curSubCanvasOffsetY = (s16) (( (s32)focusedCanvas->mheight ) * 16 - (s32)( (transitionParamOut * focusedCanvas->mheight ) >> 6 ));

		this->mSubCanvasMatrix.A = (1 << 20);
		this->mSubCanvasMatrix.B = (0 << 20);
		this->mSubCanvasMatrix.C = (0 << 20);
		this->mSubCanvasMatrix.D = (1 << 20);
		this->mSubCanvasMatrix.E = -(curSubCanvasOffsetX << 9);
		this->mSubCanvasMatrix.F = -(curSubCanvasOffsetY << 9);
#ifndef WHOLE_TRIBLE_BUFFER
		refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
		refreshMsg.mElementPtr.pageptr = this;
		sendToRefreshQueue(&refreshMsg);	
#endif
		break;
	case( SUBCANVAS_ANIMATION_ZOOM ): //ZOOM from center

		if(transitionParamOut < 16) //in case that A exceeds the boundary
			transitionParamOut = 16;
		curSubCanvasOffsetX = (s32)(focusedCanvas->mwidth * 16) - (s32)( transitionParamOut * (focusedCanvas->mwidth) / 64 );
		curSubCanvasOffsetY =  (s32)(focusedCanvas->mheight * 16) - (s32)( (transitionParamOut * (focusedCanvas->mheight) ) / 64 ) ;
		this->mSubCanvasMatrix.A = (s32)((0x80000 / transitionParamOut) << 11); //1.6.9
		if(this->mSubCanvasMatrix.A >= 0x100000 )
			this->mSubCanvasMatrix.A = 0xfffff; //the maxium number of the 1.6.9
		else if(this->mSubCanvasMatrix.A == 0)
			this->mSubCanvasMatrix.A = 1;     //the munium number
		this->mSubCanvasMatrix.B = (0 << 20);
		this->mSubCanvasMatrix.C = (0 << 20);
		this->mSubCanvasMatrix.D = this->mSubCanvasMatrix.A;
		this->mSubCanvasMatrix.E = -(curSubCanvasOffsetX << 9);
		this->mSubCanvasMatrix.F = -(curSubCanvasOffsetY << 9);
#ifndef WHOLE_TRIBLE_BUFFER
		refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
		refreshMsg.mElementPtr.pageptr = this;
		sendToRefreshQueue(&refreshMsg);	
#endif
		break;
	default:
#ifdef AHMI_DEBUG
		ERROR_PRINT("current canvas don't support such kind of animation");
#endif
		return AHMI_FUNC_FAILURE;
	}

	return AHMI_FUNC_SUCCESS;
}

#endif
