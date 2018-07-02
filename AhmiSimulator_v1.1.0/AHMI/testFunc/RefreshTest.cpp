////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2017/06/14
// File Name:     animationTest.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2017/06/14 by Zhou Yuzhi(周钰致)
// Additional Comments:
//   test animation function
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"
#include "moduleTest.h"
#include "ButtonClass.h"
#include "testFunc.h"
#include "RefreshTest.h"
#include <iostream>
#include <fstream>

extern AnimationClass  gAnimationClass;
extern u8 animationDuration;
extern u8 animationExist;
extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;
extern u16             WorkingPageID;
extern ConfigInfoClass  ConfigData;
extern u8*     TagChangedListPtr;
extern QueueHandle_t   RefreshQueue;
extern QueueHandle_t   RefreshQueueWithoutDoubleBuffer;
QueueHandle_t RefreshQueue_temp;

#ifdef AHMI_CORE

//-----------------------------
// 函数名： animationTest
// 测试animationClass的相关函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void RefreshTaskTest(void)
{
	std::ofstream animationTestLogFile;
	RefreshTestClass refreshTest;
	animationTestLogFile.open("testLog\\RefreshTest.Log",std::ios::out);
	if(!animationTestLogFile.is_open())
	{
		ERROR_PRINT("ERROR in tagClass test: connot open file RefreshTest.Log");
		return;
	}
	refreshTest.widgetRefreshTaskTest(&animationTestLogFile);
	refreshTest.drawImmediatelyTest(&animationTestLogFile);

	refreshTest.drawPageTest(&animationTestLogFile);
	refreshTest.drawCanvasTest(&animationTestLogFile);
	refreshTest.drawWidgetTest(&animationTestLogFile);
	refreshTest.drawTextureTest(&animationTestLogFile);
	refreshTest.sourceBoxRecomputeTest(&animationTestLogFile);
	refreshTest.writeSourceBufferTest(&animationTestLogFile);
	refreshTest.renewTextureSourceBoxTest(&animationTestLogFile);
	animationTestLogFile.close();

}

//-----------------------------
// 函数名： tagSetValueTest
// 测试tagClass::setValue函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void RefreshTestClass::widgetRefreshTaskTest(std::ofstream* pAnimationTestLogFile)
{
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	RefreshMsg refreshMsg;
	ERROR_PRINT("testing widgetRefreshTask function\n");
	*pAnimationTestLogFile << "testing widgetRefreshTask function\n";


	//illegal input
	ERROR_PRINT("test illegal input\n");
	*pAnimationTestLogFile << "test illegal input\n";
	refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
	refreshMsg.mElementPtr.pageptr = NULL;
	xQueueSendToBack(RefreshQueue,&refreshMsg,portMAX_DELAY);

	*pAnimationTestLogFile << "check whether the trace window print error information. only when error information is printed is this test passed\n";

	//boundary
	ERROR_PRINT("test boundary data\n");
	*pAnimationTestLogFile << "test boundary data\n";
	refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
	refreshMsg.mElementPtr.pageptr = gPagePtr;
	for(int i = 0; i < 1000; i++)
		xQueueSendToBack(RefreshQueue,&refreshMsg,0);
	ERROR_PRINT("boundary data test passed\n");
	*pAnimationTestLogFile << "test boundary data test passed\n";
}

//-----------------------------
// 函数名： drawImmediatelyTest
// 测试tagClass::setValue函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void RefreshTestClass::drawImmediatelyTest(std::ofstream* pAnimationTestLogFile)
{
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	RefreshMsg refreshMsg;
	ERROR_PRINT("testing drawImmediately function\n");
	*pAnimationTestLogFile << "testing drawImmediately function\n";
	//illegal input
	ERROR_PRINT("test illegal input\n");
	*pAnimationTestLogFile << "test illegal input\n";
	//page 
	//null 
	refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
	refreshMsg.mElementPtr.pageptr = NULL;
	drawimmediately(&refreshMsg);
	refreshMsg.mElementType = ANIMATION_REFRESH_DOUBLE_BUFFER;
	refreshMsg.mElementPtr.pageptr = NULL;
	drawimmediately(&refreshMsg);
	refreshMsg.mElementType = ANIMAITON_REFRESH_STATIC_BUFFER;
	refreshMsg.mElementPtr.pageptr = NULL;
	drawimmediately(&refreshMsg);
	//other pointer
	refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
	refreshMsg.mElementPtr.cptr = &gPagePtr[WorkingPageID].pCanvasList[0];
	drawimmediately(&refreshMsg);
	refreshMsg.mElementType = ANIMATION_REFRESH_DOUBLE_BUFFER;
	refreshMsg.mElementPtr.cptr = &gPagePtr[WorkingPageID].pCanvasList[0];
	drawimmediately(&refreshMsg);
	refreshMsg.mElementType = ANIMAITON_REFRESH_STATIC_BUFFER;
	refreshMsg.mElementPtr.cptr = &gPagePtr[WorkingPageID].pCanvasList[0];
	drawimmediately(&refreshMsg);

	*pAnimationTestLogFile << "check whether the trace window print error information. only when error information is printed is this test passed\n";

	//corruption
	ERROR_PRINT("test corruption data\n");
	*pAnimationTestLogFile << "test corruption data\n";
	drawimmediately(NULL);
	ERROR_PRINT("corruption data test passed\n");
	*pAnimationTestLogFile << "corruption data test passed\n";
}

//-----------------------------
// 函数名： drawPageTest
// 测试tagClass::setValue函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void RefreshTestClass::drawPageTest(std::ofstream* pAnimationTestLogFile)
{
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
//	RefreshMsg refreshMsg;
	TileBoxClass pageBox;
	CanvasClassPtr canvasListTemp;
	SubCanvasClassPtr subcanvasListTemp;
	u32 sourceShift;


	ERROR_PRINT("testing drawPage function\n");
	*pAnimationTestLogFile << "testing drawPage function\n";
	pageBox.LeftBox = 0;
	pageBox.RightBox = 0;
	pageBox.TopBox = 0;
	pageBox.ButtomBox = 0;

	//illegal input
	ERROR_PRINT("test illegal input\n");
	*pAnimationTestLogFile << "test illegal input\n";
	gPagePtr[WorkingPageID].drawPage(NULL,NULL,0,0,0);
	gPagePtr[WorkingPageID].drawPage(&pageBox,NULL,0,0,0);

	//data corruption
	ERROR_PRINT("test illegal input\n");
	*pAnimationTestLogFile << "test illegal input\n";
	taskENTER_CRITICAL();	
	canvasListTemp = gPagePtr[WorkingPageID].pCanvasList;
	gPagePtr[WorkingPageID].pCanvasList = NULL;
	gPagePtr[WorkingPageID].drawPage(&pageBox,&sourceShift,0,0,0);
	gPagePtr[WorkingPageID].pCanvasList = canvasListTemp;

	subcanvasListTemp = gPagePtr[WorkingPageID].pSubCanvasList;
	gPagePtr[WorkingPageID].pSubCanvasList = NULL;
	gPagePtr[WorkingPageID].drawPage(&pageBox,&sourceShift,0,0,0);
	gPagePtr[WorkingPageID].pSubCanvasList = subcanvasListTemp;
	taskEXIT_CRITICAL();
}

//-----------------------------
// 函数名： drawCanvasTest
// 测试tagClass::setValue函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void RefreshTestClass::drawCanvasTest(std::ofstream* pAnimationTestLogFile)
{
	ERROR_PRINT("testing drawCanvas function\n");
	*pAnimationTestLogFile << "testing drawCanvas function\n";

	//illegal input
	ERROR_PRINT("test illegal input\n");
	*pAnimationTestLogFile << "test illegal input\n";
	gPagePtr[WorkingPageID].pCanvasList->drawCanvas(NULL, NULL, 0, NULL, NULL, 0, 0);
}

void RefreshTestClass::drawWidgetTest(std::ofstream* pAnimationTestLogFile)
{
	ERROR_PRINT("testing drawWidget function\n");
	*pAnimationTestLogFile << "testing drawWidget function\n";

	//illegal input
	ERROR_PRINT("test illegal input\n");
	*pAnimationTestLogFile << "test illegal input\n";
	gPagePtr[WorkingPageID].pWidgetList->drawWidget(NULL, NULL, 0, NULL, 0, 0);
}

void RefreshTestClass::drawTextureTest(std::ofstream* pAnimationTestLogFile)
{
	WidgetClassInterface WidgetClassInterfaceTest;
	ERROR_PRINT("testing drawTexture function\n");
	*pAnimationTestLogFile << "testing drawTexture function\n";

	//illegal input
	ERROR_PRINT("test illegal input\n");
	*pAnimationTestLogFile << "test illegal input\n";
	WidgetClassInterfaceTest.drawTexture(NULL, NULL, 0, NULL, 0);
}

void RefreshTestClass::sourceBoxRecomputeTest(std::ofstream* pAnimationTestLogFile)
{
	TileBoxClass TileBoxTest;
	ElementPtr ptrTest;

	ERROR_PRINT("testing sourceBoxRecompute function\n");
	*pAnimationTestLogFile << "testing sourceBoxRecompute function\n";

	//illegal input
	ERROR_PRINT("test illegal input\n");
	*pAnimationTestLogFile << "test illegal input invalid pointer for parameter\n";
	ptrTest.cptr = NULL;
	ptrTest.scptr = NULL;
	ptrTest.pageptr = NULL;
	ptrTest.tptr = NULL;
	ptrTest.wptr = NULL;
	TileBoxTest.sourceReCompute(ptrTest, NULL, NULL);

}

void RefreshTestClass::writeSourceBufferTest(std::ofstream* pAnimationTestLogFile)
{
	TextureClassPtr  DrawingTexTest = &gPagePtr[WorkingPageID].pBasicTextureList[0];
	ERROR_PRINT("testing writeSourceBufferTest function\n");
	*pAnimationTestLogFile << "testing writeSourceBufferTest function\n";

	ERROR_PRINT("test illegal input parameter\n");
	*pAnimationTestLogFile << "test illegal input invalid pointer for parameter\n";

	DrawingTexTest->writeSourceBuffer(NULL, NULL, NULL, NULL);
}

//is there need to test
void RefreshTestClass::renewTextureSourceBoxTest(std::ofstream* pAnimationTestLogFile)
{
	TextureClassPtr  DrawingTexTest = &gPagePtr[WorkingPageID].pBasicTextureList[0];
	ERROR_PRINT("testing renewTextureSourceBoxTest function\n");
	*pAnimationTestLogFile << "testing renewTextureSourceBoxTest function\n";

	ERROR_PRINT("test illegal input parameter\n");
	DrawingTexTest->renewTextureSourceBox(NULL, NULL, NULL);
}

#endif
