////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     Trace.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef TRACE__H
#define TRACE__H

#ifdef PC_SIM
#include "stdio.h"
extern	void TraceDlgPrint(char* printdata);
void TraceDlgPrint_cpp(char* printdata);

#ifndef __cplusplus
#define ERROR_PRINT(errordata)  TraceDlgPrint(errordata)
#endif

#ifdef __cplusplus
#define ERROR_PRINT(errordata) TraceDlgPrint_cpp(errordata)
#endif

#endif

#ifdef EMBEDDED

#ifndef __cplusplus
#define ERROR_PRINT(errordata)
#endif

#ifdef __cplusplus
#define ERROR_PRINT(errordata) 

#endif

#endif

#define  WriteText sprintf

#endif


