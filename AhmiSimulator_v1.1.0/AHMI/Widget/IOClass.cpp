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
#include "publicType.h"
#include "IOClass.h"

#ifdef AHMI_CORE
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
//extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;

IOClass::IOClass()
{

}

IOClass::~IOClass()
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
funcStatus IOClass::initWidget(
	WidgetClassPtr p_wptr,   //�ؼ�ָ��
	u32 *u32p_sourceShift,   //sourcebufferָ��
	u8 u8_pageRefresh,       //ҳ��ˢ��
	TileBoxClassPtr pTileBox, //��Χ��
	u8 staticTextureEn          //�Ƿ���Ƶ���̬�洢�ռ�
	)
{
	TagClassPtr bindTag;
	ActionTriggerClass tagtrigger;
	if(NULL == p_wptr)
		return AHMI_FUNC_FAILURE;

	bindTag = &TagPtr[p_wptr->BindTagID];

	if(u8_pageRefresh)
	{
		tagtrigger.mTagPtr = bindTag;
		tagtrigger.mInputType = ACTION_TAG_SET_VALUE;
		if(widgetCtrl(p_wptr,&tagtrigger,1) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}
	return AHMI_FUNC_SUCCESS;
}

//******************************************
//IO���ƺ���
//����Ҫ����
//WidgetAttr��ʶ��
//15-5:����
//4-0:�ؼ����ͣ�������ӦΪ0xD
//******************************************
funcStatus IOClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//�ؼ�ָ��
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//ҳ��ˢ��
	)
{
#ifdef EMBEDDED
	u32 value;
	u16 pinValue;
	u16 controlValue;

	if((NULL == p_wptr) || (NULL == ActionPtr))
		return AHMI_FUNC_FAILURE;

	value = ActionPtr->mTagPtr->mValue;
	pinValue = (u16)value;
	controlValue = (u16)(value >> 16); 

	if(controlValue & p_wptr->PinMusk)
	{
		if(openIOWithWidget(p_wptr->IOX,controlValue,p_wptr->PinMusk) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
		p_wptr->OpenPin |= controlValue & p_wptr->PinMusk;
	}

	if((pinValue & p_wptr->PinMusk) & p_wptr->OpenPin)
	{
		if(setPinValueWithWidget(p_wptr->IOX,p_wptr->OpenPin,pinValue,p_wptr->PinMusk) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}

#endif
	return AHMI_FUNC_SUCCESS;

}

#endif
