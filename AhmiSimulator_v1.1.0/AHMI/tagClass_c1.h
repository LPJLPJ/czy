////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     Tagclass.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/09/04 by zuz
// Additional Comments:
//    the basic structure of tag
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef TAGCLASS_C1__H
#define TAGCLASS_C1__H

#include "publicType.h"

class DynamicPageClass   ;
class WidgetClass        ;
class CanvasClass        ;
class ActionTriggerClass ;

typedef DynamicPageClass*  DynamicPageClassPtr  ;
typedef WidgetClass   	*  WidgetClassPtr       ;
typedef CanvasClass     *  CanvasClassPtr       ;
typedef ActionTriggerClass* ActionTriggerClassPtr;

typedef struct TagElementLinkData
{
	void*	mLinkElementPtr;
	u8	mElementType; //当mElementType = 0时表示widget，1表示canvas，2表示page，3表示NULL
}* ElemenLinkDataPtr;


#endif

