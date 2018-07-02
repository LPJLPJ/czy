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
// 函数名： ProgBarClass::initWidget
// 初始化并绘制数字框，如果是全屏刷新，先根据所绑定的tag刷新该控件，在绘制该控件
//  @param   WidgetClassPtr p_wptr,   //控件指针
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //页面刷新
//  @param   TileBoxClassPtr pTileBox //包围盒
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus IOClass::initWidget(
	WidgetClassPtr p_wptr,   //控件指针
	u32 *u32p_sourceShift,   //sourcebuffer指针
	u8 u8_pageRefresh,       //页面刷新
	TileBoxClassPtr pTileBox, //包围盒
	u8 staticTextureEn          //是否绘制到静态存储空间
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
//IO控制函数
//不需要纹理
//WidgetAttr标识：
//15-5:保留
//4-0:控件类型，进度条应为0xD
//******************************************
funcStatus IOClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//控件指针
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//页面刷新
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
