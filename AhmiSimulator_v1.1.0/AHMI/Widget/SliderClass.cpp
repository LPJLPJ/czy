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
#include "SliderClass.h"

#ifdef AHMI_CORE
extern ActionTriggerClass		gWidgetTagTrigger;
extern ActionTriggerClass		gMouseTagTrigger;
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
//extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;

extern u32 startOfDynamicPage;
extern u32 endOfDynamicPage;

//���
extern PIDPoint pressPoint;
extern PIDPoint releasePoint;
//���Ŀǰ״̬
extern u8 mouseStatus;


//-----------------------------
// �������� MeterClass
// ���캯��
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
SliderClass::SliderClass()
{

}

//-----------------------------
// �������� ~MeterClass
// ��������
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
SliderClass::~SliderClass()
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
funcStatus SliderClass::initWidget(
	WidgetClassPtr p_wptr,   //�ؼ�ָ��
	u32 *u32p_sourceShift,   //sourcebufferָ��
	u8 u8_pageRefresh,       //ҳ��ˢ��
	u8 RefreshType ,       //���ƵĶ������ͣ����ݶ������͸ı���ƿؼ��İ�Χ��
	TileBoxClassPtr pTileBox, //��Χ��
	u8 staticTextureEn          //�Ƿ���Ƶ���̬�洢�ռ�
	)
{
//	u32 value;
	TagClassPtr bindTag;
	ActionTriggerClass tagtrigger;
	WidgetClassInterface myWidgetClassInterface;

	if((NULL == p_wptr) || (NULL == u32p_sourceShift) || (NULL == pTileBox))
		return AHMI_FUNC_FAILURE;

	bindTag = &TagPtr[p_wptr->BindTagID];
//	value = bindTag->getValue();

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

//******************************************
//������ƺ���
//��Ҫ3������
//��һ������Ϊ
//�ڶ�������Ϊ
//����������Ϊ
//WidgetAttr��ʶ��
//15-5:����
// 5��ʾ��������� 0��ʾ���� 1��ʾ����
//4-0:�ؼ����ͣ�����Ϊ0x5
//******************************************
funcStatus SliderClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//�ؼ�ָ��
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//ҳ��ˢ��
	)
{	
	s32 tag_value= 0;
	s32 cur_x;
	s32 cur_y;
	u32 actionAddr;
	s32 oldValue=0;
	s32 value;
	s32	maxValue;
	s32	minValue;
	s32 lowAlarmValue ;
	s32 highAlarmValue;

	u16 HalfWidth1;
	u16 Width0;
	u16 type;
	u16 oldValueinit=0;

	TextureClassPtr texturePtr;
//	RefreshMsg refreshMsg;

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
		ERROR_PRINT("ERROR: when drawing slider widght, the offset\\width\\height exceeds the boundary");
		return AHMI_FUNC_FAILURE;
	}

	texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
	//cur_x = (ActionPtr->mMousePID.x) << 4;
	//cur_y = (ActionPtr->mMousePID.y) << 4;
	type  = (p_wptr->WidgetAttr & 0x20)  ;
	oldValueinit = (p_wptr->WidgetAttr & 0x8000);
	value = (s32)(ActionPtr->mTagPtr->mValue);

	maxValue = (s32)((p_wptr->MaxValueH << 16) + p_wptr->MaxValueL);
	minValue = (s32)((p_wptr->MinValueH << 16) + p_wptr->MinValueL);
	lowAlarmValue  = (s32)((p_wptr->LowAlarmValueH  << 16) + p_wptr->LowAlarmValueL );
	highAlarmValue = (s32)((p_wptr->HighAlarmValueH << 16) + p_wptr->HighAlarmValueL);

	if(maxValue == minValue){
		ERROR_PRINT("ERROR: can't be divided by zero");
		return AHMI_FUNC_FAILURE;
	}

#ifdef EMBEDDED
	if((texturePtr < (void*)startOfDynamicPage) || (texturePtr > (void*)endOfDynamicPage))
			return AHMI_FUNC_FAILURE;
#endif

	if( ActionPtr->mInputType == ACTION_MOUSE_PRESS )
	{
		cur_x  = ((s16)(pressPoint.x) << 4);
		cur_y  = ((s16)(pressPoint.y) << 4);
		if(type==0)///����
		{
			HalfWidth1 = (texturePtr[1].TexWidth/2) <<4;
			Width0 = texturePtr[0].TexWidth << 4;
	
			if((cur_x - texturePtr[0].OffsetX) > HalfWidth1)
				{
				if(( texturePtr[0].OffsetX + Width0 - cur_x) < HalfWidth1)
					texturePtr[1].OffsetX =   texturePtr[0].OffsetX + Width0 - HalfWidth1*2;		/////////�ұ�����
				else 
					texturePtr[1].OffsetX =   cur_x - HalfWidth1;
				}
			else 	/////////�������
				texturePtr[1].OffsetX =   texturePtr[0].OffsetX;

			tag_value = (texturePtr[1].OffsetX - texturePtr[0].OffsetX) * maxValue / ( Width0 - HalfWidth1*2) + minValue ; //������ֵ
		}
		else  //����
		{
			HalfWidth1 = (texturePtr[1].TexHeight/2) <<4;
			Width0 = texturePtr[0].TexHeight << 4;

			if((cur_y - texturePtr[0].OffsetY) > HalfWidth1)
				{
				if(( texturePtr[0].OffsetY + Width0 - cur_y) < HalfWidth1)
					texturePtr[1].OffsetY =   texturePtr[0].OffsetY + Width0 - HalfWidth1*2;		/////////�±�����
				else 
					texturePtr[1].OffsetY =   cur_y - HalfWidth1;
				}
			else 	/////////�ϱ�����
				texturePtr[1].OffsetY =   texturePtr[0].OffsetY;


			tag_value = (( Width0 - HalfWidth1*2) - texturePtr[1].OffsetY + texturePtr[0].OffsetY) * maxValue / ( Width0 - HalfWidth1*2) + minValue ; //������ֵ
		}

		TagPtr[p_wptr->BindTagID].setValue(tag_value,p_wptr->BindTagID);
		return AHMI_FUNC_SUCCESS;

	}

	else if(ActionPtr->mInputType == ACTION_MOUSE_HOLDING)
	{
		cur_x  = ((s16)(releasePoint.x)) << 4;
		cur_y  = ((s16)(releasePoint.y)) << 4;
		if(type==0)///����
		{
				HalfWidth1 = (texturePtr[1].TexWidth/2) <<4;
				Width0 = texturePtr[0].TexWidth << 4;
	
				if((cur_x - texturePtr[0].OffsetX) > HalfWidth1)
					{
					if(( texturePtr[0].OffsetX + Width0 - cur_x) < HalfWidth1)
						texturePtr[1].OffsetX =   texturePtr[0].OffsetX + Width0 - HalfWidth1*2;		/////////�ұ�����
					else 
						texturePtr[1].OffsetX =   cur_x - HalfWidth1;
					}
				else 	/////////�������
					texturePtr[1].OffsetX =   texturePtr[0].OffsetX;

				tag_value = (texturePtr[1].OffsetX - texturePtr[0].OffsetX) * maxValue / ( Width0 - HalfWidth1*2) + minValue ; //������ֵ
		}
		else  //����
		{
				HalfWidth1 = (texturePtr[1].TexHeight/2) <<4;
				Width0 = texturePtr[0].TexHeight << 4;

				if((cur_y - texturePtr[0].OffsetY) > HalfWidth1)
					{
					if(( texturePtr[0].OffsetY + Width0 - cur_y) < HalfWidth1)
						texturePtr[1].OffsetY =   texturePtr[0].OffsetY + Width0 - HalfWidth1*2;		/////////�±�����
					else 
						texturePtr[1].OffsetY =   cur_y - HalfWidth1;
					}
				else 	/////////�ϱ�����
					texturePtr[1].OffsetY =   texturePtr[0].OffsetY;

				tag_value = (( Width0 - HalfWidth1*2) - texturePtr[1].OffsetY + texturePtr[0].OffsetY) * maxValue / ( Width0 - HalfWidth1*2) + minValue ;
		}
		TagPtr[p_wptr->BindTagID].setValue(tag_value,p_wptr->BindTagID);
		return AHMI_FUNC_SUCCESS;

	}
	else if(ActionPtr->mInputType == ACTION_TAG_SET_VALUE)
	{
		if(value > maxValue)
			value = maxValue;
		else if(value < minValue)
			value = minValue;

		

		if(type==0)///����
			{
					HalfWidth1 = (texturePtr[1].TexWidth/2) <<4;
					Width0 = texturePtr[0].TexWidth << 4;
					texturePtr[1].OffsetX = texturePtr[0].OffsetX + (value - minValue) * (Width0 - HalfWidth1 *2)  / (maxValue - minValue);

			}
			else  //����
			{
					HalfWidth1 = (texturePtr[1].TexHeight/2) <<4;
					Width0 = texturePtr[0].TexHeight << 4;
					texturePtr[1].OffsetY = texturePtr[0].OffsetY + ( Width0 - HalfWidth1*2) - (value - minValue) * (Width0 - HalfWidth1 *2) / (maxValue - minValue);
			}
	}


	if(u8_pageRefresh == 0 && ActionPtr->mInputType == ACTION_TAG_SET_VALUE)
	{
		//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
		refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
		refreshMsg.mElementPtr.wptr = p_wptr;
		sendToRefreshQueue(&refreshMsg);
#endif
		if(oldValueinit)
		{
			oldValue = (s32)((p_wptr->OldValueH  << 16) + p_wptr->OldValueL);
			if(p_wptr->EnterLowAlarmAction && oldValue > lowAlarmValue && value <= lowAlarmValue)
			{
				//�����ֵԤ��
				if(p_wptr->EnterLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
				{
					actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterLowAlarmAction);
					xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
				}		
			}
			else if(p_wptr->LeaveLowAlarmAction && oldValue <= lowAlarmValue && value > lowAlarmValue)
			{
				//�뿪��ֵԤ��
				if(p_wptr->LeaveLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
				{
					actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveLowAlarmAction);
					xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
				}		
			}
			if(p_wptr->EnterHighAlarmAction && oldValue < highAlarmValue && value >= highAlarmValue)
			{
				//�����ֵԤ��
				if(p_wptr->EnterHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
				{
					actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterHighAlarmAction);
					xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);		
				}
			}
			else if(p_wptr->LeaveHighAlarmAction && oldValue >= highAlarmValue && value < highAlarmValue)
			{
				//�뿪��ֵԤ��
				if(p_wptr->LeaveHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
				{
					actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveHighAlarmAction);
					xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
				}	
			}
			//tagchange��֧�� by Mr.z
			if(oldValue != value){
				actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->TagChangeAction);
				xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
			}
		}
		p_wptr->WidgetAttr |= 0x8000;
		p_wptr->OldValueL = (u16)value;
		p_wptr->OldValueH = (u16)(value >> 16); 
	}
	return AHMI_FUNC_SUCCESS;
}

#endif
