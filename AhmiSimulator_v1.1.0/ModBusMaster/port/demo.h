#ifndef _DENO_H
#define _DEMO_H
#include "port.h"

#define DISCRETE_INPUT_START        1
#define DISCRETE_INPUT_NDISCRETES   96
#define COIL_START                  1
#define COIL_NCOILS                 96
#define REG_INPUT_START             1
#define REG_INPUT_NREGS             100
#define REG_HOLDING_START           1
#define REG_HOLDING_NREGS           100
#define FALSH_USER_START_ADDRESS    (uint32_t)0x08010000           //用于用户存数据使用的内部闪存的起始地址
///add by Smaller

#define STRING_NSTRINGS			20
#define STRING_START			1
#define STRING_MAXLEN				40
//void	xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits,UCHAR ucValue );
//UCHAR	xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits );

#endif