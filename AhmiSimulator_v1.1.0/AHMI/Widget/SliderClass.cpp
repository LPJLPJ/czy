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

//鼠标
extern PIDPoint pressPoint;
extern PIDPoint releasePoint;
//鼠标目前状态
extern u8 mouseStatus;


//-----------------------------
// 函数名： MeterClass
// 构造函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
SliderClass::SliderClass()
{

}

//-----------------------------
// 函数名： ~MeterClass
// 析构函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
SliderClass::~SliderClass()
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
funcStatus SliderClass::initWidget(
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
		tagtrigger.mInputType = ACTION_TAG_SET_VALUE;
		if(widgetCtrl(p_wptr,&tagtrigger,1) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}

	if(myWidgetClassInterface.drawTexture(p_wptr,u32p_sourceShift,RefreshType,pTileBox,staticTextureEn) == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;
	return AHMI_FUNC_SUCCESS;

}

//******************************************
//滑块控制函数
//需要3张纹理
//第一张纹理为
//第二张纹理为
//第三张纹理为
//WidgetAttr标识：
//15-5:保留
// 5表示横向和纵向 0表示横向 1表示纵向
//4-0:控件类型，滑块为0x5
//******************************************
funcStatus SliderClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//控件指针
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//页面刷新
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
		if(type==0)///横向
		{
			HalfWidth1 = (texturePtr[1].TexWidth/2) <<4;
			Width0 = texturePtr[0].TexWidth << 4;
	
			if((cur_x - texturePtr[0].OffsetX) > HalfWidth1)
				{
				if(( texturePtr[0].OffsetX + Width0 - cur_x) < HalfWidth1)
					texturePtr[1].OffsetX =   texturePtr[0].OffsetX + Width0 - HalfWidth1*2;		/////////右边限制
				else 
					texturePtr[1].OffsetX =   cur_x - HalfWidth1;
				}
			else 	/////////左边限制
				texturePtr[1].OffsetX =   texturePtr[0].OffsetX;

			tag_value = (texturePtr[1].OffsetX - texturePtr[0].OffsetX) * maxValue / ( Width0 - HalfWidth1*2) + minValue ; //计算数值
		}
		else  //纵向
		{
			HalfWidth1 = (texturePtr[1].TexHeight/2) <<4;
			Width0 = texturePtr[0].TexHeight << 4;

			if((cur_y - texturePtr[0].OffsetY) > HalfWidth1)
				{
				if(( texturePtr[0].OffsetY + Width0 - cur_y) < HalfWidth1)
					texturePtr[1].OffsetY =   texturePtr[0].OffsetY + Width0 - HalfWidth1*2;		/////////下边限制
				else 
					texturePtr[1].OffsetY =   cur_y - HalfWidth1;
				}
			else 	/////////上边限制
				texturePtr[1].OffsetY =   texturePtr[0].OffsetY;


			tag_value = (( Width0 - HalfWidth1*2) - texturePtr[1].OffsetY + texturePtr[0].OffsetY) * maxValue / ( Width0 - HalfWidth1*2) + minValue ; //计算数值
		}

		TagPtr[p_wptr->BindTagID].setValue(tag_value,p_wptr->BindTagID);
		return AHMI_FUNC_SUCCESS;

	}

	else if(ActionPtr->mInputType == ACTION_MOUSE_HOLDING)
	{
		cur_x  = ((s16)(releasePoint.x)) << 4;
		cur_y  = ((s16)(releasePoint.y)) << 4;
		if(type==0)///横向
		{
				HalfWidth1 = (texturePtr[1].TexWidth/2) <<4;
				Width0 = texturePtr[0].TexWidth << 4;
	
				if((cur_x - texturePtr[0].OffsetX) > HalfWidth1)
					{
					if(( texturePtr[0].OffsetX + Width0 - cur_x) < HalfWidth1)
						texturePtr[1].OffsetX =   texturePtr[0].OffsetX + Width0 - HalfWidth1*2;		/////////右边限制
					else 
						texturePtr[1].OffsetX =   cur_x - HalfWidth1;
					}
				else 	/////////左边限制
					texturePtr[1].OffsetX =   texturePtr[0].OffsetX;

				tag_value = (texturePtr[1].OffsetX - texturePtr[0].OffsetX) * maxValue / ( Width0 - HalfWidth1*2) + minValue ; //计算数值
		}
		else  //纵向
		{
				HalfWidth1 = (texturePtr[1].TexHeight/2) <<4;
				Width0 = texturePtr[0].TexHeight << 4;

				if((cur_y - texturePtr[0].OffsetY) > HalfWidth1)
					{
					if(( texturePtr[0].OffsetY + Width0 - cur_y) < HalfWidth1)
						texturePtr[1].OffsetY =   texturePtr[0].OffsetY + Width0 - HalfWidth1*2;		/////////下边限制
					else 
						texturePtr[1].OffsetY =   cur_y - HalfWidth1;
					}
				else 	/////////上边限制
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

		

		if(type==0)///横向
			{
					HalfWidth1 = (texturePtr[1].TexWidth/2) <<4;
					Width0 = texturePtr[0].TexWidth << 4;
					texturePtr[1].OffsetX = texturePtr[0].OffsetX + (value - minValue) * (Width0 - HalfWidth1 *2)  / (maxValue - minValue);

			}
			else  //纵向
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
				//进入低值预警
				if(p_wptr->EnterLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
				{
					actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterLowAlarmAction);
					xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
				}		
			}
			else if(p_wptr->LeaveLowAlarmAction && oldValue <= lowAlarmValue && value > lowAlarmValue)
			{
				//离开低值预警
				if(p_wptr->LeaveLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
				{
					actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveLowAlarmAction);
					xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
				}		
			}
			if(p_wptr->EnterHighAlarmAction && oldValue < highAlarmValue && value >= highAlarmValue)
			{
				//进入高值预警
				if(p_wptr->EnterHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
				{
					actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterHighAlarmAction);
					xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);		
				}
			}
			else if(p_wptr->LeaveHighAlarmAction && oldValue >= highAlarmValue && value < highAlarmValue)
			{
				//离开高值预警
				if(p_wptr->LeaveHighAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
				{
					actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->LeaveHighAlarmAction);
					xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
				}	
			}
			//tagchange的支持 by Mr.z
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
