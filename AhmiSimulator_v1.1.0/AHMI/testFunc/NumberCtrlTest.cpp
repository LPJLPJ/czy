////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     DashBoardCtrlTest.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//   test number module
/************************************************************************/
/* Test Items
   1\ negative value\boundary value for Width\Height| OffsetX\OffsetY
   2\ Illegal pointer NULL
   3\ LowAlarmValueL LowAlarmValueH 
	  HighAlarmValueL HighAlarmValueH
*/
/************************************************************************/
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "moduleTest.h"
#include "NumberClass.h"
#include "testFunc.h"
#include <iostream>
#include <fstream>

extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;
extern u16             WorkingPageID;

#ifdef AHMI_CORE

void NumberCtrlTest(void){
	WidgetClass testNumber;
	WidgetClassPtr curWidget;
	NumberClass NumberWidget;
	int i;
	u16 widgetCount;
	u8 widgetType;
	std::ofstream NumberTestLogFile;
	ActionTriggerClass actionTest;
//	TagClassPtr pBindingTag;
	TextureClassPtr pBasicTextureListTmp;
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	NumberTestLogFile.open("testLog\\NumberTestLogFile.Log",std::ios::out);
	ERROR_PRINT("testing Number\n");

	ERROR_PRINT("test illegal data\n");
	NumberTestLogFile << "test illegal data\n";

	for(widgetCount = 0; widgetCount < gPagePtr[WorkingPageID].mNumOfWidget; widgetCount ++)
	{
		curWidget = &gPagePtr[WorkingPageID].pWidgetList[widgetCount];
		widgetType = curWidget->WidgetAttr &0x1f;
		if(widgetType == NUMBOX_TEX)
		{
			//copy the attribute
			for(i = 0; i < WIDGET_UNVAR; i++)
			{
				testNumber.WDG_u16_userVariables[i] = curWidget->WDG_u16_userVariables[i];
				testNumber.mWidgetMatrix.A = curWidget-> mWidgetMatrix.A;
				testNumber.mWidgetMatrix.B = curWidget-> mWidgetMatrix.B;
				testNumber.mWidgetMatrix.C = curWidget-> mWidgetMatrix.C;
				testNumber.mWidgetMatrix.D = curWidget-> mWidgetMatrix.D;
				testNumber.mWidgetMatrix.E = curWidget-> mWidgetMatrix.E;
				testNumber.mWidgetMatrix.F = curWidget-> mWidgetMatrix.F;
			}
			break;
		}
	}
	if(widgetCount == gPagePtr[WorkingPageID].mNumOfWidget)
	{
		ERROR_PRINT("connot find button widget");
		return;
	}

	ERROR_PRINT("set offset,width\\height to negative value or boundary");
	NumberTestLogFile << "set width\\height to negative value or boundary\n";
	//test for the widget Width\Height and OffsetX\OffsetY negative value
	testNumber.WidgetWidth = (u16)(-10);
	testNumber.WidgetHeight = 5678;
	testNumber.WidgetOffsetX = -5000;
	testNumber.WidgetOffsetY = 10000;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testNumber.BindTagID];
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);
	//test for the widget Width\Height and OffsetX\OffsetY on the boundary
	testNumber.WidgetWidth = 0;
	testNumber.WidgetHeight = 0;
	testNumber.WidgetOffsetX = 0;
	testNumber.WidgetOffsetY = 0;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);
	NumberTestLogFile << "check whether the trace window print error information. only when error information is printed , this test passed\n";

	//illegal input: 1\ NULL ptr for widgetclassptr NULL ptr for actiontriggerclass ptr
	//				 2\ illegal angle value
	ERROR_PRINT("set illegal input\n");
	NumberTestLogFile << "set illegal input\n";
	NumberTestLogFile << "   1\\ set illegal Ptr widgetPtr\\actionPtr\\pBasicTextureList input\n";
	NumberWidget.widgetCtrl( NULL, NULL, 0);
	testNumber.WidgetWidth = 10;
	testNumber.WidgetHeight = 10;
	testNumber.WidgetOffsetX = 0;
	testNumber.WidgetOffsetY = 0;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = NULL;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);

	testNumber.WidgetWidth = 10;
	testNumber.WidgetHeight = 10;
	testNumber.WidgetOffsetX = 0;
	testNumber.WidgetOffsetY = 0;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr =  &TagPtr[testNumber.BindTagID];
	pBasicTextureListTmp = gPagePtr->pBasicTextureList;
	gPagePtr->pBasicTextureList = NULL;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);
	ERROR_PRINT("illegal input test passed\n");
	NumberTestLogFile << "illegal input test passed\n";
	//after destroy the gPagePtr, use the original data
	gPagePtr->pBasicTextureList = pBasicTextureListTmp;

	NumberTestLogFile << "   2\\ set illegal or boundary MinValue\\MaxValue\\lowalarmvalue\\highalarmvalue input\n";
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testNumber.BindTagID];
	//boundary of LowAlarmValue
	testNumber.LowAlarmValueL  = 0;
	testNumber.LowAlarmValueH  = 0;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);
	testNumber.LowAlarmValueL  = (u16)65536;
	testNumber.LowAlarmValueH  = (u16)65536;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);

	//boundary of HighAlarmValue
	testNumber.LowAlarmValueL  = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0];
	testNumber.LowAlarmValueH  = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1];
	testNumber.HighAlarmValueL = 0;
	testNumber.HighAlarmValueH = 0;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);
	testNumber.HighAlarmValueL = (u16)65536;
	testNumber.HighAlarmValueH = (u16)65536;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);

	//boundary of MinValue
	testNumber.HighAlarmValueL = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 2];
	testNumber.HighAlarmValueH = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 3];
	testNumber.MinValueL       = 0;
	testNumber.MinValueH       = 0;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);
	testNumber.MinValueL       = (u16)65536;
	testNumber.MinValueH       = (u16)65536;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);

	//boundary of MaxVlaue
	testNumber.MinValueL       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 4];
	testNumber.MinValueH       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 5];
	testNumber.MaxValueL       = 0;
	testNumber.MaxValueH       = 0;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);
	testNumber.MaxValueL       = (u16)65536;
	testNumber.MaxValueH       = (u16)65536;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);
	testNumber.MaxValueL       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 6];
	testNumber.MaxValueH       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 7];
	
	//test the boundary value for numbers of number and decimals
	ERROR_PRINT("test boundary data for number of decimals and number\n");
	NumberTestLogFile << "test boundary data for number of decimals and number\n";	
	testNumber.WidgetAttr = testNumber.WidgetAttr | 0x1e00;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);
	testNumber.WidgetAttr = curWidget->WidgetAttr;
	testNumber.WidgetAttr = testNumber.WidgetAttr | 0x1e0;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);
	testNumber.WidgetAttr = curWidget->WidgetAttr;

	//boundary value for OffsetX\OffsetY
	ERROR_PRINT("test boundary data for OffsetX\\OffsetY\n");
	NumberTestLogFile << "test boundary data for OffsetX\\OffsetY\n";	
	//Offset ranges from -1920-1920
	//Width Height ranges from 1-1920
	int offsetX = -1920;
	int offsetY = -1920;
	int width = 1;
	int height = 1;
	for(offsetX = -1920; offsetX <= 1920; offsetX ++ )			
	{
		testNumber.WidgetWidth = width;
		testNumber.WidgetHeight = height;
		testNumber.WidgetOffsetX = offsetX;
		testNumber.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testNumber.BindTagID];
		if(NumberWidget.widgetCtrl( &testNumber, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}

	offsetX = -1920;
	for(offsetY = -1920; offsetY <= 1920; offsetY ++)
	{
		testNumber.WidgetWidth = width;
		testNumber.WidgetHeight = height;
		testNumber.WidgetOffsetX = offsetX;
		testNumber.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testNumber.BindTagID];
		if(NumberWidget.widgetCtrl( &testNumber, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	offsetY = -1920;
	for(width = 1; width <= 1920; width ++)
	{
		testNumber.WidgetWidth = width;
		testNumber.WidgetHeight = height;
		testNumber.WidgetOffsetX = offsetX;
		testNumber.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testNumber.BindTagID];
		if(NumberWidget.widgetCtrl( &testNumber, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	width = 1;
	for(height = 1; height <= 1920; height ++)
	{
		testNumber.WidgetWidth = width;
		testNumber.WidgetHeight = height;
		testNumber.WidgetOffsetX = offsetX;
		testNumber.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testNumber.BindTagID];
		if(NumberWidget.widgetCtrl( &testNumber, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	if(ahmi_func_status == AHMI_FUNC_SUCCESS)
	{
		ERROR_PRINT("boundary data test passed\n");
		NumberTestLogFile << "boundary data test passed\n";
	}
	else 
	{
		ERROR_PRINT("boundary data test failed\n");
		NumberTestLogFile << "boundary data test failed\n";
	}
	// revover the attribute to original
	testNumber.WidgetWidth = curWidget->WidgetWidth;
	testNumber.WidgetHeight = curWidget->WidgetHeight;
	testNumber.WidgetOffsetX = curWidget->WidgetOffsetX;
	testNumber.WidgetOffsetY = curWidget->WidgetOffsetY;
	NumberWidget.widgetCtrl( &testNumber, &actionTest, 0);
}

#endif