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
// 函数名： KnobClass
// 构造函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
KnobClass::KnobClass()
{

}

//-----------------------------
// 函数名： ~KnobClass
// 析构函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
KnobClass::~KnobClass()
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
funcStatus KnobClass::initWidget(
	WidgetClassPtr p_wptr,   //控件指针
	u32 *u32p_sourceShift,   //sourcebuffer指针
	u8 u8_pageRefresh,       //页面刷新
	u8 RefreshType ,       //绘制的动画类型，根据动画类型改变绘制控件的包围盒
	TileBoxClassPtr pTileBox, //包围盒
	u8 staticTextureEn          //是否绘制到静态存储空间
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
//旋钮控件
//一共两张纹理
//tag的值代表旋钮角度
//WidgetAttr标识：
//15-5:保留
//4-0:控件类型，旋钮为0xB
//*****************************
funcStatus KnobClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//控件指针
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//页面刷新
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

