////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     Texture.cpp
// Project Name:  AHMISimGenDemoDoublePage800x480.cpp
// 
// Revision:
// Revision 2.01 - File Created 2015/11/19 by...
// Additional Comments:
//    定义控制台应用程序的入口点。
// 
////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "private.h"
#include <stdlib.h>
#include "AHMISerializer\AHMISerializer.h"
//#include "jason.h"
#include "globalArg.h"
#include "uncompress.h"
#include <iostream>
#include <windows.h>

extern "C" extern char* filename;
extern "C" extern int interfaceType;

int main(/*int argc, _TCHAR* argv[]*/)
{

	//初始化AHMISerializer类
	AHMISerializer mAHMISerializer(
		VERSION,
		800,
		480,
		16,
		32,
		1,// num of timer
		1,
		100,
		//add the refresh information 2015/12/07
		0,
		INTERFACETYPE_AHMI,
		255
		);

	//

	mAHMISerializer.mPage.AddPage(0,0,0,"page0");
	//mAHMISerializer.mTag.AddTag("constantTag");
	//mAHMISerializer.mTag.SetTagValue("constantTag",(u32)0);
	//mAHMISerializer.mFont.AddFontAllFromFile("font\\font_alpha4.dat");

	////页面设置2ms时钟
	Instruction Page0Action(&mAHMISerializer.mTag);
	//Page0Action.AddInst(SET,"Number",(u32)0);
	Page0Action.AddInst_withDoubleSource(SET_TIMER,(u8)IntervalTime_variable,(u32)200);
	Page0Action.AddInst_withDoubleSource(SET_TIMER,(u8)StepValue_variable,(u32)1);
	Page0Action.AddInst_withDoubleSource(SET_TIMER,(u8)StopValue_variable,(u32)200);
	Page0Action.AddInst_withDoubleSource(SET_TIMER,(u8)startValue_variable,(u32)0);
	//Page0Action.AddInst(SET,"SysTmr_0_CurVal",(u32)0);
	Page0Action.AddInst_withDoubleSource(SET_TIMER,(u8)TimerMode_variable,(u32)TimerStart | TimerRoundRun | TimerIncrement);
	mAHMISerializer.mPage.CurPage()->AddPageAction(NULL,&Page0Action);
	Page0Action.clear();
	mAHMISerializer.mPage.CurPage()->mCanvasClass.AddCanvas(800,480,0,0,1);
	/*mAHMISerializer.mPage.CurPage()->mCanvasClass.AddCanvas(800,480,0,0,1,"constantTag");
	mAHMISerializer.mPage.CurPage()->mWidgetClass.AddClock(100,100,0,CLOCK_GANG_YEAR_MODE,0,"System_RTC_Year_Tag",255,255,255,255,NULL);
	WidgetClassPtr curWidgetPtr; 
	mAHMISerializer.mPage.CurPage()->mWidgetClass.findWidgetByID(curWidgetPtr, 0);
	mAHMISerializer.mPage.CurPage()->mWidgetClass.addWidgetToEnlightedVector(curWidgetPtr, 0);*/
	//mAHMISerializer.mTag.AddTag("num");
	//mAHMISerializer.mTag.SetTagValue("num",(u32)0);
	mAHMISerializer.mPage.CurPage()->mWidgetClass.AddSimplifyMeter(0,0,0,360,0,360,0,360,0, NULL, NULL, NULL, NULL,  "SysTmr_0_CurVal", 400, 400, "image12_3\\meter\\l_background_test.png",true);

	////开机动画界面
	//mAHMISerializer.mPage.AddPage("image12_3\\三色.bmp","page0");
	//mAHMISerializer.mPage.CurPage()->mCanvasClass.AddCanvas(800, 480, 0, 0, 1, "constantTag");

	//mAHMISerializer.mTag.AddTag("num");
	//mAHMISerializer.mFont.AddFontAllFromFile("font\\font_alpha4.dat");
	//mAHMISerializer.mPage.CurPage()->mWidgetClass.AddNumDisp(100,100,3,0,0,0,100,0,0,999,0,999,NULL,NULL,NULL,NULL,"num",255,255,255,255,0,0,false,0,"numDisp");

	//Instruction buttonAction(&mAHMISerializer.mTag);
	//buttonAction.AddInst(GOTO, "numDisp", (u32)(1 + (10 << 8) + (0 << 16) ), &(mAHMISerializer.mPage.CurPage()->mWidgetClass) );
	//mAHMISerializer.mPage.CurPage()->mWidgetClass.AddButton(200,200,NULL,&buttonAction,NULL,"1.bmp","2.bmp");

	////小键盘页面
	//mAHMISerializer.mPage.AddNumKeyPadPage(255,255,255,"NULL","num");
	//写入文件
	mAHMISerializer.WriteToFile("testdata.acf");

	///////////////////////////////////////////////////////////////jason 开启//////////////////////////////////////////////////////////
////获取文件名
//	GetGlobalArgs();
//	std::cout <<"the file name : ";
//	std::cout << filename << std::endl;
//
////解压缩
//	CreateDirectory(_T("json"),NULL);
//	uncompressFromFile(filename);
////解析jason
//	ParseJsonFromFile("json\\data.json", interfaceType);
//
//
	return 0;

}