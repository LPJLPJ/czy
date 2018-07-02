////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     ActionTriggerClass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.04 - C++, 20160321 by 于春营
// Additional Comments:
//    definition of tagtrigger input
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef ACTION_TRIGGER_CLASS_CD__H
#define ACTION_TRIGGER_CLASS_CD__H
//tagtrigger
#include "publicDefine.h"
//#define MOUSETOUCH 1
//#define TAGSETVALUE 2
//#define NOINPUT 0
//#define KEYBOARDINPUT 4 
//#define KEYBOARD_NONE 0    //什么都没按
//#define KEYBOARD_LEFT 1    //按下向左
//#define KEYBOARD_RIGHT 2   //按下向右
//#define KEYBOARD_ENTERED 3 //按下确认

#define ACTION_NULL          0    //0无事件。
#define ACTION_MOUSE_PRESS   1    //1 MousePress
#define ACTION_MOUSE_RELEASE 2    //2 MouseRelease
#define ACTION_MOUSE_HOLDING 3    //3 MouseHolding
#define ACTION_MOUSE_CANCLE  4    //4 MouseCancle 
#define ACTION_KEYBOARD_PRE  5    //5 KeyboardPre 指向前一个控件
#define ACTION_KEYBOARD_NEXT 6    //6 KeyboardNext 指向下一个控件
#define ACTION_KEYBOARD_OK   7    //7 KeyboardOK   按下OK
#define ACTION_TAG_SET_VALUE 8    //8 TagSetValue  给tag赋值
#define ACTION_KEYBOARD_CLEAR 9   //9 clear the highlight

#endif


