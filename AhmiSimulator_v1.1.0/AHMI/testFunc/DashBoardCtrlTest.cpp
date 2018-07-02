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
//   test dashboard module
/************************************************************************/
/* Test Items
   1\ negative value\boundary value for Width\Height| OffsetX\OffsetY
   2\ Illegal pointer NULL
   3\ StartAngle MeterMinAngle MeterMaxAngle
	  LowAlarmValueL LowAlarmValueH 
	  HighAlarmValueL HighAlarmValueH
*/
/************************************************************************/
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "moduleTest.h"
#include "MeterClass.h"
#include "testFunc.h"
#include <iostream>
#include <fstream>

extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;
extern u16             WorkingPageID;

#ifdef AHMI_CORE

void DashBoardCtrlTest(void){
	WidgetClass testDashBoard;
	WidgetClassPtr curWidget;
	MeterClass DashBoardWidget;
	int i;
	u16 widgetCount;
	u8 widgetType;
	std::ofstream DashBoardTestLogFile;
	ActionTriggerClass actionTest;
//	TagClassPtr pBindingTag;
	TextureClassPtr pBasicTextureListTmp;
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	DashBoardTestLogFile.open("testLog\\DashBoardTestLogFile.Log",std::ios::out);
	ERROR_PRINT("testing DashBoard\n");

	ERROR_PRINT("test illegal data\n");
	DashBoardTestLogFile << "test illegal data\n";

	for(widgetCount = 0; widgetCount < gPagePtr[WorkingPageID].mNumOfWidget; widgetCount ++)
	{
		curWidget = &gPagePtr[WorkingPageID].pWidgetList[widgetCount];
		widgetType = curWidget->WidgetAttr &0x1f;
		if(widgetType == METER_TEX)
		{
			//copy the attribute
			for(i = 0; i < WIDGET_UNVAR; i++)
			{
				testDashBoard.WDG_u16_userVariables[i] = curWidget->WDG_u16_userVariables[i];
				testDashBoard.mWidgetMatrix.A = curWidget-> mWidgetMatrix.A;
				testDashBoard.mWidgetMatrix.B = curWidget-> mWidgetMatrix.B;
				testDashBoard.mWidgetMatrix.C = curWidget-> mWidgetMatrix.C;
				testDashBoard.mWidgetMatrix.D = curWidget-> mWidgetMatrix.D;
				testDashBoard.mWidgetMatrix.E = curWidget-> mWidgetMatrix.E;
				testDashBoard.mWidgetMatrix.F = curWidget-> mWidgetMatrix.F;
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
	DashBoardTestLogFile << "set width\\height to negative value or boundary\n";
	//test for the widget Width\Height and OffsetX\OffsetY negative value
	testDashBoard.WidgetWidth = (u16)(-10);
	testDashBoard.WidgetHeight = 5678;
	testDashBoard.WidgetOffsetX = -5000;
	testDashBoard.WidgetOffsetY = 10000;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testDashBoard.BindTagID];
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	//test for the widget Width\Height and OffsetX\OffsetY on the boundary
	testDashBoard.WidgetWidth = 0;
	testDashBoard.WidgetHeight = 0;
	testDashBoard.WidgetOffsetX = 0;
	testDashBoard.WidgetOffsetY = 0;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	DashBoardTestLogFile << "check whether the trace window print error information. only when error information is printed , this test passed\n";

	//illegal input: 1\ NULL ptr for widgetclassptr NULL ptr for actiontriggerclass ptr
	//				 2\ illegal angle value
	ERROR_PRINT("set illegal input\n");
	DashBoardTestLogFile << "set illegal input\n";
	DashBoardTestLogFile << "   1\\ set illegal Ptr widgetPtr\\actionPtr\\pBasicTextureList input\n";
	DashBoardWidget.widgetCtrl( NULL, NULL, 0);
	testDashBoard.WidgetWidth = 10;
	testDashBoard.WidgetHeight = 10;
	testDashBoard.WidgetOffsetX = 0;
	testDashBoard.WidgetOffsetY = 0;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = NULL;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);

	testDashBoard.WidgetWidth = 10;
	testDashBoard.WidgetHeight = 10;
	testDashBoard.WidgetOffsetX = 0;
	testDashBoard.WidgetOffsetY = 0;
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr =  &TagPtr[testDashBoard.BindTagID];
	pBasicTextureListTmp = gPagePtr->pBasicTextureList;
	gPagePtr->pBasicTextureList = NULL;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	ERROR_PRINT("illegal input test passed\n");
	DashBoardTestLogFile << "illegal input test passed\n";
	//after destroy the gPagePtr, use the original data
	gPagePtr->pBasicTextureList = pBasicTextureListTmp;

	//test for illegal angle/rotate_angle/
	DashBoardTestLogFile << "   2\\ set illegal angle input\n";
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testDashBoard.BindTagID];
	//startangle out of limit
	testDashBoard.StartAngle = (u16)-390;					// -360~~360
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	testDashBoard.StartAngle = 390;					// -360~~360
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);	
	//minangle out of limit
	testDashBoard.StartAngle    = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10];
	testDashBoard.MeterMinAngle = (u16)-390;						//-360~~360
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	testDashBoard.MeterMinAngle = 390;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	//maxangle out of limit
	testDashBoard.MeterMinAngle = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8];
	testDashBoard.MeterMaxAngle = (u16)-390;				//-360~~360 & > MinAngle
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	testDashBoard.MeterMaxAngle = 390;						//-360~~360 & > MinAngle
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	//MinAngle > MaxAngle
	testDashBoard.MeterMinAngle = 120;
	testDashBoard.MeterMaxAngle = 90;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	//startangle out of min-max limit
	testDashBoard.MeterMinAngle = 50;
	testDashBoard.MeterMaxAngle = 120;
	testDashBoard.StartAngle    = (u16)-390;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	testDashBoard.MeterMinAngle = 50;
	testDashBoard.MeterMaxAngle = 120;
	testDashBoard.StartAngle    = 390;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	ERROR_PRINT("   illegal angle input test passed\n");
	DashBoardTestLogFile << "illegal angle input test passed\n";

	DashBoardTestLogFile << "   2\\ set illegal or boundary MinValue\\MaxValue\\lowalarmvalue\\highalarmvalue input\n";
	actionTest.mInputType = ACTION_TAG_SET_VALUE;
	actionTest.mTagPtr = &TagPtr[testDashBoard.BindTagID];
	testDashBoard.MeterMinAngle = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 8];
	testDashBoard.MeterMaxAngle = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 9];
	testDashBoard.StartAngle    = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 10];
	//boundary of LowAlarmValue
	testDashBoard.LowAlarmValueL  = 0;
	testDashBoard.LowAlarmValueH  = 0;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	testDashBoard.LowAlarmValueL  = (u16)65536;
	testDashBoard.LowAlarmValueH  = (u16)65536;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);

	//boundary of HighAlarmValue
	testDashBoard.LowAlarmValueL  = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 0];
	testDashBoard.LowAlarmValueH  = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 1];
	testDashBoard.HighAlarmValueL = 0;
	testDashBoard.HighAlarmValueH = 0;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	testDashBoard.HighAlarmValueL = (u16)65536;
	testDashBoard.HighAlarmValueH = (u16)65536;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);

	//boundary of MinValue
	testDashBoard.HighAlarmValueL = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 2];
	testDashBoard.HighAlarmValueH = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 3];
	testDashBoard.MinValueL       = 0;
	testDashBoard.MinValueH       = 0;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	testDashBoard.MinValueL       = (u16)65536;
	testDashBoard.MinValueH       = (u16)65536;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);

	//boundary of MaxVlaue
	testDashBoard.MinValueL       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 4];
	testDashBoard.MinValueH       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 5];
	testDashBoard.MaxValueL       = 0;
	testDashBoard.MaxValueH       = 0;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	testDashBoard.MaxValueL       = (u16)65536;
	testDashBoard.MaxValueH       = (u16)65536;
	DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0);
	testDashBoard.MaxValueL       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 6];
	testDashBoard.MaxValueH       = curWidget->WDG_u16_userVariables[WIDGET_GENERAL_NUM + 7];
	//boundary value for OffsetX\OffsetY
	ERROR_PRINT("test boundary data for OffsetX\\OffsetY\n");
	DashBoardTestLogFile << "test boundary data for OffsetX\\OffsetY\n";	
	//Offset ranges from -1920-1920
	//Width Height ranges from 1-1920
	int offsetX = -1920;
	int offsetY = -1920;
	int width = 1;
	int height = 1;
	for(offsetX = -1920; offsetX <= 1920; offsetX ++ )			
	{
		testDashBoard.WidgetWidth = width;
		testDashBoard.WidgetHeight = height;
		testDashBoard.WidgetOffsetX = offsetX;
		testDashBoard.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testDashBoard.BindTagID];
		if(DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}

	offsetX = -1920;
	for(offsetY = -1920; offsetY <= 1920; offsetY ++)
	{
		testDashBoard.WidgetWidth = width;
		testDashBoard.WidgetHeight = height;
		testDashBoard.WidgetOffsetX = offsetX;
		testDashBoard.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testDashBoard.BindTagID];
		if(DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	offsetY = -1920;
	for(width = 1; width <= 1920; width ++)
	{
		testDashBoard.WidgetWidth = width;
		testDashBoard.WidgetHeight = height;
		testDashBoard.WidgetOffsetX = offsetX;
		testDashBoard.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testDashBoard.BindTagID];
		if(DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	width = 1;
	for(height = 1; height <= 1920; height ++)
	{
		testDashBoard.WidgetWidth = width;
		testDashBoard.WidgetHeight = height;
		testDashBoard.WidgetOffsetX = offsetX;
		testDashBoard.WidgetOffsetY = offsetY;
		actionTest.mInputType = ACTION_TAG_SET_VALUE;
		actionTest.mTagPtr = &TagPtr[testDashBoard.BindTagID];
		if(DashBoardWidget.widgetCtrl( &testDashBoard, &actionTest, 0) == AHMI_FUNC_FAILURE)
			ahmi_func_status = AHMI_FUNC_FAILURE;
	}
	height = 1;
	if(ahmi_func_status == AHMI_FUNC_SUCCESS)
	{
		ERROR_PRINT("boundary data test passed\n");
		DashBoardTestLogFile << "boundary data test passed\n";
	}
	else 
	{
		ERROR_PRINT("boundary data test failed\n");
		DashBoardTestLogFile << "boundary data test failed\n";
	}

}

#endif