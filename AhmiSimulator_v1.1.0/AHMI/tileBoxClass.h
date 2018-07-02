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
	//变量
	s8 LeftBox; //左包围盒
	s8 TopBox;  //上包围盒
	s8 RightBox; //右包围盒
	s8 ButtomBox; //下包围盒
	//函数
	TileBoxClass(void);
	~TileBoxClass(void);
	//左乘矩阵，并重新计算包围盒
	funcStatus leftMulMatrix(matrixClassPtr pMatrix);
	//调整包围盒
	funcStatus adjustSourceBuffer();
	//包围盒拷贝
	funcStatus sourceBoxCopy(TileBoxClassPtr pSourceBox);
	//包围盒相交判断
	funcStatus sourceBoxOverlap(TileBoxClassPtr pSourceBox);
	//判断包围盒包含
	funcStatus sourceBoxContain(TileBoxClassPtr pSourceBox);
	//包围盒或
	funcStatus sourceBoxOr(TileBoxClassPtr pSourceBox);
	//包围盒重计算
	funcStatus sourceReCompute(
	ElementPtr ptr              ,     //pointer
	u8         elementType      ,     //type
	matrixClassPtr matrixPtr         //matrix
	);
};

#endif

#endif
