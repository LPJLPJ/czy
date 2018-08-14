////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     tileBoxClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#include "tileBoxClass.h"
#include "publicDefine.h"
#include "publicInclude.h"

#ifdef AHMI_CORE

extern u16             WorkingPageID;
extern DynamicPageClassPtr  gPagePtr;

extern"C"
{
extern struct globalArgs_t globalArgs;
}

TileBoxClass::TileBoxClass(void)
{}

TileBoxClass::~TileBoxClass(void)
{}

funcStatus TileBoxClass::leftMulMatrix(matrixClassPtr pMatrix)
{
	if(pMatrix == NULL)
		return AHMI_FUNC_FAILURE;
	return AHMI_FUNC_SUCCESS;
}

//调整包围盒
funcStatus TileBoxClass::adjustSourceBuffer()
{
	if(this->LeftBox < 0)
		this->LeftBox = 0;
	if(this->TopBox < 0)
		this->TopBox = 0;
	if(this->RightBox > (s8)((TILE_NUM_X) - 1))
		this->RightBox = (s8)((TILE_NUM_X) - 1);
	if(this->ButtomBox > (s8)((TILE_NUM_Y) - 1))
		this->ButtomBox = (s8)((TILE_NUM_Y) - 1);
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名：  sourceBoxCopy
// 拷贝输入的包围盒
// 参数列表：
// @param1 TileBoxClassPtr   pSourceBox    输入的包围盒
// 备注(各个版本之间的修改):
// ...
//-----------------------------
funcStatus TileBoxClass::sourceBoxCopy(TileBoxClassPtr pSourceBox)
{
	this->LeftBox   = pSourceBox->LeftBox   ;
	this->RightBox  = pSourceBox->RightBox  ;
	this->TopBox    = pSourceBox->TopBox    ;
	this->ButtomBox = pSourceBox->ButtomBox ;
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名：  sourceBoxOr
// 包围盒取或，只有当两个包围盒包含相同区域的时候才会合并
// 参数列表：
// @param1 TileBoxClassPtr   pSourceBox    输入的包围盒
// 备注(各个版本之间的修改):
// ...
//-----------------------------
funcStatus TileBoxClass::sourceBoxOr(TileBoxClassPtr pSourceBox)
{
	if(pSourceBox->LeftBox < this->LeftBox)
		this->LeftBox = pSourceBox->LeftBox;
	if(pSourceBox->RightBox > this->RightBox)
		this->RightBox = pSourceBox->LeftBox;
	if(pSourceBox->TopBox > this->TopBox)
		 this->TopBox = pSourceBox->TopBox;
	if(pSourceBox->ButtomBox <  this->ButtomBox)
		 this->ButtomBox = pSourceBox->ButtomBox;
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名：  sourceBoxOverlap
// 判断两个包围盒是否有相交部分,有则返回AHMI_FUNC_SUCCESS
// 参数列表：
// @param1 TileBoxClassPtr   pSourceBox    输入的包围盒
// 备注(各个版本之间的修改):
// ...
//-----------------------------
funcStatus TileBoxClass::sourceBoxOverlap(TileBoxClassPtr pSourceBox)
{
	if(
		pSourceBox->LeftBox  >  this->RightBox  || 
		pSourceBox->RightBox <  this->LeftBox   || 
		pSourceBox->TopBox   >  this->ButtomBox ||
		pSourceBox->ButtomBox < this->TopBox 
		)
		return AHMI_FUNC_FAILURE;
	else 
		return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名：  sourceBoxContain
// 判断该包围盒是否包含输入的包围盒，是则返回AHMI_FUNC_SUCCESS
// 参数列表：
// @param1 TileBoxClassPtr   pSourceBox    输入的包围盒
// 备注(各个版本之间的修改):
// ...
//-----------------------------
funcStatus TileBoxClass::sourceBoxContain(TileBoxClassPtr pSourceBox)
{
	if(
		pSourceBox->LeftBox   >= this->LeftBox  && 
		pSourceBox->RightBox  <= this->RightBox && 
		pSourceBox->TopBox    >= this->TopBox   &&
		pSourceBox->ButtomBox <= this->ButtomBox 
		)
		return AHMI_FUNC_SUCCESS;
	else
		return AHMI_FUNC_FAILURE;
}

//-----------------------------
// 函数名：  sourceReCompute
// 重新计算包围盒,只包括平移和放缩
// widget和canvas都按照绝对坐标计算
// 参数列表：
// @param1 matrixClassPtr   pMatrix    输入的矩阵
// 备注(各个版本之间的修改):
// ...
//-----------------------------
funcStatus TileBoxClass::sourceReCompute(
	ElementPtr ptr              ,     //pointer
	u8         elementType      ,     //type
	matrixClassPtr matrixPtr         //matrix
	)
{
	s32 curX,curY; //1.27.4
	u32 curWidth, curHeight; //28.4
	CanvasClassPtr focusedCanvas;
	PointClass point1(0,0),point2(0,0),point3(0,0),point4(0,0);
	WidgetClassPtr pWidget;
	TextureClassPtr texturePtr;
	u8 flag = 0;
	//   1  ------  2
	//     |      |
	//     |      |
	//   4  ------  3
	if(NULL == ptr.cptr || NULL == ptr.pageptr || NULL ==ptr.scptr || NULL ==ptr.tptr || NULL == ptr.wptr || NULL == matrixPtr){
		ERROR_PRINT("ERROR: null pointer for parameter");
		return AHMI_FUNC_FAILURE;
	}
	if(elementType == ANIMATION_REFRESH_NULL)
		return AHMI_FUNC_FAILURE;
	else if(elementType == ANIMATION_REFRESH_PAGE)
	{
		curX = 0;
		curY = 0;
#ifdef PC_SIM
		curWidth = (globalArgs.ScreenWidth) << 4;
		curHeight = (globalArgs.ScreenHeigth) << 4;
#endif
#ifdef EMBEDDED
		curWidth = (SCREEN_WIDTH) << 4;
		curHeight = (SCREEN_HEIGHT) << 4;
#endif
	}
	else if(elementType == ANIMATION_REFRESH_CANVAS)
	{
		focusedCanvas = &(gPagePtr[WorkingPageID].pCanvasList[ptr.scptr->attachCanvas]);
		curX = (focusedCanvas->moffsetX) << 4;
		curY = (focusedCanvas->moffsetY) << 4;
		curHeight = (focusedCanvas->mheight) << 4;
		curWidth = (focusedCanvas->mwidth) << 4;
	}
	else if( elementType == ANIMATION_REFRESH_WIDGET)
	{
		pWidget = ptr.wptr;
		if(pWidget - gPagePtr[WorkingPageID].pWidgetList > gPagePtr[WorkingPageID].mNumOfWidget || pWidget - gPagePtr[WorkingPageID].pWidgetList < 0){
			ERROR_PRINT("ERROR: wrong pointer parameter");
			return AHMI_FUNC_FAILURE;
		}
		focusedCanvas = &gPagePtr[WorkingPageID].pCanvasList[pWidget->ATTATCH_CANVAS];
		if((pWidget->WidgetAttr & 0x1f) == DYNAMIC_TEX && ((pWidget->WidgetAttr & DYNAMIC_TYPE_BIT) >> 5) == CENTRAL_ROTATE && (pWidget->OldValueL != 0 || pWidget->OldValueH != 0)) //rotating texture
		{
			texturePtr = &gPagePtr[WorkingPageID].pBasicTextureList[pWidget->StartNumofTex];
			this->LeftBox     = texturePtr->TexLeftTileBox   ;
			this->TopBox      = texturePtr->TexTopTileBox    ;
			this->RightBox    = texturePtr->TexRightTileBox  ;
			this->ButtomBox   = texturePtr->TexButtomTileBox ;
			flag = 1;
		}
		else if( ( (pWidget->WidgetAttr & 0x1f) == METER_TEX) && ((pWidget->WidgetAttr & METER_SIMPLIFY) == METER_SIMPLIFY ) )//仪表盘单独处理
		{
			texturePtr = &gPagePtr[WorkingPageID].pBasicTextureList[pWidget->StartNumofTex];
			//绘制指针
			this->LeftBox     = texturePtr[6].TexLeftTileBox   ;
			this->TopBox      = texturePtr[6].TexTopTileBox    ;
			this->RightBox    = texturePtr[6].TexRightTileBox  ;
			this->ButtomBox   = texturePtr[6].TexButtomTileBox ;
			flag = 1;
		}
		else if((pWidget->WidgetAttr & 0x1f) == OSCSCP_TEX)//单独处理示波器
		{
			gPagePtr[WorkingPageID].RefreshOSCWidgetBox(pWidget,this);
			flag = 1;
		}
		if(flag == 1)
		{
			curX = (s32)((this->LeftBox) << 9) - (s32)(focusedCanvas->moffsetX << 4);
			curY = (s32)((this->TopBox ) << 9) - (s32)(focusedCanvas->moffsetY << 4);
			//curX = ((this->LeftBox) << 9) ;
			//curY = ((this->TopBox ) << 9) ;
			curWidth = (this->RightBox - this->LeftBox ) << 9;
			curHeight = (this->ButtomBox - this->TopBox ) << 9;
		}
		else
		{
			curX = (s32)( (s16)ptr.wptr->WidgetOffsetX - (s16)focusedCanvas->moffsetX) * 16;//相对canvas的偏移
			curY = (s32)( (s16)ptr.wptr->WidgetOffsetY - (s16)focusedCanvas->moffsetY) * 16;
			//curX = (ptr.wptr->WidgetOffsetX ) << 4;
			//curY = (ptr.wptr->WidgetOffsetY ) << 4;
			curWidth = (ptr.wptr->WidgetWidth) << 4;
			curHeight =  (ptr.wptr->WidgetHeight) << 4;
		}
		
	}
	//else if(elementType == ANIMATION_REFRESH_TEXTURE)
	//{
	//	curX = (ptr.tptr->OffsetX);
	//	curY = ptr.tptr->OffsetY;
	//	curWidth = (ptr.tptr->TexWidth) << 4;
	//	curHeight = (ptr.tptr->TexHeight) << 4;
	//}
	if( elementType == ANIMATION_REFRESH_WIDGET)
	{
		//计算坐标点
		point1.mPointX = (long long)curX * 0x100000 / matrixPtr->A - (matrixPtr->E >> 9) + (focusedCanvas->moffsetX << 4); //相对偏移量需要乘以缩放系数，然后加上矩阵中传过来的绝对偏移量
		point1.mPointY = (long long)curY * 0x100000 / matrixPtr->A - (matrixPtr->F >> 9) + (focusedCanvas->moffsetY << 4); //注意：widget中矩阵传来的平移量是减去了canvas原始位置得到的量

	}else{
	    point1.mPointX = curX - (matrixPtr->E >> 9); //canvas矩阵中传来的偏移量就是相对于当前未缩放的canvas的相对偏移量，不需要再乘以缩放量
	    point1.mPointY = curY - (matrixPtr->F >> 9);
	}
	//point2.mPointX = curX + curWidth;
	//point2.mPointY = curY;
	point3.mPointX = (long long)curWidth * 0x100000 / matrixPtr->A;
	point3.mPointY = (long long)curHeight * 0x100000 / matrixPtr->A;
	//point4.mPointX = curX;
	//point4.mPointY = curY + curHeight;
	//point1.leftMulMatrixInv(matrixPtr);
	//point2.leftMulMatrixInv(matrixPtr);
	//point3.leftMulMatrixInv(matrixPtr);
	//point4.leftMulMatrixInv(matrixPtr);
	this->LeftBox = (point1.mPointX) / 512;	 // 16 * TILESIZE
	this->TopBox  = (point1.mPointY) / 512;  // 16 * TILESIZE
	//
	#if 0
	this->RightBox = ((point1.mPointX + point3.mPointX) /16 + TILESIZE - 1)/ TILESIZE ;
	this->ButtomBox = ((point1.mPointY + point3.mPointY) / 16 + TILESIZE - 1) / TILESIZE ;
	#else
	this->RightBox = ((point1.mPointX + point3.mPointX) /16 )/ TILESIZE ;
	this->ButtomBox = ((point1.mPointY + point3.mPointY) / 16 ) / TILESIZE ;
	#endif
	this->adjustSourceBuffer();

	return AHMI_FUNC_SUCCESS;
}

#endif
