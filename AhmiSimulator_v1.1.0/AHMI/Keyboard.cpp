////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     Keyboard.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/09/01 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    definition of keyboard trigger
// 
////////////////////////////////////////////////////////////////////////////////

#include "Keyboard.h"
#include "publicInclude.h"
#include "ActionTriggerClass.h"

extern QueueHandle_t        keyboardQueue;
extern TagClassPtr          TagPtr;

//void keyboardReveiveTask(void* pvParameters) //接受键盘的键值
//{
//	u8 keyboardValue;
//	BaseType_t xStatus;  
//	for(;;) 
// 	{
// 		xStatus=xQueueReceive(keyboardQueue,
// 							(void*)&keyboardValue,
// 							portMAX_DELAY);
// 		if(xStatus!=pdPASS)
// 		{
// 			return;
// 		}
//		TagPtr[SYSTEM_KEYBOARD_TAG].setValue(keyboardValue,SYSTEM_KEYBOARD_TAG);
//  		taskYIELD();
// 	}
//}

void keyboardLeft( void )
{
	ActionTriggerClass tagtrigger;
	tagtrigger.mInputType = ACTION_KEYBOARD_PRE;
	tagtrigger.keyboardTouch();
}
void keyboardRight( void )
{
	ActionTriggerClass tagtrigger;
	tagtrigger.mInputType = ACTION_KEYBOARD_NEXT;
	tagtrigger.keyboardTouch();
}
void keyboardEntered( void )
{
	ActionTriggerClass tagtrigger;
	tagtrigger.mInputType = ACTION_KEYBOARD_OK;
	tagtrigger.keyboardTouch();
}

//clear the highlight 
//reset the highlight pointer into the first highlight widget
void keyboardClear( void )     
{
	ActionTriggerClass tagtrigger;
	tagtrigger.mInputType = ACTION_KEYBOARD_CLEAR;
	tagtrigger.keyboardTouch();
}

void sentToKeyboardQueue(u8 key)
{
	xQueueSendToBack(keyboardQueue, &key, portMAX_DELAY);
}

