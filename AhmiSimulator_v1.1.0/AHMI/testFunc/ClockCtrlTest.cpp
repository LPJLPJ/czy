////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2017/06/13
// File Name:     DashBoardCtrlTest.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 201X/0X/XX
// Additional Comments:
//   test dashboard module
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "moduleTest.h"
#include "ClockClass.h"
#include "testFunc.h"
#include <iostream>
#include <fstream>

extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;
extern u16             WorkingPageID;

#ifdef AHMI_CORE

//-----------------------------
// 函数名： ClockCtrlTest
// 调用模块测试函数
// 参数列表： void
// 备注(各个版本之间的修改):
/************************************************************************/
/* Test Items
   1\ Negative value or boundary value for Width\Height| OffsetX\OffsetY
   2\ Illegal pointer NULL
   3\ make the value of Width\Height| OffsetX\OffsetY in (-1920,1920)
*/
/************************************************************************/
//-----------------------------
void ClockCtrlTest(void)
{
	WidgetClass testClock;
	WidgetClassPtr curWidget;
	ClockClass clockWidget;
	int i;
	u16 widgetCount;
	u8 widgetType;
	std::ofstream clockTestLogFile;
	ActionTriggerClass actionTest;
//	TagClassPtr pBindingTag;
	TextureClassPtr pBasicTextureListTmp;
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	clockTestLogFile.open("testLog\\clockTestLogFile.Log",std::ios::out);
	ERROR_PRINT("testing clock\n");

	ERROR_PRINT("test illegal data\n");
	clockTestLogFile << "test illegal data\n";
	//test illegal data, we need to create one clock widget, then set this widget illegal data
	//find the first clock widget and copy the attritube
	for(widgetCount = 0; widgetCount < gPagePtr[WorkingPageID].mNumOfWidget; widgetCount ++)
	{
		curWidget = &gPagePtr[WorkingPageID].pWidgetList[widgetCount];
		widgetType = curWidget->WidgetAttr &0x1f;
		if(widgetType == CLOCK_TEX)
		{
			//copy the attribute
			for(i = 0; i < WIDGET_UNVAR; i++)
			{
				testClock.WDG_u16_userVariables[i] = curWidget->WDG_u16_userVariables[i];
				testClock.mWidgetMatrix.A = curWidget-> mWidgetMatrix.A;
				testClock.mWidgetMatrix.B = curWidget-> mWidgetMatrix.B;
				testClock.mWidgetMatrix.C = curWidget-> mWidgetMatrix.C;
				testClock.mWidgetMatrix.D = curWidget-> mWidgetMatrix.D;
				testClock.mWidgetMatrix.E = curWidget-> mWidgetMatrix.E;
				testClock.mWidgetMatrix.F = curWidget-> mWidgetMatrix.F;

			}
			break;
		}
	}
	if(widgetCount == gPagePtr[WorkingPageID].mNumOfWidget)
	{
		ERROR_PRINT("cannot find clock widget");
		return;
	}
	
	//set the width 
	ERROR_PRINT("set offset,width and height to negative value or out of boundary");
	clockTestLogFile << "set width and height to negative value or out of boundary\n";
	testClock.WidgetWidth = (u16)(-10);
	testClock.WidgetHeight = 5678;
	testClock.WidgetOffsetX = -5000;
	testClock.WidgetOffsetY = 10000;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testClock.BindTagID];
	if(clockWidget.widgetCtrl( &testClock, &actionTest, 0) == AHMI_FUNC_FAILURE)
		ahmi_func_status = AHMI_FUNC_FAILURE;
	testClock.WidgetWidth = 0;
	testClock.WidgetHeight = 0;
	testClock.WidgetOffsetX = 0;
	testClock.WidgetOffsetY = 0;
	if(clockWidget.widgetCtrl( &testClock, &actionTest, 0) == AHMI_FUNC_FAILURE)
		ahmi_func_status = AHMI_FUNC_FAILURE;
	clockTestLogFile << "check whether the trace window print error information. only when error information is printed , this test passed\n";


	//NULL pointer for widgetClassptr\ActionTriggerPtr 
	ERROR_PRINT("set NULL pointer input\n");
	clockTestLogFile << "set NULL pointer input\n";
	if(clockWidget.widgetCtrl( NULL, NULL, 0) ==AHMI_FUNC_FAILURE)
		ahmi_func_status = AHMI_FUNC_FAILURE;
	//NULL pointer for ActionTriggerPtr.mtagPtr
	testClock.WidgetWidth = 10;
	testClock.WidgetHeight = 10;
	testClock.WidgetOffsetX = 0;
	testClock.WidgetOffsetY = 0;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = NULL;
	if(clockWidget.widgetCtrl( &testClock, &actionTest, 0) == AHMI_FUNC_FAILURE)
		ahmi_func_status = AHMI_FUNC_FAILURE;
	//NULL pointer for gPagePtr->pBasicTextureList
	actionTest.mTagPtr = &TagPtr[testClock.BindTagID];
	pBasicTextureListTmp = gPagePtr->pBasicTextureList;
	gPagePtr->pBasicTextureList = NULL;
	if(clockWidget.widgetCtrl( &testClock, &actionTest, 0) == AHMI_FUNC_FAILURE)
		ahmi_func_status = AHMI_FUNC_FAILURE;
	if(AHMI_FUNC_SUCCESS == ahmi_func_status)
	{
		ERROR_PRINT("set NULL pointer input test passed\n");
		clockTestLogFile << "set NULL pointer input test passed\n";
	}else{
		ERROR_PRINT("set NULL pointer input test failed\n");
		clockTestLogFile << "set NULL pointer input test failed\n";
	}
	gPagePtr->pBasicTextureList = pBasicTextureListTmp;

	//boundary
	ERROR_PRINT("test boundary data for OffsetX\\OffsetY Width\\Height\n");
	clockTestLogFile << "test boundary data for OffsetX\\OffsetY Width\\Height\n";
	//offset ranges from -1920-1920
	//width height ranges from 1-1920
	int offsetX = -1920;
	int offsetY = -1920;
	int width = 1;
	int height = 1;
	for(offsetX = -1920; offsetX <= 1920; offsetX ++ )			
	{
		testClock.WidgetWidth = width;
		testClock.WidgetHeight = height;
		testClock.WidgetOffsetX = offsetX;
		testClock.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testClock.BindTagID];
		if(clockWidget.widgetCtrl( &testClock, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}

	offsetX = -1920;
	for(offsetY = -1920; offsetY <= 1920; offsetY ++)
	{
		testClock.WidgetWidth = width;
		testClock.WidgetHeight = height;
		testClock.WidgetOffsetX = offsetX;
		testClock.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testClock.BindTagID];
		if(clockWidget.widgetCtrl( &testClock, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	offsetY = -1920;
	for(width = 1; width <= 1920; width ++)
	{
		testClock.WidgetWidth = width;
		testClock.WidgetHeight = height;
		testClock.WidgetOffsetX = offsetX;
		testClock.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testClock.BindTagID];
		if(clockWidget.widgetCtrl( &testClock, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	width = 1;
	for(height = 1; height <= 1920; height ++)
	{
		testClock.WidgetWidth = width;
		testClock.WidgetHeight = height;
		testClock.WidgetOffsetX = offsetX;
		testClock.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testClock.BindTagID];
		if(clockWidget.widgetCtrl( &testClock, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	height = 1;
	if(ahmi_func_status == AHMI_FUNC_SUCCESS)
	{
		ERROR_PRINT("boundary data test passed\n");
		clockTestLogFile << "boundary data test passed\n";
	}
	else 
	{
		ERROR_PRINT("boundary data test failed\n");
		clockTestLogFile << "boundary data test failed\n";
	}

}
#endif