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
#include "MeterClass.h"
#include "publicInclude.h"
#include "AHMIBasicDefine.h"
#include "aniamtion.h"
#include "drawImmediately_cd.h"

#ifdef AHMI_CORE
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
//extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;

extern u32 startOfDynamicPage;
extern u32 endOfDynamicPage;


//-----------------------------
// �������� MeterClass
// ���캯��
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
MeterClass::MeterClass()
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
MeterClass::~MeterClass()
{

}

//-----------------------------
// �������� MeterClass::initWidget
// ��ʼ�������Ƹÿؼ��������ȫ��ˢ�£��ȸ������󶨵�tagˢ�¸ÿؼ����ڻ��Ƹÿؼ�
// �����б�
//  @param   WidgetClassPtr p_wptr,   //�ؼ�ָ��
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //ҳ��ˢ��
//  @param   TileBoxClassPtr pTileBox //��Χ��
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus MeterClass::initWidget(
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
//�Ǳ��̿��ƺ���
//�������ò�ͬ�����԰���3�Ż�8������
//������Ϊ��ģʽʱ������2����3�������ֱ��ǣ�
//0.���������Ǳ�������
//1.ָ����������ת��
//2.ָ�붥����Բ��(��ѡ)
//
//������Ϊ����ģʽʱ������12��13�������ֱ��ǣ�
//0.���������Ǳ�������
//1-4��ָ����Ĥ��������ֻ��ʾ�Ǳ����ϵ�ָ��
//5.ָ����������ת����ָ��ת���Ƕȵ�0��λ��Ϊ��ֱ���¡�
//           ָ������ĳ�ʼ�Ƕ���ת��ԭ������½ǣ���45�ȳ��֡�
//           ��ָ��ĳ�ʼ�Ƕ���Ҫ��MeterMinAngle��ʾ�Ƕȡ�
//6-10:����Ĥ����������ʾ���Ч����1��Ϊ���alpha buffer����������4������ֲ���������Ϊ����ԭ���4������
//11:���Ч������ҪDXT3��PNG��ʽ
//12.ָ�붥����Բ�㣨��ѡ��
//WidgetAttr��ʶ��
//15:�����ж�old_value�Ƿ��б仯
//14-9:����
//8:�Ƿ�������
//7:����ģʽ
//6:������ת����1��ʾ˳ʱ�룬0��ʾ��ʱ��
//5:�Ǳ���ģʽ��0Ϊ��ģʽ��1Ϊ����ģʽ
//4-0:�ؼ����ͣ��Ǳ���Ϊ0x3
//**************************************
funcStatus MeterClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//�ؼ�ָ��
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//ҳ��ˢ��
	)
{
	
	s32 value;
	
	u32 actionAddr;
//	RefreshMsg refreshMsg;
	u16 oldValueinit = 0;
	s32 oldValue = 0;
	s32 lowAlarmValue ;
	s32 highAlarmValue;

	if( NULL == p_wptr || NULL == ActionPtr || NULL == ActionPtr->mTagPtr){
		ERROR_PRINT("ERROR: widgetPtr or ActionPtr is NULL");
		return AHMI_FUNC_FAILURE;
	}
	if( NULL == gPagePtr->pBasicTextureList){
		ERROR_PRINT("ERROR: when drawing DashBoard widght, the texture list corrupt");
		return AHMI_FUNC_FAILURE;
	}

	oldValueinit = (p_wptr->WidgetAttr & 0x8000);
	lowAlarmValue  = (s32)((p_wptr->LowAlarmValueH  << 16) + p_wptr->LowAlarmValueL );
	highAlarmValue = (s32)((p_wptr->HighAlarmValueH << 16) + p_wptr->HighAlarmValueL);
	value = (s32)(ActionPtr->mTagPtr->mValue);

	if( (s16)(p_wptr->WidgetOffsetX) > MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetY) > MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetX) < -MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetY) < -MAX_WIDTH_AND_HEIGHT ||
		p_wptr->WidgetWidth > MAX_WIDTH_AND_HEIGHT ||
		p_wptr->WidgetHeight > MAX_WIDTH_AND_HEIGHT || 
		p_wptr->WidgetWidth == 0 ||  
		p_wptr->WidgetHeight == 0)
	{
		ERROR_PRINT("ERROR: when drawing DashBoard widght, the offset\\width\\height exceeds the boundary");
		return AHMI_FUNC_FAILURE;
	}
	if (     p_wptr->MeterMinAngle > MAX_ANGLE ||
		 (s16)p_wptr->MeterMinAngle < -MAX_ANGLE ||
		      p_wptr->MeterMaxAngle > MAX_ANGLE ||
		 (s16)p_wptr->MeterMaxAngle < -MAX_ANGLE ||
		(p_wptr->MeterMaxAngle < p_wptr->MeterMinAngle))
	{		
		ERROR_PRINT("ERROR: when drawing DashBoard widght, the Angle exceeds the boundary");
		return AHMI_FUNC_FAILURE;
	}
	//render the texture 
	if( ((p_wptr->TOTALFRAME_AND_NOWFRAME) >> 8) == 0) //no animation
	{
		renderTexture(p_wptr, value);	//���ݵ�ǰ��ֵ��meter��texture�����Խ���һ���޸�
	}
	else if(u8_pageRefresh)	  //�ж�����ʱ����Ҫˢ��ҳ���ʱ��ִ�иò���
	{
		//��Ҫˢ��ҳ���ʱ��, ʹ�ö�����ֵ����Ϊ�´ζ����ĳ�ʼֵ�� �����ݵ�ǰ��valueֵ����ˢ�¿ؼ���״̬ by Mr.z
		p_wptr->START_TAG_L = p_wptr->ANIMATIONCURVALUE_L;
		p_wptr->START_TAG_H = p_wptr->ANIMATIONCURVALUE_H;
		p_wptr->STOP_TAG_L = (u16)( (u32)value );
		p_wptr->STOP_TAG_H = (u16)(( (u32)value ) >> 16);
		//renderTexture(p_wptr, value);	     //��Ҫ��Ʒ��һ����֤������Ƿ�ɾ��
		//p_wptr->START_TAG_L =  (u16)( (u32)value );
		//p_wptr->START_TAG_H = (u16)(( (u32)value ) >> 16);
		//p_wptr->STOP_TAG_L =  (u16)( (u32)value );
		//p_wptr->STOP_TAG_H = (u16)(( (u32)value ) >> 16);
	}
	else //if( (p_wptr->TOTALFRAME_AND_NOWFRAME & 0xff) == 0) // first frame
	{
		p_wptr->START_TAG_L = p_wptr->STOP_TAG_L;
		p_wptr->START_TAG_H = p_wptr->STOP_TAG_H;
		p_wptr->STOP_TAG_L = (u16)( (u32)value );
		p_wptr->STOP_TAG_H = (u16)(( (u32)value ) >> 16);
	}

	if(u8_pageRefresh == 0)
	{
		//send refresh message
		if( (p_wptr->TOTALFRAME_AND_NOWFRAME >> 8) == 0)//no animation
		{
#ifndef WHOLE_TRIBLE_BUFFER
			refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
			refreshMsg.mElementPtr.wptr = p_wptr;
			sendToRefreshQueue(&refreshMsg);
#endif
		}
		else
		{
			p_wptr->mWidgetMatrix.matrixInit();
			p_wptr->loadTextureWithAnimation();
		}

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
			else if(p_wptr->LeaveLowAlarmAction && oldValue <= lowAlarmValue && value >lowAlarmValue)
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

//-----------------------------
// �������� MeterClass::initWidget
// compute the texture using the tag value
// �����б�
//  @param   WidgetClassPtr p_wptr,   //�ؼ�ָ��
//  @param	 u32 tagValue             //the value of the tag
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus MeterClass::renderTexture
	(
	WidgetClassPtr p_wptr,  //�ؼ�ָ��
	s32 value
	)
{
	s32	maxValue;
	s32	minValue;
	
	s16 Angle;


	

	maxValue = (s32)((p_wptr->MaxValueH << 16) + p_wptr->MaxValueL);
	minValue = (s32)((p_wptr->MinValueH << 16) + p_wptr->MinValueL);
	


	//todo:  ��Ҫ�޸ĺ����޸��жϺ���
	//if(MeterMode == 0 && p_wptr->NumOfTex!=3)
	//{
	//	return AHMI_FUNC_FAILURE;
	//	;//��ģʽ����0= 
	//}
	//if(MeterMode == 1 && p_wptr->NumOfTex!=8)
	//{
	//	return AHMI_FUNC_FAILURE;
	//	;//����ģʽ����
	//}

	if(value > maxValue) 
		value = maxValue;
	if(value < minValue) 
		value = minValue;
	if(maxValue == minValue){
		ERROR_PRINT("ERROR: draw dashboard error, divided by zero");
		return AHMI_FUNC_FAILURE;
	}
	Angle = 16* (value - minValue) * ( (s16)(p_wptr->MeterMaxAngle) - (s16)(p_wptr->MeterMinAngle) ) /(maxValue - minValue);//12.4

	renderTextureAngle(
		 p_wptr,  //�ؼ�ָ��
		 Angle
	);

	
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// �������� MeterClass::renderTextureAngle
// set the texture's angle
// �����б�
//  @param   WidgetClassPtr p_wptr,   //�ؼ�ָ��
//  @param	 u32 tagValue             //the value of the tag
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus MeterClass::renderTextureAngle
		 (
		 WidgetClassPtr p_wptr,  //�ؼ�ָ��
		 s16 Angle
		 )
{
	
	

	u8 MeterMode = (u8)(( p_wptr->WidgetAttr & 0xC0 ) >> 6);
	u8 rotatingDir = ( p_wptr->WidgetAttr & 0x20 ) ?1:0;
	u8 pointerMask = ( p_wptr->WidgetAttr & 0x100 ) ?1:0;
	//u8 meterSimplify = ( p_wptr->WidgetAttr & 0x80 ) ?1:0;
	u8 PointerPtr;
	//u8 StartPoint;
	
	s16 startAngle;
	s16 ShearAngleX;				//12.4
	s16 ShearAngleY;
	s16 pureMaskAdjustAngle;    //puremask�ĵ����Ƕ�

	s16 RotateAngle;			//12.4
	u16 ScaleX,ScaleY;
	
	u16 centralLenght = ((p_wptr->WidgetWidth) * 3)/8;
	
	

	TextureClassPtr TexturePtr;
	TileBoxClass meterBox;

	Angle += ((s16)(p_wptr->MeterMinAngle)) << 4;//12.4 ������Ǳ�����С�Ƕȵ�ƫ����

	TexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);

	/*if((TexturePtr < (void*)startOfDynamicPage) || (TexturePtr > (void*)endOfDynamicPage))
		return AHMI_FUNC_FAILURE;*/
	//0����ģʽ 1������ģʽ 2:����ģʽ   3��˫��ģʽ��˫��������
	if(MeterMode == 2)
		PointerPtr = 6;
	else if(MeterMode == 1|| MeterMode == 3)//����ģʽ
		if(p_wptr->NumOfTex == 7 || p_wptr->NumOfTex == 12){  //������������tex���� by Mr.z
			PointerPtr = 6;	 
		}else{
			PointerPtr = 7;
		}
	else if(MeterMode == 0){
		if(p_wptr->NumOfTex == 1 || p_wptr->NumOfTex == 6){	 //������������tex���� by Mr.z
			PointerPtr = 0;	 //��ģʽ��ָ�����texture 0
		}else{
			PointerPtr = 1;
		}
	}
	if(pointerMask)
		PointerPtr += 5;

	//����ڴ�ֱ�Ƕȵ�startangle
	if(MeterMode == 2)
	{
		if(rotatingDir)
			startAngle = (s16)(p_wptr->StartAngle);
		else 
			startAngle = -(s16)(p_wptr->StartAngle);
	}

	else
	{
		if(rotatingDir)
			startAngle = (s16)(p_wptr->StartAngle) + (45 * 16); //��ʼ�Ƕ�
		else 
			startAngle = -(s16)(p_wptr->StartAngle) + (45 * 16);
	}

	pureMaskAdjustAngle = (s16)(p_wptr->StartAngle) + (((s16)(p_wptr->MeterMinAngle)) << 4); //12.4

	if(rotatingDir)//˳ʱ��
		{
			TexturePtr[PointerPtr].RotateAngle = Angle + startAngle;//�������ָ������ĳ�ʼ�Ƕ�,//12.4
			TexturePtr[PointerPtr].mTexAttr |= ABCDEFMATRIX;
			TexturePtr[PointerPtr].mTexAttr |= USING_PIXEL_RATIO;//pointer needs to be fixed, by zuz20180802
			Angle = TexturePtr[PointerPtr].RotateAngle - ((s16)(p_wptr->MeterMinAngle) << 4) - startAngle;// + (45*16);//////ָ��ǶȺ���ʼ�Ƕ�֮����ԽǶ�  ��СС������
		}
	else //��ʱ��
		{
			TexturePtr[PointerPtr].RotateAngle = -Angle + startAngle;
			TexturePtr[PointerPtr].mTexAttr |= ABCDEFMATRIX;
			TexturePtr[PointerPtr].mTexAttr |= USING_PIXEL_RATIO;//pointer needs to be fixed, by zuz20180802
			Angle =   TexturePtr[PointerPtr].RotateAngle + ((s16)(p_wptr->MeterMinAngle)<<4) - startAngle;// - 45*16; //��ֵΪָ����ת�ĽǶ�
	   }
	  //StartPoint = ((startAngle + 45*16 ) / 1440) % 4;///////��ʼ��ת�Ƕ��������� 
	
	if(MeterMode == 2)//����ģʽ
	{
		//����ָ���ƫ����
		//TexturePtr[PointerPtr].RotateAngle += (45 * 16);
		TexturePtr[PointerPtr].adjustMoving(centralLenght,(( (p_wptr->WidgetWidth)/2 + p_wptr->WidgetOffsetX) << 4),(( (p_wptr->WidgetHeight)/2 + p_wptr->WidgetOffsetY) << 4) );
	}
	//�Ż�ָ���Χ��
	TexturePtr[PointerPtr].renewTextureSourceBox(NULL,NULL,NULL);

	//if(rotatingDir)//˳ʱ����ת,������0-360
	//{
	//	while(Angle < 0)
	//		Angle += 16 * 360;
	//	while(Angle > 360 * 16)
	//		Angle -= 16 * 360;
	//}
	//else //��ʱ����ת��������-360 - 0
	//{
	//	while(Angle < -360 * 16)
	//		Angle += 16 * 360;
	//	while(Angle > 0)
	//		Angle -= 16 * 360;
	//}

	if(MeterMode == 1)//����ģʽ
	{
		TexturePtr[1].mTexAttr |= TEXTURE_USING_WIDGET_BOX;
		if(rotatingDir)//˳ʱ����ת
		{
			
			for(PointerPtr = 1  ;PointerPtr<5;PointerPtr++)
			{
				if(Angle>= 16 * 90 * (PointerPtr)) //ָ��Խ����ǰ���ޣ���������ʾ����
				{
					ShearAngleX = 0;//����������
					ShearAngleY = 0;
					RotateAngle = 16 * 90 * (PointerPtr );//��ת��������λ��
					ScaleX = 512;
					ScaleY = 512;
				}
				else if(Angle<= 16 * 90 * ((PointerPtr ) - 1 ))//ָ��δ��������ޣ������޲���ʾ
				{
					ShearAngleX = 0 ;//����������
					ShearAngleY = 0;
					RotateAngle = 16 * 90 * (PointerPtr);//��ת��������λ��
					ScaleX = 0xf;//��Сmask��ʹ�ø����޲���ʾ
					ScaleY = 0xf;
				}
				else //ָ��ͣ���ڸ�������
				{
					ShearAngleX = 90*16*(PointerPtr ) - Angle;
					if(ShearAngleX >= 89*16)
						ShearAngleX = 89*16;
					if(ShearAngleX <= -89*16)
						ShearAngleX = -89*16;
					ShearAngleY = 0;
					RotateAngle = 16 * 90 * ((PointerPtr ));//��ת��������λ��
					ScaleX = 512;
					ScaleY = 512;
				}
				TexturePtr[(PointerPtr - 1)%4 + 2 ].RotateAngle = RotateAngle + pureMaskAdjustAngle;
				TexturePtr[(PointerPtr - 1)%4 + 2 ].ShearAngleX = ShearAngleX;
				TexturePtr[(PointerPtr - 1)%4 + 2 ].ShearAngleY = ShearAngleY;
				TexturePtr[(PointerPtr - 1)%4 + 2 ].ScalerX = ScaleX;
				TexturePtr[(PointerPtr - 1)%4 + 2 ].ScalerY = ScaleY;
				TexturePtr[(PointerPtr - 1)%4 + 2 ].renewTextureSourceBox(NULL,NULL,NULL);
			}
		}
		else //��ʱ����ת
		{
			
			for(PointerPtr = (1 );PointerPtr<(5 );PointerPtr++)
			{
				if( (s16)Angle <= (s16)(- 16 * 90 * (PointerPtr )) ) //ָ��Խ����ǰ���ޣ���������ʾ����
				{
					ShearAngleX = 0;//����������
					ShearAngleY = 0;//����������
					RotateAngle = - 16 * 90 * (PointerPtr - 1);//��ת��������λ��
					ScaleX = 512;
					ScaleY = 512;
				}
				else if( (s16)Angle>= (s16)(- 16 * 90 * (PointerPtr - 1 ) ))//ָ��δ��������ޣ������޲���ʾ
				{
					ShearAngleX = 0;//����������
					ShearAngleY = 0;//����������
					RotateAngle = -16 * 90 * (PointerPtr - 1 );//��ת��������λ��
					ScaleX = 0xf;//��Сmask��ʹ�ø����޲���ʾ
					ScaleY = 0xf;
				}
				else //ָ��ͣ���ڸ�������
				{
					ShearAngleX = 0;
					ShearAngleY = Angle + PointerPtr * 90 * 16;
					if(ShearAngleY >= 89*16)
						ShearAngleY = 89*16;
					if(ShearAngleY <= -89*16)
						ShearAngleY = -89*16;
					RotateAngle = -16 * 90 * (PointerPtr - 1) + ShearAngleX;//��ת��������λ��
					ScaleX = 512;
					ScaleY = 512;
				}
				TexturePtr[PointerPtr + 1].RotateAngle = RotateAngle - pureMaskAdjustAngle;
				TexturePtr[PointerPtr + 1].ShearAngleX = ShearAngleX;
				TexturePtr[PointerPtr + 1].ShearAngleY = ShearAngleY;
				TexturePtr[PointerPtr + 1].ScalerX = ScaleX;
				TexturePtr[PointerPtr + 1].ScalerY = ScaleY;
				TexturePtr[PointerPtr + 1].renewTextureSourceBox(NULL,NULL,NULL);
			}
		}
	}
	else if(MeterMode == 2)//����ģʽ
	{
		TexturePtr[0].mTexAttr |= TEXTURE_USING_WIDGET_BOX;
		if(rotatingDir)//˳ʱ����ת
		{
			for(PointerPtr = 1  ;PointerPtr<5;PointerPtr++)
			{
				if(Angle>= 16 * 90 * (PointerPtr)) //ָ��Խ����ǰ���ޣ���������ʾ����
				{
					ShearAngleX = 0;//����������
					ShearAngleY = 0;//����������
					RotateAngle = 16 * 90 * (PointerPtr );//��ת��������λ��
					ScaleX = 512;
					ScaleY = 512;
				}
				else if(Angle<= 16 * 90 * ((PointerPtr ) - 1 ))//ָ��δ��������ޣ������޲���ʾ
				{
					ShearAngleX = 0;//����������
					ShearAngleY = 0;//����������
					RotateAngle = 16 * 90 * (PointerPtr);//��ת��������λ��
					ScaleX = 0xf;//��Сmask��ʹ�ø����޲���ʾ
					ScaleY = 0xf;
				}
				else //ָ��ͣ���ڸ�������
				{
					ShearAngleX = 90*16*(PointerPtr ) - Angle;
					if(ShearAngleX >= 89*16)
						ShearAngleX = 89*16;
					if(ShearAngleX <= -89*16)
						ShearAngleX = -89*16;
					ShearAngleY = 0;
					RotateAngle = 16 * 90 * ((PointerPtr ));//��ת��������λ��
					ScaleX = 512;
					ScaleY = 512;
				}
				TexturePtr[(PointerPtr - 1)%4 + 1 ].RotateAngle = RotateAngle + pureMaskAdjustAngle;
				TexturePtr[(PointerPtr - 1)%4 + 1 ].ShearAngleX = ShearAngleX;
				TexturePtr[(PointerPtr - 1)%4 + 1 ].ShearAngleY = ShearAngleY;
				TexturePtr[(PointerPtr - 1)%4 + 1 ].ScalerX = ScaleX;
				TexturePtr[(PointerPtr - 1)%4 + 1 ].ScalerY = ScaleY;
				TexturePtr[(PointerPtr - 1)%4 + 1 ].renewTextureSourceBox(NULL,NULL,NULL);
			}
		}
		else //��ʱ����ת
		{
			
			for(PointerPtr = (1 );PointerPtr<(5 );PointerPtr++)
			{
				if( (s16)Angle <= (s16)(- 16 * 90 * (PointerPtr )) ) //ָ��Խ����ǰ���ޣ���������ʾ����
				{
					ShearAngleX = 0;//����������
					ShearAngleY = 0;//����������
					RotateAngle = - 16 * 90 * (PointerPtr - 1);//��ת��������λ��
					ScaleX = 512;
					ScaleY = 512;
				}
				else if( (s16)Angle>= (s16)(- 16 * 90 * (PointerPtr - 1 ) ))//ָ��δ��������ޣ������޲���ʾ
				{
					ShearAngleX = 0;//����������
					ShearAngleY = 0;//����������
					RotateAngle = -16 * 90 * (PointerPtr - 1 );//��ת��������λ��
					ScaleX = 0xf;//��Сmask��ʹ�ø����޲���ʾ
					ScaleY = 0xf;
				}
				else //ָ��ͣ���ڸ�������
				{
					ShearAngleX = Angle + PointerPtr * 90 * 16;
					if(ShearAngleX >= 89*16)
						ShearAngleX = 89*16;
					RotateAngle = -16 * 90 * (PointerPtr - 1) + ShearAngleX;//��ת��������λ��
					if(ShearAngleX >= 89*16)
						ShearAngleX = 89*16;
					ScaleX = 512;
					ScaleY = 512;
				}
				TexturePtr[PointerPtr].RotateAngle = RotateAngle - pureMaskAdjustAngle;
				TexturePtr[PointerPtr].ShearAngleX = ShearAngleX;
				TexturePtr[PointerPtr].ScalerX = ScaleX;
				TexturePtr[PointerPtr].ScalerY = ScaleY;
				TexturePtr[PointerPtr].renewTextureSourceBox(NULL,NULL,NULL);
			}
		}
	}

	return AHMI_FUNC_SUCCESS;

}

funcStatus MeterClass::setATag(
	WidgetClassPtr p_wptr,   //�ؼ�ָ��
	u8 value                 //value of animation tag
	)
{
	//u16 transitionParamIn; //0.10
	//u16 transitionParamOut; //0.10
	u8 totalFrame;
	s32 ValueStart;
	s32 ValueStop;
	s32 maxValue;
	s32 minValue;
	s32 curValue;
	s16 Angle;
	s16 startAngle;
	s16 stopAngle;
#ifndef WHOLE_TRIBLE_BUFFER
	RefreshMsg refreshMsg;
#endif

	totalFrame = (u8)( (p_wptr->TOTALFRAME_AND_NOWFRAME) >> 8);

	if(totalFrame == 0 ) //no animation
		return AHMI_FUNC_SUCCESS;

	taskENTER_CRITICAL();
#ifdef AHMI_DEBUG
	char text[100];
	sprintf(text,"set meter animation tag %d", value);
	ERROR_PRINT(text);
#endif
	p_wptr->TOTALFRAME_AND_NOWFRAME |= value;
	ValueStart = (s32)((p_wptr->START_TAG_L) +  ( (p_wptr->START_TAG_H) << 8));
	ValueStop = (s32)((p_wptr->STOP_TAG_L) + ( (p_wptr->STOP_TAG_H) << 8));
	maxValue = (s32)((p_wptr->MaxValueH << 16) + p_wptr->MaxValueL);
	minValue = (s32)((p_wptr->MinValueH << 16) + p_wptr->MinValueL);

	//transitionParamIn = (value) * 1024 / totalFrame;//0.10
	//gPagePtr[WorkingPageID].inoutTransitioin(transitionParamIn, transitionParamOut);

	curValue = ValueStart + (ValueStop - ValueStart) * value / totalFrame;

	Angle = 16 * (value) * ( (s16)(p_wptr->MeterMaxAngle) - (s16)(p_wptr->MeterMinAngle) ) * (ValueStop - ValueStart) /(totalFrame * (maxValue - minValue)) + 16 * (ValueStart) * ( (s16)(p_wptr->MeterMaxAngle) - (s16)(p_wptr->MeterMinAngle) ) / (maxValue - minValue);//12.4

	p_wptr->OldValueL = (u16)curValue;
	p_wptr->OldValueH = (u16)(curValue >> 16); 
	
	//��¼��ǰ�Ķ���ִ�е�ֵ by Mr.z
	p_wptr->ANIMATIONCURVALUE_L = (u16)curValue;
	p_wptr->ANIMATIONCURVALUE_H = (u16)(curValue >> 16); 
	//renderTexture(p_wptr, curValue);
	renderTextureAngle(p_wptr, Angle);

	if(value == totalFrame)
		p_wptr->TOTALFRAME_AND_NOWFRAME &= 0xff00; // set now frame to 0
#ifndef WHOLE_TRIBLE_BUFFER
	refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
	refreshMsg.mElementPtr.wptr = p_wptr;
	sendToRefreshQueue(&refreshMsg);
#endif
	taskEXIT_CRITICAL();
	return AHMI_FUNC_SUCCESS;
}



#endif
