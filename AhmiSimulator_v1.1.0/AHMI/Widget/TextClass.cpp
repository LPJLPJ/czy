////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     CanvasClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 3.00 - File Created 2016/04/13 by 于春营
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
// 函数名： TextClass
// 构造函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
TextClass::TextClass()
{

}

//-----------------------------
// 函数名： ~TextClass
// 析构函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
TextClass::~TextClass()
{

}

//-----------------------------
// 函数名： DynamicTexClass::initWidget
// 初始化并绘制该控件，如果是全屏刷新，先根据所绑定的tag刷新该控件，在绘制该控件
//  @param   WidgetClassPtr p_wptr,   //控件指针
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //页面刷新
//  @param   TileBoxClassPtr pTileBox //包围盒
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus TextClass::initWidget(
	WidgetClassPtr p_wptr,   //控件指针
	u32 *u32p_sourceShift,   //sourcebuffer指针
	u8 u8_pageRefresh,       //页面刷新
	u8 RefreshType ,       //绘制的动画类型，根据动画类型改变绘制控件的包围盒
	TileBoxClassPtr pTileBox, //包围盒
	u8 staticTextureEn          //是否绘制到静态存储空间
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
	WidgetClassPtr p_wptr,			//控件指针
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//页面刷新
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
