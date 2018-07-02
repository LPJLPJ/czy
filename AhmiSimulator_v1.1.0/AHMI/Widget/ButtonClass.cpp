////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     CanvasClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    definition of button widget
// 
////////////////////////////////////////////////////////////////////////////////

#include "ButtonClass.h"
#include "publicInclude.h"
#include "AHMIBasicDefine.h"
#include "aniamtion.h"
#include "drawImmediately_cd.h"
#ifdef AHMI_DEBUG
#include <stdio.h>
#include "trace.h"
#endif

#ifdef AHMI_CORE
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
extern QueueHandle_t		ActionInstructionQueue;

extern u32 startOfDynamicPage;
extern u32 endOfDynamicPage;

//鼠标
extern PIDPoint pressPoint;
extern PIDPoint releasePoint;
//鼠标目前状态
extern u8 mouseStatus;

ButtonClass::ButtonClass(void)
{
}


ButtonClass::~ButtonClass(void)
{
}

funcStatus ButtonClass::initWidget(
	WidgetClassPtr p_wptr,   //控件指针
	u32 *u32p_sourceShift,   //sourcebuffer指针
	u8 u8_pageRefresh,       //页面刷新
	u8 RefreshType ,       //绘制的动画类型，根据动画类型改变绘制控件的包围盒
	TileBoxClassPtr pTileBox, //包围盒
	u8 staticTextureEn          //是否绘制到静态存储空间
	)
{
	WidgetClassInterface myWidgetClassInterface;
	TagClassPtr bindTag = &TagPtr[p_wptr->BindTagID];
	ActionTriggerClass tagtrigger;

	if((NULL == p_wptr) || (NULL == u32p_sourceShift) || (NULL == pTileBox))
		return AHMI_FUNC_FAILURE;
	

	if(u8_pageRefresh)
	{
		tagtrigger.mInputType = ACTION_TAG_SET_VALUE;
		tagtrigger.mTagPtr = bindTag;
		if(widgetCtrl(p_wptr,&tagtrigger,1) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}
	if(myWidgetClassInterface.drawTexture(p_wptr,u32p_sourceShift,RefreshType,pTileBox,staticTextureEn) == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;
	return AHMI_FUNC_SUCCESS;
}

//**************************************
//config and add widget
//WidgetAttr标识：
//15-11:保留
//11:  按钮是否为颜色按钮
//10-7:仅复杂模式有效，按钮数量
//6-5:按钮模式，0为单个按钮模式，1为开关模式，2为复杂模式
//4-0:控件类型
//**************************************
funcStatus ButtonClass::widgetCtrl(
	WidgetClassPtr p_wptr,  //控件指针
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh      //页面刷新
	)
{
	u8 button_type;
	u8 button_num;
	u16 button_width;
	u16 button_height;
	s16 button_offsetX;
	s16 button_offsetY;	
	TextureClassPtr TexturePtr;
	u32 actionAddr;
	s32 tag_value = 0;
	u8 curHighLight; //当前高亮的按钮
	u8 flag = 0;
	u16 cur_x;
	u16 cur_y;
//	AnimationMsg animationMsg;
	//RefreshMsg refreshMsg;
	u8 signleColorButtonEn; //是否为简单的颜色按钮
	u8 r,g,b,a;
	//TextureClassPtr curTexture;
	int i;
#ifdef AHMI_DEBUG
	//char text[100];
#endif

	//check the parameters
	if((NULL == p_wptr) || (NULL == ActionPtr))
	{
		ERROR_PRINT("ERROR: button widget illegal input");
		return AHMI_FUNC_FAILURE;
	}
	if( (s16)(p_wptr->WidgetOffsetX) > MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetY) > MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetX) < -MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetY) < -MAX_WIDTH_AND_HEIGHT ||
		p_wptr->WidgetWidth > MAX_WIDTH_AND_HEIGHT ||
		p_wptr->WidgetHeight > MAX_WIDTH_AND_HEIGHT || 
		p_wptr->WidgetWidth == 0 ||  
		p_wptr->WidgetHeight == 0)
	{
		ERROR_PRINT("ERROR: when drawing button widght, the offset or width or height exceeds the banduary");
		return AHMI_FUNC_FAILURE;
	}

	//get texture
	if(gPagePtr[WorkingPageID].pBasicTextureList == NULL)
	{
		ERROR_PRINT("ERROR: when drawing button widght, the texture list corrupt");
		return AHMI_FUNC_FAILURE;
	}

	if(p_wptr->StartNumofTex < gPagePtr[WorkingPageID].mTotalNumOfTexture )
		TexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
	else 
	{
		ERROR_PRINT("ERROR: when drawing button widght, the start texture is more then total number of texture");
		return AHMI_FUNC_FAILURE;
	}

	if((TexturePtr < (void*)startOfDynamicPage) || (TexturePtr > (void*)endOfDynamicPage))
		return AHMI_FUNC_FAILURE;

	button_type = (p_wptr->WidgetAttr & 0x60) >> 5;

	if(button_type == SINGLEBUTTON)
	{
		signleColorButtonEn = (p_wptr->WidgetAttr & SINGLE_COLOR_BUTTON_BIT) ? 1 : 0; //判断是否为单个颜色按钮
		//鼠标触发
		if( ActionPtr->mInputType == ACTION_MOUSE_PRESS ) //鼠标按下
		{
			tag_value = 1;
			if(signleColorButtonEn)
			{
				//计算press的颜色并赋值
				a = 0xff;
				r = (p_wptr-> PressColorValueRG & 0xff00) >> 8;
				g = (p_wptr-> PressColorValueRG & 0x00ff) ;
				b = (p_wptr -> PressValueBAndReleaseColorValueR & 0xff00) >> 8;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].TexAddr = (a << 24) + (r << 16) + (g << 8) + b;
			}
			else
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].FocusedSlice = 1;
			if(p_wptr->BindTagID != 0)
				TagPtr[p_wptr->BindTagID].mValue = tag_value;
		}
		else if(ActionPtr->mInputType == ACTION_MOUSE_RELEASE) //鼠标抬起
		{
			tag_value = 0;
			if(signleColorButtonEn)
			{
				//计算release的颜色并赋值
				a = 0xff;
				r = (p_wptr->PressValueBAndReleaseColorValueR & 0x00ff) ;
				g = (p_wptr->ReleaseColorValueGB & 0xff00) >> 8;
				b = (p_wptr->ReleaseColorValueGB & 0x00ff) ;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].TexAddr = (a << 24) + (r << 16) + (g << 8) + b;
			}
			else 
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].FocusedSlice = 0;
			if(p_wptr->OnRealeaseAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
			{
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->OnRealeaseAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
			}
			TagPtr[p_wptr->BindTagID].mValue = tag_value;

		}
		else if(ActionPtr->mInputType == ACTION_MOUSE_CANCLE) //取消，不发送指令，只抬起
		{
			tag_value = 0;
			if(signleColorButtonEn)
			{
				//计算release的颜色并赋值
				a = 0xff;
				r = (p_wptr->PressValueBAndReleaseColorValueR & 0x00ff) ;
				g = (p_wptr->ReleaseColorValueGB & 0xff00) >> 8;
				b = (p_wptr->ReleaseColorValueGB & 0x00ff) ;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].TexAddr = (a << 24) + (r << 16) + (g << 8) + b;
			}
			else 
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].FocusedSlice = 0;
			TagPtr[p_wptr->BindTagID].mValue = tag_value;
		}
		//end of MOUSE_TOUCH

		else if(ActionPtr->mInputType == ACTION_TAG_SET_VALUE)
		{
			if(ActionPtr->mTagPtr == NULL)//no tag
			{
				ERROR_PRINT("ERROR: the set tag value trigger runs failure in button widget, no tag pointer valid");
				return AHMI_FUNC_FAILURE;
			}
			tag_value = ActionPtr->mTagPtr->mValue;
			TagPtr[p_wptr->BindTagID].mValue = tag_value;
			if(u8_pageRefresh)
				tag_value = 0;
			if(tag_value == 0)
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].FocusedSlice = 0;
			else
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].FocusedSlice = 1;
			//return AHMI_FUNC_SUCCESS;
		}//end of TAG_SET_VALUE

		else if(ActionPtr->mInputType == ACTION_KEYBOARD_PRE || ActionPtr->mInputType == ACTION_KEYBOARD_NEXT || ActionPtr->mInputType == ACTION_KEYBOARD_OK)//键盘上按钮被按下
		{
			//tag_value = ActionPtr->mTagPtr->mValue;
			curHighLight = (u8)(p_wptr->NumOfButtonAndCurHighLight);
			if(ActionPtr->mInputType == ACTION_KEYBOARD_PRE ||ActionPtr->mInputType == ACTION_KEYBOARD_NEXT) //向左或者向右，单个按钮移走高亮
			{
				if(curHighLight == 0) //LIGHT ON
				{
					curHighLight = 1;
					gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].mTexAttr |= (DRAWING); //颜色按钮显示
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;
				}
				else if(curHighLight == 1)//LIGHT OFF
				{
					curHighLight = 0;
					gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].mTexAttr &= ~(DRAWING); //颜色按钮熄灭
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;
				}
			}
			else if(ActionPtr->mInputType == ACTION_KEYBOARD_OK)
			{
				if(p_wptr->OnEnteredAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
				{
					actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->OnEnteredAction);
					xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
				}
				return AHMI_FUNC_SUCCESS;
			}
		}
		else if(ActionPtr->mInputType == ACTION_KEYBOARD_CLEAR)// clear the highlight 
		{
			curHighLight = 0;
			gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].mTexAttr &= ~(DRAWING); //颜色按钮熄灭
			p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;
		}
		else 
		{
			ERROR_PRINT("ERROR: illegal action trigger button widget");
			return AHMI_FUNC_FAILURE;
		}
	
	}
	else if(button_type == COMPLEXBUTTON)
	{
		button_num = (u8)((u16)(p_wptr->NumOfButtonAndCurHighLight) >> 8);
		if(ActionPtr->mInputType == ACTION_MOUSE_PRESS) //按钮被按下
		{
			cur_x = ((s16)(pressPoint.x)) << 4;
			cur_y = ((s16)(pressPoint.y)) << 4;
			for(i = 0;i < button_num;i++)
			{
				button_width = TexturePtr[i].TexWidth;
				button_height = TexturePtr[i].TexHeight;
				button_offsetX = TexturePtr[i].OffsetX;
				button_offsetY = TexturePtr[i].OffsetY;	
				//TexturePtr[i].FocusedSlice = 0;
				if(!(cur_x < button_offsetX || cur_x > button_offsetX + (button_width << 4) || cur_y < button_offsetY || cur_y > button_offsetY + (button_height << 4)) && !flag)
				{
					flag = i + 1;
					tag_value = i + 1;
					
				}
				//else
				//{
					//TexturePtr[i].FocusedSlice = 0;
				//}
			}
			if(flag)
			{
				//调用tag_set_value
				TagPtr[p_wptr->BindTagID].setValue(tag_value,p_wptr->BindTagID);
				/*for(i = 0;i < button_num;i++)
					TexturePtr[i].FocusedSlice = 0;
				TexturePtr[flag-1].FocusedSlice = 1;*/
			}
			//if(!flag) tag_value = 0;
			//TagPtr[p_wptr->BindTagID].mValue = tag_value;
		}//end of mouse press
		else if(ActionPtr->mInputType == ACTION_TAG_SET_VALUE)
		{
			tag_value = (s32)ActionPtr->mTagPtr->mValue;
			for(i = 0;i < button_num;i++)
					TexturePtr[i].FocusedSlice = 0;
			if(tag_value > 0 && tag_value <= button_num )
			{
				//if(u8_pageRefresh == 0)
				//{
					flag = tag_value;
					TexturePtr[flag-1].FocusedSlice = 1;
				//}
			}
		}
		else if(ActionPtr->mInputType == ACTION_KEYBOARD_NEXT || ActionPtr->mInputType == ACTION_KEYBOARD_OK || ActionPtr->mInputType == ACTION_KEYBOARD_PRE) //键盘触发
		{
			curHighLight = (u8)(p_wptr->NumOfButtonAndCurHighLight);
			//高亮挪走
			if( (curHighLight == 1 && ActionPtr->mInputType == ACTION_KEYBOARD_PRE) || (curHighLight == button_num && ActionPtr->mInputType == ACTION_KEYBOARD_NEXT))   //控件内第一个或者最后一个按钮
			{
				//TagPtr[p_wptr->BindTagID].mValue = 0;
				//for(i = 0;i < button_num;i++)
				//	TexturePtr[i].FocusedSlice = 0;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].mTexAttr &= ~(DRAWING); //颜色按钮熄灭
				curHighLight = 0;
				p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;
			}
			//高亮从右边挪过来
			else if(curHighLight == 0 && ActionPtr->mInputType == ACTION_KEYBOARD_PRE)
			{
				//TagPtr[p_wptr->BindTagID].mValue = button_num;
				//tag_value = TagPtr[p_wptr->BindTagID].mValue;
				//for(i = 0;i < button_num;i++)
				//	TexturePtr[i].FocusedSlice = 0;
				//TexturePtr[tag_value-1].FocusedSlice = 1;
				curHighLight = (u8)button_num;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].mTexAttr |= (DRAWING); //颜色按钮显示
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].OffsetX = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].OffsetX;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].OffsetY = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].OffsetY;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].TexWidth = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].TexWidth;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].TexHeight = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].TexHeight;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].renewTextureSourceBox(NULL,NULL,NULL);
				p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;

			}
			//高亮从左边挪过来
			else if(curHighLight == 0 && ActionPtr->mInputType == ACTION_KEYBOARD_NEXT)
			{
				//TagPtr[p_wptr->BindTagID].mValue = 1;
				//tag_value = TagPtr[p_wptr->BindTagID].mValue;
				//for(i = 0;i < button_num;i++)
				//	TexturePtr[i].FocusedSlice = 0;
				//TexturePtr[tag_value-1].FocusedSlice = 1;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].mTexAttr |= (DRAWING); //颜色按钮显示
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].OffsetX = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].OffsetX;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].OffsetY = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].OffsetY;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].TexWidth = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].TexWidth;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].TexHeight = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].TexHeight;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].renewTextureSourceBox(NULL,NULL,NULL);
				curHighLight = 1;
				p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;
			}
			else if(ActionPtr->mInputType == ACTION_KEYBOARD_PRE || ActionPtr->mInputType == ACTION_KEYBOARD_NEXT)//高亮换个纹理
			{
				if(ActionPtr->mInputType == ACTION_KEYBOARD_PRE)
					curHighLight --;
				else if(ActionPtr->mInputType == ACTION_KEYBOARD_NEXT)
					curHighLight ++;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].OffsetX = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].OffsetX;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].OffsetY = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].OffsetY;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].TexWidth = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].TexWidth;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].TexHeight = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].TexHeight;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].renewTextureSourceBox(NULL,NULL,NULL);
				p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;
			}
			else if(ActionPtr->mInputType == ACTION_KEYBOARD_OK)//键盘确认
			{
				tag_value = curHighLight;
				for(i = 0;i < button_num;i++)
					TexturePtr[i].FocusedSlice = 0;
				if(tag_value > 0 && tag_value <= button_num )
				{
					//if(u8_pageRefresh == 0)
					//{
						flag = tag_value;
						TexturePtr[flag-1].FocusedSlice = 1;
					//}
				}
				if(ActionPtr->mTagPtr == NULL)//no tag
				{
					ERROR_PRINT("ERROR: the set tag value trigger runs failure in button widget, no tag pointer valid");
					return AHMI_FUNC_FAILURE;
				}
				ActionPtr->mTagPtr->mValue = tag_value;
				if(p_wptr->OnEnteredAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
				{
					actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->OnEnteredAction);
					xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
				}
				//return AHMI_FUNC_SUCCESS;
			}
			
		}
		else if(ActionPtr->mInputType == ACTION_KEYBOARD_CLEAR)// clear the highlight 
		{
			gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].mTexAttr &= ~(DRAWING); //颜色按钮熄灭
			curHighLight = 0;
			p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;
		}//end of clear highlight
		else 
		{
			ERROR_PRINT("ERROR: illegal action trigger button widget");
			return AHMI_FUNC_FAILURE;
		}
	}
	if(u8_pageRefresh == 0 )
	{
		//更新此控件
//		if( ((p_wptr->ANIMATION_TYPE) & 0xff) == NO_ANIMATION) //无动画，需要重新绘制
//		{
			//send refresh message
//#ifdef STATIC_BUFFER_EN
//#ifdef AHMI_DEBUG
//			ERROR_PRINT("sending the refresh static buffer cmd");
//#endif
//		    refreshMsg.mElementType = ANIMAITON_REFRESH_STATIC_BUFFER;
//			refreshMsg.mElementPtr.pageptr = gPagePtr + WorkingPageID;
//			sendToRefreshQueue(&refreshMsg);
//#endif
#ifndef WHOLE_TRIBLE_BUFFER
			refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
			refreshMsg.mElementPtr.wptr = p_wptr;
			sendToRefreshQueue(&refreshMsg);
#endif

			return AHMI_FUNC_SUCCESS;
//		}
		//动画按钮，释放时需要绘制动画
//		else if(ActionPtr->mInputType == ACTION_MOUSE_RELEASE && ((p_wptr->ANIMATION_TYPE) & 0xff) != NO_ANIMATION && button_type != COMPLEXBUTTON)
//		{
			//send msg to animation list

//			return AHMI_FUNC_SUCCESS;
//		}
	}
	return AHMI_FUNC_SUCCESS;
}


#endif
