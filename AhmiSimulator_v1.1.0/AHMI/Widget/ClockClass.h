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
#ifndef CLOCKCLASS__H
#define CLOCKCLASS__H
#include "widgetClassInterface.h"

#ifdef AHMI_CORE
class ActionTriggerClass;

class ClockClass
{
public:
	ClockClass();
	~ClockClass();

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
	 //clock 专用函数
	 //获取当前控件的时分秒
	 funcStatus getHourMiniteSecond(WidgetClassPtr p_wptr,u8* hour, u8* minute, u8* second);
	 //获取当前的年月日
	 funcStatus getYearMonthDay(WidgetClassPtr p_wptr, u16* year, u8* month, u8* day);
	 //增加当前时分秒
	 funcStatus accumlateHourMinuteSecond(u8 curHighLight, u8* hour, u8* minute, u8* second);
	 //增加当前年月日
	 funcStatus accumlateYearMonthDay(u8 curHighLight, u16* year, u8* month, u8* day);
	 //减少当前时分秒
	 funcStatus decrementHourMinuteSecond(u8 curHighLight, u8* hour, u8* minute, u8* second);
	 //减少当前年月日
	 funcStatus decrementYearMonthDay(u8 curHighLight, u16* year, u8* month, u8* day);
	 //从BCD码转为十进制
	 funcStatus translateFromBCDIntoNumber(u16 bcdNum, u16* num);
};

#endif

#endif
