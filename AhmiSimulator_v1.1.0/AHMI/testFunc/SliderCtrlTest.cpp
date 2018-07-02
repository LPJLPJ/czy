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
#include "SliderClass.h"
#include "testFunc.h"
#include <iostream>
#include <fstream>

extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;
extern u16             WorkingPageID;

#ifdef AHMI_CORE

void SliderCtrlTest(void){
	WidgetClass testSlider;
	WidgetClassPtr curWidget;
	SliderClass SliderWidget;
	int i;
	u16 widgetCount;
	u8 widgetType;
	std::ofstream SliderTestLogFile;
	ActionTriggerClass actionTest;
//	TagClassPtr pBindingTag;
	TextureClassPtr pBasicTextureListTmp;
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	SliderTestLogFile.open("testLog\\SliderTestLogFile.Log",std::ios::out);
	ERROR_PRINT("testing slider widget\n");

	ERROR_PRINT("test illegal data\n");
	SliderTestLogFile << "test illegal data\n";

	for(widgetCount = 0; widgetCount < gPagePtr[WorkingPageID].mNumOfWidget; widgetCount ++)
	{
		curWidget = &gPagePtr[WorkingPageID].pWidgetList[widgetCount];
		widgetType = curWidget->WidgetAttr &0x1f;
		if(widgetType == SLIDER_TEX)
		{
			//copy the attribute
			for(i = 0; i < WIDGET_UNVAR; i++)
			{
				testSlider.WDG_u16_userVariables[i] = curWidget->WDG_u16_userVariables[i];
				testSlider.mWidgetMatrix.A = curWidget-> mWidgetMatrix.A;
				testSlider.mWidgetMatrix.B = curWidget-> mWidgetMatrix.B;
				testSlider.mWidgetMatrix.C = curWidget-> mWidgetMatrix.C;
				testSlider.mWidgetMatrix.D = curWidget-> mWidgetMatrix.D;
				testSlider.mWidgetMatrix.E = curWidget-> mWidgetMatrix.E;
				testSlider.mWidgetMatrix.F = curWidget-> mWidgetMatrix.F;
			}
			break;
		}
	}
	if(widgetCount == gPagePtr[WorkingPageID].mNumOfWidget)
	{
		ERROR_PRINT("cannot find slider widget");
		return;
	}

	ERROR_PRINT("set offset,width\\height to negative value or boundary");
	SliderTestLogFile << "set width\\height to negative value or boundary\n";
	//test for the widget Width\Height and OffsetX\OffsetY negative value
	testSlider.WidgetWidth = (u16)(-10);
	testSlider.WidgetHeight = 5678;
	testSlider.WidgetOffsetX = -5000;
	testSlider.WidgetOffsetY = 10000;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testSlider.BindTagID];
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);
	//test for the widget Width\Height and OffsetX\OffsetY on the boundary
	testSlider.WidgetWidth = 0;
	testSlider.WidgetHeight = 0;
	testSlider.WidgetOffsetX = 0;
	testSlider.WidgetOffsetY = 0;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);
	SliderTestLogFile << "check whether the trace window print error information. only when error information is printed , this test passed\n";

	//illegal input: 1\ NULL ptr for widgetclassptr NULL ptr for actiontriggerclass ptr
	//				 2\ illegal angle value
	ERROR_PRINT("set illegal input\n");
	SliderTestLogFile << "set illegal input\n";
	SliderTestLogFile << "   1\\ set illegal Ptr widgetPtr\\actionPtr\\pBasicTextureList input\n";
	SliderWidget.widgetCtrl( NULL, NULL, 0);
	testSlider.WidgetWidth = 10;
	testSlider.WidgetHeight = 10;
	testSlider.WidgetOffsetX = 0;
	testSlider.WidgetOffsetY = 0;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = NULL;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);

	testSlider.WidgetWidth = 10;
	testSlider.WidgetHeight = 10;
	testSlider.WidgetOffsetX = 0;
	testSlider.WidgetOffsetY = 0;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr =  &TagPtr[testSlider.BindTagID];
	pBasicTextureListTmp = gPagePtr->pBasicTextureList;
	gPagePtr->pBasicTextureList = NULL;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);
	ERROR_PRINT("illegal input test passed\n");
	SliderTestLogFile << "illegal input test passed\n";
	//after destroy the gPagePtr, use the original data
	gPagePtr->pBasicTextureList = pBasicTextureListTmp;

	SliderTestLogFile << "   2\\ set illegal or boundary MinValue\\MaxValue\\lowalarmvalue\\highalarmvalue input\n";
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testSlider.BindTagID];
	//boundary of LowAlarmValue
	testSlider.LowAlarmValueL  = 0;
	testSlider.LowAlarmValueH  = 0;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);
	testSlider.LowAlarmValueL  = (u16)65536;
	testSlider.LowAlarmValueH  = (u16)65536;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);

	//boundary of HighAlarmValue
	testSlider.LowAlarmValueL  = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0];
	testSlider.LowAlarmValueH  = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1];
	testSlider.HighAlarmValueL = 0;
	testSlider.HighAlarmValueH = 0;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);
	testSlider.HighAlarmValueL = (u16)65536;
	testSlider.HighAlarmValueH = (u16)65536;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);

	//boundary of MinValue
	testSlider.HighAlarmValueL = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 2];
	testSlider.HighAlarmValueH = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 3];
	testSlider.MinValueL       = 0;
	testSlider.MinValueH       = 0;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);
	testSlider.MinValueL       = (u16)65536;
	testSlider.MinValueH       = (u16)65536;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);

	//boundary of MaxVlaue
	testSlider.MinValueL       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 4];
	testSlider.MinValueH       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 5];
	testSlider.MaxValueL       = 0;
	testSlider.MaxValueH       = 0;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);
	testSlider.MaxValueL       = (u16)65536;
	testSlider.MaxValueH       = (u16)65536;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);
	testSlider.MaxValueL       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 6];
	testSlider.MaxValueH       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 7];

	//boundary value for OffsetX\OffsetY
	ERROR_PRINT("test boundary data for OffsetX\\OffsetY\n");
	SliderTestLogFile << "test boundary data for OffsetX\\OffsetY\n";	
	//Offset ranges from -1920-1920
	//Width Height ranges from 1-1920
	int offsetX = -1920;
	int offsetY = -1920;
	int width = 1;
	int height = 1;
	for(offsetX = -1920; offsetX <= 1920; offsetX ++ )			
	{
		testSlider.WidgetWidth = width;
		testSlider.WidgetHeight = height;
		testSlider.WidgetOffsetX = offsetX;
		testSlider.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testSlider.BindTagID];
		if(SliderWidget.widgetCtrl( &testSlider, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}

	offsetX = -1920;
	for(offsetY = -1920; offsetY <= 1920; offsetY ++)
	{
		testSlider.WidgetWidth = width;
		testSlider.WidgetHeight = height;
		testSlider.WidgetOffsetX = offsetX;
		testSlider.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testSlider.BindTagID];
		if(SliderWidget.widgetCtrl( &testSlider, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	offsetY = -1920;
	for(width = 1; width <= 1920; width ++)
	{
		testSlider.WidgetWidth = width;
		testSlider.WidgetHeight = height;
		testSlider.WidgetOffsetX = offsetX;
		testSlider.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testSlider.BindTagID];
		if(SliderWidget.widgetCtrl( &testSlider, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	width = 1;
	for(height = 1; height <= 1920; height ++)
	{
		testSlider.WidgetWidth = width;
		testSlider.WidgetHeight = height;
		testSlider.WidgetOffsetX = offsetX;
		testSlider.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testSlider.BindTagID];
		if(SliderWidget.widgetCtrl( &testSlider, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	if(ahmi_func_status == AHMI_FUNC_SUCCESS)
	{
		ERROR_PRINT("boundary data test passed\n");
		SliderTestLogFile << "boundary data test passed\n";
	}
	else 
	{
		ERROR_PRINT("boundary data test failed\n");
		SliderTestLogFile << "boundary data test failed\n";
	}
	// revover the attribute to original
	testSlider.WidgetWidth = curWidget->WidgetWidth;
	testSlider.WidgetHeight = curWidget->WidgetHeight;
	testSlider.WidgetOffsetX = curWidget->WidgetOffsetX;
	testSlider.WidgetOffsetY = curWidget->WidgetOffsetY;
	SliderWidget.widgetCtrl( &testSlider, &actionTest, 0);
}

#endif