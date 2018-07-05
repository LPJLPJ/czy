////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2016/03/21
// File Name:     DynamicPageClass.cpp
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
#include "publicDefine.h"
#include "myMathClass.h"
#include "drawImmediately_cd.h"
#ifdef AHMI_DEBUG
#include "trace.h"
#endif
#ifdef PC_SIM
#include <stdio.h>
#endif

#include "animationDefine.h"
#ifdef EMBEDDED
#include "spi_if.h"
#endif

#ifdef AHMI_CORE

#ifdef PC_SIM
extern FILE *fp;
#endif
extern ConfigInfoClass ConfigData;
extern QueueHandle_t   RefreshQueue;
extern QueueHandle_t   RefreshQueueWithoutDoubleBuffer;
extern AnimationClass  gAnimationClass;

extern u16             WorkingPageID;
extern DynamicPageClassPtr  gPagePtr;
extern void*           PageSpace;
extern TagClassPtr     TagPtr;

//extern QueueHandle_t   RefreshQueue;
extern QueueHandle_t   ActionInstructionQueue;
extern TagClassPtr     TagPtr;
extern u8 gPageNeedRefresh;

extern "C"
{
extern struct globalArgs_t globalArgs;
}


DynamicPageClass::DynamicPageClass(void)
{
}


DynamicPageClass::~DynamicPageClass(void)
{
}

funcStatus DynamicPageClass::InitPage(
		void*          BassAddress             , //��Ŷ�̬���ݵĻ���ַ
		u32            StoreOffset            , //����ƫ�� 
		u16            PageAttr               , //page����
		u16            NumOfWidget            , //widget������
		u8 			   NumOfCanvas			   , //Canvas����
		u8             NumOfSubCanvas         , //subCanvas����
		u16            ActionInstructionsSize , //��ָ���
		u16            TotalNumOfTexture	   , //��������
		u16            NumofTouchWidget       , //�ɴ�����������
		u16            NumOfPageAction        , //Action����
		u16            NumOfCanvasAction	,	 //CanvasAction����
		u8             animationType        ,    //�л���������
		u8             curEnlightedWidgetID ,    //��ǰ������widget
		u8             animationFrame       ,    //����֡��
		u8             numOfAnimationActions     //�û��Զ��嶯����Ŀ
	)
{
			
		//u8 i;
		this->mStoreOffset            = StoreOffset           ;
		this->mPageAttr               = PageAttr             ;
		this->mNumOfCanvas			  = NumOfCanvas			;
		this->pCanvasList             = (CanvasClass*)BassAddress     ;
		this->mNumOfSubCanvas         = NumOfSubCanvas        ;
		this->pSubCanvasList          = (SubCanvasClassPtr)(this->pCanvasList + this->mNumOfCanvas);
		this->mNumOfWidget            = NumOfWidget           ;
		this->pWidgetList			  = (WidgetClass *)(this->pSubCanvasList + this->mNumOfSubCanvas);
		this->mActionInstructionsSize =  ActionInstructionsSize;
		this->pAnimationActionList    = (AnimationActionClassPtr)(this->pWidgetList + this->mNumOfWidget);
		this->mNumOfAnimationActions  = numOfAnimationActions;
		this->pActionInstructions     = (u8*)(this->pAnimationActionList + this->mNumOfAnimationActions);
		this->mTotalNumOfTexture      = TotalNumOfTexture  ;
		this->pBasicTextureList       = (TextureClass*)(this->pActionInstructions + this->mActionInstructionsSize);
		this->mNumofTouchWidget       = NumofTouchWidget;
		this->pTouchWidgetList        = (touchWidgetRange*)(this->pBasicTextureList + this->mTotalNumOfTexture);
		this->mNumOfPageAction        = NumOfPageAction;
		this->pActionStartADDR        = (u8*)(this->pTouchWidgetList + this->mNumofTouchWidget);
		this->mNumOfCanvasAction		= NumOfCanvasAction;
		this->pCanvasActionStartADDR	= (u8*)(this->pActionStartADDR+this->mNumOfPageAction);
		this->pBackgroundTexture      = (this->pBasicTextureList);	
		this->pMouseTexture           = this->pBasicTextureList + 1;
		this->pMouseWidget            = (this->pWidgetList) +1 ;
		this->mTotalSize              = (u32)(this->pActionStartADDR + this->mNumOfPageAction+ this->mNumOfCanvasAction) - (u32)BassAddress;
		this->mAnimationType          = animationType;
		this->curEnlightedWidgetID    = curEnlightedWidgetID;
		this->mtotoalFrame            = animationFrame;
		this->curEnlightedWidgetLighted = 0;
		this->mATag = 0;
		this->mPageMatrix.matrixInit();

		
		return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// �������� loadPage
// ��ʼ����Ա����
// �����б�
//   @param1 u8 page ҳ��
//   @param2 u8 systemInitial ��ʾϵͳ���ʼ��ҳ�棬���ڸ����ؼ�
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus DynamicPageClass::loadPage(u8 page, u8 systemInitial)
{
	u32 baseaddr;
	u32 totalsize;
#ifdef PC_SIM
	size_t count;
#endif
	RefreshMsg refreshMsg;
	funcStatus AHMI_status;
	ElementPtr  pNewElement;
	int i;
	u32 addr;
	//WidgetClassPtr canvasWidgetList=gPagePtr[WorkingPageID].pWidgetList;
	//touchWidgetRange* canvasTouchWidgetList=gPagePtr[WorkingPageID].pTouchWidgetList;
//	u8* canvasActionStartAddr=gPagePtr[WorkingPageID].pActionStartADDR+gPagePtr[WorkingPageID].mNumOfPageAction;

	baseaddr = gPagePtr[page].mStoreOffset;
	totalsize = gPagePtr[page].mTotalSize;
#ifdef PC_SIM
	fseek(fp,baseaddr,SEEK_SET);

	if (totalsize >ConfigData.DynamicPageSize)  
	{
#ifdef AHMI_DEBUG
		ERROR_PRINT("DynamicPageSize too small.\r\n");
#endif
		return AHMI_FUNC_FAILURE;
	}

	count = fread_s(PageSpace,totalsize,1,totalsize,fp);
	if(count!=totalsize)
	{
#ifdef AHMI_DEBUG
		ERROR_PRINT("There is no enough data to load page.\r\n");
#endif
		return AHMI_FUNC_FAILURE;
	}
#endif
#ifdef EMBEDDED
	SeekSTMFlashAddr(baseaddr);
	
	if (totalsize >ConfigData.DynamicPageSize) 
	{
#ifdef AHMI_DEBUG
		ERROR_PRINT("DynamicPageSize too small.\r\n");
#endif
		return AHMI_FUNC_FAILURE;
	}
	ReadDataFromSTMFlash(PageSpace,totalsize);
#endif

	WorkingPageID = page;
	pNewElement.pageptr = gPagePtr + WorkingPageID;

	//��ʼ��canvas��widget����
	for(i = 0; i < gPagePtr[WorkingPageID].mNumOfCanvas; i++)
	{
		gPagePtr[WorkingPageID].pCanvasList[i].initCanvas();
	}
	for(i = 0; i < gPagePtr[WorkingPageID].mNumOfSubCanvas; i++)
	{
		gPagePtr[WorkingPageID].pSubCanvasList[i].initSubCanvas();
	}
	for(i = 0; i < gPagePtr[WorkingPageID].mNumOfWidget; i++)
	{
		gPagePtr[WorkingPageID].pWidgetList[i].initWidget();
	}
	initHighlightWidget(systemInitial);

//#ifndef	WHOLE_TRIBLE_BUFFER
	gPageNeedRefresh = 1;
	if(gPagePtr[WorkingPageID].mAnimationType == NO_ANIMATION) //loading page without animation
	{
#ifdef STATIC_BUFFER_EN
		//�ȷ���һ��ˢ�¾�̬�ռ�
//		refreshMsg.mElementPtr.pageptr = &gPagePtr[WorkingPageID];
//		refreshMsg.mElementType = ANIMAITON_REFRESH_STATIC_BUFFER;
//		sendToRefreshQueue(&refreshMsg);
#endif
#if (defined PARTIAL_DOUBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
		refreshMsg.mElementPtr.pageptr = &gPagePtr[WorkingPageID];
		refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
		sendToRefreshQueue(&refreshMsg);
#endif
	}
	else 
	{
#ifdef STATIC_BUFFER_EN
		//�ȷ���һ��ˢ�¾�̬�ռ�
//		refreshMsg.mElementPtr.pageptr = &gPagePtr[WorkingPageID];
//		refreshMsg.mElementType = ANIMAITON_REFRESH_STATIC_BUFFER;
//		sendToRefreshQueue(&refreshMsg);
#endif
		AHMI_status= sendToAnimationQueue(
			ANIMATION_REFRESH_PAGE,
			pNewElement,
			pNewElement
			);
		if(AHMI_status == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}
	
//#endif
	for(i=0;i != gPagePtr[page].mNumOfPageAction; i++)
	{
		addr = (u32)(gPagePtr[page].pActionStartADDR[i] + gPagePtr[page].pActionInstructions);
		xQueueSendToBack(ActionInstructionQueue,&addr,portMAX_DELAY);
	}
	

	

	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// �������� loadPageWithAnimation
// ��ʼ����Ա����
// �����б�
//     @param1 DynamicPageClassPtr pCurPage  , //��ǰҳ��ָ��
//     @param2 DynamicPageClassPtr pNextPage , //��һҳ��ָ��
//     @param3 u8 curFrame                   , //��ǰ֡
//     @param4 u8 totalFrame                 , //�ܹ�֡
//     @param5 u8 animationType                //��������
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus DynamicPageClass::loadPageWithAnimation(
		u8 curPageID, //��ǰҳ��
		u8 nextPageID, //��һҳ��
        u8 curFrame                   , //��ǰ֡
		u8 totalFrame                 , //�ܹ�֡
		u8 animationType                //��������
		)
{
#ifdef PC_SIM
	ElementPtr pOldElement, pNewElement;
	funcStatus AHMI_status;
//	u8 i;
	u32 baseaddr;
	u16 totalsize;
	size_t count;
//	u32 addr;
	WidgetClassPtr canvasWidgetList=gPagePtr[WorkingPageID].pWidgetList;
	touchWidgetRange* canvasTouchWidgetList=gPagePtr[WorkingPageID].pTouchWidgetList;
	u8* canvasActionStartAddr=gPagePtr[WorkingPageID].pActionStartADDR+gPagePtr[WorkingPageID].mNumOfPageAction;
	u16 i;

	pOldElement.pageptr = gPagePtr+curPageID;
	pNewElement.pageptr = gPagePtr+nextPageID;

	if(totalFrame == 0)
		return AHMI_FUNC_FAILURE;

	baseaddr = (gPagePtr+nextPageID)->mStoreOffset;
	totalsize = (gPagePtr+nextPageID)->mTotalSize;
#ifdef PC_SIM
	fseek(fp,baseaddr,SEEK_SET);
#endif

	if (totalsize >ConfigData.DynamicPageSize) 
	{
#ifdef AHMI_DEBUG
		ERROR_PRINT("DynamicPageSize too small.\r\n");
#endif
		return AHMI_FUNC_FAILURE;
	}

#ifdef PC_SIM
	count = fread_s(PageSpace,totalsize,1,totalsize,fp);
	if(count!=totalsize)
	{
#ifdef AHMI_DEBUG
		ERROR_PRINT("There is no enough data to load page.\r\n");
#endif
		return AHMI_FUNC_FAILURE;
	}	
#endif

#ifdef EMBEDDED
	SeekSTMFlashAddr(baseaddr);

	if (totalsize >ConfigData.DynamicPageSize) 
	{
#ifdef AHMI_DEBUG
		ERROR_PRINT("DynamicPageSize too small.\r\n");
#endif
		return AHMI_FUNC_FAILURE;
	}
	ReadDataFromSTMFlash(PageSpace,totalsize);
#endif

	WorkingPageID = nextPageID;
	//��ʼ��canvas��widget����
	for(i = 0; i < this->mNumOfCanvas; i++)
	{
		gPagePtr[WorkingPageID].pCanvasList[i].initCanvas();
	}
	for(i = 0; i < this->mNumOfSubCanvas; i++)
	{
		gPagePtr[WorkingPageID].pSubCanvasList[i].initSubCanvas();
	}
	for(i = 0; i < this->mNumOfWidget; i++)
	{
		gPagePtr[WorkingPageID].pWidgetList[i].initWidget();
	}
	initHighlightWidget(1);

	gPageNeedRefresh = 1;
	AHMI_status= sendToAnimationQueue(
		ANIMATION_REFRESH_PAGE,
		pNewElement,
		pOldElement
		);
	if(AHMI_status == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;
	//send the action after animation
	//for(i=0;i != gPagePtr[nextPageID].mNumOfPageAction; i++)
	//{
	//	addr = (u32)(gPagePtr[nextPageID].pActionStartADDR[i] + gPagePtr[nextPageID].pActionInstructions);
	//	xQueueSendToBack(ActionInstructionQueue,&addr,portMAX_DELAY);
	//}
	return AHMI_FUNC_SUCCESS;
#endif
	
#ifdef EMBEDDED
	ElementPtr pOldElement, pNewElement;
	funcStatus AHMI_status;
	u8 i;
	//??????????????????
	u32 baseaddr;
	u16 totalsize;
	u32 addr;
//	WidgetClassPtr canvasWidgetList=gPagePtr[WorkingPageID].pWidgetList;
//	touchWidgetRange* canvasTouchWidgetList=gPagePtr[WorkingPageID].pTouchWidgetList;
//	u8* canvasActionStartAddr=gPagePtr[WorkingPageID].pActionStartADDR+gPagePtr[WorkingPageID].mNumOfPageAction;

	pOldElement.pageptr = gPagePtr+curPageID;
	pNewElement.pageptr = gPagePtr+nextPageID;

	if(totalFrame == 0)
		return AHMI_FUNC_FAILURE;

	baseaddr = (gPagePtr+nextPageID)->mStoreOffset;
	totalsize = (gPagePtr+nextPageID)->mTotalSize;
	
	SeekSTMFlashAddr(baseaddr);

	if (totalsize >ConfigData.DynamicPageSize) 
	{
#ifdef AHMI_DEBUG
		ERROR_PRINT("DynamicPageSize too small.\r\n");
#endif
		return AHMI_FUNC_FAILURE;
	}

	ReadDataFromSTMFlash(PageSpace,totalsize);

	WorkingPageID = nextPageID;

	//initital canvas and widget
	for(i = 0; i < gPagePtr[WorkingPageID].mNumOfCanvas; i++)
	{
		gPagePtr[WorkingPageID].pCanvasList[i].initCanvas();
	}
	for(i = 0; i < gPagePtr[WorkingPageID].mNumOfSubCanvas; i++)
	{
		gPagePtr[WorkingPageID].pSubCanvasList[i].initSubCanvas();
	}
	for(i = 0; i < gPagePtr[WorkingPageID].mNumOfWidget; i++)
	{
		gPagePtr[WorkingPageID].pWidgetList[i].initWidget();
	}
	initHighlightWidget(1);


	AHMI_status= sendToAnimationQueue(
		ANIMATION_REFRESH_PAGE,
		pNewElement,
		pOldElement
		);
	if(AHMI_status == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;
	for(i=0;i != gPagePtr[nextPageID].mNumOfPageAction; i++)
	{
		addr = (u32)(gPagePtr[nextPageID].pActionStartADDR[i] + gPagePtr[nextPageID].pActionInstructions);
		xQueueSendToBack(ActionInstructionQueue,&addr,portMAX_DELAY);
	}
	return AHMI_FUNC_SUCCESS;
#endif

}

//����ҳ��
funcStatus DynamicPageClass::drawPage(
	TileBoxClassPtr tileBox,
	u32 *sourceShift       ,
	u8  pageEnable         ,
	u8 RefreshType        ,//��������
    u8 staticTextureEn        //�Ƿ���Ƶ���̬�洢�ռ�
	)
{
	funcStatus status;
	CanvasClassPtr cptr;
	SubCanvasClassPtr scptr;
		
	TagClassPtr bindTag;
//	u32 TagValueTmp = 0;
	if(tileBox == NULL || sourceShift == NULL)
	{
		ERROR_PRINT("ERROR in drawPage: input tilebox or sourceshift donnot exist");
		return AHMI_FUNC_FAILURE;
	}
	int numOfCanvas = this->mNumOfCanvas;
	for(int CanvasCount = 0; CanvasCount < numOfCanvas; CanvasCount ++)
	{
		if(this->pCanvasList == NULL)
		{
			ERROR_PRINT("ERROR in drawPage: canvas list data corruption");
			return AHMI_FUNC_FAILURE;
		}
		cptr = (CanvasClassPtr)(this->pCanvasList) + CanvasCount;
		if(cptr->bindTagID < ConfigData.NumofTags)
			bindTag = &TagPtr[cptr->bindTagID];
		else 
		{
			ERROR_PRINT("ERROR in drawPage: canvas tag num out of baundary");
			return AHMI_FUNC_FAILURE;
		}
		if(cptr->bindTagID == 0) 
		  //�ж��Ƿ�Ϊ0��tag�������0��tag����tagtmp=-0
			cptr->mFocusedCanvas = 0;
		else
			cptr->mFocusedCanvas = bindTag->mValue;
		//�ж�tag��ֵ��û�г����ӻ�������������������Ļ��͵����ӻ���������ֵ
		if(cptr->mFocusedCanvas >= cptr->mNumOfSubCanvas) 
			cptr->mFocusedCanvas = cptr->mNumOfSubCanvas - 1;
		if(this->pSubCanvasList == NULL)
		{
			ERROR_PRINT("ERROR in drawPage: subcanvas list data corruption");
			return AHMI_FUNC_FAILURE;
		}
		scptr = (SubCanvasClassPtr)(this->pSubCanvasList) + cptr->mStartAddrOfSubCanvas + cptr->mFocusedCanvas;
		//����canvas����
		status = cptr->drawCanvas(tileBox, sourceShift, pageEnable, &(this->mPageMatrix), scptr, RefreshType, staticTextureEn);
		if(status == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}
	//if(RefreshType == ANIMATION_REFRESH_PAGE)
	//	this->mPageMatrix.matrixInit();
	return AHMI_FUNC_SUCCESS;
}

	//���ƶ���ҳ��
funcStatus DynamicPageClass::drawPageWithAnimation(
	DynamicPageClass* pCurPage    , //��ǰҳ��ָ��
	TileBoxClassPtr tileBox       ,  //��Χ��
	u32* pSourceShift               ,  //sourcebufferƫ��
    u8  staticTextureEn                //�Ƿ���µ���̬������
	)
{
	//s16 curPageOffsetX ; //1.11.4
	//s16 curPageOffsetY ; //1.11.4
	//s16 nextPageOffsetX; //1.11.4
	//s16 nextPageOffsetY; //1.11.4
	//float f_zoomScaler;
	//s16 nextPageZoom   ; //���ű�����1.7.9
	TextureClassPtr  pCurrentTexture;
	//TextureClassPtr  pNextTexture;
	//TileBoxClass tileBoxTemp;  //�������浱ǰ��tilebox
	//myMathClass myMathTemp;   //������ѧ����
	u8 animationType = this->mAnimationType;
	ElementPtr curElementPtr;
	
	//sourcebox
	//pCurPage->refreshWidgetBox(NULL, NULL, NULL, 0, 0, 0, 0, tileBox);
	//pCurPage->refreshWidgetBox(NULL, NULL, NULL, 0, 0, 0, 0, &tileBoxTemp);
	if(tileBox->LeftBox > (s8)(TILE_NUM_X-1)) return AHMI_FUNC_SUCCESS;
	if(tileBox->RightBox > (s8)(TILE_NUM_X-1)) tileBox->RightBox = TILE_NUM_X-1;
	if(tileBox->TopBox > (s8)(TILE_NUM_Y-1)) return AHMI_FUNC_SUCCESS;
	if(tileBox->ButtomBox > (s8)(TILE_NUM_Y-1)) tileBox->ButtomBox = TILE_NUM_Y-1;
	curElementPtr.pageptr = pCurPage;
	//tileBox->sourceReCompute(curElementPtr, ANIMATION_REFRESH_PAGE, &(pCurPage->mPageMatrix) );
	//����Ҫ���ƶ���
	//if(totalFrame == 0 || animationType == NO_ANIMATION)
	//{
		//д��ڵ�һҳ�ı�������
		//pCurPage->mPageMatrix.matrixInit();
		pCurrentTexture = pCurPage->pBackgroundTexture;
		//pCurrentTexture->copyBox(tileBox);
		pCurrentTexture->writeSourceBuffer( pSourceShift, &(pCurPage->mPageMatrix), NULL,NULL);
		//д��widget��Ϣ��sourcebuffer
		drawPage(tileBox,pSourceShift,1,ANIMATION_REFRESH_PAGE,staticTextureEn);
	//}
	////��Ҫ���ƶ���
	//else
	//{
//		pCurPage->mPageMatrix.matrixInit();
//		pNextPage->mPageMatrix.matrixInit();
//		switch(animationType)
//		{
//			case(ANIMATION_HERIZONTAL_SHIFTING_L):
//				//ˮƽƽ�ƶ���
//#ifdef PC_SIM
//				curPageOffsetX = (s16)(0 - (float)curFrame / (float) totalFrame * globalArgs.ScreenWidth) << 4;
//				curPageOffsetY = 0;
//				nextPageOffsetX = (s16)(globalArgs.ScreenWidth - (float)curFrame / (float) totalFrame * globalArgs.ScreenWidth) << 4;
//				nextPageOffsetY = 0;
//#endif
//#ifdef EMBEDDED
//				curPageOffsetX = (s16)(0 - (float)curFrame / (float) totalFrame * SCREEN_WIDTH) << 4;
//				curPageOffsetY = 0;
//				nextPageOffsetX = (s16)(SCREEN_WIDTH - (float)curFrame / (float) totalFrame * SCREEN_WIDTH) << 4;
//				nextPageOffsetY = 0;
//#endif
//				pCurPage->mPageMatrix.A = (1 << 9);
//				pCurPage->mPageMatrix.B = (0 << 9);
//				pCurPage->mPageMatrix.C = (0 << 9);
//				pCurPage->mPageMatrix.D = (1 << 9);
//				pCurPage->mPageMatrix.E = -curPageOffsetX;
//				pCurPage->mPageMatrix.F = -curPageOffsetY;
//				pNextPage->mPageMatrix.A = (1 << 9);
//				pNextPage->mPageMatrix.B = (0 << 9);
//				pNextPage->mPageMatrix.C = (0 << 9);
//				pNextPage->mPageMatrix.D = (1 << 9);
//				pNextPage->mPageMatrix.E = -nextPageOffsetX;
//				pNextPage->mPageMatrix.F = -nextPageOffsetY;
//				
//				//�����Χ��
//				if(refreshWidgetBox(NULL, NULL, NULL,0,0,0,0,tileBox) == AHMI_FUNC_FAILURE)
//					return AHMI_FUNC_FAILURE;
//				tileBox->LeftBox = ( (nextPageOffsetX >> 4) + TILESIZE - 1) / TILESIZE;
//				tileBox->TopBox  = ( (nextPageOffsetY >> 4) + TILESIZE - 1) / TILESIZE;
//				//д��ڶ�����ҳ��ı�������
//				pNextTexture = pNextPage->pBackgroundTexture;
//				pNextTexture ->writeSourceBuffer(pSourceShift, &(pNextPage->mPageMatrix), 1, tileBox);
//				//���û�ͼ����
//				if(pNextPage->drawPage(&tileBoxTemp, pSourceShift, 1, ANIMATION_REFRESH_PAGE) == AHMI_FUNC_FAILURE) //��������ҳ��İ�Χ�У���Ϊ���ƶ�����ʱ��ÿ��Ԫ������Ҫ����
//					return AHMI_FUNC_FAILURE;
//				break;
//				//ˮƽƽ�ƶ�������
//
//				case(ANIMATION_VERTICAL_SHIFTING_DOWN):
//				//��ֱƽ�ƶ���
//#ifdef PC_SIM
//				curPageOffsetX = 0;
//				curPageOffsetY = (s16)(0 - (float)curFrame / (float) totalFrame * globalArgs.ScreenHeigth) << 4;
//				nextPageOffsetX = 0;
//				nextPageOffsetY = (s16)(globalArgs.ScreenHeigth - (float)curFrame / (float) totalFrame * globalArgs.ScreenHeigth) << 4;
//#endif
//#ifdef EMBEDDED
//				curPageOffsetX = 0;
//				curPageOffsetY = (s16)(0 - (float)curFrame / (float) totalFrame * SCREEN_HEIGHT) << 4;
//				nextPageOffsetX = 0;
//				nextPageOffsetY = (s16)(SCREEN_HEIGHT - (float)curFrame / (float) totalFrame * SCREEN_HEIGHT) << 4;
//#endif
//				pCurPage->mPageMatrix.A = (1 << 9);
//				pCurPage->mPageMatrix.B = (0 << 9);
//				pCurPage->mPageMatrix.C = (0 << 9);
//				pCurPage->mPageMatrix.D = (1 << 9);
//				pCurPage->mPageMatrix.E = -curPageOffsetX;
//				pCurPage->mPageMatrix.F = -curPageOffsetY;
//				pNextPage->mPageMatrix.A = (1 << 9);
//				pNextPage->mPageMatrix.B = (0 << 9);
//				pNextPage->mPageMatrix.C = (0 << 9);
//				pNextPage->mPageMatrix.D = (1 << 9);
//				pNextPage->mPageMatrix.E = -nextPageOffsetX;
//				pNextPage->mPageMatrix.F = -nextPageOffsetY;
//				
//				//�����Χ��
//				if(refreshWidgetBox(NULL,NULL,NULL,0,0,0,0,tileBox) == AHMI_FUNC_FAILURE)
//					return AHMI_FUNC_FAILURE;
//				tileBox->LeftBox = ( (nextPageOffsetX >> 4) + TILESIZE - 1) / TILESIZE;
//				tileBox->TopBox = ( (nextPageOffsetY >> 4) + TILESIZE - 1) / TILESIZE;
//				//д��ڶ�����ҳ��ı�������
//				pNextTexture = pNextPage->pBackgroundTexture;
//				pNextTexture ->writeSourceBuffer(pSourceShift, &(pNextPage->mPageMatrix), 1, tileBox);
//				//���û�ͼ����
//				if(pNextPage->drawPage(&tileBoxTemp, pSourceShift, 1, ANIMATION_REFRESH_PAGE) == AHMI_FUNC_FAILURE) //��������ҳ��İ�Χ�У���Ϊ���ƶ�����ʱ��ÿ��Ԫ������Ҫ����
//					return AHMI_FUNC_FAILURE;
//				break;
//				//��ֱƽ�ƶ�������
//
//				case(ANIMATION_ZOOM_OUT):
//				//���Ŷ���
//				f_zoomScaler = (float) curFrame / (float) totalFrame; //�������ű���
//				myMathTemp.transferFloatToS16(f_zoomScaler, &nextPageZoom); //ת��Ϊ1.6.9��ʽ
//				pNextPage->mPageMatrix.matrixScaler(nextPageZoom,nextPageZoom); //�������ź����
//				//�����Χ��
//				if(refreshWidgetBox(NULL,NULL,NULL,0,0,0,0,tileBox) == AHMI_FUNC_FAILURE)
//					return AHMI_FUNC_FAILURE;
//				//д��ڶ�����ҳ��ı�������
//				pNextTexture = pNextPage->pBackgroundTexture;
//				pNextTexture ->writeSourceBuffer(pSourceShift, &(pNextPage->mPageMatrix), 1, tileBox);
//				//���û�ͼ����
//				if(pNextPage->drawPage(&tileBoxTemp, pSourceShift, 1, ANIMATION_REFRESH_PAGE) == AHMI_FUNC_FAILURE) //��������ҳ��İ�Χ�У���Ϊ���ƶ�����ʱ��ÿ��Ԫ������Ҫ����
//					return AHMI_FUNC_FAILURE;
//				break;
//				//���Ŷ�������
//
//				//�������
//			default:
//#ifdef DEBUG
//				ERROR_PRINT("This type of animation doesn't exist.\r\n");
//#endif
//				return AHMI_FUNC_FAILURE;
//		}
//	}
	return AHMI_FUNC_SUCCESS;
}

//���°�Χ��
funcStatus DynamicPageClass::refreshWidgetBox(
    WidgetClassPtr pWidget,
	CanvasClassPtr pCanvas,
	TextureClassPtr pTexture,
	u8 textureEnable,
	u8 widgetEnable,
	u8 canvasEnable,
	u8 doubleBufferEnable,
	TileBoxClassPtr pTileBox
)
{
//	CanvasClassPtr pCanvasTemp;
	TextureClassPtr texturePtr;
	if(textureEnable)//texture������ʱ��ֻ����widget
	{
		pTileBox->LeftBox = (pWidget->WidgetOffsetX  )>>5;
		pTileBox->TopBox = (pWidget->WidgetOffsetY   )>>5;
		pTileBox->RightBox = ((pWidget->WidgetOffsetX + pWidget->WidgetWidth)>>5) ;
		pTileBox->ButtomBox = ((pWidget->WidgetOffsetY + pWidget->WidgetHeight )>>5) ;
		return AHMI_FUNC_SUCCESS;
	}
	if(widgetEnable)//ֻ����һ��widget
	{
//		if(pWidget->ANIMATION_TYPE != NO_ANIMATION)
		//		{
		//			pCanvasTemp = gPagePtr[WorkingPageID].pCanvasList + pWidget->ATTATCH_CANVAS;
		//			pTileBox->LeftBox   = (pCanvasTemp->moffsetX  )>>5;
		//			pTileBox->TopBox    = (pCanvasTemp->moffsetY   )>>5;
		//			pTileBox->RightBox  = ((pCanvasTemp->moffsetX + pCanvasTemp->mwidth)>>5) ;
		//			pTileBox->ButtomBox = ((pCanvasTemp->moffsetY + pCanvasTemp->mheight)>>5) ;	
		//			return AHMI_FUNC_SUCCESS;
		//		}
		if((pWidget->WidgetAttr & 0x1f) == DYNAMIC_TEX && ((pWidget->WidgetAttr & DYNAMIC_TYPE_BIT) >> 5) == CENTRAL_ROTATE) //��ת����
		{
			texturePtr = &gPagePtr[WorkingPageID].pBasicTextureList[pWidget->StartNumofTex];
			pTileBox->LeftBox     = texturePtr->TexLeftTileBox   ;
			pTileBox->TopBox      = texturePtr->TexTopTileBox    ;
			pTileBox->RightBox    = texturePtr->TexRightTileBox  ;
			pTileBox->ButtomBox   = texturePtr->TexButtomTileBox ;
			
		}
		else if( ( (pWidget->WidgetAttr & 0x1f) == METER_TEX) && ((pWidget->WidgetAttr & METER_SIMPLIFY) == METER_SIMPLIFY ) )//�Ǳ��̵�������
		{
			texturePtr = &gPagePtr[WorkingPageID].pBasicTextureList[pWidget->StartNumofTex];
			//����ָ��
			pTileBox->LeftBox     = texturePtr[6].TexLeftTileBox   ;
			pTileBox->TopBox      = texturePtr[6].TexTopTileBox    ;
			pTileBox->RightBox    = texturePtr[6].TexRightTileBox  ;
			pTileBox->ButtomBox   = texturePtr[6].TexButtomTileBox ;
		}
		else if((pWidget->WidgetAttr & 0x1f) != OSCSCP_TEX)
		{
			pTileBox->LeftBox = (pWidget->WidgetOffsetX  )>>5;
			pTileBox->TopBox = (pWidget->WidgetOffsetY   )>>5;
			pTileBox->RightBox = ((pWidget->WidgetOffsetX + pWidget->WidgetWidth)>>5) ;
			pTileBox->ButtomBox = ((pWidget->WidgetOffsetY + pWidget->WidgetHeight)>>5) ;
			return AHMI_FUNC_SUCCESS;
		}
		else//��������ʾ����
		{
			return RefreshOSCWidgetBox(pWidget,pTileBox);
		}
		
	}
	else if(canvasEnable)//ֻ����һ��canvas
	{
		pTileBox->LeftBox = (pCanvas->moffsetX  )>>5;
		pTileBox->TopBox = (pCanvas->moffsetY   )>>5;
		pTileBox->RightBox = ((pCanvas->moffsetX + pCanvas->mwidth + TILESIZE - 1 )>>5) - 1 ;
		pTileBox->ButtomBox = ((pCanvas->moffsetY + pCanvas->mheight + TILESIZE - 1)>>5) - 1 ;	
		return AHMI_FUNC_SUCCESS;
	}
	else if(doubleBufferEnable) //����һ������
	{
		pTileBox->LeftBox = 0;
		pTileBox->TopBox = 0;
		pTileBox->RightBox =  TILE_NUM_X - 1;
		pTileBox->ButtomBox = TILE_NUM_Y - 1;
		return AHMI_FUNC_SUCCESS;
	}
	else
	{
		pTileBox->LeftBox = 0;
		pTileBox->TopBox = 0;
		pTileBox->RightBox =  TILE_NUM_X - 1;
		pTileBox->ButtomBox = TILE_NUM_Y - 1;
		return AHMI_FUNC_SUCCESS;
	}
	pTileBox->adjustSourceBuffer();
	
//#ifdef PC_SIM
	return AHMI_FUNC_FAILURE;
//#endif
}

	
	//����ʾ������Χ��
funcStatus DynamicPageClass::RefreshOSCWidgetBox(
	WidgetClassPtr pWidget,
	TileBoxClassPtr pTileBox
)
	{
	u8   LineWidth = (pWidget->WidgetAttr >>12) & 0xf;
	u8   StepX = (pWidget->WidgetAttr>>5) & 0x3f;
	u16 Curposi = pWidget->CurPosI - 1;//��ǰ�����
	u16 CurposX = Curposi * StepX  + pWidget->WidgetOffsetX; //��ǰ��X����


	u8 CrossTile;//���ڴ���Tile�ֽ����ϵ��ߣ���Ҫ������Tile������
	u8 refreshOsc; //��Ҫˢ������ʾ����

	if(Curposi!=0 && (CurposX>>5) != ((CurposX - StepX)>>5))//��Ҫ�����߿�Խ��2��tile
	{
		CrossTile = 1;
	}
	else CrossTile = 0;

	refreshOsc = pWidget->WidgetAttr & 0x800 ? 1 : 0;
	
	if(refreshOsc)
		pTileBox->LeftBox = (pWidget->WidgetOffsetX - pWidget->BLANK_X) >> 5;
	else 
	    pTileBox->LeftBox = CurposX>>5;
	if(CrossTile) pTileBox->LeftBox --;
	pTileBox->TopBox = (pWidget->WidgetOffsetY) >>5;
	if(refreshOsc)
		pTileBox->RightBox = (pWidget->WidgetOffsetX + pWidget->WidgetWidth - pWidget->BLANK_X) >> 5;
	else 
	    pTileBox->RightBox = CurposX>>5;;
	pTileBox->ButtomBox = (pWidget->WidgetOffsetY + pWidget->WidgetHeight + LineWidth)>>5;
	return AHMI_FUNC_SUCCESS;
}





WidgetClassPtr DynamicPageClass::GetWidgetList(u8 Value)
{
	// assert(Value<Ptr->NumOfWidget); 
	return &(this->pWidgetList[Value]);
}

TextureClassPtr DynamicPageClass::GetBackgroundTexture()
{
	return this->pBackgroundTexture;
}

funcStatus DynamicPageClass::initHighlightWidget(u8 systemInitial)
{
	WidgetClassPtr curWidgetPtr;
	u8 widgetType;
	//u8 buttonType;
	ButtonClass button;
	ActionTriggerClass actionTrigger;
	if( ( (s8)this->curEnlightedWidgetID ) < 0)
		return AHMI_FUNC_SUCCESS;
	curWidgetPtr = &(this->pWidgetList[this->curEnlightedWidgetID]);
	widgetType = ((curWidgetPtr->WidgetAttr) & 0x1f);
	//buttonType = (curWidgetPtr->WidgetAttr & 0x60) >> 5;
	if(widgetType == BUTTON_TEX && this->curEnlightedWidgetLighted)
	{
		actionTrigger.mInputType = ACTION_KEYBOARD_NEXT;
		actionTrigger.mTagPtr = NULL;
		button.widgetCtrl(curWidgetPtr, &actionTrigger, 1);
	}
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// �������� setATag
// ����tag�����ı�
// �����б�
//     @param1 u8 value  �ı���ֵ
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus DynamicPageClass::setATag(u8 value)
{
	s16 curPageOffsetX ; //1.11.4
	s16 curPageOffsetY ; //1.11.4
	u16 transitionParamIn; //0.10
	u16 transitionParamOut; //0.10
//	s16 nextPageZoom   ; //���ű�����1.7.9
	TileBoxClass tileBoxTemp;  //�������浱ǰ��tilebox
//	myMathClass myMathTemp;   //������ѧ����
	u8 animationType = this->mAnimationType;
//	RefreshMsg refreshMsg;
	TextureClassPtr pBackgroundTexturePtr = gPagePtr[WorkingPageID].pBackgroundTexture; //background texture
//	ElementPtr curElementPtr;

	//����Ҫ���ƶ���
	if(mtotoalFrame == 0 || animationType == NO_ANIMATION)
	{
		//atag�ı䲻�����仯
		return AHMI_FUNC_SUCCESS;
	}
	//��Ҫ���ƶ���
	else
	{
		if(value > mtotoalFrame || value == 0) //out of baundary
		{
			ERROR_PRINT("ERROR in setting page animaion tag: value out of baundary");
			return AHMI_FUNC_FAILURE;
		}
		this->mATag = value;
		this->mPageMatrix.matrixInit();
		//pNextPage->mPageMatrix.matrixInit();
		switch(animationType)
		{
			case(PAGE_ANIMATION_SHIFTING_L):
				//ˮƽƽ�ƶ���
				transitionParamIn = ( mtotoalFrame - mATag) * 1024 / mtotoalFrame;//0.10
				inoutTransitioin(transitionParamIn, transitionParamOut);
				curPageOffsetX = 0 - (s32)( (transitionParamOut * (SCREEN_WIDTH) ) / 64 ) ;
				curPageOffsetY = 0;
				this->mPageMatrix.A = (1 << 20);
				this->mPageMatrix.B = (0 << 20);
				this->mPageMatrix.C = (0 << 20);
				this->mPageMatrix.D = (1 << 20);
				this->mPageMatrix.E = -(curPageOffsetX << 9);
				this->mPageMatrix.F = -(curPageOffsetY << 9);
				//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
				refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
				refreshMsg.mElementPtr.pageptr = this;
				sendToRefreshQueue(&refreshMsg);
#endif
				//ˮƽƽ�ƶ�������
				break;

				case(PAGE_ANIMATION_SHIFTING_R):
					//shift from right to left
					transitionParamIn = (mATag) * 1024 / mtotoalFrame;//0.10
					inoutTransitioin(transitionParamIn, transitionParamOut);
					curPageOffsetX = ((SCREEN_WIDTH) << 4) - (s32)( (transitionParamOut * (SCREEN_WIDTH) ) / 64 ) ;
					curPageOffsetY = 0;
					this->mPageMatrix.A = (1 << 20);
					this->mPageMatrix.B = (0 << 20);
					this->mPageMatrix.C = (0 << 20);
					this->mPageMatrix.D = (1 << 20);
					this->mPageMatrix.E = -(curPageOffsetX << 9);
					this->mPageMatrix.F = -(curPageOffsetY << 9);
					//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
					refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
					refreshMsg.mElementPtr.pageptr = this;
					sendToRefreshQueue(&refreshMsg);		
#endif
					//end of shift from left to right
					break;

				case(PAGE_ANIMATION_SHIFTING_UP):
				//shifting from up to down
					transitionParamIn = (mATag) * 1024 / mtotoalFrame;//0.10
					inoutTransitioin(transitionParamIn, transitionParamOut);
					curPageOffsetX = 0;
					curPageOffsetY =  (s32)( (transitionParamOut * (SCREEN_HEIGHT) ) / 64 ) - (s32)(SCREEN_HEIGHT << 4) ;
					this->mPageMatrix.A = (1 << 20);
					this->mPageMatrix.B = (0 << 20);
					this->mPageMatrix.C = (0 << 20);
					this->mPageMatrix.D = (1 << 20);
					this->mPageMatrix.E = -(curPageOffsetX << 9);
					this->mPageMatrix.F = -(curPageOffsetY << 9);
				//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
					refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
					refreshMsg.mElementPtr.pageptr = this;
					sendToRefreshQueue(&refreshMsg);		
#endif	
					//end of shift from up to down
					break;

					case(PAGE_ANIMATION_SHIFTING_DOWN):
					//shifting from down to top
					transitionParamIn = (mATag) * 1024 / mtotoalFrame;//0.10
					inoutTransitioin(transitionParamIn, transitionParamOut);
					curPageOffsetX = 0;
					curPageOffsetY =  (s32)(SCREEN_HEIGHT << 4) - (s32)( (transitionParamOut * (SCREEN_HEIGHT) ) / 64 ) ;
					this->mPageMatrix.A = (1 << 20);
					this->mPageMatrix.B = (0 << 20);
					this->mPageMatrix.C = (0 << 20);
					this->mPageMatrix.D = (1 << 20);
					this->mPageMatrix.E = -(curPageOffsetX << 9);
					this->mPageMatrix.F = -(curPageOffsetY << 9);
					//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
					refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
					refreshMsg.mElementPtr.pageptr = this;
					sendToRefreshQueue(&refreshMsg);		
#endif	

					//end of shift from down to top
					break;

				case(PAGE_ANIMATION_ZOOM):
				//zoom from center
					transitionParamIn = (mATag) * 1024 / mtotoalFrame;//0.10
					inoutTransitioin(transitionParamIn, transitionParamOut);  //scalter parameter
					//offsetx = (w/2)*(1-scaler)
					//offsety = (h/2)*(1-scaler)
					if(transitionParamOut < 2) //in case that A exceeds the boundary
						transitionParamOut = 2;
					curPageOffsetX = (s32)(SCREEN_WIDTH * 16) - (s32)( transitionParamOut * (SCREEN_WIDTH) / 64 );//( (1 - trans) * SCREEN_WIDTH) / 2
					curPageOffsetY =  (s32)(SCREEN_HEIGHT * 16) - (s32)( (transitionParamOut * (SCREEN_HEIGHT) ) / 64 ) ;
					this->mPageMatrix.A = (s32)((0x80000 / transitionParamOut) << 11); //1.11.20

					if(this->mPageMatrix.A >= 0x100000 )
						this->mPageMatrix.A = 0xfffff; //the maxium number of the 1.11.20
					else if(this->mPageMatrix.A == 0)
						this->mPageMatrix.A = 1;     //the munium number
					this->mPageMatrix.B = (0 << 20);
					this->mPageMatrix.C = (0 << 20);
					this->mPageMatrix.D = mPageMatrix.A;
					this->mPageMatrix.E = -(curPageOffsetX << 9);
					this->mPageMatrix.F = -(curPageOffsetY << 9);
					//send refresh message
#ifndef WHOLE_TRIBLE_BUFFER
					refreshMsg.mElementType = ANIMATION_REFRESH_PAGE;
					refreshMsg.mElementPtr.pageptr = this;
					sendToRefreshQueue(&refreshMsg);		
#endif
					//end of zoom animation
					break;

				//�������
			default:
#ifdef AHMI_DEBUG
				ERROR_PRINT("This type of animation doesn't exist.\r\n");
#endif
				return AHMI_FUNC_FAILURE;
		}
	}
	//refresh source box of background texture
	//curElementPtr.pageptr = &(gPagePtr[WorkingPageID]);
	//tileBoxTemp.sourceReCompute(curElementPtr, ANIMATION_REFRESH_PAGE, &(this->mPageMatrix) );
	//pBackgroundTexturePtr->copyBox(&tileBoxTemp);
	
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// �������� inoutTransitioin
// ���㻺�亯��
// if(x < 0.5) y = 4*x^3
// else        y = 4(x-1)^3 + 1
// �����б�
//     @param1 s16 in 0.10��ʽ����
//     @param2 s16 out 0.10��ʽ���
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus DynamicPageClass::inoutTransitioin(
	u16 in,   //0.10
	u16 &out  //0.10
	)
{

	if(in < 512)
	{
		out = (in * in * in) >> 18;
		return AHMI_FUNC_SUCCESS;
	}
	else if(in <= 1024)
	{

		out = 1024 - (((1024 - in) * (1024 - in) * (1024 - in)) >> 18) ;
		return AHMI_FUNC_SUCCESS;
	}
	out = 0;
	return AHMI_FUNC_FAILURE;
}

//-----------------------------
// �������� writeBackGround
// д�뾲̬����ı���
// �����б�
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus DynamicPageClass::writeBackGround(u32 *psourceshift, matrixClassPtr addtionalMatrix)
{
	TextureClass backTexture;
	backTexture.mTexAttr = RGBA8888 | ADDRTEXTURE | EFMATRIX;
	backTexture.TexWidth = (u16)gScreenWidth;
	backTexture.TexHeight = (u16)gScreenHeight;
	backTexture.OffsetX = 0;//1.11.4
	backTexture.OffsetY = 0;//1.11.4
	backTexture.RotateAngle =(short)0;//1.11.4
	backTexture.ShearAngleX =(short)0;//1.11.4
	backTexture.ScalerX =	(short)0;//1.6.9
	backTexture.ScalerY = 	(short)0;//1.6.9
	backTexture.TexLeftTileBox = 0  ;//left
	backTexture.TexTopTileBox = 0   ;//top
	backTexture.TexRightTileBox = TILE_NUM_X - 1 ;//right
	backTexture.TexButtomTileBox = TILE_NUM_Y - 1;//bottom
	backTexture.FocusedSlice =	0;                    //focus��slice
	backTexture.SingleSliceSize =	0;              //�ߴ�
	backTexture.TexAddr =	START_ADDR_OF_RAM + START_ADDR_OF_DISPLAY + 3 * SIZE_OF_DISPLAY_BUFFER;          //static texture �洢�ռ�
	backTexture.staticTexture = 1;
	backTexture.writeSourceBuffer(psourceshift, addtionalMatrix, NULL,NULL);
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// �������� clearPage
// д��һ�Ŵ���ɫ����
// �����б�
// ��ע(�����汾֮����޸�):
//   ��
//-----------------------------
funcStatus DynamicPageClass::clearPage(
		u32 *psourceshift
		)
{
	TextureClass backTexture;
	matrixClass matrixTemp;
	matrixTemp.matrixInit();
	backTexture.mTexAttr = PURECOLOR | ADDRTEXTURE | EFMATRIX;
	backTexture.TexWidth = (u16)gScreenWidth;
	backTexture.TexHeight = (u16)gScreenHeight;
	backTexture.OffsetX = 0;//1.11.4
	backTexture.OffsetY = 0;//1.11.4
	backTexture.RotateAngle =(short)0;//1.11.4
	backTexture.ShearAngleX =(short)0;//1.11.4
	backTexture.ScalerX =	(short)0;//1.6.9
	backTexture.ScalerY = 	(short)0;//1.6.9
	backTexture.TexLeftTileBox = 0  ;//left
	backTexture.TexTopTileBox = 0   ;//top
	backTexture.TexRightTileBox = TILE_NUM_X - 1 ;//right
	backTexture.TexButtomTileBox = TILE_NUM_Y - 1;//bottom
	backTexture.FocusedSlice =	0;                    //focus��slice
	backTexture.SingleSliceSize =	0;              //�ߴ�
	backTexture.TexAddr =	0xff000000;          //static texture �洢�ռ�
	backTexture.staticTexture = 1;
	backTexture.writeSourceBuffer(psourceshift, &matrixTemp, NULL,NULL);
	return AHMI_FUNC_SUCCESS;
}



#endif
