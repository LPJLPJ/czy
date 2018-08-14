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
//    the math function
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef MYMATHCLASS__H
#define MYMATHCLASS__H

#include "publicDefine.h"

#ifdef AHMI_CORE

class myMathClass
{
public:
	void CORDIC(s32 Radian, s16* COS, s16 *SIN);
	void CORDIC_ANGLE(u16* Radian, s16 COS, s16 SIN);
	void CORDIC_32(s32 s32_Radian, s32* s32p_COS, s32 *s32p_SIN);
	void MatrixRotate(s16 degrees, s32 ipointmatrix[]);
	void MatrixScaler(s16 ScaleX, s16 ScaleY, s32 ipointmatrix[]);
	void MatrixShearX(s16 degrees,s32 ipointmatrix[]);
	void MatrixShearY(s16 degrees,s32 ipointmatrix[]);
	void FindAngle(s32* x, s32 *y);
	funcStatus transferFloatToS16(float f_in, s16* s16_out);
};

#endif

#endif
