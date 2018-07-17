////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:     ActionTriggerClass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.04 - C++, 20160321 by �ڴ�Ӫ
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
    PIDState mMousePID;				//mouseָ�룬������Ļ��������ʱʹ�á�0����mouse�¼�
    TagClassPtr mTagPtr;			    //tagָ�룬tag��������ʱʹ��
	u8	mInputType;					
	//0���¼���
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

	//����¼�
	funcStatus MouseTouch(PIDPoint* pPressPoint,PIDPoint* pReleasePoint);
	//�����¼�
	funcStatus keyboardTouch();
	//�ؼ�����
	funcStatus widgetActionTrigger(
		 WidgetClassPtr pWidgetPtr,
		 u8 u8_widgetRefresh    //�Ƿ���Ҫˢ�¿ؼ���added by zuz 20160622
		 );
};

typedef class ActionTriggerClass* ActionTriggerClassPtr;

void initTagTrigger(void);

#endif

#endif

