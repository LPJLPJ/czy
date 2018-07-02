////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIÐ¡×é³ÉÔ±
//
// Create Date:   2016/03/21
// File Name:     DynamicPageClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/08/11 by Zhou Yuzhi(ÖÜîÚÖÂ)
// Additional Comments:
//    handle the interruption
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef INTERRUPT_TASK__H
#define INTERRUPT_TASK__H

#include "semphr.h"
#include "publicDefine.h"

#ifdef AHMI_CORE

class TagClass;
typedef TagClass* TagClassPtr;

extern SemaphoreHandle_t interuptSemaphore;
extern TagClassPtr     TagPtr;

void interuptTask(void* pvParameters);

#endif

#endif
