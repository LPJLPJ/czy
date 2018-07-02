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
//    
// 
////////////////////////////////////////////////////////////////////////////////

#ifndef TILEBOXCLASS__H
#define TILEBOXCLASS__H

#include "publicType.h"
#include "publicDefine.h"
#include "refreshQueueHandler.h"

#ifdef AHMI_CORE

class matrixClass;
class TileBoxClass;

typedef matrixClass* matrixClassPtr;
typedef TileBoxClass* TileBoxClassPtr;

class TileBoxClass
{
public:	
	//����
	s8 LeftBox; //���Χ��
	s8 TopBox;  //�ϰ�Χ��
	s8 RightBox; //�Ұ�Χ��
	s8 ButtomBox; //�°�Χ��
	//����
	TileBoxClass(void);
	~TileBoxClass(void);
	//��˾��󣬲����¼����Χ��
	funcStatus leftMulMatrix(matrixClassPtr pMatrix);
	//������Χ��
	funcStatus adjustSourceBuffer();
	//��Χ�п���
	funcStatus sourceBoxCopy(TileBoxClassPtr pSourceBox);
	//��Χ���ཻ�ж�
	funcStatus sourceBoxOverlap(TileBoxClassPtr pSourceBox);
	//�жϰ�Χ�а���
	funcStatus sourceBoxContain(TileBoxClassPtr pSourceBox);
	//��Χ�л�
	funcStatus sourceBoxOr(TileBoxClassPtr pSourceBox);
	//��Χ���ؼ���
	funcStatus sourceReCompute(
	ElementPtr ptr              ,     //pointer
	u8         elementType      ,     //type
	matrixClassPtr matrixPtr         //matrix
	);
};

#endif

#endif
