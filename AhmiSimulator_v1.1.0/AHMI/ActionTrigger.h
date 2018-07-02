////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     ActionTrigger.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef  ACTIONTRIGGER__H
#define  ACTIONTRIGGER__H


#include "AHMICfgDefinition.h"

#ifdef PC_SIM
#define  MOUSE_PRESS    0
#define  MOUSE_RELEASE  1
#define  MOUSE_HOLD     2
#endif

#ifdef EMBEDDED
#define  MOUSE_PRESS    1
#define  MOUSE_RELEASE  2
#define  MOUSE_HOLD     3
#endif


#endif
