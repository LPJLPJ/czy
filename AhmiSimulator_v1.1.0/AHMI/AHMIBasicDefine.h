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
//    the original difinition in C
// 
////////////////////////////////////////////////////////////////////////////////
#ifndef AHMIBASICDEFINE__H
#define AHMIBASICDEFINE__H
#include "AHMICfgDefinition.h"
#ifdef AHMI_CORE
//*************************dynamicPage******************************
#define REFRESHPAGE 0x00
#define REFRESHWIDGET 0x01
#define REFRESHCANVAS 0x02

//******************InterfaceType****************************

#define  INTERFACETYPE_AHMI     0
#define  INTERFACETYPE_MODMST   1
#define  INTERFACETYPE_MODSLV   2

#endif//end of AHMI_CORE
#endif

