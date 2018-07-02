////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     CanvasClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//   
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef BUTTONCLASS__H
#define BUTTONCLASS__H
#include "widgetClassInterface.h"

#ifdef AHMI_CORE
#ifdef __cplusplus
class ActionTriggerClass;

class ButtonClass 
{
public:
	ButtonClass(void);
	~ButtonClass(void);
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
		u8 u8_pageRefresh,       //页面刷新
		u8 RefreshType ,       //绘制的动画类型，根据动画类型改变绘制控件的包围盒
		TileBoxClassPtr pTileBox, //包围盒
		u8 staticTextureEn          //是否绘制到静态存储空间
		 );
};

typedef ButtonClass* ButtonClassPtr;
typedef ActionTriggerClass* ActionTriggerClassPtr;
#endif
#endif

#endif
