////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     CanvasClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
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

//���
extern PIDPoint pressPoint;
extern PIDPoint releasePoint;
//���Ŀǰ״̬
extern u8 mouseStatus;

ButtonClass::ButtonClass(void)
{
}


ButtonClass::~ButtonClass(void)
{
}

funcStatus ButtonClass::initWidget(
	WidgetClassPtr p_wptr,   //�ؼ�ָ��
	u32 *u32p_sourceShift,   //sourcebufferָ��
	u8 u8_pageRefresh,       //ҳ��ˢ��
	u8 RefreshType ,       //���ƵĶ������ͣ����ݶ������͸ı���ƿؼ��İ�Χ��
	TileBoxClassPtr pTileBox, //��Χ��
	u8 staticTextureEn          //�Ƿ���Ƶ���̬�洢�ռ�
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
//WidgetAttr��ʶ��
//15-11:����
//11:  ��ť�Ƿ�Ϊ��ɫ��ť
//10-7:������ģʽ��Ч����ť����
//6-5:��ťģʽ��0Ϊ������ťģʽ��1Ϊ����ģʽ��2Ϊ����ģʽ
//4-0:�ؼ�����
//**************************************
funcStatus ButtonClass::widgetCtrl(
	WidgetClassPtr p_wptr,  //�ؼ�ָ��
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh      //ҳ��ˢ��
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
	u8 curHighLight; //��ǰ�����İ�ť
	u8 flag = 0;
	u16 cur_x;
	u16 cur_y;
//	AnimationMsg animationMsg;
	//RefreshMsg refreshMsg;
	u8 signleColorButtonEn; //�Ƿ�Ϊ�򵥵���ɫ��ť
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
		signleColorButtonEn = (p_wptr->WidgetAttr & SINGLE_COLOR_BUTTON_BIT) ? 1 : 0; //�ж��Ƿ�Ϊ������ɫ��ť
		//��괥��
		if( ActionPtr->mInputType == ACTION_MOUSE_PRESS ) //��갴��
		{
			tag_value = 1;
			if(signleColorButtonEn)
			{
				//����press����ɫ����ֵ
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
		else if(ActionPtr->mInputType == ACTION_MOUSE_RELEASE) //���̧��
		{
			tag_value = 0;
			if(signleColorButtonEn)
			{
				//����release����ɫ����ֵ
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
		else if(ActionPtr->mInputType == ACTION_MOUSE_CANCLE) //ȡ����������ָ�ֻ̧��
		{
			tag_value = 0;
			if(signleColorButtonEn)
			{
				//����release����ɫ����ֵ
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

		else if(ActionPtr->mInputType == ACTION_KEYBOARD_PRE || ActionPtr->mInputType == ACTION_KEYBOARD_NEXT || ActionPtr->mInputType == ACTION_KEYBOARD_OK)//�����ϰ�ť������
		{
			//tag_value = ActionPtr->mTagPtr->mValue;
			curHighLight = (u8)(p_wptr->NumOfButtonAndCurHighLight);
			if(ActionPtr->mInputType == ACTION_KEYBOARD_PRE ||ActionPtr->mInputType == ACTION_KEYBOARD_NEXT) //����������ң�������ť���߸���
			{
				if(curHighLight == 0) //LIGHT ON
				{
					curHighLight = 1;
					gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].mTexAttr |= (DRAWING); //��ɫ��ť��ʾ
					p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;
				}
				else if(curHighLight == 1)//LIGHT OFF
				{
					curHighLight = 0;
					gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].mTexAttr &= ~(DRAWING); //��ɫ��ťϨ��
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
			gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1].mTexAttr &= ~(DRAWING); //��ɫ��ťϨ��
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
		if(ActionPtr->mInputType == ACTION_MOUSE_PRESS) //��ť������
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
				//����tag_set_value
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
		else if(ActionPtr->mInputType == ACTION_KEYBOARD_NEXT || ActionPtr->mInputType == ACTION_KEYBOARD_OK || ActionPtr->mInputType == ACTION_KEYBOARD_PRE) //���̴���
		{
			curHighLight = (u8)(p_wptr->NumOfButtonAndCurHighLight);
			//����Ų��
			if( (curHighLight == 1 && ActionPtr->mInputType == ACTION_KEYBOARD_PRE) || (curHighLight == button_num && ActionPtr->mInputType == ACTION_KEYBOARD_NEXT))   //�ؼ��ڵ�һ���������һ����ť
			{
				//TagPtr[p_wptr->BindTagID].mValue = 0;
				//for(i = 0;i < button_num;i++)
				//	TexturePtr[i].FocusedSlice = 0;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].mTexAttr &= ~(DRAWING); //��ɫ��ťϨ��
				curHighLight = 0;
				p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;
			}
			//�������ұ�Ų����
			else if(curHighLight == 0 && ActionPtr->mInputType == ACTION_KEYBOARD_PRE)
			{
				//TagPtr[p_wptr->BindTagID].mValue = button_num;
				//tag_value = TagPtr[p_wptr->BindTagID].mValue;
				//for(i = 0;i < button_num;i++)
				//	TexturePtr[i].FocusedSlice = 0;
				//TexturePtr[tag_value-1].FocusedSlice = 1;
				curHighLight = (u8)button_num;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].mTexAttr |= (DRAWING); //��ɫ��ť��ʾ
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].OffsetX = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].OffsetX;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].OffsetY = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].OffsetY;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].TexWidth = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].TexWidth;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].TexHeight = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + curHighLight - 1].TexHeight;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].renewTextureSourceBox(NULL,NULL,NULL);
				p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;

			}
			//���������Ų����
			else if(curHighLight == 0 && ActionPtr->mInputType == ACTION_KEYBOARD_NEXT)
			{
				//TagPtr[p_wptr->BindTagID].mValue = 1;
				//tag_value = TagPtr[p_wptr->BindTagID].mValue;
				//for(i = 0;i < button_num;i++)
				//	TexturePtr[i].FocusedSlice = 0;
				//TexturePtr[tag_value-1].FocusedSlice = 1;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].mTexAttr |= (DRAWING); //��ɫ��ť��ʾ
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].OffsetX = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].OffsetX;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].OffsetY = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].OffsetY;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].TexWidth = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].TexWidth;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].TexHeight = gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex].TexHeight;
				gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].renewTextureSourceBox(NULL,NULL,NULL);
				curHighLight = 1;
				p_wptr->NumOfButtonAndCurHighLight = ((p_wptr->NumOfButtonAndCurHighLight) & 0xff00) + curHighLight;
			}
			else if(ActionPtr->mInputType == ACTION_KEYBOARD_PRE || ActionPtr->mInputType == ACTION_KEYBOARD_NEXT)//������������
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
			else if(ActionPtr->mInputType == ACTION_KEYBOARD_OK)//����ȷ��
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
			gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + button_num].mTexAttr &= ~(DRAWING); //��ɫ��ťϨ��
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
		//���´˿ؼ�
//		if( ((p_wptr->ANIMATION_TYPE) & 0xff) == NO_ANIMATION) //�޶�������Ҫ���»���
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
		//������ť���ͷ�ʱ��Ҫ���ƶ���
//		else if(ActionPtr->mInputType == ACTION_MOUSE_RELEASE && ((p_wptr->ANIMATION_TYPE) & 0xff) != NO_ANIMATION && button_type != COMPLEXBUTTON)
//		{
			//send msg to animation list

//			return AHMI_FUNC_SUCCESS;
//		}
	}
	return AHMI_FUNC_SUCCESS;
}


#endif
