////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     Trace.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/07/27  by 于春营 
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef USER_INTERFACE
#define USER_INTERFACE

#ifdef EMBEDDED
#include "stm32f10x_gpio.h"
#include "AHMICfgDefinition.h"
#endif

#ifdef __cplusplus 
extern "C" {
#endif
	

	
typedef struct AHMIPinCfgTag
{
	unsigned int USETOUCH;              //为0表示不使用触屏，为1表示使用触屏
	
}AHMIPinCfg;

typedef struct TaskStackStr            //用于检测任务堆栈是否溢出
{
	unsigned int mBottomOfStack;                  //任务堆栈栈低地址
	unsigned int mCurOfStack;                     //任务堆栈当前地址
}TaskStack;

void CheckTaskStack(TaskStack* pTaskStack);
void initAHMIHardware(AHMIPinCfg* AHMIpincfg);
void initQueue(void);
int startAHMI(AHMIPinCfg* AHMIpincfg);
unsigned int getTagValue(unsigned int ID);
void setTagValue(unsigned int ID, unsigned int value);
void setRegValue(unsigned int ID, unsigned int value);
#ifdef EMBEDDED
void AHMISystemReset(void);
void AHMIEnterStopMode(void);
void MCO_Init(void);
#endif
	
#ifdef __cplusplus 
}
#endif

#endif
