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
// 函数名： MeterClass
// 构造函数
// 参数列表：
//
// 备注(各个版本之间的修改):
//   无
//-----------------------------
MeterClass::MeterClass()
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
MeterClass::~MeterClass()
{

}

//-----------------------------
// 函数名： MeterClass::initWidget
// 初始化并绘制该控件框，如果是全屏刷新，先根据所绑定的tag刷新该控件，在绘制该控件
// 参数列表：
//  @param   WidgetClassPtr p_wptr,   //控件指针
//  @param	 u32 *u32p_sourceShift,   //sourceb
//  @param   u8 u8_pageRefresh,       //页面刷新
//  @param   TileBoxClassPtr pTileBox //包围盒
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus MeterClass::initWidget(
	WidgetClassPtr p_wptr,   //控件指针
	u32 *u32p_sourceShift,   //sourcebuffer指针
	u8 u8_pageRefresh,       //页面刷新
	u8 RefreshType ,       //绘制的动画类型，根据动画类型改变绘制控件的包围盒
	TileBoxClassPtr pTileBox, //包围盒
	u8 staticTextureEn          //是否绘制到静态存储空间
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
//仪表盘控制函数
//根据配置不同，可以包括3张或8张纹理
//当配置为简单模式时，包括2或者3张纹理，分别是：
//0.背景纹理，仪表盘盘面
//1.指针纹理，可以转动
//2.指针顶部的圆点(可选)
//
//当配置为复杂模式时，包括12或13张纹理，分别是：
//0.背景纹理，仪表盘盘面
//1-4：指针掩膜纹理，用于只显示仪表盘上的指针
//5.指针纹理，可以转动，指针转动角度的0度位置为竖直向下。
//           指针纹理的初始角度在转动原点的坐下角，以45度呈现。
//           但指针的初始角度需要在MeterMinAngle所示角度。
//6-10:纯掩膜纹理，用于显示光带效果，1张为清空alpha buffer的纹理，另外4张纹理分布在以盘体为坐标原点的4个象限
//11:光带效果，需要DXT3或PNG格式
//12.指针顶部的圆点（可选）
//WidgetAttr标识：
//15:用于判断old_value是否有变化
//14-9:保留
//8:是否有遮罩
//7:极简模式
//6:表盘旋转方向，1表示顺时针，0表示逆时针
//5:仪表盘模式，0为简单模式，1为复杂模式
//4-0:控件类型，仪表盘为0x3
//**************************************
funcStatus MeterClass::widgetCtrl(
	WidgetClassPtr p_wptr,			//控件指针
	ActionTriggerClassPtr ActionPtr,
	u8 u8_pageRefresh				//页面刷新
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
		renderTexture(p_wptr, value);	//根据当前的值对meter的texture的属性进行一次修改
	}
	else if(u8_pageRefresh)	  //有动画的时候，需要刷新页面的时候，执行该操作
	{
		//需要刷新页面的时候, 使用动画的值对作为下次动画的初始值， 不根据当前的value值进行刷新控件的状态 by Mr.z
		p_wptr->START_TAG_L = p_wptr->ANIMATIONCURVALUE_L;
		p_wptr->START_TAG_H = p_wptr->ANIMATIONCURVALUE_H;
		p_wptr->STOP_TAG_L = (u16)( (u32)value );
		p_wptr->STOP_TAG_H = (u16)(( (u32)value ) >> 16);
		//renderTexture(p_wptr, value);	     //需要产品进一步验证后决定是否删除
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
				//进入低值预警
				if(p_wptr->EnterLowAlarmAction <= gPagePtr[WorkingPageID].mActionInstructionsSize)
				{
					actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + p_wptr->EnterLowAlarmAction);
					xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
				}		
			}
			else if(p_wptr->LeaveLowAlarmAction && oldValue <= lowAlarmValue && value >lowAlarmValue)
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
// 函数名： MeterClass::initWidget
// compute the texture using the tag value
// 参数列表：
//  @param   WidgetClassPtr p_wptr,   //控件指针
//  @param	 u32 tagValue             //the value of the tag
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus MeterClass::renderTexture
	(
	WidgetClassPtr p_wptr,  //控件指针
	s32 value
	)
{
	s32	maxValue;
	s32	minValue;
	
	s16 Angle;


	

	maxValue = (s32)((p_wptr->MaxValueH << 16) + p_wptr->MaxValueL);
	minValue = (s32)((p_wptr->MinValueH << 16) + p_wptr->MinValueL);
	


	//todo:  需要修改后再修改判断函数
	//if(MeterMode == 0 && p_wptr->NumOfTex!=3)
	//{
	//	return AHMI_FUNC_FAILURE;
	//	;//简单模式错误0= 
	//}
	//if(MeterMode == 1 && p_wptr->NumOfTex!=8)
	//{
	//	return AHMI_FUNC_FAILURE;
	//	;//复杂模式错误
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
		 p_wptr,  //控件指针
		 Angle
	);

	
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： MeterClass::renderTextureAngle
// set the texture's angle
// 参数列表：
//  @param   WidgetClassPtr p_wptr,   //控件指针
//  @param	 u32 tagValue             //the value of the tag
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus MeterClass::renderTextureAngle
		 (
		 WidgetClassPtr p_wptr,  //控件指针
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
	s16 pureMaskAdjustAngle;    //puremask的调整角度

	s16 RotateAngle;			//12.4
	u16 ScaleX,ScaleY;
	
	u16 centralLenght = ((p_wptr->WidgetWidth) * 3)/8;
	
	

	TextureClassPtr TexturePtr;
	TileBoxClass meterBox;

	Angle += ((s16)(p_wptr->MeterMinAngle)) << 4;//12.4 相对于仪表盘最小角度的偏移量

	TexturePtr = &(gPagePtr[WorkingPageID].pBasicTextureList[p_wptr->StartNumofTex]);

	/*if((TexturePtr < (void*)startOfDynamicPage) || (TexturePtr > (void*)endOfDynamicPage))
		return AHMI_FUNC_FAILURE;*/
	//0：简单模式 1：复杂模式 2:精简模式   3：双向模式（双向带光带）
	if(MeterMode == 2)
		PointerPtr = 6;
	else if(MeterMode == 1|| MeterMode == 3)//复杂模式
		if(p_wptr->NumOfTex == 7 || p_wptr->NumOfTex == 12){  //不包括背景的tex个数 by Mr.z
			PointerPtr = 6;	 
		}else{
			PointerPtr = 7;
		}
	else if(MeterMode == 0){
		if(p_wptr->NumOfTex == 1 || p_wptr->NumOfTex == 6){	 //不包含背景的tex个数 by Mr.z
			PointerPtr = 0;	 //简单模式下指针放在texture 0
		}else{
			PointerPtr = 1;
		}
	}
	if(pointerMask)
		PointerPtr += 5;

	//相对于垂直角度的startangle
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
			startAngle = (s16)(p_wptr->StartAngle) + (45 * 16); //初始角度
		else 
			startAngle = -(s16)(p_wptr->StartAngle) + (45 * 16);
	}

	pureMaskAdjustAngle = (s16)(p_wptr->StartAngle) + (((s16)(p_wptr->MeterMinAngle)) << 4); //12.4

	if(rotatingDir)//顺时针
		{
			TexturePtr[PointerPtr].RotateAngle = Angle + startAngle;//额外加上指针纹理的初始角度,//12.4
			TexturePtr[PointerPtr].mTexAttr |= ABCDEFMATRIX;
			TexturePtr[PointerPtr].mTexAttr |= USING_PIXEL_RATIO;//pointer needs to be fixed, by zuz20180802
			Angle = TexturePtr[PointerPtr].RotateAngle - ((s16)(p_wptr->MeterMinAngle) << 4) - startAngle;// + (45*16);//////指针角度和起始角度之间相对角度  （小小火车侠）
		}
	else //逆时针
		{
			TexturePtr[PointerPtr].RotateAngle = -Angle + startAngle;
			TexturePtr[PointerPtr].mTexAttr |= ABCDEFMATRIX;
			TexturePtr[PointerPtr].mTexAttr |= USING_PIXEL_RATIO;//pointer needs to be fixed, by zuz20180802
			Angle =   TexturePtr[PointerPtr].RotateAngle + ((s16)(p_wptr->MeterMinAngle)<<4) - startAngle;// - 45*16; //赋值为指针旋转的角度
	   }
	  //StartPoint = ((startAngle + 45*16 ) / 1440) % 4;///////起始旋转角度所在象限 
	
	if(MeterMode == 2)//极简模式
	{
		//计算指针的偏移量
		//TexturePtr[PointerPtr].RotateAngle += (45 * 16);
		TexturePtr[PointerPtr].adjustMoving(centralLenght,(( (p_wptr->WidgetWidth)/2 + p_wptr->WidgetOffsetX) << 4),(( (p_wptr->WidgetHeight)/2 + p_wptr->WidgetOffsetY) << 4) );
	}
	//优化指针包围盒
	TexturePtr[PointerPtr].renewTextureSourceBox(NULL,NULL,NULL);

	//if(rotatingDir)//顺时针旋转,调整到0-360
	//{
	//	while(Angle < 0)
	//		Angle += 16 * 360;
	//	while(Angle > 360 * 16)
	//		Angle -= 16 * 360;
	//}
	//else //逆时针旋转，调整到-360 - 0
	//{
	//	while(Angle < -360 * 16)
	//		Angle += 16 * 360;
	//	while(Angle > 0)
	//		Angle -= 16 * 360;
	//}

	if(MeterMode == 1)//复杂模式
	{
		TexturePtr[1].mTexAttr |= TEXTURE_USING_WIDGET_BOX;
		if(rotatingDir)//顺时针旋转
		{
			
			for(PointerPtr = 1  ;PointerPtr<5;PointerPtr++)
			{
				if(Angle>= 16 * 90 * (PointerPtr)) //指针越过当前象限，该象限显示矩形
				{
					ShearAngleX = 0;//不发生错切
					ShearAngleY = 0;
					RotateAngle = 16 * 90 * (PointerPtr );//旋转到该象限位置
					ScaleX = 512;
					ScaleY = 512;
				}
				else if(Angle<= 16 * 90 * ((PointerPtr ) - 1 ))//指针未到达该象限，该象限不显示
				{
					ShearAngleX = 0 ;//不发生错切
					ShearAngleY = 0;
					RotateAngle = 16 * 90 * (PointerPtr);//旋转到该象限位置
					ScaleX = 0xf;//缩小mask，使得该象限不显示
					ScaleY = 0xf;
				}
				else //指针停留在该象限中
				{
					ShearAngleX = 90*16*(PointerPtr ) - Angle;
					if(ShearAngleX >= 89*16)
						ShearAngleX = 89*16;
					if(ShearAngleX <= -89*16)
						ShearAngleX = -89*16;
					ShearAngleY = 0;
					RotateAngle = 16 * 90 * ((PointerPtr ));//旋转到该象限位置
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
		else //逆时针旋转
		{
			
			for(PointerPtr = (1 );PointerPtr<(5 );PointerPtr++)
			{
				if( (s16)Angle <= (s16)(- 16 * 90 * (PointerPtr )) ) //指针越过当前象限，该象限显示矩形
				{
					ShearAngleX = 0;//不发生错切
					ShearAngleY = 0;//不发生错切
					RotateAngle = - 16 * 90 * (PointerPtr - 1);//旋转到该象限位置
					ScaleX = 512;
					ScaleY = 512;
				}
				else if( (s16)Angle>= (s16)(- 16 * 90 * (PointerPtr - 1 ) ))//指针未到达该象限，该象限不显示
				{
					ShearAngleX = 0;//不发生错切
					ShearAngleY = 0;//不发生错切
					RotateAngle = -16 * 90 * (PointerPtr - 1 );//旋转到该象限位置
					ScaleX = 0xf;//缩小mask，使得该象限不显示
					ScaleY = 0xf;
				}
				else //指针停留在该象限中
				{
					ShearAngleX = 0;
					ShearAngleY = Angle + PointerPtr * 90 * 16;
					if(ShearAngleY >= 89*16)
						ShearAngleY = 89*16;
					if(ShearAngleY <= -89*16)
						ShearAngleY = -89*16;
					RotateAngle = -16 * 90 * (PointerPtr - 1) + ShearAngleX;//旋转到该象限位置
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
	else if(MeterMode == 2)//极简模式
	{
		TexturePtr[0].mTexAttr |= TEXTURE_USING_WIDGET_BOX;
		if(rotatingDir)//顺时针旋转
		{
			for(PointerPtr = 1  ;PointerPtr<5;PointerPtr++)
			{
				if(Angle>= 16 * 90 * (PointerPtr)) //指针越过当前象限，该象限显示矩形
				{
					ShearAngleX = 0;//不发生错切
					ShearAngleY = 0;//不发生错切
					RotateAngle = 16 * 90 * (PointerPtr );//旋转到该象限位置
					ScaleX = 512;
					ScaleY = 512;
				}
				else if(Angle<= 16 * 90 * ((PointerPtr ) - 1 ))//指针未到达该象限，该象限不显示
				{
					ShearAngleX = 0;//不发生错切
					ShearAngleY = 0;//不发生错切
					RotateAngle = 16 * 90 * (PointerPtr);//旋转到该象限位置
					ScaleX = 0xf;//缩小mask，使得该象限不显示
					ScaleY = 0xf;
				}
				else //指针停留在该象限中
				{
					ShearAngleX = 90*16*(PointerPtr ) - Angle;
					if(ShearAngleX >= 89*16)
						ShearAngleX = 89*16;
					if(ShearAngleX <= -89*16)
						ShearAngleX = -89*16;
					ShearAngleY = 0;
					RotateAngle = 16 * 90 * ((PointerPtr ));//旋转到该象限位置
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
		else //逆时针旋转
		{
			
			for(PointerPtr = (1 );PointerPtr<(5 );PointerPtr++)
			{
				if( (s16)Angle <= (s16)(- 16 * 90 * (PointerPtr )) ) //指针越过当前象限，该象限显示矩形
				{
					ShearAngleX = 0;//不发生错切
					ShearAngleY = 0;//不发生错切
					RotateAngle = - 16 * 90 * (PointerPtr - 1);//旋转到该象限位置
					ScaleX = 512;
					ScaleY = 512;
				}
				else if( (s16)Angle>= (s16)(- 16 * 90 * (PointerPtr - 1 ) ))//指针未到达该象限，该象限不显示
				{
					ShearAngleX = 0;//不发生错切
					ShearAngleY = 0;//不发生错切
					RotateAngle = -16 * 90 * (PointerPtr - 1 );//旋转到该象限位置
					ScaleX = 0xf;//缩小mask，使得该象限不显示
					ScaleY = 0xf;
				}
				else //指针停留在该象限中
				{
					ShearAngleX = Angle + PointerPtr * 90 * 16;
					if(ShearAngleX >= 89*16)
						ShearAngleX = 89*16;
					RotateAngle = -16 * 90 * (PointerPtr - 1) + ShearAngleX;//旋转到该象限位置
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
	WidgetClassPtr p_wptr,   //控件指针
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
	
	//记录当前的动画执行的值 by Mr.z
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
