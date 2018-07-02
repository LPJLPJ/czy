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
//    handle the animation queue
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "moduleTest.h"
//#include "testFunc.h"

#ifdef AHMI_CORE

//-----------------------------
// 函数名： moduleTestTask
// 调用模块测试函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void moduleTestTask(void* pvParameters)
{
//	void (*moduleTaskFunctionPtr)(void); //the function pointer of module test	
	//change here to point the pointer to different test functions
	ERROR_PRINT("module Test Task create successful, now testing ... .\r\n");
	//ERROR_PRINT("	1\now testing button... .\r\n");
	//moduleTaskFunctionPtr = buttonCtrlTest;	
	////enter the test function
	//moduleTaskFunctionPtr();
	//ERROR_PRINT("	2\now testing OnOffTextureCtrlTest... .\r\n");	
	//moduleTaskFunctionPtr = SliderCtrlTest;	
	//moduleTaskFunctionPtr();
	//ERROR_PRINT("	3\now testing ClockCtrlTest... .\r\n");	
	//moduleTaskFunctionPtr = ClockCtrlTest;	
	//moduleTaskFunctionPtr();
	//ERROR_PRINT("	4\now testing DashBoardCtrlTest... .\r\n");	
	//moduleTaskFunctionPtr = DashBoardCtrlTest;	
	//moduleTaskFunctionPtr();
	//ERROR_PRINT("	5\now testing DynamicTexCtrlTest... .\r\n");	
	//moduleTaskFunctionPtr = DynamicTexCtrlTest;	
	//moduleTaskFunctionPtr();
	//ERROR_PRINT("	6\now testing NumberCtrlTest... .\r\n");	
	//moduleTaskFunctionPtr = NumberCtrlTest;	
	//moduleTaskFunctionPtr();
	//ERROR_PRINT("	7\now testing OnOffTextureCtrlTest... .\r\n");	
	//moduleTaskFunctionPtr = OnOffTextureCtrlTest;	
	//moduleTaskFunctionPtr();
	//ERROR_PRINT("	8\now testing ProbarCtrlTest... .\r\n");	
	//moduleTaskFunctionPtr = ProbarCtrlTest;	
	//moduleTaskFunctionPtr();
	//
	//
	//ERROR_PRINT("	9\now testing RefreshTaskTest... .\r\n");	
//	moduleTaskFunctionPtr = RefreshTaskTest;	
	//moduleTaskFunctionPtr();
	//ERROR_PRINT("	10\now testing tagClassTest... .\r\n");	
	//moduleTaskFunctionPtr = tagClassTest;	
	//moduleTaskFunctionPtr();
	//ERROR_PRINT("	11\now testing actionProcessorTest... .\r\n");	
	//moduleTaskFunctionPtr = actionProcessorTest;	
	//moduleTaskFunctionPtr();
	//ERROR_PRINT("	12\now testing animationTest... .\r\n");	
	//moduleTaskFunctionPtr = animationTest;	
	//moduleTaskFunctionPtr();
	//the task should not return 
	while(1)
	{
		vTaskDelay( SCREEN_UPDATE_TIME / portTICK_RATE_MS );
	}
}


#endif
