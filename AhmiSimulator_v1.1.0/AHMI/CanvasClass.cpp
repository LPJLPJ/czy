////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     CanvasClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(������)
// Additional Comments:
//    definition used only by widget
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicInclude.h"
#include "AHMIBasicStructure.h"
#include "AHMIBasicDefine.h"
#include "AHMIBasicDefine.h"
#include "publicDefine.h"
#ifdef AHMI_DEBUG
#include "trace.h"
#include "stdio.h"
#endif

#ifdef AHMI_CORE

extern DynamicPageClassPtr  gPagePtr;
extern u16           WorkingPageID;
//extern QueueHandle_t   RefreshQueue;
extern QueueHandle_t		ActionInstructionQueue;
extern AnimationClass  gAnimationClass;
extern u8 gPageNeedRefresh;

#ifdef PC_SIM
extern"C"
{
extern struct globalArgs_t globalArgs;
}
#endif

funcStatus CanvasClass::drawCanvas(
		TileBoxClassPtr pTileBox , //��Χ��
		u32 *u32p_sourceShift        , //sourcebufferָ��
		u8 pageEnable             ,//�Ƿ�ˢ������ҳ��
		matrixClassPtr pPageMatrix, //���ӵı任����
		SubCanvasClassPtr scptr   ,
		u8 RefreshType            , //ʹ��canvas�İ�Χ�дӶ����Կؼ��İ�Χ��
		u8 staticTextureEn          //�Ƿ���Ƶ���̬�洢�ռ�
		)
{
	TileBoxClass drawingCanvasTileBox;
	ElementPtr   curPtr;
	funcStatus stutas = AHMI_FUNC_SUCCESS;
	matrixClass curCanvasMatrix;
	matrixClass matrixTemp;
	if( NULL == pTileBox || NULL == u32p_sourceShift || NULL == pPageMatrix || NULL == scptr ){
		ERROR_PRINT("ERROR: NULL��pointer occcured! ");
		return AHMI_FUNC_FAILURE;
	}
	int NumofWidget = scptr->NumOfWidget;

	//computing tile box
	//scptr->mSubCanvasMatrix.matrixMulti(pPageMatrix);
	curCanvasMatrix.matrixInit();
	matrixTemp.matrixInit();
	//curCanvasMatrix.matrixMulti(pPageMatrix);
	curCanvasMatrix.matrixMulti( &(this->mCanvasMatrix) );
	curCanvasMatrix.matrixMulti(&(scptr->mSubCanvasMatrix));
	matrixTemp.matrixMulti(pPageMatrix);
	
	this->computeCanvasMatrix(pPageMatrix, &curCanvasMatrix);//�õ��ľ�����canvas�ľ��Ծ���
	//matrixTemp.E += (this->mCanvasMatrix.E + scptr->mSubCanvasMatrix.E); //���ڼ���canvas��ǰλ�õľ��󣬷�������page�ķ�������ƽ�����Ǽ�����ƽ����
	//matrixTemp.F += (this->mCanvasMatrix.F + scptr->mSubCanvasMatrix.F);
	

	curPtr.scptr = scptr;
	drawingCanvasTileBox.sourceReCompute(curPtr, ANIMATION_REFRESH_CANVAS, &(curCanvasMatrix) );

	//s8 drawingCanvasLeftBox   = (( (s16)(this->moffsetX)  )>>5);
	//s8 drawingCanvasRightBox  = (( ( (s16)(this->moffsetX + this->mwidth) )>>5) );
	//s8 drawingCanvasTopBox    = (( (s16) (this->moffsetY)   )>>5);
	//s8 drawingCanvasButtomBox = (( ( (s16)(this->moffsetY + this->mheight) )>>5) );
	
	//compute whether the source box is overlapped
	//if(drawingCanvasLeftBox < 0)
	//	drawingCanvasLeftBox = 0;
	//if(drawingCanvasTopBox < 0)
	//	drawingCanvasTopBox = 0;
	//if(drawingCanvasRightBox >  (s8)(TILE_NUM_X - 1) )
	//	drawingCanvasRightBox = (s8)(TILE_NUM_X - 1) ;
	//if(drawingCanvasButtomBox > (s8)(TILE_NUM_Y - 1) )
	//	drawingCanvasButtomBox = (s8)(TILE_NUM_Y - 1) ;
	//�жϰ�Χ��
	//if(
	//	pTileBox->LeftBox   > drawingCanvasRightBox     ||
	//	pTileBox->TopBox    > drawingCanvasButtomBox    ||
	//	pTileBox->RightBox  < drawingCanvasLeftBox	    ||
	//	pTileBox->ButtomBox < drawingCanvasTopBox
	//)
	if(drawingCanvasTileBox.sourceBoxOverlap(pTileBox) == AHMI_FUNC_FAILURE) //no overlap
		return AHMI_FUNC_SUCCESS;
	//дһ�����������пؼ�
	for (int WidgetCount = 0; WidgetCount < NumofWidget; WidgetCount++)
	{
		WidgetClassPtr wptr = &(gPagePtr[WorkingPageID].pWidgetList[ (scptr->StartAddrOfWidget + WidgetCount) ]);
		//�ж�widget�Ƿ���canvas�ڲ�
		if( 
			(s16)(wptr->WidgetOffsetX) > (s16)(this->moffsetX) + (this->mwidth) ||//�����ұ߽� 
			( (s16)(wptr->WidgetOffsetX) + (wptr->WidgetWidth) ) < (s16)(this->moffsetX) || //С����߽�
			(s16)(wptr->WidgetOffsetY) > (s16)(this->moffsetY + this->mheight)  || //�����±߽�
			( (s16)(wptr->WidgetOffsetY) + (wptr->WidgetHeight) ) < (s16)(this->moffsetY)
			)
			continue;//�����ڲ��Ļ�������
		//����widget����
		stutas = wptr->drawWidget(pTileBox, u32p_sourceShift, pageEnable, &(curCanvasMatrix), RefreshType, staticTextureEn);
		if(stutas == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}
	//if(RefreshType == ANIMATION_REFRESH_CANVAS || RefreshType == ANIMATION_REFRESH_PAGE)
	//	scptr->mSubCanvasMatrix.matrixInit();
	return AHMI_FUNC_SUCCESS;
}

funcStatus CanvasClass::focusNewSubCanvas(
		u8 subCanvasID
		)
{
	TileBoxClass canvasTileBox;
#if (defined PARTIAL_DOUBLE_BUFFER || defined PARTIAL_TRIBLE_BUFFER || defined STATIC_BUFFER_EN) 
	RefreshMsg refreshMsg;
#endif
	SubCanvasClassPtr nextSubCan;
	u8 widgetCount;
	WidgetClassPtr curWidgetPtr;
	u32 actionAddr;

#ifdef AHMI_DEBUG
	char text[100];
	sprintf(text,"loading subcanvas %d", subCanvasID);
	ERROR_PRINT(text);
#endif
	this->mFocusedCanvas = subCanvasID;
	nextSubCan = &(gPagePtr[WorkingPageID].pSubCanvasList[subCanvasID + this->mStartAddrOfSubCanvas]);
	nextSubCan->initSubCanvas();
	for(widgetCount = 0; widgetCount < nextSubCan->NumOfWidget; widgetCount++)
	{
		curWidgetPtr = &(gPagePtr[WorkingPageID].pWidgetList[ nextSubCan->StartAddrOfWidget + widgetCount ]);
		curWidgetPtr->initWidget();
	}
	 
	if(animationType == ANIMATION_REFRESH_NULL)
	{
#ifndef WHOLE_TRIBLE_BUFFER
		refreshMsg.mElementType = ANIMATION_REFRESH_CANVAS;
		refreshMsg.mElementPtr.cptr = this;
		sendToRefreshQueue(&refreshMsg);
#endif
#ifdef STATIC_BUFFER_EN
//		refreshMsg.mElementType = ANIMAITON_REFRESH_STATIC_BUFFER;
//		refreshMsg.mElementPtr.pageptr = gPagePtr + WorkingPageID;
//		sendToRefreshQueue(&refreshMsg);
#endif
		gPageNeedRefresh = 1;
	}
	else 
	{
		loadSubCanvasWithAnimation(nextSubCan, nextSubCan);
	}

	//����subcanvasָ��
	if(nextSubCan->startAddrOfSubCanAction < gPagePtr[WorkingPageID].mActionInstructionsSize)
	{
		actionAddr = (u32)(gPagePtr[WorkingPageID].pActionInstructions + nextSubCan->startAddrOfSubCanAction);
		xQueueSendToBack(ActionInstructionQueue,&actionAddr,portMAX_DELAY);
	}
	return AHMI_FUNC_SUCCESS;
}


//�����µ��ӻ�������ʾ����
funcStatus CanvasClass::loadSubCanvasWithAnimation(
	SubCanvasClassPtr pCurSubCanvas,
	SubCanvasClassPtr pNextSubCanvas
	)
{
	funcStatus AHMI_status;
	ElementPtr pOldElement, pNewElement;
	//u32 actionAddr;

	pOldElement.scptr = pCurSubCanvas;
	pNewElement.scptr = pNextSubCanvas;

	AHMI_status= gAnimationClass.Create_AnimatianData(
		ANIMATION_REFRESH_SUBCANVAS,
		pNewElement,
		pOldElement
		);

	if(AHMI_status == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;

	return AHMI_FUNC_SUCCESS;
}

funcStatus CanvasClass::drawCanvasWithAnimation(
		TileBoxClassPtr tileBox,
		u32* pSourceShift
		)
{
	matrixClass curCanvasMatrix;
	ElementPtr curPtr;

	//caculate the matrix
	curCanvasMatrix.matrixInit();
	curCanvasMatrix.matrixMulti(&gPagePtr[WorkingPageID].mPageMatrix);
	curCanvasMatrix.matrixMulti( &(this->mCanvasMatrix));
	curPtr.cptr = this;
	//caculte the source box
	tileBox->sourceReCompute(curPtr, ANIMATION_REFRESH_CANVAS, &curCanvasMatrix);

	//draw the page
	gPagePtr[WorkingPageID].drawPage(tileBox, pSourceShift, 0, ANIMATION_REFRESH_CANVAS,0);

	//matrixClassPtr pCurSubCanvasMatrix = &(pCurSubCanvas->mSubCanvasMatrix);
	//matrixClassPtr pNextSubCanvasMatrix = &(pNextSubCanvas->mSubCanvasMatrix);
	//SubCanvasClassPtr scptr = gPagePtr[WorkingPageID].pSubCanvasList + this->mStartAddrOfSubCanvas + this->mFocusedCanvas;
	//CanvasClassPtr drawingCanvas = gPagePtr[WorkingPageID].pCanvasList + pCurSubCanvas->attachCanvas;
//	WidgetClass curWidget;
//	u8 animationType = this->animationType;
//	u8 animationParam = this->animationParam;
//	u8 widgetCount;
//#ifdef DEBUG
//	char text[100];
//#endif // DEBUG
//
//	//sourcebox
//	if(gPagePtr[WorkingPageID].refreshWidgetBox(NULL, drawingCanvas, NULL,0, 0, 1,0, tileBox) == AHMI_FUNC_FAILURE)
//		return AHMI_FUNC_FAILURE;
//	if(tileBox->LeftBox   > (s8)(TILE_NUM_X-1) ) return AHMI_FUNC_SUCCESS;
//	if(tileBox->RightBox  > (s8)(TILE_NUM_X-1) ) tileBox->RightBox = TILE_NUM_X-1;
//	if(tileBox->TopBox    > (s8)(TILE_NUM_Y-1) ) return AHMI_FUNC_SUCCESS;
//	if(tileBox->ButtomBox > (s8)(TILE_NUM_Y-1) ) tileBox->ButtomBox = TILE_NUM_Y-1;
//	pageMatrixTemp.matrixInit();
	//����Ҫ���ƶ���
	//if(totalFrame == 0 || animationType == NO_ANIMATION)
	//{
	//	//д��widget��Ϣ��sourcebuffer
	//	pCurSubCanvasMatrix->matrixInit();
	//	pNextSubCanvasMatrix->matrixInit();
	//	for(widgetCount = 0; widgetCount < pNextSubCanvas->NumOfWidget; widgetCount++)
	//	{
	//		curWidget = gPagePtr[WorkingPageID].pWidgetList[ pNextSubCanvas->StartAddrOfWidget + widgetCount ];
	//		curWidget.initWidget();
	//	}
	//	gPagePtr[WorkingPageID].drawPage(tileBox, pSourceShift, 1, ANIMATION_REFRESH_NULL);
	//}
	////��Ҫ���ƶ���
	//else
	//{
	//	pCurSubCanvasMatrix->matrixInit();
	//	pNextSubCanvasMatrix->matrixInit();
	//	switch(animationType)
	//	{
	//		case(ANIMATION_HERIZONTAL_SHIFTING_L):
	//			//ˮƽ����ƽ�ƶ���
	//			curCanvasOffsetX = (s16)(this->moffsetX - (float)curFrame / (float) totalFrame * this->mwidth) << 4 ;
	//			curCanvasOffsetY = this->moffsetY;
	//			nextCanvasOffsetX = (s16)(this->moffsetX + this->mwidth - (float)curFrame / (float) totalFrame * this->mwidth) << 4;
	//			nextCanvasOffsetY = this->moffsetY;
	//			pCurSubCanvas->mSubCanvasMatrix.A = (1 << 9);
	//			pCurSubCanvas->mSubCanvasMatrix.B = (0 << 9);
	//			pCurSubCanvas->mSubCanvasMatrix.C = (0 << 9);
	//			pCurSubCanvas->mSubCanvasMatrix.D = (1 << 9);
	//			pCurSubCanvas->mSubCanvasMatrix.E = -curCanvasOffsetX;
	//			pCurSubCanvas->mSubCanvasMatrix.F = -curCanvasOffsetY;
	//			pNextSubCanvas->mSubCanvasMatrix.A = (1 << 9);
	//			pNextSubCanvas->mSubCanvasMatrix.B = (0 << 9);
	//			pNextSubCanvas->mSubCanvasMatrix.C = (0 << 9);
	//			pNextSubCanvas->mSubCanvasMatrix.D = (1 << 9);
	//			pNextSubCanvas->mSubCanvasMatrix.E = -nextCanvasOffsetX;
	//			pNextSubCanvas->mSubCanvasMatrix.F = -nextCanvasOffsetY;
	//
	//			#ifdef DEBUG
	//			sprintf(text,"Drawing canvas animation frame %d\n",curFrame);
	//			ERROR_PRINT(text);
	//			sprintf(text,"cur subcanvas offset %d, next subcanvas offset %d",(curCanvasOffsetX >> 4),(nextCanvasOffsetX >> 4) );
	//			ERROR_PRINT(text);
	//			#endif // DEBUG
	//
	//			//���û�ͼ����
	//			if(this->drawCanvas(tileBox, pSourceShift, 0, &pageMatrixTemp,pCurSubCanvas,ANIMATION_REFRESH_CANVAS) == AHMI_FUNC_FAILURE)
	//				return AHMI_FUNC_FAILURE;
	//			if(this->drawCanvas(tileBox, pSourceShift, 0, &pageMatrixTemp,pNextSubCanvas,ANIMATION_REFRESH_CANVAS) == AHMI_FUNC_FAILURE)
	//				return AHMI_FUNC_FAILURE;
	//			break;
	//
	//		case(ANIMATION_ZOOM_OUT):
	//			//���Ͻ����Ŷ���
	//			pNextSubCanvas->mSubCanvasMatrix.A = (s32)((1 << 9)-(animationParam-1)*(float)curFrame / (float) (totalFrame+1)*512);
	//			pNextSubCanvas->mSubCanvasMatrix.B = (0 << 9);
	//			pNextSubCanvas->mSubCanvasMatrix.C = (0 << 9);
	//			pNextSubCanvas->mSubCanvasMatrix.D = (s32)((1 << 9)-(animationParam-1)*(float)curFrame / (float) (totalFrame+1)*512);
	//			pNextSubCanvas->mSubCanvasMatrix.E = 0;
	//			pNextSubCanvas->mSubCanvasMatrix.F = 0;
	//			//���û�ͼ����
	//			if(this->drawCanvas(tileBox, pSourceShift, 0, &pageMatrixTemp,pNextSubCanvas, ANIMATION_REFRESH_CANVAS) == AHMI_FUNC_FAILURE)
	//				return AHMI_FUNC_FAILURE;
	//			break;
	//		default:
	//			return AHMI_FUNC_FAILURE;
	//	}
	//}
	return AHMI_FUNC_SUCCESS;
}

funcStatus CanvasClass::initCanvas(void)
{
	//todo
	this->curCustomAnimationPtr = 0xff; //indicating that there is no animation
	this->mCanvasMatrix.matrixInit();
	return AHMI_FUNC_SUCCESS;
}


//-----------------------------
// �������� setATag
// set the animation tag, compute the matrix
// canvas animation is the custom animation for canvas
// �����б�
//     @param1 u8 value
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus CanvasClass::setATag(u8 value)
{
	AnimationActionClassPtr curCanvasAnimation;
	s16 curCanvasOffsetX; //1.11.4
	s16 curCanvasOffsetY; //1.11.4
	s32 curWidth;     //1.21.10
	s32 curHeight;    //1.21.10
	s32 scalerX;     //1.6.9
	s32 scalerY;     //1.6.9
	u16 transitionParamIn; //0.10
	u16 transitionParamOut; //0.10
#if(defined PARTIAL_DOUBLE_BUFFER) || (defined STATIC_BUFFER_EN) || (defined PARTIAL_TRIBLE_BUFFER)
//	RefreshMsg refreshMsg;
#endif

taskENTER_CRITICAL();
#ifdef AHMI_DEBUG
	char text[100];
	sprintf(text,"set canvas aTag %d", value);
	ERROR_PRINT(text);
#endif
	
	curCanvasAnimation = &(gPagePtr[WorkingPageID].pAnimationActionList[this->curCustomAnimationPtr]);
	if(curCanvasAnimation == NULL || curCanvasAnimation->mTotalFrame == 0) //no animation
	{
		return AHMI_FUNC_FAILURE;
	}
	if(value == 0 || value > curCanvasAnimation->mTotalFrame)
	{
		ERROR_PRINT("ERROR in setting canvas animation tag, value out of baundary");
		return AHMI_FUNC_FAILURE;
	}
	this->mATag = value;
	//caculating the matrix
	//this->mCanvasMatrix.matrixInit();
	transitionParamIn = (mATag) * 1024 / curCanvasAnimation->mTotalFrame;//0.10
	gPagePtr[WorkingPageID].inoutTransitioin(transitionParamIn, transitionParamOut);
	curCanvasOffsetX = (curCanvasAnimation->mStartX * 16) + (s32)(transitionParamOut * (curCanvasAnimation->mStopX - curCanvasAnimation->mStartX) / 64 );
	curCanvasOffsetY = (curCanvasAnimation->mStartY * 16) + (s32)(transitionParamOut * (curCanvasAnimation->mStopY - curCanvasAnimation->mStartY) / 64 );
	//curWidth = (curCanvasAnimation->mWidthBeforeAnimation) + (transitionParamOut * (curCanvasAnimation->mWidthAfterAnimation - curCanvasAnimation->mWidthBeforeAnimation) >> 10);
	//curHeight = (curCanvasAnimation->mHeightBeforeAnimation) + (transitionParamOut * (curCanvasAnimation->mHeightAfterAnimation - curCanvasAnimation->mHeightBeforeAnimation) >> 10);
	//if(curWidth == 0 || curHeight == 0)
	//{
	//	ERROR_PRINT("something error in computing canvas animation");
	//	return AHMI_FUNC_FAILURE;
	//}
	//scalerX = (u32)( (curCanvasAnimation->mWidthBeforeAnimation) + (s32)(transitionParamOut * (curCanvasAnimation->mWidthAfterAnimation - curCanvasAnimation->mWidthBeforeAnimation) / 1024)    );
	//scalerY = (u32)( (curCanvasAnimation->mHeightBeforeAnimation) + (s32)(transitionParamOut * (curCanvasAnimation->mHeightAfterAnimation - curCanvasAnimation->mHeightBeforeAnimation) / 1024) );
	curWidth = (u32)( (curCanvasAnimation->mWidthBeforeAnimation) + (s32)(transitionParamOut * (curCanvasAnimation->mWidthAfterAnimation - curCanvasAnimation->mWidthBeforeAnimation) / 1024)    );
	curHeight = (u32)( (curCanvasAnimation->mHeightBeforeAnimation) + (s32)(transitionParamOut * (curCanvasAnimation->mHeightAfterAnimation - curCanvasAnimation->mHeightBeforeAnimation) / 1024));
	if(curWidth == 0)
		curWidth = 1;
	if(curHeight == 0)
		curHeight = 1;
	scalerX = ((1024 * 512 / curWidth ) << 11)  ; //trans into 1.6.9
	scalerY = ((1024 * 512 / curHeight ) << 11) ; //trans into 1.6.9
	if(scalerX > 0x100000)
		scalerX = 0xfffff; //the maxium
	else if(scalerX == 0)
		scalerX = 1;
	if(scalerY > 0x100000)
		scalerY = 0xfffff; //the maxium
	else if(scalerY == 0)
		scalerY = 1;
	this->mCanvasMatrix.A = scalerX;
	this->mCanvasMatrix.B = 0;
	this->mCanvasMatrix.C = 0;
	this->mCanvasMatrix.D = scalerY;
	this->mCanvasMatrix.E = -((curCanvasOffsetX - (this->moffsetX * 16) ) << 9);
	this->mCanvasMatrix.F = -((curCanvasOffsetY - (this->moffsetY * 16) ) << 9);
taskEXIT_CRITICAL();
#ifndef WHOLE_TRIBLE_BUFFER
	refreshMsg.mElementType = ANIMATION_REFRESH_CANVAS;
	refreshMsg.mElementPtr.cptr = this;
	sendToRefreshQueue(&refreshMsg);
#endif

	return AHMI_FUNC_SUCCESS;

}



//-----------------------------
// �������� triggerAnimation
//trigger canvas custom animation
// �����б�
//     @param1 u8 animationID
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus CanvasClass::triggerAnimation(u8 animationID)
{
	ElementPtr curPtr;
	u8 i;
	AnimationActionClassPtr curAnimationAction;
	curPtr.cptr = this;
	//judge whether the animation is available
	//if(animationID >= this->startAddrOfCanvasCustomAnimation && animationID < (this->startAddrOfCanvasCustomAnimation + this->numOfCanvasCustomAnimation) )
	for(i = this->startAddrOfCanvasCustomAnimation; i < this->startAddrOfCanvasCustomAnimation + this->numOfCanvasCustomAnimation;i++)
	{
		curAnimationAction = gPagePtr[WorkingPageID].pAnimationActionList + i;
		if(curAnimationAction->mAnimationID == animationID)
		{
			curCustomAnimationPtr = i;
			return gAnimationClass.Create_AnimatianData(ANIMATION_REFRESH_CANVAS, curPtr, curPtr);
		}
	}
	return AHMI_FUNC_FAILURE;
}



//-----------------------------
// �������� computeCanvasMatrix
//compute the matrix of canvas
// ��page�������ŵ�ʱ��canvas�����page��ƫ����Ӧ�ó���page������ϵ��
// ע�⣺���ǵľ����������
// ���գ�canvas��ƫ�����������moffset��ƫ����
// �����б�
//     @param1 u8 animationID
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus CanvasClass::computeCanvasMatrix(matrixClassPtr pPageMatrix, matrixClassPtr curCanvasMatrix)
{
	curCanvasMatrix->A = (s32)( ((long long)pPageMatrix->A * curCanvasMatrix->A + (long long)pPageMatrix -> B * curCanvasMatrix->C  ) / (1024*1024) );
	curCanvasMatrix->B = (s32)( ((long long)pPageMatrix->A * curCanvasMatrix->B + (long long)pPageMatrix -> B *curCanvasMatrix->D  ) / (1024*1024)  );
	curCanvasMatrix->C = (s32)( ((long long)pPageMatrix->C * curCanvasMatrix->A + (long long)pPageMatrix -> D *curCanvasMatrix->C  ) / (1024*1024)  );
	curCanvasMatrix->D = (s32)( ((long long)pPageMatrix->C * curCanvasMatrix->B + (long long)pPageMatrix -> D *curCanvasMatrix->D  ) / (1024*1024)  );
	curCanvasMatrix->E = pPageMatrix->E + ( (long long)(curCanvasMatrix->E - (this->moffsetX * 16) ) * 1024 * 1024 / pPageMatrix->A) + this->moffsetX * 16; //�ھ�������ϵ�£�canvas����ڱ����ƫ��
	curCanvasMatrix->F = pPageMatrix->F + ( (long long)(curCanvasMatrix->F - (this->moffsetY * 16) ) * 1024 * 1024 / pPageMatrix->A) + this->moffsetY * 16; 
	return AHMI_FUNC_SUCCESS;
}


#endif
