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
//    definition used only by widget
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef WIDGETCLASS_CD__H
#define WIDGETCLASS_CD__H
#include "publicDefine.h"
#ifdef AHMI_CORE

#define WIDGET_GENERAL_NUM (21) //通用参数个数 增加tagchange的指令操作，增加一个属性
#define WIDGET_UNVAR (WIDGET_GENERAL_NUM + 19)  //widget变量宽度

#define WidgetAttr							WDG_u16_userVariables[(u32)0]  
#define NumOfTex							WDG_u16_userVariables[1]  
#define StartNumofTex						WDG_u16_userVariables[2]  //控件中纹理在纹理结构体的第一个元素下标
#define WidgetWidth							WDG_u16_userVariables[3]  //控件整体宽度
#define WidgetHeight						WDG_u16_userVariables[4]  //控件整体高度
#define WidgetOffsetX						WDG_u16_userVariables[5] //控件位置，左上角X坐标,整数
#define WidgetOffsetY						WDG_u16_userVariables[6] //控件位置，左上角Y坐标，整数
#define BindTagID							WDG_u16_userVariables[7]//所绑定的TagID
#define BindMatrix							WDG_u16_userVariables[8]  //绑定的矩阵


//触发事件专用参数
#define EnterHighAlarmAction				WDG_u16_userVariables[9]
#define EnterLowAlarmAction					WDG_u16_userVariables[10]
#define LeaveHighAlarmAction				WDG_u16_userVariables[11]
#define LeaveLowAlarmAction					WDG_u16_userVariables[12]
#define OnRealeaseAction					WDG_u16_userVariables[13]	
#define TagChangeAction					    WDG_u16_userVariables[20]   //add by Mr.z to realize tag change

//控件动画专用参数
#define TOTALFRAME_AND_NOWFRAME             WDG_u16_userVariables[14]   //[7:0] now frame [15:8] total frame. if total frame == 0, indicates that the widget contains no animaiton
#define START_TAG_L                         WDG_u16_userVariables[15]   //start value, used for animation
#define START_TAG_H                         WDG_u16_userVariables[16]	//start value, used for animation
#define STOP_TAG_L                          WDG_u16_userVariables[17]   //stop value, used for animation
#define STOP_TAG_H                          WDG_u16_userVariables[18]   //stop value, used for animation
#define ATTATCH_CANVAS						WDG_u16_userVariables[19]   //附着的canvas

//仪表盘、滑块、示波器、数字框专用参数
#define LowAlarmValueL						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0] 
#define LowAlarmValueH						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1] 
#define HighAlarmValueL						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 2]
#define HighAlarmValueH						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 3]
#define MinValueL							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 4] 
#define MinValueH							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 5]
#define MaxValueL							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 6]  
#define MaxValueH							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 7]

//进度条专用
#define StartColorValueGB					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8]
#define EndColorValueBAndStartColorValueR	WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9]
#define EndColorValueRG						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10]

//多色进度条专用
#define COLOR0GB                            WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8]
#define COLOR1B_COLOR0R                     WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9]
#define COLOR1RG							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10]
#define COLOR2GB							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 11]
#define COLOR2R                             WDG_u16_userVariables[WIDGET_GENERAL_NUM + 14]
#define THRESHOLD0_L                        WDG_u16_userVariables[WIDGET_GENERAL_NUM + 15]
#define THRESHOLD0_H                        WDG_u16_userVariables[WIDGET_GENERAL_NUM + 16]
#define THRESHOLD1_L                        WDG_u16_userVariables[WIDGET_GENERAL_NUM + 17]
#define THRESHOLD1_H                        WDG_u16_userVariables[WIDGET_GENERAL_NUM + 18]

//按钮专用
#define OnEnteredAction                     WDG_u16_userVariables[13]                         //键盘上确认按钮按下的指令
#define KeyboardPreAndNextWidget            WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0]     //{前一个高亮，后一个高亮}
#define NumOfButtonAndCurHighLight          WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1]     //{按钮个数，当前高亮值}
#define ReleaseColorValueGB					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 2]
#define PressValueBAndReleaseColorValueR	WDG_u16_userVariables[WIDGET_GENERAL_NUM + 3]
#define PressColorValueRG					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 4]

//仪表盘专用参数
#define MeterMinAngle						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8] 
#define MeterMaxAngle						WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9] 	
#define StartAngle							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10]    //初始角度
     
//动态纹理专用参数
#define DynamicTexMinAngle					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8] 
#define DynamicTexMaxAngle					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9] 	 
                                            
//示波器专用参数       
#define CurPosI								WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8] //光标序号
#define OSCColor							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9] //波形颜色，R5G6B5
#define BLANK_X                             WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10] //X方向留白                 
#define BLANK_Y                             WDG_u16_userVariables[WIDGET_GENERAL_NUM + 11] //Y方向留白
	

	
//动画纹理，视频纹理专用参数
#define FrameSpeed							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0] //每隔多少毫秒显示一帧
#define NowTime								WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1] //现在经过了多少毫秒

//动画纹理用参数
#define TotalSlice							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1] //总共的Slice数量

//多行控件专用参数
#define NumOfLine							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8]        //8~11位表示数字框的对齐方式，高12~15位表示数据超出最大值后的处理方式 [7:0] 表征数字控件的动画类型，目前仅有0表示无动画，1表示两个数字纹理的切换动画
#define PreviousTexturePtrFlag				WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9]        //用于单行模式,0表示动画下移，1表示动画上移
#define CurValueL							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10]        //中间数字框显示的值
#define CurValueH							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 11]        //中间数字框显示的值


//进度条、动态纹理、仪表盘、数字框、示波器专用
#define OldValueL							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 12]       //低16位
#define OldValueH							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 13]       //高16位
#define SPACINGX							WDG_u16_userVariables[WIDGET_GENERAL_NUM + 14]       //数字间距 SpacingX                                                
#define ANIMATIONCURVALUE_L					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 15]       //仪表盘或者进度条中动画执行的当前值 by Mr.z
#define ANIMATIONCURVALUE_H					WDG_u16_userVariables[WIDGET_GENERAL_NUM + 16]       //仪表盘或者进度条中动画执行的当前值 by Mr.z 
//文本框专用参数 数字框专用参    数                            
                                               


//IO专用
#define IOX									WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0]      //选定的IO口
#define PinMusk								WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1]      //有效的Pin
#define OpenPin								WDG_u16_userVariables[WIDGET_GENERAL_NUM + 2]      //已经打开的Pin


//纹理类型

#define   DYNAMIC_TEX	 1                 //基本纹理，纹理地址可变
#define   BUTTON_TEX	 2                 //按钮纹理，基本纹理的特殊应用，增加action
#define   METER_TEX		 3                 //仪表盘
#define   PROGBAR_TEX	 4                 //进度条
#define   SLIDER_TEX	 5                 //滑块
#define   OSCSCP_TEX	 6                 //示波器
#define   TEXTBOX_TEX	 7                 //文本框
#define   NUMBOX_TEX	 8                 //数字框，文本框的特殊应用
#define   ANIME_TEX      9                 //动画纹理，基本纹理的特殊应用，可以自动改变所贴纹理
#define   VIDEO_TEX     10                 //视频纹理，连接摄像头
#define   KNOB_TEX      11                 //旋钮
#define   CLOCK_TEX     12                 //时间框
#define   ON_OFF_TEX    13                 //亮暗
#define   TEXTURE_NUM    14                 //图层数字控件
#define   TEXTURE_TIME    15                 //图层时间控件


//纹理所用到的宏
//dynamicTex
#define  DYNAMIC_TYPE_BIT 0x00E0 //动态纹理类型的位

//button
#define  SINGLEBUTTON (0) //简单按钮
#define  SWITCHBUTTON (1) //开关
#define  COMPLEXBUTTON (2) //复杂按钮
#define  SINGLE_COLOR_BUTTON_BIT (0x800) //区别是否为颜色按钮，widget属性第11位

//clock
#define CLOCK_HOUR_MINUTE_SECOND_MODE (0) //时分秒
#define CLOCK_HOUR_MINUTE_MODE        (1) //时分
#define CLOCK_GANG_YEAR_MODE          (2) //斜杠年月日
#define CLOCK_MINUS_YEAR_MODE         (3) //减号年月日  
//rotating dynamic texture
#define ROTATING_DYNAMIC_TEX 0x20
#define DIM_TEX              0x60



//Progbar
#define  CHANGECOLOR (0x100) //进度条是否改变颜色
#define  CHANGECOLOR_MODE (0x200) //0表示渐变，1表示突变
#define  BAR_IS_COLOR   (0x80)  //进度条是不是颜色
#define	 LINE_FILE_EN (0x40)  //是否有指针纹理
#define  LINE_CHANGE_COLOR (0x0400) //指针是否改变颜色
#define  PROBARDISPLAY		(0x0800)//是否显示
#define  THRESHOLD_NUM      (0x1000) //阈值个数，0表示1个，1表示2个
#define  PROGBAR_DIR        (0x20)  //方向

//数字框对齐方式
#define		RIGHTALIGN	0
#define     CENTERALIGN	1
#define		LEFTALIGN	2

//数据超过最大值后的处理方式 0：显示最大值， 1：不显示
#define    OVERFLOW_DISPLAY     0
#define    OVERFLOW_NON_DISPLAY 1


//旋转方式
#define CENTRAL_ROTATE (1) //中心旋转
#define TRANSLATION    (2) //平移
#define DIM            (3) //亮暗

//meter
#define  METER_SIMPLIFY 0x80

//video
#define VIDEO_TYPE_HDMI 0x20

#define  NUMOFNUMBER 32/4//最多显示多少个数字

//test
#define MAX_WIDTH_AND_HEIGHT 1920
#define MAX_ANGLE            360

#endif

#endif
