////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     DynamicPage.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    the original structure in C
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef AHMIBASICSTRUCTURE__H
#define AHMIBASICSTRUCTURE__H
#include "publicType.h"
#include "publicDefine.h"


#ifdef AHMI_CORE
typedef struct WidgetLinkData
{
	u8 LinkWidgetPageID;//WIdget所属Page
	u8 LinkWidgetID;//WIdget的数组下标
} *WidgetLinkDataPtr;

typedef struct CanvasLinkData
{
	u8 LinkCanvasPageID;
	u8 LinkCanvasID;	//CanvasId
}*CanvasLinkDataPtr;

typedef struct PageLinkData
{
	u8 LinkDynamicPageID;//Page的ID
	u16 LinkActionIDInDynamicPage;//Page Action的数组下标
} *PageLinkDataPtr;

//*************************ActionTriggerClass*************************************
typedef struct PID
{
u16 x;
u16 y;
u8 press;
}PIDState;

typedef struct PIDPoint
{
u16 x;
u16 y;
}PIDPoint;

typedef PIDState* PIDPtr;
typedef PIDPoint* PIDPointPtr;

//******************ModbusTask***************************************
typedef struct ModBusRefresh
{
	u16 type;//tag的类型，0表示整数，1表示字符串的地址
	u32 value;
	u8 tagRW;         //tag从modbus读或者写。0表示读，1表示写 by Arthas 20151210
	u16 regID;		 //tag绑定的reg by Arthas 20151207
}ModBusRefreshMsg; // tag刷新队列所需的结构体 by Arthas 20151210

typedef struct ModBusRW
{
	u16 regID;
	u16 numofReg;        
	u32 value;	
	u8 opCode;
}ModBusMasterRWMsg; // ModBus读写队列所需的结构体

struct globalArgs_t
{
	unsigned int ScreenWidth; //-w(W) option 屏幕宽
	unsigned int ScreenHeigth;//-h(H) option 屏幕高
	char* FileName;  //-f(F) option 输入文件
};

#endif

#endif

