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
#include "ProgBarClass.h"
#include "testFunc.h"
#include <iostream>
#include <fstream>

extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;
extern u16             WorkingPageID;

#ifdef AHMI_CORE

void ProbarCtrlTest(void){
	WidgetClass testProgBar;
	WidgetClassPtr curWidget;
	ProgBarClass ProgBarWidget;
	int i;
	u16 widgetCount;
	u8 widgetType;
	std::ofstream ProgBarTestLogFile;
	ActionTriggerClass actionTest;
//	TagClassPtr pBindingTag;
	TextureClassPtr pBasicTextureListTmp;
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	ProgBarTestLogFile.open("testLog\\ProgBarTestLogFile.Log",std::ios::out);
	ERROR_PRINT("testing progressbar\n");

	ERROR_PRINT("test illegal data\n");
	ProgBarTestLogFile << "test illegal data\n";

	for(widgetCount = 0; widgetCount < gPagePtr[WorkingPageID].mNumOfWidget; widgetCount ++)
	{
		curWidget = &gPagePtr[WorkingPageID].pWidgetList[widgetCount];
		widgetType = curWidget->WidgetAttr &0x1f;
		if(widgetType == PROGBAR_TEX)
		{
			//copy the attribute
			for(i = 0; i < WIDGET_UNVAR; i++)
			{
				testProgBar.WDG_u16_userVariables[i] = curWidget->WDG_u16_userVariables[i];
				testProgBar.mWidgetMatrix.A = curWidget-> mWidgetMatrix.A;
				testProgBar.mWidgetMatrix.B = curWidget-> mWidgetMatrix.B;
				testProgBar.mWidgetMatrix.C = curWidget-> mWidgetMatrix.C;
				testProgBar.mWidgetMatrix.D = curWidget-> mWidgetMatrix.D;
				testProgBar.mWidgetMatrix.E = curWidget-> mWidgetMatrix.E;
				testProgBar.mWidgetMatrix.F = curWidget-> mWidgetMatrix.F;
			}
			break;
		}
	}
	if(widgetCount == gPagePtr[WorkingPageID].mNumOfWidget)
	{
		ERROR_PRINT("connot find progressbar widget");
		return;
	}

	ERROR_PRINT("set offset,width\\height to negative value or boundary");
	ProgBarTestLogFile << "set width\\height to negative value or boundary\n";
	//test for the widget Width\Height and OffsetX\OffsetY negative value
	testProgBar.WidgetWidth = (u16)(-10);
	testProgBar.WidgetHeight = 5678;
	testProgBar.WidgetOffsetX = -5000;
	testProgBar.WidgetOffsetY = 10000;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testProgBar.BindTagID];
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);
	//test for the widget Width\Height and OffsetX\OffsetY on the boundary
	testProgBar.WidgetWidth = 0;
	testProgBar.WidgetHeight = 0;
	testProgBar.WidgetOffsetX = 0;
	testProgBar.WidgetOffsetY = 0;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);
	ProgBarTestLogFile << "check whether the trace window print error information. only when error information is printed , this test passed\n";

	//illegal input: 1\ NULL ptr for widgetclassptr NULL ptr for actiontriggerclass ptr
	//				 2\ illegal angle value
	ERROR_PRINT("set illegal input\n");
	ProgBarTestLogFile << "set illegal input\n";
	ProgBarTestLogFile << "   1\\ set illegal Ptr widgetPtr\\actionPtr\\pBasicTextureList input\n";
	ProgBarWidget.widgetCtrl( NULL, NULL, 0);
	testProgBar.WidgetWidth = 10;
	testProgBar.WidgetHeight = 10;
	testProgBar.WidgetOffsetX = 0;
	testProgBar.WidgetOffsetY = 0;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = NULL;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);

	testProgBar.WidgetWidth = 10;
	testProgBar.WidgetHeight = 10;
	testProgBar.WidgetOffsetX = 0;
	testProgBar.WidgetOffsetY = 0;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr =  &TagPtr[testProgBar.BindTagID];
	pBasicTextureListTmp = gPagePtr->pBasicTextureList;
	gPagePtr->pBasicTextureList = NULL;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);
	ERROR_PRINT("illegal input test passed\n");
	ProgBarTestLogFile << "illegal input test passed\n";
	//after destroy the gPagePtr, use the original data
	gPagePtr->pBasicTextureList = pBasicTextureListTmp;

	ProgBarTestLogFile << "   2\\ set illegal or boundary MinValue\\MaxValue\\lowalarmvalue\\highalarmvalue input\n";
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testProgBar.BindTagID];
	//boundary of LowAlarmValue
	testProgBar.LowAlarmValueL  = 0;
	testProgBar.LowAlarmValueH  = 0;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);
	testProgBar.LowAlarmValueL  = (u16)65536;
	testProgBar.LowAlarmValueH  = (u16)65536;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);

	//boundary of HighAlarmValue
	testProgBar.LowAlarmValueL  = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0];
	testProgBar.LowAlarmValueH  = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1];
	testProgBar.HighAlarmValueL = 0;
	testProgBar.HighAlarmValueH = 0;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);
	testProgBar.HighAlarmValueL = (u16)65536;
	testProgBar.HighAlarmValueH = (u16)65536;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);

	//boundary of MinValue
	testProgBar.HighAlarmValueL = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 2];
	testProgBar.HighAlarmValueH = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 3];
	testProgBar.MinValueL       = 0;
	testProgBar.MinValueH       = 0;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);
	testProgBar.MinValueL       = (u16)65536;
	testProgBar.MinValueH       = (u16)65536;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);

	//boundary of MaxVlaue
	testProgBar.MinValueL       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 4];
	testProgBar.MinValueH       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 5];
	testProgBar.MaxValueL       = 0;
	testProgBar.MaxValueH       = 0;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);
	testProgBar.MaxValueL       = (u16)65536;
	testProgBar.MaxValueH       = (u16)65536;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);
	testProgBar.MaxValueL       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 6];
	testProgBar.MaxValueH       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 7];

	//boundary value for OffsetX\OffsetY
	ERROR_PRINT("test boundary data for OffsetX\\OffsetY\n");
	ProgBarTestLogFile << "test boundary data for OffsetX\\OffsetY\n";	
	//Offset ranges from -1920-1920
	//Width Height ranges from 1-1920
	int offsetX = -1920;
	int offsetY = -1920;
	int width = 1;
	int height = 1;
	for(offsetX = -1920; offsetX <= 1920; offsetX ++ )			
	{
		testProgBar.WidgetWidth = width;
		testProgBar.WidgetHeight = height;
		testProgBar.WidgetOffsetX = offsetX;
		testProgBar.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testProgBar.BindTagID];
		if(ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}

	offsetX = -1920;
	for(offsetY = -1920; offsetY <= 1920; offsetY ++)
	{
		testProgBar.WidgetWidth = width;
		testProgBar.WidgetHeight = height;
		testProgBar.WidgetOffsetX = offsetX;
		testProgBar.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testProgBar.BindTagID];
		if(ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	offsetY = -1920;
	for(width = 1; width <= 1920; width ++)
	{
		testProgBar.WidgetWidth = width;
		testProgBar.WidgetHeight = height;
		testProgBar.WidgetOffsetX = offsetX;
		testProgBar.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testProgBar.BindTagID];
		if(ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	width = 1;
	for(height = 1; height <= 1920; height ++)
	{
		testProgBar.WidgetWidth = width;
		testProgBar.WidgetHeight = height;
		testProgBar.WidgetOffsetX = offsetX;
		testProgBar.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testProgBar.BindTagID];
		if(ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	if(ahmi_func_status == AHMI_FUNC_SUCCESS)
	{
		ERROR_PRINT("boundary data test passed\n");
		ProgBarTestLogFile << "boundary data test passed\n";
	}
	else 
	{
		ERROR_PRINT("boundary data test failed\n");
		ProgBarTestLogFile << "boundary data test failed\n";
	}
	// revover the attribute to original
	testProgBar.WidgetWidth = curWidget->WidgetWidth;
	testProgBar.WidgetHeight = curWidget->WidgetHeight;
	testProgBar.WidgetOffsetX = curWidget->WidgetOffsetX;
	testProgBar.WidgetOffsetY = curWidget->WidgetOffsetY;
	ProgBarWidget.widgetCtrl( &testProgBar, &actionTest, 0);
}

#endif