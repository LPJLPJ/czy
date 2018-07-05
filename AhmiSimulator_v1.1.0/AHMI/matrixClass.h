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
//    matrix
// 
////////////////////////////////////////////////////////////////////////////////


#ifndef MATRIXCLASS__H
#define MATRIXCLASS__H

#include "publicType.h"
#include "AHMIBasicDefine.h"
#include "publicDefine.h"

#ifdef AHMI_CORE
class matrixClass
{
public:
	s32 A;	//1.11.20
	s32 B;	//1.11.20
	s32 C;	//1.11.20
	s32 D;	//1.11.20
	s32 E;  //1.18.13
	s32 F;  //1.18.13
	matrixClass(void);
	~matrixClass(void);
	//矩阵相乘
	funcStatus matrixMulti(
		matrixClass* pM1
		);

	//初始化为单位阵
	funcStatus matrixInit(void);

	//矩阵缩放
	funcStatus matrixScaler(s16 scalerX, s16 scalerY);
};


typedef matrixClass* matrixClassPtr;

//坐标点类
class PointClass
{
public:
	s32 mPointX; //1.27.4
	s32 mPointY; //1.27.4
	PointClass (s32 x, s32 y);
	funcStatus leftMulMatrix(matrixClassPtr pMatrix);//左乘矩阵
	funcStatus leftMulMatrixInv(matrixClassPtr pMatrix);//左乘逆矩阵
	funcStatus pointRotating(s16 RotateAngle);//1.6.9，纹理旋转角度，度表示，顺时针为正
};

#endif

#endif

