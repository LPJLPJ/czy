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
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef ACTION_TRIGGER_CLASS__H
#define ACTION_TRIGGER_CLASS__H
#include "publicType.h"
#include "ActionTriggerClass_c1.h"
#include "AHMIBasicStructure.h"
#include "ActionTriggerClass_cd.h"
#include "publicDefine.h"

#ifdef AHMI_CORE
class ActionTriggerClass {
public:
    PIDState mMousePID;				//mouse指针，触摸屏幕触发更新时使用。0：无mouse事件
    TagClassPtr mTagPtr;			    //tag指针，tag触发更新时使用
	u8	mInputType;					
	//0无事件。
    //1 MousePress
    //2 MouseRelease
	//3 MouseHolding
    //4 MouseCancle 
    //5 KeyboardPre 
    //6 KeyboardNext
    //7 KeyboardOK
    //8 TagSetValue
	ActionTriggerClass();
	~ActionTriggerClass();
	funcStatus initActionTrigger(void);
	int MouseTouch(DynamicPageClassPtr Ptr);

	//鼠标事件
	funcStatus MouseTouch(PIDPoint* pPressPoint,PIDPoint* pReleasePoint);
	//键盘事件
	funcStatus keyboardTouch();
	//控件处理
	funcStatus widgetActionTrigger(
		 WidgetClassPtr pWidgetPtr,
		 u8 u8_widgetRefresh    //是否需要刷新控件，added by zuz 20160622
		 );
};

typedef class ActionTriggerClass* ActionTriggerClassPtr;

void initTagTrigger(void);

#endif

#endif

