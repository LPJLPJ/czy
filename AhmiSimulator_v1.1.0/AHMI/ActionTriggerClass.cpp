////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:     ActionTriggerClass.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.04 - C++, 20160321 by �ڴ�Ӫ
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
// �������� initActionTriggerClass
// ��ʼ����Ա����
// �����б�
//   ��
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus ActionTriggerClass::initActionTrigger(){
	mInputType = 0;
	mTagPtr = NULL;
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// �������� MouseTouch
// ��������¼�������Ӧ����Ӧ��widget
// �����б�
// @param1       DynamicPagePtr    Ptr                      ��ǰҳ��ָ��
// @param2		 ActionTriggerClass*  TagTrigger               �����ź�
// ��ע(�����汾֮����޸�):
//   ֻ��MOUSETOUCH���͵Ĵ�������Ӧ
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
		//���жϴ�����canvas
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
				//�ж���������widget
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
		//���жϴ�����canvas
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
				//�ж���������widget
				for(j = curSubCan->StartAddrOfTouchWidget; j < (curSubCan->StartAddrOfTouchWidget) + (curSubCan->NumofTouchWidget); j++ )
				{
					curWidget = &curPage->pWidgetList[(curPage->pTouchWidgetList[j])];
					if(
						(s16)(curWidget->WidgetOffsetX) <= tempX &&
						(s16)(curWidget->WidgetOffsetY) <= tempY &&
						(s16)(curWidget->WidgetOffsetX + curWidget->WidgetWidth)  >= tempX && 
						(s16)(curWidget->WidgetOffsetY + curWidget->WidgetHeight) >= tempY
					) //Ѱ�ҵõ���ǰ�ؼ�
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
							)//release��������Ȼ�ڿؼ��ڣ�����������RELEASE
							this->mInputType = ACTION_MOUSE_RELEASE;
						else //������Ϊ�û�cancle��
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
// �������� keyboardTouch
// ���ݼ����¼��������ؼ�����Ӧ
// �����б�
// @param1       DynamicPagePtr    Ptr                      ��ǰҳ��ָ��
// @param2		 ActionTriggerClass*  TagTrigger               �����ź�
// ��ע(�����汾֮����޸�):
//   ֻ��KEYBOARD���͵Ĵ�������Ӧ
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
		if( ( (s8)(curPagePtr->curEnlightedWidgetID) ) < 0 ) //û����Ҫ�����Ŀؼ�
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

		//���ʱ�ӿؼ���ȷ��״̬����Ҫ�ж�ֱ�Ӵ���
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
		//���󰴶���ť
		if(this->mInputType == ACTION_KEYBOARD_PRE)
		{
			if(button_type == SINGLEBUTTON)//������ť
			{
				if(preWidgetID == curPagePtr->curEnlightedWidgetID) //��ʾ����ߵĿؼ�������
					return AHMI_FUNC_SUCCESS;
				else 
				{
					preWidgetClassPtr = &(curPagePtr->pWidgetList[preWidgetID]);
					while(preWidgetClassPtr->displayEn(preWidgetID) == AHMI_FUNC_FAILURE) //ǰһ���ؼ�����ʾ��������ǰ
					{
						preOfPreWidgetID = (preWidgetClassPtr->KeyboardPreAndNextWidget) >> 8;
						if(preOfPreWidgetID == preWidgetID) //��ʾ��ǰ��û�пؼ��ˣ�֤��Ҳ������߿ؼ�������
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
			else if(button_type == COMPLEXBUTTON) //���Ӱ�ť
			{
				if(preWidgetID == curPagePtr->curEnlightedWidgetID && (curEnlighted == 1 || curEnlighted == 0) ) //��ʾ����ߵĿؼ�������
					return AHMI_FUNC_SUCCESS;
				else if(preWidgetID == curPagePtr->curEnlightedWidgetID) //����߿ؼ�����û�е������
				{
					if(widgetType == BUTTON_TEX)
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == CLOCK_TEX)
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == TEXTURE_TIME)
						TextureTimeWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
				}
				else if(preWidgetID != curPagePtr->curEnlightedWidgetID && curEnlighted == 1)//��߻��а�ť�Ұ�����
				{
					preWidgetClassPtr = &(curPagePtr->pWidgetList[preWidgetID]);
					while(preWidgetClassPtr->displayEn(preWidgetID) == AHMI_FUNC_FAILURE) //ǰһ���ؼ�����ʾ��������ǰ
					{
						preOfPreWidgetID = (preWidgetClassPtr->KeyboardPreAndNextWidget) >> 8;
						if(preOfPreWidgetID == preWidgetID) //��ʾ��ǰ��û�пؼ��ˣ�֤��Ҳ������߿ؼ�������
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
				else  //�ڲ��ƶ�
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
		//���Ұ�����ť
		else if(this->mInputType == ACTION_KEYBOARD_NEXT)
		{
			if(button_type == SINGLEBUTTON)//������ť
			{
				if(nextWidgetID == curPagePtr->curEnlightedWidgetID && curEnlighted == button_num) //��ʾ���ұߵĿؼ�������
					return AHMI_FUNC_SUCCESS;
				else if(curEnlighted == 0)//��ʾ��ʼ״̬
				{
					while(curEnlightedWidgetClassPtr->displayEn(curPagePtr->curEnlightedWidgetID) == AHMI_FUNC_FAILURE) //��һ����ť����ʾ������Ѱ����һ��
					{
						nextOfNextWidgetID = (curEnlightedWidgetClassPtr->KeyboardPreAndNextWidget) & 0xff ;
						if(nextOfNextWidgetID == curPagePtr->curEnlightedWidgetID) //��ʾ�������һ��
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
					while(nextWidgetClassPtr->displayEn(nextWidgetID) == AHMI_FUNC_FAILURE) //��һ���ؼ�����ʾ
					{
						nextOfNextWidgetID = (nextWidgetClassPtr->KeyboardPreAndNextWidget) & 0xff ;
						if(nextOfNextWidgetID == nextWidgetID) //��ʾ�������һ��
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
			else if(button_type == COMPLEXBUTTON) //���Ӱ�ť
			{
				if(nextWidgetID == curPagePtr->curEnlightedWidgetID && (curEnlighted == button_num) ) //��ʾ���ұߵĿؼ�������
					return AHMI_FUNC_SUCCESS;
				else if(nextWidgetID == curPagePtr->curEnlightedWidgetID && (curEnlighted == 0)) //��ʼ��
				{
					while(curEnlightedWidgetClassPtr->displayEn(curPagePtr->curEnlightedWidgetID) == AHMI_FUNC_FAILURE) //��һ����ť����ʾ������Ѱ����һ��
					{
						nextOfNextWidgetID = (curEnlightedWidgetClassPtr->KeyboardPreAndNextWidget) & 0xff ;
						if(nextOfNextWidgetID == curPagePtr->curEnlightedWidgetID) //��ʾ�������һ��
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
				else if(nextWidgetID == curPagePtr->curEnlightedWidgetID) //�ؼ����ƶ�
				{
					if(widgetType == BUTTON_TEX)
						widget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == CLOCK_TEX)
						clockWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
					else if(widgetType == TEXTURE_TIME)
						TextureTimeWidget.widgetCtrl(curEnlightedWidgetClassPtr, this, 0);
				}
				else if(nextWidgetID != curPagePtr->curEnlightedWidgetID && curEnlighted == button_num) //�ұ߻��а�ť�Ұ�����
				{
					nextWidgetClassPtr = &(curPagePtr->pWidgetList[nextWidgetID]);
					while(nextWidgetClassPtr->displayEn(nextWidgetID) == AHMI_FUNC_FAILURE) //��һ���ؼ�����ʾ
					{
						nextOfNextWidgetID = (nextWidgetClassPtr->KeyboardPreAndNextWidget) & 0xff ;
						if(nextOfNextWidgetID == nextWidgetID) //��ʾ�������һ��
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
				else //�ڲ��ƶ�
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
		//�س�
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
// �������� PageActionTrigger
// ����ҳ��ָ��
// �����б�
// @param1 WidgetClassPtr pWidgetPtr widgetָ��
// @param2 ActionTriggerClassPtr pTagtrigger ����ָ��
// ��ע(�����汾֮����޸�):
//    
//-----------------------------
funcStatus ActionTriggerClass::widgetActionTrigger(
		 WidgetClassPtr pWidgetPtr,
		 u8 u8_widgetRefresh    //�Ƿ���Ҫˢ�¿ؼ�,1��ʾˢ�£�added by zuz 20160622
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
