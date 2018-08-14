////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     myMathClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"
#include "myMathClass.h"

#ifdef AHMI_CORE
matrixClass::matrixClass(void)
{
}


matrixClass::~matrixClass(void)
{
}

//-----------------------------
// 函数名： matrixMulti
// 矩阵相乘,this = pM * this 
// 参数列表：
//   @param1 pM1 1号矩阵的指针
//   @param2 pM2 2号矩阵的指针
// 备注(各个版本之间的修改):
//   created by arthas 20160114
//-----------------------------
funcStatus matrixClass::matrixMulti(
	matrixClass* pM
	)
{
	 matrixClass matrixTemp;
	 matrixTemp.A = ((long long)pM->A * this->A + (long long)pM -> B * this->C ) >> 20;
	 matrixTemp.B = ((long long)pM->A * this->B + (long long)pM -> B * this->D ) >> 20;
	 matrixTemp.E = this->E +  pM->E ;
	 matrixTemp.C = ((long long)pM->C * this->A + (long long)pM -> D * this->C ) >> 20;
	 matrixTemp.D = ((long long)pM->C * this->B + (long long)pM -> D * this->D ) >> 20;
	 matrixTemp.F = this->F + pM->F;
	 this->A = matrixTemp.A;
	 this->B = matrixTemp.B;
	 this->C = matrixTemp.C;
	 this->D = matrixTemp.D;
	 this->E = matrixTemp.E;
	 this->F = matrixTemp.F;
	 return AHMI_FUNC_SUCCESS;
}

funcStatus matrixClass::matrixInit(void)
{
	this->A = (1<<20);//1.11.20
	this->B = 0;
	this->C = 0;
	this->D = (1<<20);//1.11.20
	this->E = 0;
	this->F = 0;
	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// 函数名： matrixScaler
// 矩阵缩放 
// 参数列表：
//   @param1 scalerX X方向缩放比例，1.6.9
//   @param2 scalerY Y方向缩放比例，1.6.9
// 备注(各个版本之间的修改):
//   created by arthas 20160418
//-----------------------------
funcStatus matrixClass::matrixScaler(s16 scalerX, s16 scalerY)
{
	if(scalerX == 0 || scalerY == 0)
		return AHMI_FUNC_FAILURE;
	this->A = (this->A << 9) / scalerX;
	this->B = (this->B << 9) / scalerX;
	this->C = (this->C << 9) / scalerY;
	this->D = (this->D << 9) / scalerY;
	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// 函数名： PointClass
// 坐标类的构造函数 
// 参数列表：
//   @param1 x x方向坐标
//   @param2 y y方向坐标
// 备注(各个版本之间的修改):
//   created by arthas 2016506
//-----------------------------
PointClass::PointClass(s32 x, s32 y)
{
	this->mPointX = x;
	this->mPointY = y;
}


//-----------------------------
// 函数名： leftMulMatrix
// 矩阵左乘坐标点 
// 参数列表：
//   @param1 matrixClassPtr 矩阵指针
// 备注(各个版本之间的修改):
//   created by arthas 20160506
//-----------------------------
funcStatus PointClass::leftMulMatrix(matrixClassPtr pMatrix)
{
	s32 pointX, pointY;
#if 0
	pointX = ((pMatrix->A * (this->mPointX + pMatrix->E) ) >> 9) + (pMatrix->C * ((this->mPointY + pMatrix->F) ) >> 9);//4位小数位,先平移再旋转
	pointY = ((pMatrix->B * (this->mPointX + pMatrix->E) ) >> 9) + (pMatrix->D * ((this->mPointY + pMatrix->F) ) >> 9);//4位小数位,先平移再旋转
#else
	pointX = (((long long)pMatrix->A * (this->mPointX + (pMatrix->E >> 16)) ) >> 20) + (((long long)pMatrix->C * (this->mPointY + (pMatrix->F >> 16))) >> 20);//4位小数位,先平移再旋转
	pointY = (((long long)pMatrix->B * (this->mPointX + (pMatrix->E >> 16)) ) >> 20) + (((long long)pMatrix->D * (this->mPointY + (pMatrix->F >> 16))) >> 20);//4位小数位,先平移再旋转
#endif
	this->mPointX = pointX;
	this->mPointY = pointY;
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： leftMulMatrix
// 逆矩阵左乘坐标点 
// 参数列表：
//   @param1 matrixClassPtr 矩阵指针
// 备注(各个版本之间的修改):
//   created by arthas 20160506
//-----------------------------
funcStatus PointClass::leftMulMatrixInv(matrixClassPtr pMatrix)
{
	s32 pointX, pointY; //1.27.4
	pointX = ( (this->mPointX) << 9) / pMatrix->A /*+ ( (this->mPointY ) << 9 ) / pMatrix->C*/ - pMatrix->E;//4位小数位,先平移再旋转
	pointY = /*( (this->mPointX) << 9) / pMatrix->B +*/ ( (this->mPointY ) << 9 ) / pMatrix->D - pMatrix->F;//4位小数位,先平移再旋转
	this->mPointX = pointX;
	this->mPointY = pointY;
	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// 函数名： leftMulMatrix
// 矩阵左乘坐标点 
// 参数列表：
//   @param1 RotateAngle 1.6.9，纹理旋转角度，度表示，顺时针为正
// 备注(各个版本之间的修改):
//   created by arthas 20160614
//-----------------------------
funcStatus PointClass::pointRotating(
	s16 RotateAngle//1.6.9，纹理旋转角度，度表示，顺时针为正
	)
{
	s32 para1,para2;
	myMathClass myMath;
	matrixClass matrixTemp;
	para1 = 0x100000; //cos
	para2 = 0;   //sin
	myMath.CORDIC_32(RotateAngle,&para1,&para2);//先计算旋转的正矩阵
	matrixTemp.A = para1;
	matrixTemp.C = -para2;
	matrixTemp.B = para2;
	matrixTemp.D = para1; //因为我用的是转置矩阵，所以这样赋值
	//旋转的正矩阵为
	// cos -sin
	// sin cos
	matrixTemp.E = 0;
	matrixTemp.F = 0;
	
	return this->leftMulMatrix(&matrixTemp);//像素点乘以旋转矩阵
}


#endif
