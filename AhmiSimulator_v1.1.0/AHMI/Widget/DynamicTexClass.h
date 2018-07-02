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
#ifndef DYNAMICTEXCLASS__H
#define DYNAMICTEXCLASS__H
#include "widgetClassInterface.h"

#ifdef AHMI_CORE
class ActionTriggerClass;

class DynamicTexClass:
	public WidgetClassInterface
{
public:
	DynamicTexClass();
	~DynamicTexClass();

	//控件控制函数
	funcStatus widgetCtrl(
		 WidgetClassPtr p_wptr,  //控件指针
		 ActionTriggerClassPtr ActionPtr,
		 u8 u8_pageRefresh      //页面刷新
		 );
	//初始化
	 funcStatus initWidget(
		WidgetClassPtr p_wptr,   //控件指针
		u32 *u32p_sourceShift,   //sourcebuffer指针
		u8 RefreshType ,       //绘制的动画类型，根据动画类型改变绘制控件的包围盒
		u8 u8_animationEnable,   //需要绘制控件动画，取消控件本身包围盒
		TileBoxClassPtr pTileBox, //包围盒
		u8 staticTextureEn          //是否绘制到静态存储空间
		 );
};

#endif

#endif
