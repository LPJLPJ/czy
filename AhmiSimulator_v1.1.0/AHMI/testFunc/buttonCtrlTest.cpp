////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     moduleTest.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//   test button module
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "moduleTest.h"
#include "ButtonClass.h"
#include "testFunc.h"
#include <iostream>
#include <fstream>

extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;
extern u16             WorkingPageID;

#ifdef AHMI_CORE

//-----------------------------
// 函数名： moduleTestTask
// 调用模块测试函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void buttonCtrlTest(void)
{
	WidgetClass testButton;
	WidgetClassPtr curWidget;
	ButtonClass buttonWidget;
	int i;
	u16 widgetCount;
	u8 widgetType;
	std::ofstream buttonTestLogFile;
	ActionTriggerClass actionTest;
//	TagClassPtr pBindingTag;
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	buttonTestLogFile.open("testLog\\buttonTestLogFile.Log",std::ios::out);
	ERROR_PRINT("testing button\n");

	ERROR_PRINT("test illegal data\n");
	buttonTestLogFile << "test illegal data\n";
	//test illegal data, we need to create one button widget, then set this widget illegal data
	//find the first button widget and copy the attritube
	for(widgetCount = 0; widgetCount < gPagePtr[WorkingPageID].mNumOfWidget; widgetCount ++)
	{
		curWidget = &gPagePtr[WorkingPageID].pWidgetList[widgetCount];
		widgetType = curWidget->WidgetAttr &0x1f;
		if(widgetType == BUTTON_TEX)
		{
			//copy the attribute
			for(i = 0; i < WIDGET_UNVAR; i++)
			{
				testButton.WDG_u16_userVariables[i] = curWidget->WDG_u16_userVariables[i];
				testButton.mWidgetMatrix.A = curWidget-> mWidgetMatrix.A;
				testButton.mWidgetMatrix.B = curWidget-> mWidgetMatrix.B;
				testButton.mWidgetMatrix.C = curWidget-> mWidgetMatrix.C;
				testButton.mWidgetMatrix.D = curWidget-> mWidgetMatrix.D;
				testButton.mWidgetMatrix.E = curWidget-> mWidgetMatrix.E;
				testButton.mWidgetMatrix.F = curWidget-> mWidgetMatrix.F;
			
			}
			break;
		}
	}
	if(widgetCount == gPagePtr[WorkingPageID].mNumOfWidget)
	{
		ERROR_PRINT("connot find button widget");
		return;
	}
	//set the width 
	ERROR_PRINT("set offset,width and height to minus value or out of banduary");
	buttonTestLogFile << "set width and height to minus value or out of banduary\n";
	testButton.WidgetWidth = (u16)(-10);
	testButton.WidgetHeight = 5678;
	testButton.WidgetOffsetX = -5000;
	testButton.WidgetOffsetY = 10000;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testButton.BindTagID];
	buttonWidget.widgetCtrl( &testButton, &actionTest, 0);
	testButton.WidgetWidth = 0;
	testButton.WidgetHeight = 0;
	testButton.WidgetOffsetX = 0;
	testButton.WidgetOffsetY = 0;
	buttonWidget.widgetCtrl( &testButton, &actionTest, 0);
	buttonTestLogFile << "check whether the trace window print error information. only when error information is printed is this test passed\n";
	

	//illegal input
	ERROR_PRINT("set illegal input\n");
	buttonTestLogFile << "set illegal input\n";
	buttonWidget.widgetCtrl( NULL, NULL, 0);
	testButton.WidgetWidth = 10;
	testButton.WidgetHeight = 10;
	testButton.WidgetOffsetX = 0;
	testButton.WidgetOffsetY = 0;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = NULL;
	buttonWidget.widgetCtrl( &testButton, &actionTest, 0);
	ERROR_PRINT("illegal input test passed\n");
	buttonTestLogFile << "illegal input test passed\n";

	//banduary
	ERROR_PRINT("test banduary data\n");
	buttonTestLogFile << "test banduary data\n";
	//offset ranges from -1920-1920
	//width height ranges from 1-1920
	int offsetX = -1920;
	int offsetY = -1920;
	int width = 1;
	int height = 1;
	for(offsetX = -1920; offsetX <= 1920; offsetX ++ )			
	{
		testButton.WidgetWidth = width;
		testButton.WidgetHeight = height;
		testButton.WidgetOffsetX = offsetX;
		testButton.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testButton.BindTagID];
		if(buttonWidget.widgetCtrl( &testButton, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}

	offsetX = -1920;
	for(offsetY = -1920; offsetY <= 1920; offsetY ++)
	{
		testButton.WidgetWidth = width;
		testButton.WidgetHeight = height;
		testButton.WidgetOffsetX = offsetX;
		testButton.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testButton.BindTagID];
		if(buttonWidget.widgetCtrl( &testButton, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	offsetY = -1920;
	for(width = 1; width <= 1920; width ++)
	{
		testButton.WidgetWidth = width;
		testButton.WidgetHeight = height;
		testButton.WidgetOffsetX = offsetX;
		testButton.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testButton.BindTagID];
		if(buttonWidget.widgetCtrl( &testButton, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	width = 1;
	for(height = 1; height <= 1920; height ++)
	{
		testButton.WidgetWidth = width;
		testButton.WidgetHeight = height;
		testButton.WidgetOffsetX = offsetX;
		testButton.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testButton.BindTagID];
		if(buttonWidget.widgetCtrl( &testButton, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	height = 1;
	if(ahmi_func_status == AHMI_FUNC_SUCCESS)
	{
		ERROR_PRINT("banduary data test passed\n");
		buttonTestLogFile << "banduary data test passed\n";
	}
	else 
	{
		ERROR_PRINT("banduary data test failed\n");
		buttonTestLogFile << "banduary data test failed\n";
	}


}


#endif
