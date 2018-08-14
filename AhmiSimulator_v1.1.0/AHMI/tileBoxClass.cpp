////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     tileBoxClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
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

//������Χ��
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
// ��������  sourceBoxCopy
// ��������İ�Χ��
// �����б�
// @param1 TileBoxClassPtr   pSourceBox    ����İ�Χ��
// ��ע(�����汾֮����޸�):
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
// ��������  sourceBoxOr
// ��Χ��ȡ��ֻ�е�������Χ�а�����ͬ�����ʱ��Ż�ϲ�
// �����б�
// @param1 TileBoxClassPtr   pSourceBox    ����İ�Χ��
// ��ע(�����汾֮����޸�):
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
// ��������  sourceBoxOverlap
// �ж�������Χ���Ƿ����ཻ����,���򷵻�AHMI_FUNC_SUCCESS
// �����б�
// @param1 TileBoxClassPtr   pSourceBox    ����İ�Χ��
// ��ע(�����汾֮����޸�):
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
// ��������  sourceBoxContain
// �жϸð�Χ���Ƿ��������İ�Χ�У����򷵻�AHMI_FUNC_SUCCESS
// �����б�
// @param1 TileBoxClassPtr   pSourceBox    ����İ�Χ��
// ��ע(�����汾֮����޸�):
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
// ��������  sourceReCompute
// ���¼����Χ��,ֻ����ƽ�ƺͷ���
// widget��canvas�����վ����������
// �����б�
// @param1 matrixClassPtr   pMatrix    ����ľ���
// ��ע(�����汾֮����޸�):
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
		else if( ( (pWidget->WidgetAttr & 0x1f) == METER_TEX) && ((pWidget->WidgetAttr & METER_SIMPLIFY) == METER_SIMPLIFY ) )//�Ǳ��̵�������
		{
			texturePtr = &gPagePtr[WorkingPageID].pBasicTextureList[pWidget->StartNumofTex];
			//����ָ��
			this->LeftBox     = texturePtr[6].TexLeftTileBox   ;
			this->TopBox      = texturePtr[6].TexTopTileBox    ;
			this->RightBox    = texturePtr[6].TexRightTileBox  ;
			this->ButtomBox   = texturePtr[6].TexButtomTileBox ;
			flag = 1;
		}
		else if((pWidget->WidgetAttr & 0x1f) == OSCSCP_TEX)//��������ʾ����
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
			curX = (s32)( (s16)ptr.wptr->WidgetOffsetX - (s16)focusedCanvas->moffsetX) * 16;//���canvas��ƫ��
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
		//���������
		point1.mPointX = (long long)curX * 0x100000 / matrixPtr->A - (matrixPtr->E >> 9) + (focusedCanvas->moffsetX << 4); //���ƫ������Ҫ��������ϵ����Ȼ����Ͼ����д������ľ���ƫ����
		point1.mPointY = (long long)curY * 0x100000 / matrixPtr->A - (matrixPtr->F >> 9) + (focusedCanvas->moffsetY << 4); //ע�⣺widget�о�������ƽ�����Ǽ�ȥ��canvasԭʼλ�õõ�����

	}else{
	    point1.mPointX = curX - (matrixPtr->E >> 9); //canvas�����д�����ƫ������������ڵ�ǰδ���ŵ�canvas�����ƫ����������Ҫ�ٳ���������
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
