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
#include "ProgBarClass.h"
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
// �������� ProgBarClass
// ���캯��
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
ProgBarClass::ProgBarClass()
{

}

//-----------------------------
// �������� ~ProgBarClass
// ��������
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
ProgBarClass::~ProgBarClass()
{

}

//-----------------------------
// �������� ProgBarClass::initWidget
// ��ʼ�����������ֿ������ȫ��ˢ�£��ȸ������󶨵�tagˢ�¸ÿؼ����ڻ��Ƹÿؼ�
//  @param   WidgetClassPtr p_wptr,   //�ؼ�ָ��
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //ҳ��ˢ��
//  @param   TileBoxClassPtr pTileBox //��Χ��
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus ProgBarClass::initWidget(
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
//���������ƺ���
//��Ҫ3��4��5������
//0������
//1��puremask����ʾ���ǵķ�Χ
//2: ����������
//3-4: (��ѡ)ָ�������Լ�ָ��alpha
//WidgetAttr��ʶ��
//15:�ж�old value��û�г�ʼ��
//14-10:����
//9:�Ƿ���ʾ��ʾ��
//8:ָ������  ��0Ϊ����ɫָ�룬1��ʾ��ɫָ��
//7:���������ͣ�0��ʾΪ��ͨ��������1��ʾ��ɫ������
//6:�Ƿ��й�꣬1��ʾ�У�0��ʾû��
//5:����������1Ϊ����0Ϊ����
//4-0:�ؼ����ͣ�������ӦΪ0x4
//******************************************
funcStatus ProgBarClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//�ؼ�ָ��
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//ҳ��ˢ��
	)
{
	s32 value;
	//	s32	maxValue;
	//	s32	minValue;
		s32 lowAlarmValue ;
		s32 highAlarmValue;
	//	s32 threshold0;
	//	s32 threshold1;
		u32 actionAddr;
	//
	//	u16 width;
	//	u16 height;
	//	u16 newWidth;
	//	u16 newHeight;
	//	u16 offsetX;
	//	u16 offsetY;
	//	u16 rightBox;
	//	u16 topBox;
	//	TextureClassPtr texturePtr;
	//	
		s32 oldValue;
		u16 oldValueinit;
	//
	//	u8 dir;
	//	u8 lineFileEn;//�Ƿ��й��
	//	u8 lineChangeColor; //ָ���Ƿ��л���ɫ
	//	u16 changeColorMode; //�л���ɫģʽ��0��ʾ����,1��ʾͻ��
	//
	//	u8 changeColor;
	//	u8 startR;
	//	u8 startG;
	//	u8 startB;
	//	u8 endR;
	//	u8 endG;
	//	u8 endB;
	//	u8 curR;
	//	u8 curG;
	//	u8 curB;
	//	u16 barColor;
	//	u16 barPtr;
	//	s16 differenceOfR;
	//	s16 differenceOfG;
	//	s16 differenceOfB;
	//	u16 thresholdNum;
	//
		u8 display;
	//	u16 differenceOfValue;
	//
	////	RefreshMsg refreshMsg;
	//
	if((NULL == p_wptr) || (NULL == ActionPtr) || NULL == ActionPtr->mTagPtr || NULL == gPagePtr->pBasicTextureList){
		ERROR_PRINT("ERROR: for NULL pointer");
		return AHMI_FUNC_FAILURE;
	}

	if( (s16)(p_wptr->WidgetOffsetX) > MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetY) > MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetX) < -MAX_WIDTH_AND_HEIGHT || 
		(s16)(p_wptr->WidgetOffsetY) < -MAX_WIDTH_AND_HEIGHT ||
		p_wptr->WidgetWidth > MAX_WIDTH_AND_HEIGHT ||
		p_wptr->WidgetHeight > MAX_WIDTH_AND_HEIGHT )
	{
		ERROR_PRINT("ERROR: when drawing progressbar widght, the offset\\width\\height exceeds the boundary");
		return AHMI_FUNC_FAILURE;
	}

	value = (s32)ActionPtr->mTagPtr->mValue;

	//render the texture 
	if( ((p_wptr->TOTALFRAME_AND_NOWFRAME) >> 8) == 0) //no animation
		renderTexture(p_wptr,value);
	else if(u8_pageRefresh)
	{
		//��Ҫˢ��ҳ���ʱ��, ʹ�ö�����ֵ����Ϊ�´ζ����ĳ�ʼֵ�� �����ݵ�ǰ��valueֵ����ˢ�¿ؼ���״̬ by Mr.z
		p_wptr->START_TAG_L = p_wptr->ANIMATIONCURVALUE_L;
		p_wptr->START_TAG_H = p_wptr->ANIMATIONCURVALUE_H;
		p_wptr->STOP_TAG_L = (u16)( (u32)value );
		p_wptr->STOP_TAG_H = (u16)(( (u32)value ) >> 16);
		//renderTexture(p_wptr, value);	 //��Ҫ��Ʒ��һ����֤������Ƿ�ɾ��
		//p_wptr->START_TAG_L =  (u16)( (u32)value );
		//p_wptr->START_TAG_H = (u16)(( (u32)value ) >> 16);
		//p_wptr->STOP_TAG_L =  (u16)( (u32)value );
		//p_wptr->STOP_TAG_H = (u16)(( (u32)value ) >> 16);
	}
	else if( (p_wptr->TOTALFRAME_AND_NOWFRAME & 0xff) == 0) // first frame
	{
		p_wptr->START_TAG_L = p_wptr->STOP_TAG_L;
		p_wptr->START_TAG_H = p_wptr->STOP_TAG_H;
		p_wptr->STOP_TAG_L = (u16)( (u32)value );
		p_wptr->STOP_TAG_H = (u16)(( (u32)value ) >> 16);
	}

	display = (p_wptr->WidgetAttr & PROBARDISPLAY)?1:0;
	oldValueinit = (p_wptr->WidgetAttr & 0x8000) ? 1 : 0;

	lowAlarmValue  = (s32)((p_wptr->LowAlarmValueH  << 16) + p_wptr->LowAlarmValueL ) ;
	highAlarmValue = (s32)((p_wptr->HighAlarmValueH << 16) + p_wptr->HighAlarmValueL) ;

	if(display)
	{
		if(u8_pageRefresh == 0)
		{
			if( ( (p_wptr->TOTALFRAME_AND_NOWFRAME) & 0xff00 ) == 0 ) //no animation
			{
			//send refresh message
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
		}
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
		//֧��tagchange by Mr.z
		if(oldValue != value){
			actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->TagChangeAction);
			xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
		}
	}
	p_wptr->WidgetAttr |= 0x8000;
	p_wptr->OldValueL = (u16)value;
	p_wptr->OldValueH = (u16)(value >> 16);

	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// �������� ProgBarClass::renderTexture
// render the texture of progbar by tag value
//  @param   WidgetClassPtr p_wptr,   //�ؼ�ָ��
//  @param	 u32 value                // the value of tag
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus ProgBarClass::renderTexture
	(
		WidgetClassPtr p_wptr,   //�ؼ�ָ��
		s32 value                //the value of tag
	)
{
	s32	maxValue;
	s32	minValue;
	//s32 lowAlarmValue ;
	//s32 highAlarmValue;
	s32 threshold0;
	s32 threshold1;
//	u32 actionAddr;

	u16 width;
	u16 height;
	u16 newWidth;
	u16 newHeight;
	u16 offsetX;
	u16 offsetY;
	u16 rightBox;
	u16 topBox;
	TextureClassPtr texturePtr;

	//s32 oldValue;
	//u16 oldValueinit;

	u8 dir;
	u8 lineFileEn;//�Ƿ��й��
	u8 lineChangeColor; //ָ���Ƿ��л���ɫ
	u16 changeColorMode; //�л���ɫģʽ��0��ʾ����,1��ʾͻ��

	u8 changeColor;
	u8 startR;
	u8 startG;
	u8 startB;
	u8 endR;
	u8 endG;
	u8 endB;
	u8 curR;
	u8 curG;
	u8 curB;
	u16 barColor;
	u16 barPtr;
	s16 differenceOfR;
	s16 differenceOfG;
	s16 differenceOfB;
	u16 thresholdNum;

	u16 display;
	u16 differenceOfValue;

	//	RefreshMsg refreshMsg;

	if((NULL == p_wptr) )
		return AHMI_FUNC_FAILURE;

	dir = (p_wptr->WidgetAttr & PROGBAR_DIR  )? 1 : 0;
	changeColor = (p_wptr->WidgetAttr & CHANGECOLOR  )? 1 : 0;
	lineChangeColor = (p_wptr->WidgetAttr & LINE_CHANGE_COLOR  )? 1 : 0;
	lineChangeColor = 0;
	width = p_wptr->WidgetWidth;
	height = p_wptr->WidgetHeight;
	offsetX = p_wptr->WidgetOffsetX;
	offsetY = p_wptr->WidgetOffsetY;
	lineFileEn = (p_wptr->WidgetAttr & LINE_FILE_EN  )? 1 : 0;
	display = (p_wptr->WidgetAttr & PROBARDISPLAY)?1:0;
	//oldValueinit = (p_wptr->WidgetAttr & 0x8000);
	barColor = (p_wptr->WidgetAttr & BAR_IS_COLOR)?1:0;//�����Ƿ�����ɫ
	changeColorMode = (p_wptr->WidgetAttr & CHANGECOLOR_MODE)?1:0; //�Ƿ񽥱�
	thresholdNum = (p_wptr->WidgetAttr & THRESHOLD_NUM)?1:0; //��ֵ������0��ʾ1����1��ʾ2��

	maxValue = (s32)((p_wptr->MaxValueH << 16) + p_wptr->MaxValueL);
	minValue = (s32)((p_wptr->MinValueH << 16) + p_wptr->MinValueL);
	//lowAlarmValue  = (s32)((p_wptr->LowAlarmValueH  << 16) + p_wptr->LowAlarmValueL ) ;
	//highAlarmValue = (s32)((p_wptr->HighAlarmValueH << 16) + p_wptr->HighAlarmValueL) ;
	if(maxValue == minValue){
		ERROR_PRINT("ERRROR: can't divided by zero");
		return AHMI_FUNC_FAILURE;
	}
	if(value > maxValue) 
		value = maxValue;
	if(value < minValue) 
		value = minValue;

	if(display)
	{
		texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);

		//if((texturePtr < (void*)startOfDynamicPage) || (texturePtr > (void*)endOfDynamicPage))
		//	return AHMI_FUNC_FAILURE;

		if(barColor)
			barPtr = 1;
		else
		{
			barPtr = 2;
			texturePtr[barPtr - 1].mTexAttr |= TEXTURE_USING_WIDGET_BOX;
		}
		//������Ĥ����Ĵ�С,����Ҫ���Ƶ���Ĥ����ߴ�
		if(dir ==1)//����
		{

			newHeight = (u16)(((value - minValue) * height) /((maxValue - minValue)));
			offsetY = offsetY + height - newHeight;
			if(!barColor)//��������bar������ɫ
			{
				//0�������Ǳ������䣬1���������alpha��2������Ϊ��ɫ������ʾ��ͼƬ
				texturePtr[1].OffsetY = (offsetY)<<4;
				texturePtr[1].TexHeight = newHeight;

				topBox = (offsetY -1 )/TILESIZE;
				texturePtr[1].TexTopTileBox = (u8)topBox;
				texturePtr[2].TexTopTileBox = (u8)topBox;
			}
			else
			{
				//0�������Ǳ������䣬1���Ǵ���ɫ
				texturePtr[1].OffsetY = (offsetY)<<4;
				texturePtr[1].TexHeight = newHeight;

				topBox = (offsetY -1 )/TILESIZE;
				texturePtr[1].TexTopTileBox = (u8)topBox;
			}

			//bar��Ҫ����ɫ
			if(barColor && changeColor)
			{
				if(changeColorMode) //ͻ��
				{
					
					/*startB = (u8)p_wptr->COLOR0GB;
					startG = (u8)(p_wptr->COLOR0GB >> 8);
					startR = (u8)p_wptr->COLOR1B_COLOR0R;
					endB = (u8)(p_wptr->COLOR2GB);
					endG = (u8)(p_wptr->COLOR2GB >> 8);
					endR = (u8)(p_wptr->COLOR2R);
					curB = (u8)(p_wptr->COLOR1B_COLOR0R >> 8);
					curG = (u8)(p_wptr->COLOR1RG);
					curR = (u8)(p_wptr->COLOR1RG >> 8);*/
					threshold0 = p_wptr->THRESHOLD0_L + (p_wptr->THRESHOLD0_H << 8);
					threshold1 = p_wptr->THRESHOLD1_L + (p_wptr->THRESHOLD1_H << 8);

					//mask
					texturePtr[1].OffsetY = (offsetY)<<4;
					texturePtr[1].TexHeight = newHeight;
					texturePtr[1].TexTopTileBox = (u8)topBox;
					texturePtr[1].mTexAttr |= (TEXTURE_USING_WIDGET_BOX);

					if(thresholdNum) //3����ֵ
					{
						/*if(value < threshold0)
						{
							curB = startB;
							curG = startG;
							curR = startR;
						}
						else if(value >= threshold1)
						{
							curB = endB;
							curG = endG;
							curR = endR;
						}*/
						
						texturePtr[2].TexTopTileBox = (u8)topBox;
						
						texturePtr[3].TexTopTileBox = (u8)topBox;
						
						texturePtr[4].TexTopTileBox = (u8)topBox;
						
						
						barPtr = 4;

						if(value < threshold0){
							//t1
							texturePtr[2].mTexAttr |= (DRAWING);
							texturePtr[3].mTexAttr &= ~(DRAWING);
							texturePtr[4].mTexAttr &= ~(DRAWING);
						}else if(value >= threshold1){
							//t3
							texturePtr[4].mTexAttr |= (DRAWING);
							texturePtr[2].mTexAttr &= ~(DRAWING);
							texturePtr[3].mTexAttr &= ~(DRAWING);
						}else{
							//t2
							texturePtr[3].mTexAttr |= (DRAWING);
							texturePtr[2].mTexAttr &= ~(DRAWING);
							texturePtr[4].mTexAttr &= ~(DRAWING);
						}
					}
					else //2����ֵ
					{
						/*if(value < threshold0)
						{
							curB = startB;
							curG = startG;
							curR = startR;
						}*/
						
						texturePtr[2].TexTopTileBox = (u8)topBox;
						
						texturePtr[3].TexTopTileBox = (u8)topBox;

						if(value < threshold0){
							//tex1 show
							texturePtr[2].mTexAttr |= (DRAWING);
							texturePtr[3].mTexAttr &= ~(DRAWING);
						}else{
							texturePtr[2].mTexAttr &= ~(DRAWING);
							texturePtr[3].mTexAttr |= (DRAWING);
						}
						
					}
				}
				else //����
				{
					startB = (u8)p_wptr->StartColorValueGB;
					startG = (u8)(p_wptr->StartColorValueGB >> 8);
					startR = (u8)p_wptr->EndColorValueBAndStartColorValueR;
					endB = (u8)(p_wptr->EndColorValueBAndStartColorValueR >> 8);
					endG = (u8)p_wptr->EndColorValueRG;
					endR = (u8)(p_wptr->EndColorValueRG >> 8);
					differenceOfB = endB - startB;
					differenceOfG = endG - startG;
					differenceOfR = endR - startR;
					differenceOfValue = maxValue - minValue;

					curB = startB;
					curB += ((s16)(value - minValue) * differenceOfB) / differenceOfValue;
					curG = startG;
					curG += ((s16)(value - minValue) * differenceOfG) / differenceOfValue;
					curR = startR;
					curR += ((s16)(value - minValue) * differenceOfR) / differenceOfValue;
					texturePtr[1].TexAddr &= 0xFF000000;
					texturePtr[1].TexAddr |= curB;
					texturePtr[1].TexAddr |= (curG << 8);
					texturePtr[1].TexAddr |= (curR << 16);
				}

				/*texturePtr[1].TexAddr &= 0xFF000000;
				texturePtr[1].TexAddr |= curB;
				texturePtr[1].TexAddr |= (curG << 8);
				texturePtr[1].TexAddr |= (curR << 16);*/
			}

			if(lineFileEn)
			{
				if(!(p_wptr->NumOfTex == 3 || p_wptr->NumOfTex == 4 || p_wptr->NumOfTex == 5) )
					return AHMI_FUNC_FAILURE;
				texturePtr[barPtr + 1].OffsetY = offsetY << 4;
				if(lineChangeColor) 
				{
					texturePtr[4].TexAddr &= 0xFF000000;
					texturePtr[4].TexAddr |= curB;
					texturePtr[4].TexAddr |= (curG << 8);
					texturePtr[4].TexAddr |= (curR << 16);
				}
			}
		}
		else//����
		{
			newWidth = (u16)( ((value - minValue) * width) /((maxValue - minValue)) );
			texturePtr[1].TexWidth  = newWidth;
			rightBox = (offsetX + newWidth)/TILESIZE;
			texturePtr[1].TexRightTileBox = (u8)rightBox;
			if(!barColor)
				texturePtr[2].TexRightTileBox = (u8)rightBox;

			if(changeColor)
			{
				if(changeColorMode) //ͻ��
				{
					
					/*startB = (u8)p_wptr->COLOR0GB;
					startG = (u8)(p_wptr->COLOR0GB >> 8);
					startR = (u8)p_wptr->COLOR1B_COLOR0R;
					endB = (u8)(p_wptr->COLOR2GB);
					endG = (u8)(p_wptr->COLOR2GB >> 8);
					endR = (u8)(p_wptr->COLOR2R);
					curB = (u8)(p_wptr->COLOR1B_COLOR0R >> 8);
					curG = (u8)(p_wptr->COLOR1RG);
					curR = (u8)(p_wptr->COLOR1RG >> 8);*/
					threshold0 = p_wptr->THRESHOLD0_L + (p_wptr->THRESHOLD0_H << 8);
					threshold1 = p_wptr->THRESHOLD1_L + (p_wptr->THRESHOLD1_H << 8);

					//mask
					texturePtr[1].TexWidth  = newWidth;
					texturePtr[1].TexRightTileBox = (u8)rightBox;
					texturePtr[1].mTexAttr |= (TEXTURE_USING_WIDGET_BOX);
					if(thresholdNum) //3����ֵ
					{
						
						
						texturePtr[2].TexRightTileBox = (u8)rightBox;
						texturePtr[3].TexRightTileBox = (u8)rightBox;
						texturePtr[4].TexRightTileBox = (u8)rightBox;
						
						barPtr = 4;
						/*if(value < threshold0)
						{
							curB = startB;
							curG = startG;
							curR = startR;
						}
						else if(value >= threshold1)
						{
							curB = endB;
							curG = endG;
							curR = endR;
						}*/
						if(value < threshold0){
							//1
							texturePtr[2].mTexAttr |= (DRAWING);
							texturePtr[3].mTexAttr &= ~(DRAWING);
							texturePtr[4].mTexAttr &= ~(DRAWING);
						}else if(value >= threshold1){
							//3
							texturePtr[4].mTexAttr |= (DRAWING);
							texturePtr[2].mTexAttr &= ~(DRAWING);
							texturePtr[3].mTexAttr &= ~(DRAWING);
						}else{
							//2
							texturePtr[3].mTexAttr |= (DRAWING);
							texturePtr[2].mTexAttr &= ~(DRAWING);
							texturePtr[4].mTexAttr &= ~(DRAWING);
						}
					}
					else //2����ֵ
					{
						
						texturePtr[2].TexRightTileBox = (u8)rightBox;
						texturePtr[3].TexRightTileBox = (u8)rightBox;
						/*texturePtr[1].mTexAttr |= (TEXTURE_USING_WIDGET_BOX);
						texturePtr[2].mTexAttr |= (TEXTURE_USING_WIDGET_BOX);*/
						barPtr = 3;
						/*if(value < threshold0)
						{
							curB = startB;
							curG = startG;
							curR = startR;
						}*/
						if(value < threshold0){
							//tex1 show
							texturePtr[2].mTexAttr |= (DRAWING);
							texturePtr[3].mTexAttr &= ~(DRAWING);
						}else{
							texturePtr[2].mTexAttr &= ~(DRAWING);
							texturePtr[3].mTexAttr |= (DRAWING);
						}
					}
				}
				else //����
				{
					startB = (u8)p_wptr->StartColorValueGB;
					startG = (u8)(p_wptr->StartColorValueGB >> 8);
					startR = (u8)p_wptr->EndColorValueBAndStartColorValueR;
					endB = (u8)(p_wptr->EndColorValueBAndStartColorValueR >> 8);
					endG = (u8)p_wptr->EndColorValueRG;
					endR = (u8)(p_wptr->EndColorValueRG >> 8);
					differenceOfB = endB - startB;
					differenceOfG = endG - startG;
					differenceOfR = endR - startR;
					differenceOfValue = maxValue - minValue;

					curB = startB;
					curB += ((s16)(value - minValue) * differenceOfB) / differenceOfValue;
					curG = startG;
					curG += ((s16)(value - minValue) * differenceOfG) / differenceOfValue;
					curR = startR;
					curR += ((s16)(value - minValue) * differenceOfR) / differenceOfValue;

					texturePtr[1].TexAddr &= 0xFF000000;
					texturePtr[1].TexAddr |= curB;
					texturePtr[1].TexAddr |= (curG << 8);
					texturePtr[1].TexAddr |= (curR << 16);
				}

				/*texturePtr[1].TexAddr &= 0xFF000000;
				texturePtr[1].TexAddr |= curB;
				texturePtr[1].TexAddr |= (curG << 8);
				texturePtr[1].TexAddr |= (curR << 16);*/
			}

			if(lineFileEn)
			{
				if(!(p_wptr->NumOfTex == 3 || p_wptr->NumOfTex == 4 || p_wptr->NumOfTex == 5|| p_wptr->NumOfTex == 6) )
					return AHMI_FUNC_FAILURE;
				texturePtr[barPtr + 1].OffsetX = (offsetX + newWidth) << 4;
				rightBox = (offsetX + p_wptr->WidgetWidth)/TILESIZE;
				texturePtr[barPtr + 1].TexRightTileBox = (u8)rightBox;
				if(lineChangeColor) 
				{
					texturePtr[barPtr + 2].TexAddr &= 0xFF000000;
					texturePtr[barPtr + 2].TexAddr |= curB;
					texturePtr[barPtr + 2].TexAddr |= (curG << 8);
					texturePtr[barPtr + 2].TexAddr |= (curR << 16);
				}
			}
		}
	}
	return AHMI_FUNC_SUCCESS;
}



//-----------------------------
// �������� ProgBarClass::setATag
//set the animation tag
//  @param   WidgetClassPtr p_wptr,   //�ؼ�ָ��
//  @param	 u8 aTagValue             // the value of aniamtion tag
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus ProgBarClass::setATag
	(
	WidgetClassPtr p_wptr,   //�ؼ�ָ��
	u8 ATagValue
	)
{
	u8 totalFrame;
	s32 ValueStart;
	s32 ValueStop;
	s32 curValue;
#ifndef WHOLE_TRIBLE_BUFFER
	RefreshMsg refreshMsg;
#endif

	totalFrame = (u8)( (p_wptr->TOTALFRAME_AND_NOWFRAME) >> 8);

	if(totalFrame == 0 ) //no animation
		return AHMI_FUNC_SUCCESS;

	taskENTER_CRITICAL();
#ifdef AHMI_DEBUG
	char text[100];
	sprintf(text,"set meter animation tag %d", ATagValue);
	ERROR_PRINT(text);
#endif
	p_wptr->TOTALFRAME_AND_NOWFRAME |= ATagValue;
	ValueStart = (s32)((p_wptr->START_TAG_L) +  ( (p_wptr->START_TAG_H) << 8));
	ValueStop = (s32)((p_wptr->STOP_TAG_L) + ( (p_wptr->STOP_TAG_H) << 8));

	//transitionParamIn = (value) * 1024 / totalFrame;//0.10
	//gPagePtr[WorkingPageID].inoutTransitioin(transitionParamIn, transitionParamOut);

	curValue = ValueStart + (ValueStop - ValueStart) * ATagValue / totalFrame;

	renderTexture(p_wptr, curValue);

	if(ATagValue == totalFrame)
		p_wptr->TOTALFRAME_AND_NOWFRAME &= 0xff00;
#ifndef WHOLE_TRIBLE_BUFFER
	refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
	refreshMsg.mElementPtr.wptr = p_wptr;
	sendToRefreshQueue(&refreshMsg);
#endif
	taskEXIT_CRITICAL();
	return AHMI_FUNC_SUCCESS;
}


#endif
