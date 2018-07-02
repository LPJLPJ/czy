////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     DynamicPage.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
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
	u8 LinkWidgetPageID;//WIdget����Page
	u8 LinkWidgetID;//WIdget�������±�
} *WidgetLinkDataPtr;

typedef struct CanvasLinkData
{
	u8 LinkCanvasPageID;
	u8 LinkCanvasID;	//CanvasId
}*CanvasLinkDataPtr;

typedef struct PageLinkData
{
	u8 LinkDynamicPageID;//Page��ID
	u16 LinkActionIDInDynamicPage;//Page Action�������±�
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
	u16 type;//tag�����ͣ�0��ʾ������1��ʾ�ַ����ĵ�ַ
	u32 value;
	u8 tagRW;         //tag��modbus������д��0��ʾ����1��ʾд by Arthas 20151210
	u16 regID;		 //tag�󶨵�reg by Arthas 20151207
}ModBusRefreshMsg; // tagˢ�¶�������Ľṹ�� by Arthas 20151210

typedef struct ModBusRW
{
	u16 regID;
	u16 numofReg;        
	u32 value;	
	u8 opCode;
}ModBusMasterRWMsg; // ModBus��д��������Ľṹ��

struct globalArgs_t
{
	unsigned int ScreenWidth; //-w(W) option ��Ļ��
	unsigned int ScreenHeigth;//-h(H) option ��Ļ��
	char* FileName;  //-f(F) option �����ļ�
};

#endif

#endif

