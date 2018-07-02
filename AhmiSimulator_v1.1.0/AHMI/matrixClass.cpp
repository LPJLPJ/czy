////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     myMathClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
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
// �������� matrixMulti
// �������,this = pM * this 
// �����б���
//   @param1 pM1 1�ž����ָ��
//   @param2 pM2 2�ž����ָ��
// ��ע(�����汾֮����޸�):
//   created by arthas 20160114
//-----------------------------
funcStatus matrixClass::matrixMulti(
	matrixClass* pM
	)
{
	 matrixClass matrixTemp;
	 matrixTemp.A = (pM->A * this->A + pM -> B * this->C  ) >> 9;
	 matrixTemp.B = (pM->A * this->B + pM -> B * this->D  ) >> 9;
	 matrixTemp.E = this->E +  pM->E ;
	 matrixTemp.C = (pM->C * this->A + pM -> D * this->C ) >> 9;
	 matrixTemp.D = (pM->C * this->B + pM -> D * this->D ) >> 9;
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
	this->A = (1<<9);
	this->B = 0;
	this->C = 0;
	this->D = (1<<9);
	this->E = 0;
	this->F = 0;
	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// �������� matrixScaler
// �������� 
// �����б���
//   @param1 scalerX X�������ű�����1.6.9
//   @param2 scalerY Y�������ű�����1.6.9
// ��ע(�����汾֮����޸�):
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
// �������� PointClass
// ������Ĺ��캯�� 
// �����б���
//   @param1 x x��������
//   @param2 y y��������
// ��ע(�����汾֮����޸�):
//   created by arthas 2016506
//-----------------------------
PointClass::PointClass(s32 x, s32 y)
{
	this->mPointX = x;
	this->mPointY = y;
}


//-----------------------------
// �������� leftMulMatrix
// ������������ 
// �����б���
//   @param1 matrixClassPtr ����ָ��
// ��ע(�����汾֮����޸�):
//   created by arthas 20160506
//-----------------------------
funcStatus PointClass::leftMulMatrix(matrixClassPtr pMatrix)
{
	s32 pointX, pointY;
	pointX = ((pMatrix->A * (this->mPointX+ pMatrix->E) ) >> 9) + ((pMatrix->C * (this->mPointY+ pMatrix->F) ) >> 9);//4λС��λ,��ƽ������ת
	pointY = ((pMatrix->B * (this->mPointX+ pMatrix->E) ) >> 9) + ((pMatrix->D * (this->mPointY+ pMatrix->F) ) >> 9);//4λС��λ,��ƽ������ת
	this->mPointX = pointX;
	this->mPointY = pointY;
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// �������� leftMulMatrix
// ������������� 
// �����б���
//   @param1 matrixClassPtr ����ָ��
// ��ע(�����汾֮����޸�):
//   created by arthas 20160506
//-----------------------------
funcStatus PointClass::leftMulMatrixInv(matrixClassPtr pMatrix)
{
	s32 pointX, pointY; //1.27.4
	pointX = ( (this->mPointX) << 9) / pMatrix->A /*+ ( (this->mPointY ) << 9 ) / pMatrix->C*/ - pMatrix->E;//4λС��λ,��ƽ������ת
	pointY = /*( (this->mPointX) << 9) / pMatrix->B +*/ ( (this->mPointY ) << 9 ) / pMatrix->D - pMatrix->F;//4λС��λ,��ƽ������ת
	this->mPointX = pointX;
	this->mPointY = pointY;
	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// �������� leftMulMatrix
// ������������ 
// �����б���
//   @param1 RotateAngle 1.6.9��������ת�Ƕȣ��ȱ�ʾ��˳ʱ��Ϊ��
// ��ע(�����汾֮����޸�):
//   created by arthas 20160614
//-----------------------------
funcStatus PointClass::pointRotating(
	s16 RotateAngle//1.6.9��������ת�Ƕȣ��ȱ�ʾ��˳ʱ��Ϊ��
	)
{
	s16 para1,para2;
	myMathClass myMath;
	matrixClass matrixTemp;
	para1 = 512; //cos
	para2 = 0;   //sin
	myMath.CORDIC(RotateAngle,&para1,&para2);//�ȼ�����ת��������
	matrixTemp.A = para1;
	matrixTemp.C = -para2;
	matrixTemp.B = para2;
	matrixTemp.D = para1; //��Ϊ���õ���ת�þ�������������ֵ
	//��ת��������Ϊ
	// cos -sin
	// sin cos
	matrixTemp.E = 0;
	matrixTemp.F = 0;
	
	return this->leftMulMatrix(&matrixTemp);//���ص������ת����
}


#endif