////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2017/06/15
// File Name:     actionProcessorTest.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2017/06/15 by Zhou Yuzhi(周钰致)
// Additional Comments:
//   test action processor
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "moduleTest.h"
#include "actionProcessor.h"
#include "testFunc.h"
#include <iostream>
#include <fstream>

extern DynamicPageClassPtr gPagePtr;
extern TagClassPtr     TagPtr;
extern u16             WorkingPageID;
extern ActionProcessorClass gAction;
extern QueueHandle_t		ActionInstructionQueue;

#ifdef AHMI_CORE

//-----------------------------
// 函数名： moduleTestTask
// 调用模块测试函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void actionProcessorTest(void)
{
	std::ofstream buttonTestLogFile;
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	u32 actionAddr;
	buttonTestLogFile.open("testLog\\actionProcessorTest.Log",std::ios::out);
	ERROR_PRINT("testing actionProcessor\n");

	ERROR_PRINT("test illegal data\n");
	buttonTestLogFile << "test illegal data\n";
	//test illegal data
	//1. illegal instruction addr
	//2. no ending of the instruction
	//3. illegal input for all the action functions
	ERROR_PRINT("1.illegal instruction addr\n");
	buttonTestLogFile << "1.illegal instruction addr\n";
	actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + 1000);
	xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
	buttonTestLogFile << "check whether the trace window print error information. only when error information is printed is this test passed\n";

	ERROR_PRINT("2. no ending of the instruction\n");
	buttonTestLogFile << "2. no ending of the instruction\n";
	gPagePtr[0].mActionInstructionsSize = 100;
	gPagePtr[0].pActionInstructions = (u8 *)pvPortMalloc(sizeof(u8) * gPagePtr[0].mActionInstructionsSize);
	actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions);
	//set the instructions without ending
	for(int i = 0; i < 100; i++)
	{
		gPagePtr[0].pActionInstructions[i] = 50;
	}
	xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
	buttonTestLogFile << "check whether the trace window print error information. only when error information is printed is this test passed\n";

	ERROR_PRINT("3. illegal input for all the action functions\n");
	buttonTestLogFile << "3. illegal input for all the action functions\n";
	gAction.GotoPage(0,0,100,0,0);
	gAction.SetTag(0,0,100,100);
	gAction.IncTag(0,0,100,100);
	gAction.MulTag (0,0,100,100);
	gAction.DivTag (0,0,100,100);
	gAction.ModTag (0,0,100,100);
	gAction.AndTag (0,0,100,100);
	gAction.OrTag  (0,0,100,100);
	gAction.XorTag (0,0,100,100);
	gAction.NotTag (0,0,100);
	gAction.SLTag  (0,0,100,100);
	gAction.SRTag  (0,0,100,100);
	gAction.SetTagImm(0,0,100,100);
	gAction.IncTagImm(0,0,100,100);
	gAction.DecTagImm(0,0,100,100);
	gAction.MulTagImm(0,0,100,100);
	gAction.DivTagImm(0,0,100,100);
	gAction.ModTagImm(0,0,100,100);
	gAction.AndTagImm(0,0,100,100);
	gAction.OrTagImm (0,0,100,100);
	gAction.XorTagImm(0,0,100,100);
	gAction.SLTagImm (0,0,100,100);
	gAction.SRTagImm (0,0,100,100);

	//boundary
	ERROR_PRINT("test boundary data\n");
	buttonTestLogFile << "test boundary data\n";
	//set 65535 instructions
	gPagePtr[0].mActionInstructionsSize = 30000;
	gPagePtr[0].pActionInstructions = (u8 *)pvPortMalloc(sizeof(u8) * gPagePtr[0].mActionInstructionsSize);
	actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions);
	//set the instructions without ending
	for(int i = 0; i < gPagePtr[0].mActionInstructionsSize; i++)
	{
		gPagePtr[0].pActionInstructions[i] = 50;
	}
	xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
	
	if(ahmi_func_status == AHMI_FUNC_SUCCESS)
	{
		ERROR_PRINT("boundary data test passed\n");
		buttonTestLogFile << "boundary data test passed\n";
	}
	else 
	{
		ERROR_PRINT("boundary data test failed\n");
		buttonTestLogFile << "boundary data test failed\n";
	}


}


#endif
