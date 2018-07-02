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
//    task to handle the animation queue
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef MODELTEST__H
#define MODELTEST__H
#include "publicDefine.h"
#ifdef AHMI_CORE
#ifdef MODULE_TEST_EN 
void moduleTestTask(void* pvParameters);
#endif
#endif

#endif
