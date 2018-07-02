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
#include "tagClassTest.h"
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

#ifdef AHMI_CORE

//-----------------------------
// 函数名： animationTest
// 测试animationClass的相关函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void tagClassTest(void)
{
	std::ofstream animationTestLogFile;
	tagClassTestClass tagClassTest;
	animationTestLogFile.open("testLog\\tagClassTest.Log",std::ios::out);
	if(!animationTestLogFile.is_open())
	{
		ERROR_PRINT("ERROR in tagClass test: connot open file tagClassTest.Log");
		return;
	}
	tagClassTest.tagSetValueTest(&animationTestLogFile);
	tagClassTest.tagSetBindingElementTest(&animationTestLogFile);
	tagClassTest.updateAllTagsTest(&animationTestLogFile);
	animationTestLogFile.close();

}

//-----------------------------
// 函数名： tagSetValueTest
// 测试tagClass::setValue函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void tagClassTestClass::tagSetValueTest(std::ofstream* pAnimationTestLogFile)
{
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	u16 tagID;
	u32 tagValue;
	u8 *tagChangedListPtrTemp;
	ERROR_PRINT("testing create animation function\n");
	*pAnimationTestLogFile << "testing create animation function\n";


	//illegal input
	ERROR_PRINT("test illegal input\n");
	*pAnimationTestLogFile << "test illegal input\n";
	tagID = 500;
	tagValue = 1000;
	TagPtr[tagID].setValue(tagValue,tagID);

	*pAnimationTestLogFile << "check whether the trace window print error information. only when error information is printed is this test passed\n";

	//boundary
	ERROR_PRINT("test boundary data\n");
	*pAnimationTestLogFile << "test boundary data\n";
	tagID = SYSTEM_ALARM_TAG;
	//for(tagID = 0; tagID < ConfigData.NumofTags; tagID++)
	//{
		for(tagValue = 0; tagValue < 65536; tagValue++)
		{
			TagPtr[tagID].setValue(tagValue,tagID);
		}
	//}
	ERROR_PRINT("boundary data test passed\n");
	*pAnimationTestLogFile << "test boundary data test passed\n";

	//corruption
	ERROR_PRINT("test corruption data\n");
	*pAnimationTestLogFile << "test corruption data\n";
	tagChangedListPtrTemp = TagChangedListPtr;
	TagChangedListPtr = NULL;
	tagID = 1;
	tagValue = 100;
	TagPtr[tagID].setValue(tagValue,tagID);
	ERROR_PRINT("corruption data test passed\n");
	*pAnimationTestLogFile << "test corruption data test passed\n";
	TagChangedListPtr = tagChangedListPtrTemp;

}


//-----------------------------
// 函数名： tagSetBindingElement
// 测试tagClass::setValue函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void tagClassTestClass::tagSetBindingElementTest(std::ofstream* pAnimationTestLogFile)
{
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
//	u16 tagID;
//	u32 tagValue;
//	u8 *tagChangedListPtrTemp;
	ERROR_PRINT("testing tag set binding element function\n");
	*pAnimationTestLogFile << "testing tag set binding element function\n";


	//illegal input
	ERROR_PRINT("test illegal input\n");
	*pAnimationTestLogFile << "test illegal input\n";
	
	ERROR_PRINT("1.page tag illegal input\n");
	*pAnimationTestLogFile << "1.page tag illegal input\n";

	TagPtr[SYSTEM_PAGE_TAG].setValue(10,SYSTEM_PAGE_TAG);
	TagPtr[SYSTEM_PAGE_TAG].setBindingElement();

	ERROR_PRINT("2.canvas tag illegal input\n");
	*pAnimationTestLogFile << "2.canvas tag illegal input\n";
	TagPtr[SYSTEM_BACKLIGHT_TAG].setValue(10,SYSTEM_BACKLIGHT_TAG);
	TagPtr[SYSTEM_BACKLIGHT_TAG].setBindingElement();

	ERROR_PRINT("3.keyboard tag illegal input\n");
	*pAnimationTestLogFile << "3.keyboard tag illegal input\n";
	TagPtr[SYSTEM_KEYBOARD_TAG].setValue(10,SYSTEM_KEYBOARD_TAG);
	TagPtr[SYSTEM_KEYBOARD_TAG].setBindingElement();

	//baundary
	ERROR_PRINT("test baundary input\n");
	*pAnimationTestLogFile << "test baundary input\n";
	for(unsigned int i = 0; i < 65536; i ++)
	{
		TagPtr[SYSTEM_ALARM_TAG].setValue(i,SYSTEM_ALARM_TAG);
		TagPtr[SYSTEM_ALARM_TAG].setBindingElement();
	}
	ERROR_PRINT("baundary input passed\n");
	*pAnimationTestLogFile << "baundary input passed\n";

}

//-----------------------------
// 函数名： updateAllTagsTest
// 测试updateAllTagsTest函数
// 参数列表：
// 备注(各个版本之间的修改):
//   
//-----------------------------
void tagClassTestClass::updateAllTagsTest(std::ofstream* pAnimationTestLogFile)
{
	taskENTER_CRITICAL();
	u8 ahmi_func_status = AHMI_FUNC_SUCCESS;
	u16 tagID;
//	u32 tagValue;
	ERROR_PRINT("testing UpdateAllTags function\n");
	*pAnimationTestLogFile << "testing UpdateAllTags function\n";
	for(tagID = 0; tagID < ConfigData.NumofTags; tagID++)
	{
		TagPtr[tagID].setValue(tagID,tagID);
	}
	UpdateAllTags();
	ERROR_PRINT("UpdateAllTags function test passed\n");
	*pAnimationTestLogFile << "UpdateAllTags function test passed\n";
	taskEXIT_CRITICAL();
}



#endif
