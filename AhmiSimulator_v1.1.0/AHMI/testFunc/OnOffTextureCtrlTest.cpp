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
#include "OnOffTexture.h"
#include "testFunc.h"
#include <iostream>
#include <fstream>

extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;
extern u16             WorkingPageID;

#ifdef AHMI_CORE

//-----------------------------
// 函数名： OnOffTextureCtrlTest
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
void OnOffTextureCtrlTest(void)
{
	WidgetClass testOnOffTexture;
	WidgetClassPtr curWidget;
	OnOffTextureClass OnOffTextureWidget;
	int i;
	u16 widgetCount;
	u8 widgetType;
	std::ofstream OnOffTextureTestLogFile;
	ActionTriggerClass actionTest;
//	TagClassPtr pBindingTag;
	TextureClassPtr pBasicTextureListTmp;
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	OnOffTextureTestLogFile.open("testLog\\OnOffTextureTestLogFile.Log",std::ios::out);
	ERROR_PRINT("testing OnOffTexture\n");

	ERROR_PRINT("test illegal data\n");
	OnOffTextureTestLogFile << "test illegal data\n";
	//test illegal data, we need to create one OnOffTexture widget, then set this widget illegal data
	//find the first clock widget and copy the attribute
	for(widgetCount = 0; widgetCount < gPagePtr[WorkingPageID].mNumOfWidget; widgetCount ++)
	{
		curWidget = &gPagePtr[WorkingPageID].pWidgetList[widgetCount];
		widgetType = curWidget->WidgetAttr &0x1f;
		if(widgetType == ON_OFF_TEX)
		{
			//copy the attribute
			for(i = 0; i < WIDGET_UNVAR; i++)
			{
				testOnOffTexture.WDG_u16_userVariables[i] = curWidget->WDG_u16_userVariables[i];
				testOnOffTexture.mWidgetMatrix.A = curWidget-> mWidgetMatrix.A;
				testOnOffTexture.mWidgetMatrix.B = curWidget-> mWidgetMatrix.B;
				testOnOffTexture.mWidgetMatrix.C = curWidget-> mWidgetMatrix.C;
				testOnOffTexture.mWidgetMatrix.D = curWidget-> mWidgetMatrix.D;
				testOnOffTexture.mWidgetMatrix.E = curWidget-> mWidgetMatrix.E;
				testOnOffTexture.mWidgetMatrix.F = curWidget-> mWidgetMatrix.F;

			}
			break;
		}
	}
	if(widgetCount == gPagePtr[WorkingPageID].mNumOfWidget)
	{
		ERROR_PRINT("cannot find OnOffTexture widget");
		return;
	}
	
	//set the width 
	ERROR_PRINT("set offset,width and height to negative value or out of boundary");
	OnOffTextureTestLogFile << "set width and height to negative value or out of boundary\n";
	testOnOffTexture.WidgetWidth = (u16)(-10);
	testOnOffTexture.WidgetHeight = 5678;
	testOnOffTexture.WidgetOffsetX = -5000;
	testOnOffTexture.WidgetOffsetY = 10000;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testOnOffTexture.BindTagID];
	if(OnOffTextureWidget.widgetCtrl( &testOnOffTexture, &actionTest, 0) == AHMI_FUNC_FAILURE)
		ahmi_func_status = AHMI_FUNC_FAILURE;
	testOnOffTexture.WidgetWidth = 0;
	testOnOffTexture.WidgetHeight = 0;
	testOnOffTexture.WidgetOffsetX = 0;
	testOnOffTexture.WidgetOffsetY = 0;
	if(OnOffTextureWidget.widgetCtrl( &testOnOffTexture, &actionTest, 0) == AHMI_FUNC_FAILURE)
		ahmi_func_status = AHMI_FUNC_FAILURE;
	OnOffTextureTestLogFile << "check whether the trace window print error information. only when error information is printed , this test passed\n";


	//NULL pointer for widgetClassptr\ActionTriggerPtr 
	ERROR_PRINT("set NULL pointer input\n");
	OnOffTextureTestLogFile << "set NULL pointer input\n";
	if(OnOffTextureWidget.widgetCtrl( NULL, NULL, 0) ==AHMI_FUNC_FAILURE)
		ahmi_func_status = AHMI_FUNC_FAILURE;
	//NULL pointer for ActionTriggerPtr.mtagPtr
	testOnOffTexture.WidgetWidth = 10;
	testOnOffTexture.WidgetHeight = 10;
	testOnOffTexture.WidgetOffsetX = 0;
	testOnOffTexture.WidgetOffsetY = 0;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = NULL;
	if(OnOffTextureWidget.widgetCtrl( &testOnOffTexture, &actionTest, 0) == AHMI_FUNC_FAILURE)
		ahmi_func_status = AHMI_FUNC_FAILURE;
	//NULL pointer for gPagePtr->pBasicTextureList
	actionTest.mTagPtr = &TagPtr[testOnOffTexture.BindTagID];
	pBasicTextureListTmp = gPagePtr->pBasicTextureList;
	gPagePtr->pBasicTextureList = NULL;
	if(OnOffTextureWidget.widgetCtrl( &testOnOffTexture, &actionTest, 0) == AHMI_FUNC_FAILURE)
		ahmi_func_status = AHMI_FUNC_FAILURE;
	if(AHMI_FUNC_SUCCESS == ahmi_func_status)
	{
		ERROR_PRINT("set NULL pointer input test passed\n");
		OnOffTextureTestLogFile << "set NULL pointer input test passed\n";
	}else{
		ERROR_PRINT("set NULL pointer input test failed\n");
		OnOffTextureTestLogFile << "set NULL pointer input test failed\n";
	}
	gPagePtr->pBasicTextureList = pBasicTextureListTmp;

	//boundary
	ERROR_PRINT("test boundary data for OffsetX\\OffsetY Width\\Height\n");
	OnOffTextureTestLogFile << "test boundary data for OffsetX\\OffsetY Width\\Height\n";
	//offset ranges from -1920-1920
	//width height ranges from 1-1920
	int offsetX = -1920;
	int offsetY = -1920;
	int width = 1;
	int height = 1;
	for(offsetX = -1920; offsetX <= 1920; offsetX ++ )			
	{
		testOnOffTexture.WidgetWidth = width;
		testOnOffTexture.WidgetHeight = height;
		testOnOffTexture.WidgetOffsetX = offsetX;
		testOnOffTexture.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testOnOffTexture.BindTagID];
		if(OnOffTextureWidget.widgetCtrl( &testOnOffTexture, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}

	offsetX = -1920;
	for(offsetY = -1920; offsetY <= 1920; offsetY ++)
	{
		testOnOffTexture.WidgetWidth = width;
		testOnOffTexture.WidgetHeight = height;
		testOnOffTexture.WidgetOffsetX = offsetX;
		testOnOffTexture.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testOnOffTexture.BindTagID];
		if(OnOffTextureWidget.widgetCtrl( &testOnOffTexture, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	offsetY = -1920;
	for(width = 1; width <= 1920; width ++)
	{
		testOnOffTexture.WidgetWidth = width;
		testOnOffTexture.WidgetHeight = height;
		testOnOffTexture.WidgetOffsetX = offsetX;
		testOnOffTexture.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testOnOffTexture.BindTagID];
		if(OnOffTextureWidget.widgetCtrl( &testOnOffTexture, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	width = 1;
	for(height = 1; height <= 1920; height ++)
	{
		testOnOffTexture.WidgetWidth = width;
		testOnOffTexture.WidgetHeight = height;
		testOnOffTexture.WidgetOffsetX = offsetX;
		testOnOffTexture.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testOnOffTexture.BindTagID];
		if(OnOffTextureWidget.widgetCtrl( &testOnOffTexture, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	height = 1;
	if(ahmi_func_status == AHMI_FUNC_SUCCESS)
	{
		ERROR_PRINT("boundary data test passed\n");
		OnOffTextureTestLogFile << "boundary data test passed\n";
	}
	else 
	{
		ERROR_PRINT("boundary data test failed\n");
		OnOffTextureTestLogFile << "boundary data test failed\n";
	}

}
#endif