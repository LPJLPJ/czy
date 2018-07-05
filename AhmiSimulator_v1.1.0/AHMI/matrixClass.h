////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     DynamicPage.h
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
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
	//�������
	funcStatus matrixMulti(
		matrixClass* pM1
		);

	//��ʼ��Ϊ��λ��
	funcStatus matrixInit(void);

	//��������
	funcStatus matrixScaler(s16 scalerX, s16 scalerY);
};


typedef matrixClass* matrixClassPtr;

//�������
class PointClass
{
public:
	s32 mPointX; //1.27.4
	s32 mPointY; //1.27.4
	PointClass (s32 x, s32 y);
	funcStatus leftMulMatrix(matrixClassPtr pMatrix);//��˾���
	funcStatus leftMulMatrixInv(matrixClassPtr pMatrix);//��������
	funcStatus pointRotating(s16 RotateAngle);//1.6.9��������ת�Ƕȣ��ȱ�ʾ��˳ʱ��Ϊ��
};

#endif

#endif

