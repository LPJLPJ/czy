////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     CanvasClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 3.00 - File Created 2016/04/13 by �ڴ�Ӫ
// Additional Comments:
//   
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "AHMIBasicDefine.h"
#include "aniamtion.h"
#include "DynamicTexClass.h"
#include "drawImmediately_cd.h"

#ifdef AHMI_CORE
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
//extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;

//-----------------------------
// �������� DynamicTexClass
// ���캯��
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
DynamicTexClass::DynamicTexClass()
{

}

//-----------------------------
// �������� ~DynamicTexClass
// ��������
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
DynamicTexClass::~DynamicTexClass()
{

}


//-----------------------------
// �������� DynamicTexClass::initWidget
// ��ʼ�������Ƹÿؼ��������ȫ��ˢ�£��ȸ������󶨵�tagˢ�¸ÿؼ����ڻ��Ƹÿؼ�
//  @param   WidgetClassPtr p_wptr,   //�ؼ�ָ��
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //ҳ��ˢ��
//  @param   TileBoxClassPtr pTileBox //��Χ��
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus DynamicTexClass::initWidget(
	WidgetClassPtr p_wptr,   //�ؼ�ָ��
	u32 *u32p_sourceShift,   //sourcebufferָ��
	u8 u8_pageRefresh,       //ҳ��ˢ��
	u8 RefreshType ,       //���ƵĶ������ͣ����ݶ������͸ı���ƿؼ��İ�Χ��
	TileBoxClassPtr pTileBox, //��Χ��
	u8 staticTextureEn          //�Ƿ���Ƶ���̬�洢�ռ�
	)
{
	TagClassPtr bindTag;
	ActionTriggerClass tagtrigger;
	WidgetClassInterface myWidgetClassInterface;

	if((NULL == p_wptr) || (NULL == u32p_sourceShift) || (NULL == pTileBox))
		return AHMI_FUNC_FAILURE;

	bindTag = &TagPtr[p_wptr->BindTagID];

	if(u8_pageRefresh)
	{
		tagtrigger.mTagPtr = bindTag;
		tagtrigger.mInputType = ACTION_TAG_SET_VALUE;
		if(widgetCtrl(p_wptr,&tagtrigger,1) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}

	if(myWidgetClassInterface.drawTexture(p_wptr,u32p_sourceShift,RefreshType,pTileBox,staticTextureEn) == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;
	return AHMI_FUNC_SUCCESS;
}

//*****************************
//��̬����
//ֻ��һ�����������ж��Slice
//tag��ֵ����focusedSlice���
//WidgetAttr��ʶ��
//15-8 : ����
//5-7  : ��̬��������ͣ�0�����������л� 1������������ת 2�����������ƶ� 3:��͸������
//4-0  : �ؼ����ͣ���̬����ӦΪ0x0
//*****************************
funcStatus DynamicTexClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//�ؼ�ָ��
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//ҳ��ˢ��
	)
{
	s32 value;
	s32 value_temp = 0;
    RefreshMsg refreshMsg;
	TextureClassPtr texturePtr, preTexturePtr;
	u8 movingDir = 0;
	u8 dynamicType = 0;
	u16 angle = 0;
	u32 actionAddr;
	s32 sValue;
	s32	maxValue;
	s32	minValue;
	u32 curValue;
	s32 lowAlarmValue;
	s32 highAlarmValue;
	u32 oldValue = 0;
	s32 sOldValue = 0;
	u16 oldValueinit = 0;

	if((NULL == p_wptr) || (NULL == ActionPtr) || NULL == ActionPtr->mTagPtr || NULL == gPagePtr->pBasicTextureList){
		ERROR_PRINT("ERROR: for NULL pointer");
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
		ERROR_PRINT("ERROR: when drawing clock widght, the offset\\width\\height exceeds the boundary");
		return AHMI_FUNC_FAILURE;
	}

	dynamicType = (p_wptr->WidgetAttr & DYNAMIC_TYPE_BIT) >> 5;

	value = (s32)(ActionPtr->mTagPtr->mValue);

	curValue = (p_wptr->CurValueH << 16) + p_wptr->CurValueL;
	maxValue = (s32)((p_wptr->MaxValueH << 16) + p_wptr->MaxValueL);
	minValue = (s32)((p_wptr->MinValueH << 16) + p_wptr->MinValueL);
	lowAlarmValue  = (s32)((p_wptr->LowAlarmValueH  << 16) + p_wptr->LowAlarmValueL);
	highAlarmValue = (s32)((p_wptr->HighAlarmValueH << 16) + p_wptr->HighAlarmValueL);

	if(dynamicType == 0)  //���������л�
	{
		if((u32)value > (u32)maxValue) 
		{
			value_temp = value;
			value = (u32)maxValue;
		}
		else if((u32)value < (u32)minValue)
			value = (u32)minValue;
		

		if(p_wptr->NumOfLine == 1)
		{
			texturePtr = &gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex];
			if((u32)value_temp > (u32)value)
				texturePtr->mTexAttr &= ~(DRAWING);
			else
				texturePtr->mTexAttr |= (DRAWING);
			preTexturePtr = texturePtr;
		}
		else if(p_wptr->NumOfLine == 2)
		{
			if(u8_pageRefresh)
			{
				p_wptr->PreviousTexturePtrFlag = 0;
				texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
				preTexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex+1]);
			}
			else if((p_wptr->PreviousTexturePtrFlag == 0) || (p_wptr->PreviousTexturePtrFlag == 0xCCCC))
			{
				p_wptr->PreviousTexturePtrFlag = 1;
				texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1]);
				preTexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
			}
			else if(p_wptr->PreviousTexturePtrFlag == 1)
			{
				p_wptr->PreviousTexturePtrFlag = 0;
				texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
				preTexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex + 1]);
			}
		}
		else 
			return AHMI_FUNC_FAILURE;

		//texturePtr->FocusedSlice = Value;//����Ҫ��ʾ��Slice

		//�ж�ƽ�Ʒ���
		if(p_wptr->NumOfLine != 1)
		{
			if((u32)value > curValue)
			{
				//����
				movingDir = 0;
			}
			else if((u32)value < curValue)
			{
				//����
				movingDir = 1;
			}
			else 
			{
				//���ƶ�
				if(u8_pageRefresh == 0)
					return AHMI_FUNC_SUCCESS;
			}
		}

		//���������ֵ
		texturePtr->FocusedSlice = (u16)value;
		if(u8_pageRefresh)
			preTexturePtr->FocusedSlice = (u16)value;

		//���������λ��
		if(p_wptr->NumOfLine == 2 && !u8_pageRefresh)
		{
			if(movingDir == 0) //����
			{
				texturePtr->OffsetY = (p_wptr->WidgetOffsetY + p_wptr->WidgetHeight) << 4;
				preTexturePtr->OffsetY =  (p_wptr->WidgetOffsetY) << 4;
			}
			else if(movingDir == 1)//����
			{
				texturePtr->OffsetY = (p_wptr->WidgetOffsetY - p_wptr->WidgetHeight) << 4;
				preTexturePtr->OffsetY =  (p_wptr->WidgetOffsetY) << 4;
			}
		}

//		if(u8_pageRefresh == 0)
//		{
//			//���´˿ؼ�
//#ifdef STATIC_BUFFER_EN
//#ifdef DEBUG
//			ERROR_PRINT("sending the refresh static buffer cmd");
//#endif
//		    refreshMsg.mElementType = ANIMAITON_REFRESH_STATIC_BUFFER;
//			refreshMsg.mElementPtr.pageptr = gPagePtr + WorkingPageID;
//			sendToRefreshQueue(&refreshMsg);
//#endif
//		}
	}
	else if(dynamicType == CENTRAL_ROTATE) //����������ת
	{
		texturePtr = &gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex];
		sValue = (s32)value;
		//valueȡ����ֵ
		if(sValue < 0)
			value = -sValue;
		//value����Ƕ�
		if(value > 360)
			value %= 360;

		if(value > maxValue) 
			value = maxValue;
		if(value < minValue)
			value = minValue;

		//if(sValue < 0)
		//	angle = p_wptr->DynamicTexMinAngle - value;
		//else
		//	angle = p_wptr->DynamicTexMinAngle + value;
		angle = value;

		texturePtr[0].RotateAngle = angle * 16;
		texturePtr[0].mTexAttr |= TEXTURE_CENTRAL_ROTATE;
		texturePtr[0].mTexAttr |= TEXTURE_USING_WIDGET_BOX;
		texturePtr[0].mTexAttr |= USING_PIXEL_RATIO;  //needs to be fixed, by zuz 20180802
		//���¼����Χ��
		texturePtr[0].RenewRotateTextureSourceBox();

		if(u8_pageRefresh == 0)
		{
			//���´˿ؼ�

			if(oldValueinit)
			{
				oldValue = (p_wptr->OldValueH  << 16) + p_wptr->OldValueL;
				if(dynamicType == 0)
				{
					if(p_wptr->EnterLowAlarmAction && oldValue > (u32)lowAlarmValue && (u32)value <= (u32)lowAlarmValue)
					{
						//�����ֵԤ��
						if(p_wptr->EnterLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
						{
							actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterLowAlarmAction);
							xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
						}		
					}
					else if(p_wptr->LeaveLowAlarmAction && oldValue <= (u32)lowAlarmValue && (u32)value > (u32)lowAlarmValue)
					{
						//�뿪��ֵԤ��
						if(p_wptr->LeaveLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
						{
							actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveLowAlarmAction);
							xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
						}		
					}
					if(p_wptr->EnterHighAlarmAction && oldValue < (u32)highAlarmValue && (u32)value >= (u32)highAlarmValue)
					{
						//�����ֵԤ��
						if(p_wptr->EnterHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
						{
							actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterHighAlarmAction);
							xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);		
						}
					}
					else if(p_wptr->LeaveHighAlarmAction && oldValue >= (u32)highAlarmValue && (u32)value < (u32)highAlarmValue)
					{
						//�뿪��ֵԤ��
						if(p_wptr->LeaveHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
						{
							actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveHighAlarmAction);
							xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
						}	
					}
				}
				if(dynamicType == 1)
				{
					sOldValue = (u32)oldValue;
					if(p_wptr->EnterLowAlarmAction && sOldValue > (s32)lowAlarmValue && (s32)value <= (s32)lowAlarmValue)
					{
						//�����ֵԤ��
						if(p_wptr->EnterLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
						{
							actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterLowAlarmAction);
							xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
						}		
					}
					else if(p_wptr->LeaveLowAlarmAction && sOldValue <= (s32)lowAlarmValue && (s32)value > (s32)lowAlarmValue)
					{
						//�뿪��ֵԤ��
						if(p_wptr->LeaveLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
						{
							actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveLowAlarmAction);
							xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
						}		
					}
					if(p_wptr->EnterHighAlarmAction && sOldValue < (s32)highAlarmValue && (s32)value >= (s32)highAlarmValue)
					{
						//�����ֵԤ��
						if(p_wptr->EnterHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
						{
							actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterHighAlarmAction);
							xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);		
						}
					}
					else if(p_wptr->LeaveHighAlarmAction && sOldValue >= (s32)highAlarmValue && (s32)value < (s32)highAlarmValue)
					{
						//�뿪��ֵԤ��
						if(p_wptr->LeaveHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
						{
							actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveHighAlarmAction);
							xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
						}	
					}
				}
					
			}
			p_wptr->WidgetAttr |= 0x8000;
			p_wptr->OldValueL = (u16)value;
			p_wptr->OldValueH = (u16)(value >> 16); 
		}
	}
	else if(dynamicType == TRANSLATION) //�������ƶ�
	{

	}
	else if(dynamicType == DIM) //����
	{
		if(value > maxValue) 
			value = maxValue;
		if(value < minValue)
			value = minValue;
		//tag��ʾ����alpha�����focused slice
		texturePtr = &gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex];
		texturePtr->FocusedSlice = value;

		//����ҳ��ˢ�������
		if(u8_pageRefresh == 0)
		{
			//send refresh message
#ifdef STATIC_BUFFER_EN
#ifdef AHMI_DEBUG
			ERROR_PRINT("sending the refresh static buffer cmd");
#endif
//		    refreshMsg.mElementType = ANIMAITON_REFRESH_STATIC_BUFFER;
//			refreshMsg.mElementPtr.pageptr = gPagePtr + WorkingPageID;
//			sendToRefreshQueue(&refreshMsg);
#endif
#ifndef WHOLE_TRIBLE_BUFFER
			refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
			refreshMsg.mElementPtr.wptr = p_wptr;
			sendToRefreshQueue(&refreshMsg);
#endif
			return AHMI_FUNC_SUCCESS;
		}
	}
	else
		return AHMI_FUNC_FAILURE;
	return AHMI_FUNC_SUCCESS;
}


#endif
