////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     ActionTriggerClass.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.04 - C++, 20160321 by 于春营
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"
#include "ButtonClass.h"
#include "ClockClass.h"
#include "DynamicTexClass.h"
#include "IOClass.h"
#include "KnobClass.h"
#include "MeterClass.h"
#include "NumberClass.h"
#include "OnOffTexture.h"
#include "OscilloscopeClass.h"
#include "ProgBarClass.h"
#include "SliderClass.h"
#include "TextureNumberClass.h"
#include "TextClass.h"
#include "VideoClass.h"
#include "TextureTimeClass.h"

#ifdef AHMI_DEBUG
#include "trace.h"
#endif

#ifdef AHMI_CORE

extern ActionTriggerClass gWidgetTagTrigger;
extern ActionTriggerClass gMouseTagTrigger;
extern ActionTriggerClass gCanvasTagTrigger;

extern u16            WorkingPageID;
extern DynamicPageClassPtr gPagePtr;
extern QueueHandle_t   ActionInstructionQueue;
extern TagClassPtr     TagPtr;



ActionTriggerClass::ActionTriggerClass()
{}

ActionTriggerClass::~ActionTriggerClass()
{}

//-----------------------------
// 函数名： initActionTriggerClass
// 初始化成员变量
// 参数列表：
//   无
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus ActionTriggerClass::initActionTrigger(){
	mInputType = 0;
	mTagPtr = NULL;
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： MouseTouch
// 根据鼠标事件，决定应该响应的widget
// 参数列表：
// @param1       DynamicPagePtr    Ptr                      当前页面指针
// @param2		 ActionTriggerClass*  TagTrigger               触发信号
// 备注(各个版本之间的修改):
//   只有MOUSETOUCH类型的触发才响应
//-----------------------------
funcStatus ActionTriggerClass::MouseTouch(PIDPoint* pPressPoint,PIDPoint* pReleasePoint)
{
	u16 i,j;
	s16 X, tempX;
	s16 Y, tempY;
	s16 releaseX, releaseY;
	PointClass point(0, 0);
	DynamicPageClassPtr curPage = &gPagePtr[WorkingPageID];
	CanvasClassPtr curCanvas;
	SubCanvasClassPtr curSubCan;
	WidgetClassPtr curWidget;
	funcStatus ahmiStatus;
	if (this->mInputType==ACTION_MOUSE_PRESS || this->mInputType == ACTION_MOUSE_HOLDING)
    {
		if(this->mInputType==ACTION_MOUSE_PRESS)
		{
			X = ((s16)(pPressPoint->x));
			Y = ((s16)(pPressPoint->y));
            this->mMousePID.x = pPressPoint->x;
            this->mMousePID.y = pPressPoint->y;
		}
		else if(this->mInputType == ACTION_MOUSE_HOLDING)
		{
			X = (s16)(pReleasePoint->x);
			Y = (s16)(pReleasePoint->y);
            this->mMousePID.x = pReleasePoint->x;
            this->mMousePID.y = pReleasePoint->y;
		}
		ahmiStatus = AHMI_FUNC_FAILURE;
		//先判断触碰的canvas
		for(i = 0; i < curPage->mNumOfCanvas; i++)
		{
			curCanvas = &(curPage->pCanvasList[i]);
			tempX = X - curCanvas->moffsetX;		//translate the position
			tempY = Y - curCanvas->moffsetY;
			point.mPointX = tempX * 16;
			point.mPointY = tempY * 16;
			point.leftMulMatrix(&(curCanvas->mCanvasMatrix));
			tempX = point.mPointX / 16;
			tempY = point.mPointY / 16;

			tempX += curCanvas->moffsetX;
			tempY += curCanvas->moffsetY;

			/*if( (s16)(curCanvas->moffsetX) <= X &&
				(s16)(curCanvas->moffsetY) <= Y &&
				(s16)(curCanvas->moffsetX + curCanvas->mwidth)  >= X &&
				(s16)(curCanvas->moffsetY + curCanvas->mheight) >= Y )*/
			if( (s16)(curCanvas->moffsetX) <= tempX &&
				(s16)(curCanvas->moffsetY) <= tempY &&
				(s16)(curCanvas->moffsetX + curCanvas->mwidth)  >= tempX &&
				(s16)(curCanvas->moffsetY + curCanvas->mheight) >= tempY 
			)
			{
				curSubCan = &(curPage->pSubCanvasList[curCanvas->mStartAddrOfSubCanvas + curCanvas->mFocusedCanvas]);
				//判断所触碰的widget
				for(j = curSubCan->StartAddrOfTouchWidget; j < (curSubCan->StartAddrOfTouchWidget) + (curSubCan->NumofTouchWidget); j++ )
				{
					curWidget = &curPage->pWidgetList[(curPage->pTouchWidgetList[j])];
					/*if(
						(s16)(curWidget->WidgetOffsetX) <= X &&
						(s16)(curWidget->WidgetOffsetY) <= Y &&
						(s16)(curWidget->WidgetOffsetX + curWidget->WidgetWidth) >= X && 
						(s16)(curWidget->WidgetOffsetY + curWidget->WidgetHeight) >= Y
						)*/
					if(
						(s16)(curWidget->WidgetOffsetX) <= tempX &&
						(s16)(curWidget->WidgetOffsetY) <= tempY &&
						(s16)(curWidget->WidgetOffsetX + curWidget->WidgetWidth)  >= tempX && 
						(s16)(curWidget->WidgetOffsetY + curWidget->WidgetHeight) >= tempY
					)
					{
					    ahmiStatus = this->widgetActionTrigger(curWidget,1);
						if(ahmiStatus == AHMI_FUNC_FAILURE)
							return AHMI_FUNC_FAILURE;
					}
				}
			}
		}
		return AHMI_FUNC_SUCCESS;
	}//end of MOUSE_PRESS

	else if(this->mInputType==ACTION_MOUSE_RELEASE)
	{
		X = (s16)(pPressPoint->x);
		Y = (s16)(pPressPoint->y);
		releaseX = (s16)(pReleasePoint->x);
		releaseY = (s16)(pReleasePoint->y);
		ahmiStatus = AHMI_FUNC_FAILURE;
		//先判断触碰的canvas
		for(i = 0; i < curPage->mNumOfCanvas; i++)
		{
			curCanvas = &(curPage->pCanvasList[i]);

			tempX = X - curCanvas->moffsetX;
			tempY = Y - curCanvas->moffsetY;
			point.mPointX = tempX * 16;
			point.mPointY = tempY * 16;
			point.leftMulMatrix(&(curCanvas->mCanvasMatrix));
			tempX = point.mPointX / 16;
			tempY = point.mPointY / 16;
			tempX += curCanvas->moffsetX;
			tempY += curCanvas->moffsetY;

			if( (s16)(curCanvas->moffsetX) <= tempX &&
				(s16)(curCanvas->moffsetY) <= tempY &&
				(s16)(curCanvas->moffsetX + curCanvas->mwidth)  >= tempX &&
				(s16)(curCanvas->moffsetY + curCanvas->mheight) >= tempY 
			)
			{
				curSubCan = &(curPage->pSubCanvasList[curCanvas->mStartAddrOfSubCanvas + curCanvas->mFocusedCanvas]);
				//判断所触碰的widget
				for(j = curSubCan->StartAddrOfTouchWidget; j < (curSubCan->StartAddrOfTouchWidget) + (curSubCan->NumofTouchWidget); j++ )
				{
					curWidget = &curPage->pWidgetList[(curPage->pTouchWidgetList[j])];
					if(
						(s16)(curWidget->WidgetOffsetX) <= tempX &&
						(s16)(curWidget->WidgetOffsetY) <= tempY &&
						(s16)(curWidget->WidgetOffsetX + curWidget->WidgetWidth)  >= tempX && 
						(s16)(curWidget->WidgetOffsetY + curWidget->WidgetHeight) >= tempY
					) //寻找得到当前控件
					{
						tempX = releaseX - curCanvas->moffsetX;
						tempY = releaseY - curCanvas->moffsetY;
						point.mPointX = tempX * 16;
						point.mPointY = tempY * 16;
						point.leftMulMatrix(&(curCanvas->mCanvasMatrix));
						tempX = point.mPointX / 16;
						tempY = point.mPointY / 16;
						tempX += curCanvas->moffsetX;
						tempY += curCanvas->moffsetY;

						if(
							(s16)(curWidget->WidgetOffsetX) <= tempX &&
							(s16)(curWidget->WidgetOffsetY) <= tempY &&
							(s16)(curWidget->WidgetOffsetX + curWidget->WidgetWidth ) >= tempX && 
							(s16)(curWidget->WidgetOffsetY + curWidget->WidgetHeight) >= tempY
							)//release的坐标仍然在控件内，则正常触发RELEASE
							this->mInputType = ACTION_MOUSE_RELEASE;
						else //否则，认为用户cancle了
							this->mInputType = ACTION_MOUSE_CANCLE;
					    ahmiStatus = this->widgetActionTrigger(curWidget,1);
						if(ahmiStatus == AHMI_FUNC_FAILURE)
							return AHMI_FUNC_FAILURE;
					}
				}
			}
		}
	}//end of MOUSE_RELEASE
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： keyboardTouch
// 根据键盘事件，决定控件的响应
// 参数列表：
// @param1       DynamicPagePtr    Ptr                      当前页面指针
// @param2		 ActionTriggerClass*  TagTrigger               触发信号
// 备注(各个版本之间的修改):
//   只有KEYBOARD类型的触发才响应
//-----------------------------
funcStatus ActionTriggerClass::keyboardTouch()
{
	u8 preWidgetID = 0, nextWidgetID = 0;
	u8 preOfPreWidgetID = 0, nextOfNextWidgetID = 0;
	WidgetClassPtr curEnlightedWidgetClassPtr, preWidgetClassPtr, nextWidgetClassPtr;
	DynamicPageClassPtr curPagePtr = &gPagePtr[WorkingPageID];
	u8 widgetType;
	u8 changedWidgetType;
	u8 button_type;
	ButtonClass widget;
	ClockClass clockWidget;
	TextureTimeClass TextureTimeWidget;
	u8 button_num;
	u8 curEnlighted;
	if (this->mInputType==ACTION_KEYBOARD_NEXT || this->mInputType == ACTION_KEYBOARD_PRE || this->mInputType == ACTION_KEYBOARD_OK || this->mInputType == ACTION_KEYBOARD_CLEAR)
	{
		if( ( (s8)(curPagePtr->curEnlightedWidgetID) ) < 0 ) //没有需要高亮的控件
			return AHMI_FUNC_SUCCESS;
		curEnlightedWidgetClassPtr = &(curPagePtr->pWidgetList[curPagePtr->curEnlightedWidgetID]);
		widgetType = curEnlightedWidgetClassPtr->WidgetAttr & 0x1f;
		this->mTagPtr = &(TagPtr[curEnlightedWidgetClassPtr->BindTagID]);
		if(!(widgetType == BUTTON_TEX || widgetType == CLOCK_TEX || widgetType == TEXTURE_TIME))
			return AHMI_FUNC_FAILURE;
		else 
			curPagePtr->curEnlightedWidgetLighted = 1;
		if(widgetType == BUTTON_TEX)
			button_type = (curEnlightedWidgetClassPtr->WidgetAttr & 0x60) >> 5;
		else if(widgetType == CLOCK_TEX || widgetType == TEXTURE_TIME)
			button_type = COMPLEXBUTTON;

		//如果时钟控件是确认状态则不需要判断直接触发
		if(widgetType == CLOCK_TEX && ( (curEnlightedWidgetClassPtr->WidgetAttr) & 0x80) && this->mInputType != ACTION_KEYBOARD_CLEAR)
		{
			return clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
		}
		if(widgetType == TEXTURE_TIME&& ( (curEnlightedWidgetClassPtr->WidgetAttr) & 0x80) && this->mInputType != ACTION_KEYBOARD_CLEAR)
		{
			return TextureTimeWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
		}
		preWidgetID = (curEnlightedWidgetClassPtr->KeyboardPreAndNextWidget) >> 8;
		nextWidgetID = (u8)(curEnlightedWidgetClassPtr->KeyboardPreAndNextWidget);
		button_num = (u16)(curEnlightedWidgetClassPtr->NumOfButtonAndCurHighLight) >> 8;
		curEnlighted = (u8)(curEnlightedWidgetClassPtr->NumOfButtonAndCurHighLight);
		//向左按动按钮
		if(this->mInputType == ACTION_KEYBOARD_PRE)
		{
			if(button_type == SINGLEBUTTON)//单个按钮
			{
				if(preWidgetID == curPagePtr->curEnlightedWidgetID) //表示最左边的控件按了左
					return AHMI_FUNC_SUCCESS;
				else 
				{
					preWidgetClassPtr = &(curPagePtr->pWidgetList[preWidgetID]);
					while(preWidgetClassPtr->displayEn(preWidgetID) == AHMI_FUNC_FAILURE) //前一个控件不显示，则再往前
					{
						preOfPreWidgetID = (preWidgetClassPtr->KeyboardPreAndNextWidget) >> 8;
						if(preOfPreWidgetID == preWidgetID) //表示再前面没有控件了，证明也是最左边控件按了左
							return AHMI_FUNC_SUCCESS;
						preWidgetID = preOfPreWidgetID;
						preWidgetClassPtr = &(curPagePtr->pWidgetList[preWidgetID]);
					}
					if(widgetType == BUTTON_TEX)
					{
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
						changedWidgetType = preWidgetClassPtr->WidgetAttr & 0x1f;
						if(changedWidgetType == BUTTON_TEX)
							widget.widgetCtrl(preWidgetClassPtr, this, 0);
						else if(changedWidgetType == CLOCK_TEX)
							clockWidget.widgetCtrl(preWidgetClassPtr, this, 0);
						else if(changedWidgetType == TEXTURE_TIME)
							TextureTimeWidget.widgetCtrl(preWidgetClassPtr, this, 0);
					}
					else if(widgetType == CLOCK_TEX)
					{
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
						changedWidgetType = preWidgetClassPtr->WidgetAttr & 0x1f;
						if(changedWidgetType == BUTTON_TEX)
							widget.widgetCtrl(preWidgetClassPtr, this, 0);
						else if(changedWidgetType == CLOCK_TEX)
							clockWidget.widgetCtrl(preWidgetClassPtr, this, 0);
						else if(changedWidgetType == TEXTURE_TIME)
							TextureTimeWidget.widgetCtrl(preWidgetClassPtr, this, 0);
					}
					curPagePtr->curEnlightedWidgetID = preWidgetID;
				}
			}
			else if(button_type == COMPLEXBUTTON) //复杂按钮
			{
				if(preWidgetID == curPagePtr->curEnlightedWidgetID && (curEnlighted == 1 || curEnlighted == 0) ) //表示最左边的控件按了左
					return AHMI_FUNC_SUCCESS;
				else if(preWidgetID == curPagePtr->curEnlightedWidgetID) //最左边控件并且没有到最左边
				{
					if(widgetType == BUTTON_TEX)
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == CLOCK_TEX)
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == TEXTURE_TIME)
						TextureTimeWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
				}
				else if(preWidgetID != curPagePtr->curEnlightedWidgetID && curEnlighted == 1)//左边还有按钮且按了左
				{
					preWidgetClassPtr = &(curPagePtr->pWidgetList[preWidgetID]);
					while(preWidgetClassPtr->displayEn(preWidgetID) == AHMI_FUNC_FAILURE) //前一个控件不显示，则再往前
					{
						preOfPreWidgetID = (preWidgetClassPtr->KeyboardPreAndNextWidget) >> 8;
						if(preOfPreWidgetID == preWidgetID) //表示再前面没有控件了，证明也是最左边控件按了左
							return AHMI_FUNC_SUCCESS;
						preWidgetID = preOfPreWidgetID;
						preWidgetClassPtr = &(curPagePtr->pWidgetList[preWidgetID]);
					}
					if(widgetType == BUTTON_TEX)
					{
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
						changedWidgetType = preWidgetClassPtr->WidgetAttr & 0x1f;
						if(changedWidgetType == BUTTON_TEX)
							widget.widgetCtrl(preWidgetClassPtr, this, 0);
						else if(changedWidgetType == CLOCK_TEX)
							clockWidget.widgetCtrl(preWidgetClassPtr, this, 0);
						else if(changedWidgetType == TEXTURE_TIME)
							TextureTimeWidget.widgetCtrl(preWidgetClassPtr, this, 0);
					}
					else if(widgetType == CLOCK_TEX)
					{
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
						changedWidgetType = preWidgetClassPtr->WidgetAttr & 0x1f;
						if(changedWidgetType == BUTTON_TEX)
							widget.widgetCtrl(preWidgetClassPtr, this, 0);
						else if(changedWidgetType == CLOCK_TEX)
							clockWidget.widgetCtrl(preWidgetClassPtr, this, 0);
						else if(changedWidgetType == TEXTURE_TIME)
							TextureTimeWidget.widgetCtrl(preWidgetClassPtr, this, 0);
					}
					curPagePtr->curEnlightedWidgetID = preWidgetID;
				}
				else  //内部移动
				{
					preWidgetClassPtr = &(curPagePtr->pWidgetList[preWidgetID]);
					if(widgetType == BUTTON_TEX)
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == CLOCK_TEX)
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == TEXTURE_TIME)
						TextureTimeWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
				}
			}
			else
				return AHMI_FUNC_FAILURE;
		}//end of keboard left
		//向右按动按钮
		else if(this->mInputType == ACTION_KEYBOARD_NEXT)
		{
			if(button_type == SINGLEBUTTON)//单个按钮
			{
				if(nextWidgetID == curPagePtr->curEnlightedWidgetID && curEnlighted == button_num) //表示最右边的控件按了右
					return AHMI_FUNC_SUCCESS;
				else if(curEnlighted == 0)//表示初始状态
				{
					while(curEnlightedWidgetClassPtr->displayEn(curPagePtr->curEnlightedWidgetID) == AHMI_FUNC_FAILURE) //第一个按钮不显示，继续寻找下一个
					{
						nextOfNextWidgetID = (curEnlightedWidgetClassPtr->KeyboardPreAndNextWidget) & 0xff ;
						if(nextOfNextWidgetID == curPagePtr->curEnlightedWidgetID) //表示到达最后一个
							return AHMI_FUNC_SUCCESS;
						curPagePtr->curEnlightedWidgetID = nextOfNextWidgetID;
						curEnlightedWidgetClassPtr =  &(curPagePtr->pWidgetList[curPagePtr->curEnlightedWidgetID]);
					}
					if(widgetType == BUTTON_TEX)
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == CLOCK_TEX)
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == TEXTURE_TIME)
						TextureTimeWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
				}
				else 
				{
					nextWidgetClassPtr = &(curPagePtr->pWidgetList[nextWidgetID]);
					while(nextWidgetClassPtr->displayEn(nextWidgetID) == AHMI_FUNC_FAILURE) //下一个控件不显示
					{
						nextOfNextWidgetID = (nextWidgetClassPtr->KeyboardPreAndNextWidget) & 0xff ;
						if(nextOfNextWidgetID == nextWidgetID) //表示到达最后一个
							return AHMI_FUNC_SUCCESS;
						nextWidgetID = nextOfNextWidgetID;
						nextWidgetClassPtr =  &(curPagePtr->pWidgetList[nextWidgetID]);
					}
					if(widgetType == BUTTON_TEX)
					{
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
						changedWidgetType = nextWidgetClassPtr->WidgetAttr & 0x1f;
						if(changedWidgetType == BUTTON_TEX)
							widget.widgetCtrl(nextWidgetClassPtr, this, 0);
						else if(changedWidgetType == CLOCK_TEX)
							clockWidget.widgetCtrl(nextWidgetClassPtr, this, 0);
						else if(changedWidgetType == TEXTURE_TIME)
							TextureTimeWidget.widgetCtrl(nextWidgetClassPtr, this, 0);
					}
					else if(widgetType == CLOCK_TEX)
					{
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
						changedWidgetType = nextWidgetClassPtr->WidgetAttr & 0x1f;
						if(changedWidgetType == BUTTON_TEX)
							widget.widgetCtrl(nextWidgetClassPtr, this, 0);
						else if(changedWidgetType == CLOCK_TEX)
							clockWidget.widgetCtrl(nextWidgetClassPtr, this, 0);
						else if(changedWidgetType == TEXTURE_TIME)
							TextureTimeWidget.widgetCtrl(nextWidgetClassPtr, this, 0);
					}
					curPagePtr->curEnlightedWidgetID = nextWidgetID;
				}
			}
			else if(button_type == COMPLEXBUTTON) //复杂按钮
			{
				if(nextWidgetID == curPagePtr->curEnlightedWidgetID && (curEnlighted == button_num) ) //表示最右边的控件按了右
					return AHMI_FUNC_SUCCESS;
				else if(nextWidgetID == curPagePtr->curEnlightedWidgetID && (curEnlighted == 0)) //初始化
				{
					while(curEnlightedWidgetClassPtr->displayEn(curPagePtr->curEnlightedWidgetID) == AHMI_FUNC_FAILURE) //第一个按钮不显示，继续寻找下一个
					{
						nextOfNextWidgetID = (curEnlightedWidgetClassPtr->KeyboardPreAndNextWidget) & 0xff ;
						if(nextOfNextWidgetID == curPagePtr->curEnlightedWidgetID) //表示到达最后一个
							return AHMI_FUNC_SUCCESS;
						curPagePtr->curEnlightedWidgetID = nextOfNextWidgetID;
						curEnlightedWidgetClassPtr =  &(curPagePtr->pWidgetList[curPagePtr->curEnlightedWidgetID]);
					}
					if(widgetType == BUTTON_TEX)
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == CLOCK_TEX)
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == TEXTURE_TIME)
						TextureTimeWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
				}
				else if(nextWidgetID == curPagePtr->curEnlightedWidgetID) //控件内移动
				{
					if(widgetType == BUTTON_TEX)
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == CLOCK_TEX)
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == TEXTURE_TIME)
						TextureTimeWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
				}
				else if(nextWidgetID != curPagePtr->curEnlightedWidgetID && curEnlighted == button_num) //右边还有按钮且按了右
				{
					nextWidgetClassPtr = &(curPagePtr->pWidgetList[nextWidgetID]);
					while(nextWidgetClassPtr->displayEn(nextWidgetID) == AHMI_FUNC_FAILURE) //下一个控件不显示
					{
						nextOfNextWidgetID = (nextWidgetClassPtr->KeyboardPreAndNextWidget) & 0xff ;
						if(nextOfNextWidgetID == nextWidgetID) //表示到达最后一个
							return AHMI_FUNC_SUCCESS;
						nextWidgetID = nextOfNextWidgetID;
						nextWidgetClassPtr =  &(curPagePtr->pWidgetList[nextWidgetID]);
					}
					if(widgetType == BUTTON_TEX)
					{
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
						changedWidgetType = nextWidgetClassPtr->WidgetAttr & 0x1f;
						if(changedWidgetType == BUTTON_TEX)
							widget.widgetCtrl(nextWidgetClassPtr, this, 0);
						else if(changedWidgetType == CLOCK_TEX)
							clockWidget.widgetCtrl(nextWidgetClassPtr, this, 0);
						else if(changedWidgetType == TEXTURE_TIME)
							TextureTimeWidget.widgetCtrl(nextWidgetClassPtr, this, 0);
					}
					else if(widgetType == CLOCK_TEX)
					{
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
						changedWidgetType = nextWidgetClassPtr->WidgetAttr & 0x1f;
						if(changedWidgetType == BUTTON_TEX)
							widget.widgetCtrl(nextWidgetClassPtr, this, 0);
						else if(changedWidgetType == CLOCK_TEX)
							clockWidget.widgetCtrl(nextWidgetClassPtr, this, 0);
						else if(changedWidgetType == TEXTURE_TIME)
							TextureTimeWidget.widgetCtrl(nextWidgetClassPtr, this, 0);
					}
					curPagePtr->curEnlightedWidgetID = nextWidgetID;
				}
				else //内部移动
				{
					nextWidgetClassPtr = &(curPagePtr->pWidgetList[nextWidgetID]);
					if(widgetType == BUTTON_TEX)
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == CLOCK_TEX)
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == TEXTURE_TIME)
						TextureTimeWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
				}
			}
			else
				return AHMI_FUNC_FAILURE;
		}//end of keyboard right
		//回车
		else if(this->mInputType == ACTION_KEYBOARD_OK)
		{
			if(widgetType == BUTTON_TEX)
				widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
			else if(widgetType == CLOCK_TEX)
				clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
			else if(widgetType == TEXTURE_TIME)
				TextureTimeWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
		}//end of keyboard entered
		//clear
		else if(this->mInputType == ACTION_KEYBOARD_CLEAR)
		{
			//clear the highlight of the widget
			if(widgetType == BUTTON_TEX)
				widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
			else if(widgetType == CLOCK_TEX)
				clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
			else if(widgetType == TEXTURE_TIME)
				TextureTimeWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
			//reset the highlight pointer
			preWidgetID = (curEnlightedWidgetClassPtr->KeyboardPreAndNextWidget) >> 8;
			while(preWidgetID != curPagePtr->curEnlightedWidgetID)
			{
				curPagePtr->curEnlightedWidgetID = preWidgetID;
				curEnlightedWidgetClassPtr = &(curPagePtr->pWidgetList[curPagePtr->curEnlightedWidgetID]);
				preWidgetID = (curEnlightedWidgetClassPtr->KeyboardPreAndNextWidget) / 256;
			}
			curPagePtr->curEnlightedWidgetID = preWidgetID;

		}
		//defaule
		else 
		{
#ifdef AHMI_DEBUG
			ERROR_PRINT("no such kind of keyboard trigger!");
			return AHMI_FUNC_FAILURE;
#endif
		}
	}
	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// 函数名： PageActionTrigger
// 触发页面指令
// 参数列表：
// @param1 WidgetClassPtr pWidgetPtr widget指针
// @param2 ActionTriggerClassPtr pTagtrigger 触发指针
// 备注(各个版本之间的修改):
//    
//-----------------------------
funcStatus ActionTriggerClass::widgetActionTrigger(
		 WidgetClassPtr pWidgetPtr,
		 u8 u8_widgetRefresh    //是否需要刷新控件,1表示刷新，added by zuz 20160622
		 )
{
	u16 widgetType=(pWidgetPtr->WidgetAttr) & 0x1f;
	TagClassPtr bindTag = &TagPtr[pWidgetPtr->BindTagID];
	u32 value = bindTag->getValue();
	//u32 value;
	u8 u8_pageRefresh = (u8_widgetRefresh == 1) ? 0 : 1;

	this->mTagPtr = bindTag;

	if(widgetType == BUTTON_TEX)
	{
		ButtonClass button;
		return button.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == DYNAMIC_TEX)
	{
		DynamicTexClass widget;
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == METER_TEX)
	{
		MeterClass widget ;
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == PROGBAR_TEX)
	{
		ProgBarClass widget;
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == SLIDER_TEX)
	{
		SliderClass widget;
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == OSCSCP_TEX)
	{
		OSCClass widget;
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == TEXTBOX_TEX)
	{
		TextClass widget;
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == NUMBOX_TEX)
	{
		NumberClass widget ;
		if(u8_widgetRefresh == 0) //indicating that this widget runs in the background
		{
			pWidgetPtr->CurValueL = (u16)value;
			pWidgetPtr->CurValueH = (u16)(value >> 16);
		}
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == VIDEO_TEX)
	{
		VideoClass widget;
		return widget.widgetCtrl(pWidgetPtr, 0, u8_pageRefresh);
	}
	else if(widgetType == KNOB_TEX)
	{
		KnobClass widget;
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == CLOCK_TEX)
	{
		ClockClass widget;
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == ON_OFF_TEX)
	{
		OnOffTextureClass widget;
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == TEXTURE_NUM)
	{
		TextureNumberClass widget;
		if(u8_widgetRefresh == 0) //indicating that this widget runs in the background
		{
			pWidgetPtr->CurValueL = (u16)value;
			pWidgetPtr->CurValueH = (u16)(value >> 16);
		}
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else if(widgetType == TEXTURE_TIME)
	{
		TextureTimeClass widget;
		return widget.widgetCtrl(pWidgetPtr, this, u8_pageRefresh);
	}
	else
		return AHMI_FUNC_FAILURE;
	
}

#endif
