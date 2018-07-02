////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:     Trace.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/07/27  by �ڴ�Ӫ 
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
	unsigned int USETOUCH;              //Ϊ0��ʾ��ʹ�ô�����Ϊ1��ʾʹ�ô���
	
}AHMIPinCfg;

typedef struct TaskStackStr            //���ڼ�������ջ�Ƿ����
{
	unsigned int mBottomOfStack;                  //�����ջջ�͵�ַ
	unsigned int mCurOfStack;                     //�����ջ��ǰ��ַ
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
