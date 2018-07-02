////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     tileBoxClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Revision 2.00 - File Modified 2016/04/24 by 于春营
// Additional Comments:
//    function of widget
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"
#include "ButtonClass.h"
#include "ClockClass.h"
#include "KnobClass.h"
#include "MeterClass.h"
#include "NumberClass.h"
#include "OscilloscopeClass.h"
#include "ProgBarClass.h"
#include "SliderClass.h"
#include "TextClass.h"
#include "VideoClass.h"
#include "OnOffTexture.h"
#include "DynamicTexClass.h"
#include "TextureNumberClass.h"
#include "TextureTimeClass.h"
#include "trace.h"
#include "drawImmediately_cd.h"
#include "refreshQueueHandler.h"
#ifdef EMBEDDED
#include "ahmi3_function.h"
#endif


#ifdef AHMI_CORE

extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;
extern u16             WorkingPageID;
extern QueueHandle_t   ActionInstructionQueue;
extern AnimationClass  gAnimationClass;
extern u8       sourceBuffer[2048];
extern	u8 addr_w;

WidgetClass::WidgetClass(void)
{
}


WidgetClass::~WidgetClass(void)
{
}


funcStatus WidgetClass::drawWidget(
		TileBoxClassPtr pTileBox,
		u32 *sourceShift,
		u8  pageEnable ,
		matrixClassPtr pCanvasMatrix,
		u8 RefreshType,        //绘制的动画类型，根据动画类型改变绘制控件的包围盒
		u8 staticTextureEn          //是否绘制到静态存储空间
		)
{
	//funcStatus stuatus;
	//s16 drawingTexLeftBox   = ( ((s16)(this->WidgetOffsetX) )>>5);
	//s16 drawingTexRightBox  = ( (( (s16)( (s16)(this->WidgetOffsetX) + this->WidgetWidth ) )>>5) );
	//s16 drawingTexTopBox    = (( (s16) (this->WidgetOffsetY)   )>>5);
	//s16 drawingTexButtomBox = ( (( (s16)( (s16)(this->WidgetOffsetY) + this->WidgetHeight) )>>5) );
	u8 widgetType = this->WidgetAttr  & 0x1f;
	funcStatus ahmiStatus = AHMI_FUNC_FAILURE;
	TileBoxClass widgetTileBox;
	ElementPtr   curPtr;
	//matrixClass curWidgetMatrix;

	if( NULL == pTileBox || NULL == sourceShift || NULL == pCanvasMatrix ){
		ERROR_PRINT("ERROR: data been null pointer");
		return AHMI_FUNC_FAILURE;
	}
	this->mWidgetMatrix.matrixInit();
	this->mWidgetMatrix.matrixMulti(pCanvasMatrix);
	//curWidgetMatrix.matrixInit();
	//curWidgetMatrix.matrixMulti(&(this->mWidgetMatrix));
	//curWidgetMatrix.matrixMulti(pCanvasMatrix);
	curPtr.wptr = this;
	

	//判断包围盒
	//if(drawingTexLeftBox < 0)
	//	drawingTexLeftBox = 0;
	//if(drawingTexTopBox < 0)
	//	drawingTexTopBox = 0;

	//compute tilebox
	widgetTileBox.sourceReCompute(curPtr, ANIMATION_REFRESH_WIDGET, &(this->mWidgetMatrix));

	//compute whether it is overlapped
	//if(
	//	pTileBox->LeftBox   > drawingTexRightBox     ||
	//	pTileBox->TopBox    > drawingTexButtomBox    ||
	//	pTileBox->RightBox  < drawingTexLeftBox	     ||
	//	pTileBox->ButtomBox < drawingTexTopBox
	//)
	if(widgetTileBox.sourceBoxOverlap(pTileBox) != AHMI_FUNC_SUCCESS)
	{
		//if(RefreshType == ANIMATION_REFRESH_PAGE)
		//	this->mWidgetMatrix.matrixInit();
		return AHMI_FUNC_SUCCESS;
	}

#ifdef EMBEDDED	
	u8 *sourcebufferaddr;
	sourcebufferaddr = (u8 *)SoureBufferAddr;
	TextureClass newTexture;
	matrixClass tempMatrix;
	tempMatrix.matrixInit();

	if((96 * (this->NumOfTex)) >= (SRCBUFSIZE - *sourceShift))
	{
		*((u8 *)SoureBufferAddr + (*sourceShift)++) = ENDFLAG;
		AHMIDraw(sourceShift);

		newTexture.TexAddr = START_ADDR_OF_RAM + START_ADDR_OF_DISPLAY + addr_w * SIZE_OF_DISPLAY_BUFFER;
		newTexture.OffsetX = 0 * 16;
		newTexture.OffsetY = 0 * 16;
		newTexture.FocusedSlice = 0;
		newTexture.RotateAngle = 0 * 16;
		newTexture.ScalerX = 1 * 512;
		newTexture.ScalerY = 1 * 512;
		newTexture.ShearAngleX = 0 * 16;
		newTexture.SingleSliceSize = SCREEN_WIDTH * SCREEN_HEIGHT * 2;
		newTexture.mTexAttr = NONMASKTEX | EFMATRIX | ADDRTEXTURE | RGBA8888 | DRAWING;
		newTexture.TexWidth = SCREEN_WIDTH;
		newTexture.TexHeight = SCREEN_HEIGHT;
		newTexture.TexLeftTileBox = 0;
		newTexture.TexRightTileBox = TILE_NUM_X;
		newTexture.TexTopTileBox = 0;
		newTexture.TexButtomTileBox = TILE_NUM_Y;

		newTexture.writeSourceBuffer(sourceShift, &(tempMatrix), this,NULL);

	}
#endif

	//all texture use the widget texture together
	if(widgetType == BUTTON_TEX)
	{
		ButtonClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable,RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == DYNAMIC_TEX)
	{
		DynamicTexClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable,RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == METER_TEX)
	{
		MeterClass widget ;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable,RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == PROGBAR_TEX)
	{
		ProgBarClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable,RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == SLIDER_TEX)
	{
		SliderClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable,RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == OSCSCP_TEX)
	{
		OSCClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable, RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
		//示波器比较特殊
		if(pageEnable)
			return AHMI_FUNC_SUCCESS;
	}
	else if(widgetType == TEXTBOX_TEX)
	{
		TextClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable, RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == NUMBOX_TEX)
	{
		NumberClass widget ;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable, RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == VIDEO_TEX)
	{
		VideoClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable, RefreshType , &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == KNOB_TEX)
	{
		KnobClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable,RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == CLOCK_TEX)
	{
		ClockClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable, RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == ON_OFF_TEX)
	{
		OnOffTextureClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable,RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == TEXTURE_NUM)
	{
		TextureNumberClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable,RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else if(widgetType == TEXTURE_TIME)
	{
		TextureTimeClass widget;
		ahmiStatus=widget.initWidget(this, sourceShift, pageEnable,RefreshType, &widgetTileBox,staticTextureEn);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
	}
	else 
		return AHMI_FUNC_FAILURE;
	//if(RefreshType == ANIMATION_REFRESH_PAGE || RefreshType == ANIMATION_REFRESH_CANVAS || ANIMATION_REFRESH_WIDGET)
	this->mWidgetMatrix.matrixInit();
	return AHMI_FUNC_SUCCESS;
	
}

funcStatus WidgetClass::drawWidgetWithAnimation(
		u8 curFrame,     //当前帧
		u8 totalFrame,   //总共帧
		TileBoxClassPtr pSourceBox,
		u32 *sourceshift    //sourcebuffer偏移
		)
{
	matrixClass matrixTemp;
//	s16 curWidgetOffsetX;	 //1.11.4
//	s16 curWidgetOffsetY;	 //1.11.4
//	u8 animationType = this->ANIMATION_TYPE & 0xff;
//	u8 animationParam = (this->ANIMATION_TYPE >> 8) & 0xff; 
//	u32 actionAddr;
//	double scaler;
//#ifdef AHMI_DEBUG
//	char text[100];
//#endif // DEBUG
	//if(totalFrame == 0 || animationType == NO_ANIMATION)
	//{
		matrixTemp.matrixInit();
		return gPagePtr[WorkingPageID].drawPage(pSourceBox,sourceshift,0,ANIMATION_REFRESH_NULL,0);
//	}
	//需要绘制动画
//	else
//	{
//		this->mWidgetMatrix.matrixInit();
//		matrixTemp.matrixInit();
//		switch(animationType)
//		{
//			case(ANIMATION_HERIZONTAL_SHIFTING_L):
//				//水平平移动画
//				curWidgetOffsetX = (s16)( this->WidgetOffsetX + curFrame * this->ENDPOS_X /totalFrame) << 4 ;
//				curWidgetOffsetY = (this->WidgetOffsetY) << 4;
//				matrixTemp.A = (1 << 9);
//				matrixTemp.B = (0 << 9);
//				matrixTemp.C = (0 << 9);
//				matrixTemp.D = (1 << 9);
//				matrixTemp.E = -curWidgetOffsetX;
//				matrixTemp.F = -curWidgetOffsetY;
//
//				#ifdef DEBUG
//				sprintf(text,"Drawing widget animation frame %d\n",curFrame);
//				ERROR_PRINT(text);
//				sprintf(text,"cur widget offset %d",(curWidgetOffsetX >> 4));
//				ERROR_PRINT(text);
//				#endif // DEBUG
//
//				//调用绘图函数
//				this->mWidgetMatrix.matrixMulti(&matrixTemp);
//				if(gPagePtr[WorkingPageID].drawPage(pSourceBox,sourceshift,0,ANIMATION_REFRESH_WIDGET) == AHMI_FUNC_FAILURE)
//					return AHMI_FUNC_FAILURE;
//				break;
//				
//			case(ANIMATION_VERTICAL_SHIFTING_DOWN):
//				//向下平移
//				curWidgetOffsetX = 0 ;
//				curWidgetOffsetY = (s16)(0 + curFrame * 16.0 * this->ENDPOS_Y /totalFrame);
//				matrixTemp.A = (1 << 9);
//				matrixTemp.B = (0 << 9);
//				matrixTemp.C = (0 << 9);
//				matrixTemp.D = (1 << 9);
//				matrixTemp.E = -curWidgetOffsetX;
//				matrixTemp.F = -curWidgetOffsetY;
//
//				#ifdef DEBUG
//				sprintf(text,"Drawing widget animation frame %d\n",curFrame);
//				ERROR_PRINT(text);
//				sprintf(text,"cur widget offset %d",(curWidgetOffsetX >> 4));
//				ERROR_PRINT(text);
//				#endif // DEBUG
//
//				//调用绘图函数
//				this->mWidgetMatrix.matrixMulti(&matrixTemp);
//				if(gPagePtr[WorkingPageID].drawPage(pSourceBox,sourceshift,0,ANIMATION_REFRESH_WIDGET) == AHMI_FUNC_FAILURE)
//					return AHMI_FUNC_FAILURE;
//				break;
//
//				case(ANIMATION_VERTICAL_SHIFTING_UP):
//				//向上平移
//				curWidgetOffsetX = 0 << 4 ;
//				curWidgetOffsetY = (s16)( 0 - curFrame * 16.0 * this->ENDPOS_Y /totalFrame);
//				matrixTemp.A = (1 << 9);
//				matrixTemp.B = (0 << 9);
//				matrixTemp.C = (0 << 9);
//				matrixTemp.D = (1 << 9);
//				matrixTemp.E = -curWidgetOffsetX;
//				matrixTemp.F = -curWidgetOffsetY;
//
//				#ifdef DEBUG
//				sprintf(text,"Drawing widget animation frame %d\n",curFrame);
//				ERROR_PRINT(text);
//				sprintf(text,"cur widget offset %d",(curWidgetOffsetX >> 4));
//				ERROR_PRINT(text);
//				#endif // DEBUG
//
//				//调用绘图函数
//				this->mWidgetMatrix.matrixMulti(&matrixTemp);
//				if(gPagePtr[WorkingPageID].drawPage(pSourceBox,sourceshift,0,ANIMATION_REFRESH_WIDGET) == AHMI_FUNC_FAILURE)
//					return AHMI_FUNC_FAILURE;
//				break;
//
//			case(ANIMATION_ZOOM_IN):
//				//左上角缩小动画
//				matrixTemp.A = (s32)((1 << 9)+(s32)(animationParam-1) * 512.0 * curFrame / (totalFrame+1) );
//				matrixTemp.B = (0 << 9);
//				matrixTemp.C = (0 << 9);
//				matrixTemp.D = (s32)((1 << 9)+(s32)(animationParam-1) * 512.0 * curFrame / (totalFrame+1) );
//				matrixTemp.E = 0;
//				matrixTemp.F = 0;
//				//调用绘图函数
//				if(curFrame == totalFrame)
//				{
//					if(this->drawWidget(pSourceBox, sourceshift, 1, &matrixTemp,ANIMATION_REFRESH_WIDGET) == AHMI_FUNC_FAILURE)
//						return AHMI_FUNC_FAILURE;
//				}
//				else
//				{
//					//调用绘图函数
//					this->mWidgetMatrix.matrixMulti(&matrixTemp);
//					if(gPagePtr[WorkingPageID].drawPage(pSourceBox,sourceshift,0,ANIMATION_REFRESH_WIDGET) == AHMI_FUNC_FAILURE)
//						return AHMI_FUNC_FAILURE;
//				}
//				break;
//
//
//			case(ANIMATION_ZOOM_OUT):
//				//左上角放大动画
//				scaler = (float)(1.0 + ((curFrame + 1) * 1.0 * (animationParam - 1)) / totalFrame);
//				matrixTemp.A = (s32)(512.0 / scaler );
//				matrixTemp.B = (0 << 9);
//				matrixTemp.C = (0 << 9);
//				matrixTemp.D = matrixTemp.A;
//				matrixTemp.E = (s32)(this->WidgetOffsetX - this->ENDPOS_X) * 16 * curFrame / totalFrame;
//				matrixTemp.F = (s32)(this->WidgetOffsetY - this->ENDPOS_Y) * 16 * curFrame / totalFrame;
//				//调用绘图函数
//				if(curFrame == totalFrame)
//				{
//					if(this->drawWidget(pSourceBox, sourceshift, 1, &matrixTemp, ANIMATION_REFRESH_WIDGET) == AHMI_FUNC_FAILURE)
//						return AHMI_FUNC_FAILURE;
//				}
//				else
//				{
//					//调用绘图函数
//					this->mWidgetMatrix.matrixMulti(&matrixTemp);
//					if(gPagePtr[WorkingPageID].drawPage(pSourceBox,sourceshift,0,0) == AHMI_FUNC_FAILURE)
//						return AHMI_FUNC_FAILURE;
//				}
//				break;
//			default:
//				return AHMI_FUNC_FAILURE;
//
//		}
//	}
//
//	//发送指令
//	if(curFrame == totalFrame)
//	{
//		if(this->OnRealeaseAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
//					{
//						actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + this->OnRealeaseAction);
//						xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
//					}
//	}

	return AHMI_FUNC_SUCCESS;
}

funcStatus WidgetClass::initWidget(void)
{
	return this->mWidgetMatrix.matrixInit();
}

//创建控件动画的列表
//由于控件不存在切换，所以不需要传递当前元件和下一元件
funcStatus WidgetClass::loadWidgetWithAnimation(
		)
{
	funcStatus AHMI_status;
	ElementPtr pOldElement, pNewElement;



	pOldElement.wptr = this;
	pNewElement.wptr = this;

	AHMI_status= gAnimationClass.Create_AnimatianData(
		ANIMATION_REFRESH_WIDGET,
		pNewElement,
		pOldElement
		);

	if(AHMI_status == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;

	return AHMI_FUNC_SUCCESS;
}

funcStatus WidgetClass::drawTextureWithAnimation(
		u8 curFrame,     //当前帧
		u8 totalFrame,   //总共帧
		TileBoxClassPtr pSourceBox,
		u32 *sourceshift    //sourcebuffer偏移
		)
{
	matrixClass matrixTemp;
//	s16 curWidgetOffsetX;	 //1.11.4
//	s16 curWidgetOffsetY;	 //1.11.4
//	u8 animationType = this->ANIMATION_TYPE & 0xff;
//	u8 animationParam = (this->ANIMATION_TYPE >> 8) & 0xff; 
//	double scaler;
//#ifdef DEBUG
//	char text[100];
//#endif // DEBUG
//	if(totalFrame == 0 || animationType == NO_ANIMATION)
//	{
//		matrixTemp.matrixInit();
//		return gPagePtr[WorkingPageID].drawPage(pSourceBox,sourceshift,0,ANIMATION_REFRESH_NULL);
//	}
//	//需要绘制动画
//	else
//	{
//		this->mWidgetMatrix.matrixInit();
//		matrixTemp.matrixInit();
//		switch(animationType)
//		{
//			case(ANIMATION_HERIZONTAL_SHIFTING_L):
//				//水平平移动画
//				curWidgetOffsetX = (s16)( this->WidgetOffsetX + curFrame * this->ENDPOS_X /totalFrame) << 4 ;
//				curWidgetOffsetY = (this->WidgetOffsetY) << 4;
//				matrixTemp.A = (1 << 9);
//				matrixTemp.B = (0 << 9);
//				matrixTemp.C = (0 << 9);
//				matrixTemp.D = (1 << 9);
//				matrixTemp.E = -curWidgetOffsetX;
//				matrixTemp.F = -curWidgetOffsetY;
//
//				#ifdef DEBUG
//				sprintf(text,"Drawing widget animation frame %d\n",curFrame);
//				ERROR_PRINT(text);
//				sprintf(text,"cur widget offset %d",(curWidgetOffsetX >> 4));
//				ERROR_PRINT(text);
//				#endif // DEBUG
//
//				//调用绘图函数
//				this->mWidgetMatrix.matrixMulti(&matrixTemp);
////				if(gPagePtr[WorkingPageID].drawPage(pSourceBox,sourceshift,0,ANIMATION_REFRESH_TEXTURE) == AHMI_FUNC_FAILURE)
////					return AHMI_FUNC_FAILURE;
//				break;
//				
//			case(ANIMATION_VERTICAL_SHIFTING_DOWN):
//				//向下平移
//				curWidgetOffsetX = 0 ;
//				curWidgetOffsetY = (s16)(0 + curFrame * 16.0 * this->ENDPOS_Y /totalFrame);
//				matrixTemp.A = (1 << 9);
//				matrixTemp.B = (0 << 9);
//				matrixTemp.C = (0 << 9);
//				matrixTemp.D = (1 << 9);
//				matrixTemp.E = -curWidgetOffsetX;
//				matrixTemp.F = -curWidgetOffsetY;
//
//				#ifdef DEBUG
//				sprintf(text,"Drawing widget animation frame %d\n",curFrame);
//				ERROR_PRINT(text);
//				sprintf(text,"cur widget offset %d",(curWidgetOffsetX >> 4));
//				ERROR_PRINT(text);
//				#endif // DEBUG
//
//				//调用绘图函数
//				this->mWidgetMatrix.matrixMulti(&matrixTemp);
////				if(gPagePtr[WorkingPageID].drawPage(pSourceBox,sourceshift,0,ANIMATION_REFRESH_TEXTURE) == AHMI_FUNC_FAILURE)
////					return AHMI_FUNC_FAILURE;
//				break;
//
//				case(ANIMATION_VERTICAL_SHIFTING_UP):
//				//向上平移
//				curWidgetOffsetX = 0 << 4 ;
//				curWidgetOffsetY = (s16)( 0 - curFrame * 16.0 * this->ENDPOS_Y /totalFrame);
//				matrixTemp.A = (1 << 9);
//				matrixTemp.B = (0 << 9);
//				matrixTemp.C = (0 << 9);
//				matrixTemp.D = (1 << 9);
//				matrixTemp.E = -curWidgetOffsetX;
//				matrixTemp.F = -curWidgetOffsetY;
//
//				#ifdef DEBUG
//				sprintf(text,"Drawing widget animation frame %d\n",curFrame);
//				ERROR_PRINT(text);
//				sprintf(text,"cur widget offset %d",(curWidgetOffsetX >> 4));
//				ERROR_PRINT(text);
//				#endif // DEBUG
//
//				//调用绘图函数
//				this->mWidgetMatrix.matrixMulti(&matrixTemp);
////				if(gPagePtr[WorkingPageID].drawPage(pSourceBox,sourceshift,0,ANIMATION_REFRESH_TEXTURE) == AHMI_FUNC_FAILURE)
////					return AHMI_FUNC_FAILURE;
//				break;
//
//			case(ANIMATION_ZOOM_IN):
//				//左上角缩小动画
//				matrixTemp.A = (s32)((1 << 9)+(s32)(animationParam-1) * 512.0 * curFrame / (totalFrame+1) );
//				matrixTemp.B = (0 << 9);
//				matrixTemp.C = (0 << 9);
//				matrixTemp.D = (s32)((1 << 9)+(s32)(animationParam-1) * 512.0 * curFrame / (totalFrame+1) );
//				matrixTemp.E = 0;
//				matrixTemp.F = 0;
//				//调用绘图函数
//				if(curFrame == totalFrame)
//				{
////					if(this->drawWidget(pSourceBox, sourceshift, 1, &matrixTemp, ANIMATION_REFRESH_TEXTURE) == AHMI_FUNC_FAILURE)
////						return AHMI_FUNC_FAILURE;
//				}
//				else
//				{
////					if(this->drawWidget(pSourceBox, sourceshift, 0, &matrixTemp, ANIMATION_REFRESH_TEXTURE) == AHMI_FUNC_FAILURE)
////						return AHMI_FUNC_FAILURE;
//				}
//				break;
//			case(ANIMATION_ZOOM_OUT):
//				//左上角放大动画
//				scaler = 1.0 + ((curFrame + 1) * 1.0 * (animationParam - 1)) / totalFrame;
//				matrixTemp.A = (s32)(512.0 / scaler );
//				matrixTemp.B = (0 << 9);
//				matrixTemp.C = (0 << 9);
//				matrixTemp.D = matrixTemp.A;
//				matrixTemp.E = (this->WidgetOffsetX - this->ENDPOS_X) * 16 * curFrame / totalFrame;
//				matrixTemp.F = (this->WidgetOffsetY - this->ENDPOS_Y) * 16 * curFrame / totalFrame;
//				//调用绘图函数
//				if(curFrame == totalFrame)
//				{
////					if(this->drawWidget(pSourceBox, sourceshift, 1, &matrixTemp, ANIMATION_REFRESH_TEXTURE) == AHMI_FUNC_FAILURE)
////						return AHMI_FUNC_FAILURE;
//				}
//				else
//				{
////					if(this->drawWidget(pSourceBox, sourceshift, 0, &matrixTemp, ANIMATION_REFRESH_TEXTURE) == AHMI_FUNC_FAILURE)
////						return AHMI_FUNC_FAILURE;
//				}
//				break;
//			default:
//				return AHMI_FUNC_FAILURE;
//		}
//	}
	return AHMI_FUNC_FAILURE;
}

//创建纹理动画的列表
//由于控件不存在切换，所以不需要传递当前元件和下一元件
funcStatus WidgetClass::loadTextureWithAnimation(
		)
{
	funcStatus AHMI_status;
	ElementPtr pOldElement, pNewElement;

	pOldElement.wptr = this;
	pNewElement.wptr = this;

	AHMI_status= gAnimationClass.Create_AnimatianData(
		ANIMATION_REFRESH_WIDGET,
		pNewElement,
		pOldElement
		);

	if(AHMI_status == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;

	return AHMI_FUNC_SUCCESS;
}

//判断widget是否被显示出来
funcStatus WidgetClass::displayEn(
	u8 widgetID
		)
{
	CanvasClassPtr attatchCanvas = &(gPagePtr[WorkingPageID].pCanvasList[this->ATTATCH_CANVAS]);
	SubCanvasClassPtr displayingSubcanvas = &(gPagePtr[WorkingPageID].pSubCanvasList[(attatchCanvas->mStartAddrOfSubCanvas) +  (attatchCanvas->mFocusedCanvas) ]);
	if( (widgetID >= (displayingSubcanvas->StartAddrOfWidget) ) && (widgetID < (displayingSubcanvas->StartAddrOfWidget) + (displayingSubcanvas->NumOfWidget) ))
		return AHMI_FUNC_SUCCESS;
	else 
		return AHMI_FUNC_FAILURE;
}

//-----------------------------
// 函数名： setATag
// set the tag of widget
// 参数列表：
//   @param value
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus WidgetClass::setATag(
	u8 value
	)
{
	u16 widgetType=(this->WidgetAttr) & 0x1f;
	u8  widgetTotalFrame;
	widgetTotalFrame = this->TOTALFRAME_AND_NOWFRAME >> 8;
	if(widgetTotalFrame == 0)
	{
#ifdef AHMI_DEBUG
		ERROR_PRINT("current widget contains no animation");
#endif
		return AHMI_FUNC_FAILURE;
	}
	if(value == 0 || value > widgetTotalFrame)
	{
		ERROR_PRINT("ERROR in setting widget animation tag: value out of baundary");
		return AHMI_FUNC_FAILURE;
	}
	
	if(widgetType == NUMBOX_TEX)
	{
		NumberClass widget;
		return widget.setATag(this,value);
	}
	else if(widgetType == METER_TEX)
	{
		MeterClass widget;
		return widget.setATag(this, value);
	}
	else if(widgetType == PROGBAR_TEX)
	{
		ProgBarClass widget;
		return widget.setATag(this,value);
	}
	else if(widgetType == TEXTURE_NUM)
	{
		TextureNumberClass widget;
		return widget.setATag(this,value);
	}
	else 
	{
		ERROR_PRINT("current type of widget don't support animation");
		return AHMI_FUNC_FAILURE;
	}
}


#endif
