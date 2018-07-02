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
#include "TextClass.h"

#ifdef AHMI_CORE
extern TagClassPtr			TagPtr;
extern DynamicPageClassPtr  gPagePtr;
extern u16					WorkingPageID;
//extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;


//-----------------------------
// �������� TextClass
// ���캯��
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
TextClass::TextClass()
{

}

//-----------------------------
// �������� ~TextClass
// ��������
// �����б�
//
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
TextClass::~TextClass()
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
funcStatus TextClass::initWidget(
	WidgetClassPtr p_wptr,   //�ؼ�ָ��
	u32 *u32p_sourceShift,   //sourcebufferָ��
	u8 u8_pageRefresh,       //ҳ��ˢ��
	u8 RefreshType ,       //���ƵĶ������ͣ����ݶ������͸ı���ƿؼ��İ�Χ��
	TileBoxClassPtr pTileBox, //��Χ��
	u8 staticTextureEn          //�Ƿ���Ƶ���̬�洢�ռ�
	)
{
	//TODO
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

funcStatus TextClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//�ؼ�ָ��
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//ҳ��ˢ��
	)
{
	//TODO
	u32 value;
	u8* charPtr;
	u8 numOfChar;
	u8 i;
	
	TextureClassPtr texturePtr;
//	RefreshMsg refreshMsg;

	if(NULL == p_wptr)
		return AHMI_FUNC_FAILURE;

	numOfChar = (p_wptr->WidgetAttr >>5) & 0x1f;

	value = ActionPtr->mTagPtr->mValue;
	if(numOfChar == 0)      //singal char
	{
		if(ActionPtr->mInputType == ACTION_TAG_SET_VALUE)
		{
			texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
			texturePtr[1].FocusedSlice = value;
		}
	}
	else
	{
		charPtr = (u8*)value;
		texturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);
		for(i = 0;i < numOfChar;i++)
		{
			texturePtr[i + 1].FocusedSlice = *(charPtr++) - 0x20;
		}
	}

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
	}
	return AHMI_FUNC_SUCCESS;
}



#endif
