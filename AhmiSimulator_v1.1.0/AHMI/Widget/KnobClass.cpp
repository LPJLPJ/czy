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
#include "KnobClass.h"
#include "myMathClass.h"

#ifdef AHMI_CORE
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
//extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;

extern u32 startOfDynamicPage;
extern u32 endOfDynamicPage;

//-----------------------------
// �������� KnobClass
// ���캯��
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
KnobClass::KnobClass()
{

}

//-----------------------------
// �������� ~KnobClass
// ��������
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
KnobClass::~KnobClass()
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
funcStatus KnobClass::initWidget(
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
		if(widgetCtrl(p_wptr,&tagtrigger,1) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}

	if(myWidgetClassInterface.drawTexture(p_wptr,u32p_sourceShift,RefreshType,pTileBox,staticTextureEn) == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;
	return AHMI_FUNC_SUCCESS;
}

//*****************************
//��ť�ؼ�
//һ����������
//tag��ֵ������ť�Ƕ�
//WidgetAttr��ʶ��
//15-5:����
//4-0:�ؼ����ͣ���ťΪ0xB
//*****************************
funcStatus KnobClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//�ؼ�ָ��
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//ҳ��ˢ��
	)
{
	u16  angleTemp;
	u16* angle = &angleTemp;
	u16 tag_value;
//	static u16 oldValue=0;
//	static u16 oldValueinit=0;
	myMathClass mMyMath;
	TextureClassPtr texturePtr;
//	RefreshMsg refreshMsg;
	u32	maxValue;
	u32	minValue;

	if((NULL == p_wptr) || (NULL == ActionPtr))
		return AHMI_FUNC_FAILURE;

	maxValue = (p_wptr->MaxValueH << 16) + p_wptr->MaxValueL;
	minValue = (p_wptr->MinValueH << 16) + p_wptr->MinValueL;

	texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);

#ifdef EMBEDDED
	if((texturePtr < (void*)startOfDynamicPage) || (texturePtr > (void*)endOfDynamicPage))
		return AHMI_FUNC_FAILURE;
#endif

	if(ActionPtr->mInputType == 1)
	{
	u16 cur_x = 0;//(ActionPtr->mMousePID.x) << 4;
	u16 cur_y = 0;//(ActionPtr->mMousePID.y) << 4;
	u16 p0_x = p_wptr->WidgetOffsetX + p_wptr->WidgetWidth/2;
	u16 p0_y = p_wptr->WidgetOffsetY + p_wptr->WidgetHeight/2;
	s16 x = cur_x - p0_x;
	s16 y = cur_y - p0_y;
	if(ActionPtr->mInputType != MOUSE_RELEASE)
	{
		if(y == 0 && x >=0) *angle = 0;
		else if(y == 0 && x <0) *angle = 180;
		else if(x == 0 && y > 0) *angle = 90;
		else if(x == 0 && y < 0) *angle = 270;
		else
		{
			mMyMath.CORDIC_ANGLE((u16 *)angle,x,y);
		}
		texturePtr[1].RotateAngle  = (*angle) << 4;
		tag_value = (maxValue - minValue)  * (*angle)/16  / 360 + minValue;
		ActionPtr->mTagPtr->setValue(tag_value,p_wptr->BindTagID);
	}
	else if(ActionPtr->mInputType == MOUSE_RELEASE)
	{
		texturePtr[1].RotateAngle  = 0;
		tag_value = minValue;
		ActionPtr->mTagPtr->setValue(tag_value,p_wptr->BindTagID);
	}
	}
	else if(ActionPtr->mInputType == ACTION_TAG_SET_VALUE)
	{
		angle =(u16*) (16 * (ActionPtr->mTagPtr->mValue - minValue)* 360 /(maxValue - minValue));
		texturePtr[1].RotateAngle  = *(s16 *)(angle);
	}

	if(u8_pageRefresh == 0)
	{
		//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
		refreshMsg.mElementType = ANIMATION_REFRESH_WIDGET;
		refreshMsg.mElementPtr.wptr = p_wptr;
		sendToRefreshQueue(&refreshMsg);
#endif
		return AHMI_FUNC_SUCCESS;
	}
	return AHMI_FUNC_SUCCESS;
}

#endif 

