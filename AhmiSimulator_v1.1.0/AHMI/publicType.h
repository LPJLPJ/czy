////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     Type.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    the fundamental type
// 
////////////////////////////////////////////////////////////////////////////////

#include "AHMICfgDefinition.h"


#ifndef PUBLICTYPE__H
#define PUBLICTYPE__H

#ifdef EMBEDDED
#include "stm32f10x.h"
typedef uint64_t u64;
#endif

#ifdef PC_SIM

#include <stdint.h>
/*!< STM32F10x Standard Peripheral Library old types (maintained for legacy purpose) */
typedef int  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */


typedef uint64_t u64;
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const int16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */
#endif


#endif
