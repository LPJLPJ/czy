////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     DynamicPage.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef WIDGETCLASS__H
#define WIDGETCLASS__H

#include "publicType.h"
#include "publicDefine.h"
#include "widgetClass_c1.h"
#include "widgetClass_cd.h"
#include "publicDefine.h"

#ifdef AHMI_CORE


class WidgetClass
{
public:
	//变量
	u16 WDG_u16_userVariables[WIDGET_UNVAR];
	matrixClass mWidgetMatrix;
	//函数
	WidgetClass(void);
	~WidgetClass(void);
	//绘制控件
	funcStatus drawWidget(
		TileBoxClassPtr pTileBox,
		u32 *sourceShift,
		u8  pageEnable,
		matrixClassPtr pCanvasMatrix,
		u8 RefreshType,        //绘制的动画类型，根据动画类型改变绘制控件的包围盒
		u8 staticTextureEn          //是否绘制到静态存储空间
		);
	//绘制控件的时候绘制动画
	funcStatus drawWidgetWithAnimation(
		u8 curFrame,     //当前帧
		u8 totalFrame,   //总共帧
		TileBoxClassPtr pSourceBox,
		u32 *sourceshift    //sourcebuffer偏移
		);
	//初始化控件矩阵
	funcStatus initWidget(void);
	//创建控件动画绘制列表
	funcStatus loadWidgetWithAnimation(
		);
	//以动画的方式绘制widget内的纹理
	funcStatus drawTextureWithAnimation(
		u8 curFrame,     //当前帧
		u8 totalFrame,   //总共帧
		TileBoxClassPtr pSourceBox,
		u32 *sourceshift    //sourcebuffer偏移

		);
	//创建纹理动画绘制列表
	funcStatus loadTextureWithAnimation(
		);
	//判断控件是否显示
	funcStatus displayEn(
		u8 widgetID
		);
	//set widget animation tag
	funcStatus setATag(
		u8 value
		);
};
#endif

#endif

