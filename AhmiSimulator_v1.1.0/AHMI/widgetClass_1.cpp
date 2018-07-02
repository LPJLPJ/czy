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
#include "DynamicTexClass.h"
#include "trace.h"

#ifdef AHMI_CORE

extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;


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
		matrixClassPtr pCanvasMatrix
		)
{
	//funcStatus stuatus;
	u8 drawingTexLeftBox   = ((this->WidgetOffsetX  )>>5);
	u8 drawingTexRightBox  = ( ((this->WidgetOffsetX + this->WidgetWidth + TILESIZE - 1)>>5) - 1 );
	u8 drawingTexTopBox    = ((this->WidgetOffsetY   )>>5);
	u8 drawingTexButtomBox = ( ((this->WidgetOffsetY + this->WidgetHeight + TILESIZE - 1)>>5) - 1);
	u8 widgetType = this->WidgetAttr  & 0x1f;
	funcStatus ahmiStatus;
	WidgetClassInterface* widgetPtr;
	this->mWidgetMatrix.matrixMulti(pCanvasMatrix);
	
	//判断包围盒
	if(
		pTileBox->LeftBox   > drawingTexRightBox     ||
		pTileBox->TopBox    > drawingTexButtomBox    ||
		pTileBox->RightBox  < drawingTexLeftBox	  ||
		pTileBox->ButtomBox < drawingTexTopBox
	)
	{
		return AHMI_FUNC_SUCCESS;
	}

	if(widgetType == BUTTON_TEX)
	{
		widgetPtr = new ButtonClass();
		ahmiStatus=widgetPtr->initWidget(this, sourceShift, pageEnable, pTileBox);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			delete widgetPtr;
			return AHMI_FUNC_FAILURE;
		}
		delete widgetPtr;
	}
	else if(widgetType == DYNAMIC_TEX)
	{
		widgetPtr = new DynamicTexClass();
		ahmiStatus=widgetPtr->initWidget(this, sourceShift, pageEnable, pTileBox);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			delete widgetPtr;
			return AHMI_FUNC_FAILURE;
		}
		delete widgetPtr;
	}
	else if(widgetType == METER_TEX)
	{
		widgetPtr = new MeterClass();
		ahmiStatus=widgetPtr->initWidget(this, sourceShift, pageEnable, pTileBox);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			delete widgetPtr;
			return AHMI_FUNC_FAILURE;
		}
		delete widgetPtr;
	}
	else if(widgetType == PROGBAR_TEX)
	{
		widgetPtr = new ProgBarClass();
		ahmiStatus=widgetPtr->initWidget(this, sourceShift, pageEnable, pTileBox);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			delete widgetPtr;
			return AHMI_FUNC_FAILURE;
		}
		delete widgetPtr;
	}
	else if(widgetType == SLIDER_TEX)
	{
		widgetPtr = new SliderClass();
		ahmiStatus=widgetPtr->initWidget(this, sourceShift, pageEnable, pTileBox);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			delete widgetPtr;
			return AHMI_FUNC_FAILURE;
		}
		delete widgetPtr;
	}
	else if(widgetType == OSCSCP_TEX)
	{
		widgetPtr = new OSCClass();
		ahmiStatus=widgetPtr->initWidget(this, sourceShift, pageEnable, pTileBox);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			delete widgetPtr;
			return AHMI_FUNC_FAILURE;
		}
		delete widgetPtr;
	}
	else if(widgetType == TEXTBOX_TEX)
	{
		widgetPtr = new TextClass();
		ahmiStatus=widgetPtr->initWidget(this, sourceShift, pageEnable, pTileBox);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			delete widgetPtr;
			return AHMI_FUNC_FAILURE;
		}
		delete widgetPtr;
	}
	else if(widgetType == NUMBOX_TEX)
	{
		widgetPtr = new NumberClass();
		ahmiStatus=widgetPtr->initWidget(this, sourceShift, pageEnable, pTileBox);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			delete widgetPtr;
			return AHMI_FUNC_FAILURE;
		}
		delete widgetPtr;
	}
	else if(widgetType == VIDEO_TEX)
	{
		widgetPtr = new VideoClass();
		ahmiStatus=widgetPtr->initWidget(this, sourceShift, pageEnable, pTileBox);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			delete widgetPtr;
			return AHMI_FUNC_FAILURE;
		}
		delete widgetPtr;
	}
	else if(widgetType == KNOB_TEX)
	{
		widgetPtr = new KnobClass();
		ahmiStatus=widgetPtr->initWidget(this, sourceShift, pageEnable, pTileBox);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			delete widgetPtr;
			return AHMI_FUNC_FAILURE;
		}
		delete widgetPtr;
	}
	else if(widgetType == CLOCK_TEX)
	{
		widgetPtr = new ClockClass();
		ahmiStatus=widgetPtr->initWidget(this, sourceShift, pageEnable, pTileBox);
		if(ahmiStatus == AHMI_FUNC_FAILURE)
		{
			delete widgetPtr;
			return AHMI_FUNC_FAILURE;
		}
		delete widgetPtr;
	}
	else 
		return AHMI_FUNC_FAILURE;
	this->mWidgetMatrix.matrixInit();
	return AHMI_FUNC_SUCCESS;
	
}

funcStatus WidgetClass::drawWidgetWithAnimation(
		u8 curFrame,     //当前帧
		u8 totalFrame,   //总共帧
		u8 animationType, //刷新动画类型
		TileBoxClassPtr pSourceBox,
		u32 *sourceshift    //sourcebuffer偏移
		)
{
	matrixClass matrixTemp;
	if(totalFrame == 0)
	{
		matrixTemp.matrixInit();
		drawWidget(pSourceBox, sourceshift, 0, &matrixTemp);
	}

	return AHMI_FUNC_SUCCESS;
}

funcStatus WidgetClass::initWidget(void)
{
	return this->mWidgetMatrix.matrixInit();
}

#endif