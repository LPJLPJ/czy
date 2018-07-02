////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
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
	u8	mElementType; //��mElementType = 0ʱ��ʾwidget��1��ʾcanvas��2��ʾpage��3��ʾNULL
}* ElemenLinkDataPtr;


#endif
