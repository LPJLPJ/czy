////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     WidgetClassInterface.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    definition used only by widget
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef WIDGETCLASSINTERFACE__H
#define WIDGETCLASSINTERFACE__H

#include "publicType.h"
#include "publicDefine.h"
#include "widgetClassInterface_c1.h"

#ifdef AHMI_CORE
class WidgetClassInterface
{
public:
	//函数
	WidgetClassInterface(void);
	~WidgetClassInterface(void);
	//初始化tagtrigger
	 //virtual funcStatus initWidget(
		//WidgetClassPtr p_wptr,   //控件指针
		//u32 *u32p_sourceShift,   //sourcebuffer指针
		//u8 u8_pageRefresh,       //页面刷新
		//TileBoxClassPtr pTileBox //包围盒
		// ) = 0;
	 ////控件控制函数
	 //virtual funcStatus widgetCtrl(
		// WidgetClassPtr p_wptr,  //控件指针
		// ActionTriggerClassPtr ActionPtr,
		// u8 u8_pageRefresh      //页面刷新
		// ) = 0;
	 ////绘制纹理
	 funcStatus drawTexture(
		WidgetClassPtr p_wptr,   //????
		u32 *u32p_sourceShift,   //sourcebuffer??
		u8 RefreshType ,       //???????,????????????????
		TileBoxClassPtr pTileBox, //???
		u8 staticTextureEn          //???????????
		);
};
#endif

#endif
