////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     DynamicPageClass.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2016/03/21 by Zhou Yuzhi(周钰致)
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
		void*          BassAddress             , //存放动态数据的基地址
		u32            StoreOffset            , //数据偏移 
		u16            PageAttr               , //page属性
		u16            NumOfWidget            , //widget的数量
		u8 			   NumOfCanvas			   , //Canvas数量
		u8             NumOfSubCanvas         , //subCanvas数量
		u16            ActionInstructionsSize , //总指令长度
		u16            TotalNumOfTexture	   , //纹理数量
		u16            NumofTouchWidget       , //可触碰纹理数量
		u16            NumOfPageAction        , //Action数量
		u16            NumOfCanvasAction	,	 //CanvasAction数量
		u8             animationType        ,    //切换动画类型
		u8             curEnlightedWidgetID ,    //当前高亮的widget
		u8             animationFrame       ,    //动画帧数
		u8             numOfAnimationActions     //用户自定义动画数目
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
// 函数名： loadPage
// 初始化成员变量
// 参数列表：
//   @param1 u8 page 页面
//   @param2 u8 systemInitial 表示系统最初始的页面，用于高亮控件
// 备注(各个版本之间的修改):
//   无
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

	//初始化canvas和widget矩阵
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
		//先发送一次刷新静态空间
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
		//先发送一次刷新静态空间
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
// 函数名： loadPageWithAnimation
// 初始化成员变量
// 参数列表：
//     @param1 DynamicPageClassPtr pCurPage  , //当前页面指针
//     @param2 DynamicPageClassPtr pNextPage , //下一页面指针
//     @param3 u8 curFrame                   , //当前帧
//     @param4 u8 totalFrame                 , //总共帧
//     @param5 u8 animationType                //动画类型
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus DynamicPageClass::loadPageWithAnimation(
		u8 curPageID, //当前页面
		u8 nextPageID, //下一页面
        u8 curFrame                   , //当前帧
		u8 totalFrame                 , //总共帧
		u8 animationType                //动画类型
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
	//初始化canvas和widget矩阵
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

//绘制页面
funcStatus DynamicPageClass::drawPage(
	TileBoxClassPtr tileBox,
	u32 *sourceShift       ,
	u8  pageEnable         ,
	u8 RefreshType        ,//动画类型
    u8 staticTextureEn        //是否绘制到静态存储空间
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
		  //判断是否为0号tag，如果是0号tag，令tagtmp=-0
			cptr->mFocusedCanvas = 0;
		else
			cptr->mFocusedCanvas = bindTag->mValue;
		//判断tag的值有没有超过子画布的数量，如果超过的话就等于子画布的数量值
		if(cptr->mFocusedCanvas >= cptr->mNumOfSubCanvas) 
			cptr->mFocusedCanvas = cptr->mNumOfSubCanvas - 1;
		if(this->pSubCanvasList == NULL)
		{
			ERROR_PRINT("ERROR in drawPage: subcanvas list data corruption");
			return AHMI_FUNC_FAILURE;
		}
		scptr = (SubCanvasClassPtr)(this->pSubCanvasList) + cptr->mStartAddrOfSubCanvas + cptr->mFocusedCanvas;
		//调用canvas函数
		status = cptr->drawCanvas(tileBox, sourceShift, pageEnable, &(this->mPageMatrix), scptr, RefreshType, staticTextureEn);
		if(status == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
	}
	//if(RefreshType == ANIMATION_REFRESH_PAGE)
	//	this->mPageMatrix.matrixInit();
	return AHMI_FUNC_SUCCESS;
}

	//绘制动画页面
funcStatus DynamicPageClass::drawPageWithAnimation(
	DynamicPageClass* pCurPage    , //当前页面指针
	TileBoxClassPtr tileBox       ,  //包围盒
	u32* pSourceShift               ,  //sourcebuffer偏移
    u8  staticTextureEn                //是否更新到静态纹理区
	)
{
	//s16 curPageOffsetX ; //1.11.4
	//s16 curPageOffsetY ; //1.11.4
	//s16 nextPageOffsetX; //1.11.4
	//s16 nextPageOffsetY; //1.11.4
	//float f_zoomScaler;
	//s16 nextPageZoom   ; //缩放比例，1.7.9
	TextureClassPtr  pCurrentTexture;
	//TextureClassPtr  pNextTexture;
	//TileBoxClass tileBoxTemp;  //用来保存当前的tilebox
	//myMathClass myMathTemp;   //调用数学函数
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
	//不需要绘制动画
	//if(totalFrame == 0 || animationType == NO_ANIMATION)
	//{
		//写入第第一页的背景纹理
		//pCurPage->mPageMatrix.matrixInit();
		pCurrentTexture = pCurPage->pBackgroundTexture;
		//pCurrentTexture->copyBox(tileBox);
		pCurrentTexture->writeSourceBuffer( pSourceShift, &(pCurPage->mPageMatrix), NULL,NULL);
		//写入widget信息到sourcebuffer
		drawPage(tileBox,pSourceShift,1,ANIMATION_REFRESH_PAGE,staticTextureEn);
	//}
	////需要绘制动画
	//else
	//{
//		pCurPage->mPageMatrix.matrixInit();
//		pNextPage->mPageMatrix.matrixInit();
//		switch(animationType)
//		{
//			case(ANIMATION_HERIZONTAL_SHIFTING_L):
//				//水平平移动画
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
//				//计算包围盒
//				if(refreshWidgetBox(NULL, NULL, NULL,0,0,0,0,tileBox) == AHMI_FUNC_FAILURE)
//					return AHMI_FUNC_FAILURE;
//				tileBox->LeftBox = ( (nextPageOffsetX >> 4) + TILESIZE - 1) / TILESIZE;
//				tileBox->TopBox  = ( (nextPageOffsetY >> 4) + TILESIZE - 1) / TILESIZE;
//				//写入第二个个页面的背景纹理
//				pNextTexture = pNextPage->pBackgroundTexture;
//				pNextTexture ->writeSourceBuffer(pSourceShift, &(pNextPage->mPageMatrix), 1, tileBox);
//				//调用绘图函数
//				if(pNextPage->drawPage(&tileBoxTemp, pSourceShift, 1, ANIMATION_REFRESH_PAGE) == AHMI_FUNC_FAILURE) //传递整个页面的包围盒，因为绘制动画的时候每个元件都需要绘制
//					return AHMI_FUNC_FAILURE;
//				break;
//				//水平平移动画结束
//
//				case(ANIMATION_VERTICAL_SHIFTING_DOWN):
//				//垂直平移动画
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
//				//计算包围盒
//				if(refreshWidgetBox(NULL,NULL,NULL,0,0,0,0,tileBox) == AHMI_FUNC_FAILURE)
//					return AHMI_FUNC_FAILURE;
//				tileBox->LeftBox = ( (nextPageOffsetX >> 4) + TILESIZE - 1) / TILESIZE;
//				tileBox->TopBox = ( (nextPageOffsetY >> 4) + TILESIZE - 1) / TILESIZE;
//				//写入第二个个页面的背景纹理
//				pNextTexture = pNextPage->pBackgroundTexture;
//				pNextTexture ->writeSourceBuffer(pSourceShift, &(pNextPage->mPageMatrix), 1, tileBox);
//				//调用绘图函数
//				if(pNextPage->drawPage(&tileBoxTemp, pSourceShift, 1, ANIMATION_REFRESH_PAGE) == AHMI_FUNC_FAILURE) //传递整个页面的包围盒，因为绘制动画的时候每个元件都需要绘制
//					return AHMI_FUNC_FAILURE;
//				break;
//				//垂直平移动画结束
//
//				case(ANIMATION_ZOOM_OUT):
//				//缩放动画
//				f_zoomScaler = (float) curFrame / (float) totalFrame; //计算缩放比例
//				myMathTemp.transferFloatToS16(f_zoomScaler, &nextPageZoom); //转换为1.6.9格式
//				pNextPage->mPageMatrix.matrixScaler(nextPageZoom,nextPageZoom); //计算缩放后矩阵
//				//计算包围盒
//				if(refreshWidgetBox(NULL,NULL,NULL,0,0,0,0,tileBox) == AHMI_FUNC_FAILURE)
//					return AHMI_FUNC_FAILURE;
//				//写入第二个个页面的背景纹理
//				pNextTexture = pNextPage->pBackgroundTexture;
//				pNextTexture ->writeSourceBuffer(pSourceShift, &(pNextPage->mPageMatrix), 1, tileBox);
//				//调用绘图函数
//				if(pNextPage->drawPage(&tileBoxTemp, pSourceShift, 1, ANIMATION_REFRESH_PAGE) == AHMI_FUNC_FAILURE) //传递整个页面的包围盒，因为绘制动画的时候每个元件都需要绘制
//					return AHMI_FUNC_FAILURE;
//				break;
//				//缩放动画结束
//
//				//其他情况
//			default:
//#ifdef DEBUG
//				ERROR_PRINT("This type of animation doesn't exist.\r\n");
//#endif
//				return AHMI_FUNC_FAILURE;
//		}
//	}
	return AHMI_FUNC_SUCCESS;
}

//更新包围盒
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
	if(textureEnable)//texture动画的时候只更新widget
	{
		pTileBox->LeftBox = (pWidget->WidgetOffsetX  )>>5;
		pTileBox->TopBox = (pWidget->WidgetOffsetY   )>>5;
		pTileBox->RightBox = ((pWidget->WidgetOffsetX + pWidget->WidgetWidth)>>5) ;
		pTileBox->ButtomBox = ((pWidget->WidgetOffsetY + pWidget->WidgetHeight )>>5) ;
		return AHMI_FUNC_SUCCESS;
	}
	if(widgetEnable)//只更新一个widget
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
		if((pWidget->WidgetAttr & 0x1f) == DYNAMIC_TEX && ((pWidget->WidgetAttr & DYNAMIC_TYPE_BIT) >> 5) == CENTRAL_ROTATE) //旋转纹理
		{
			texturePtr = &gPagePtr[WorkingPageID].pBasicTextureList[pWidget->StartNumofTex];
			pTileBox->LeftBox     = texturePtr->TexLeftTileBox   ;
			pTileBox->TopBox      = texturePtr->TexTopTileBox    ;
			pTileBox->RightBox    = texturePtr->TexRightTileBox  ;
			pTileBox->ButtomBox   = texturePtr->TexButtomTileBox ;
			
		}
		else if( ( (pWidget->WidgetAttr & 0x1f) == METER_TEX) && ((pWidget->WidgetAttr & METER_SIMPLIFY) == METER_SIMPLIFY ) )//仪表盘单独处理
		{
			texturePtr = &gPagePtr[WorkingPageID].pBasicTextureList[pWidget->StartNumofTex];
			//绘制指针
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
		else//单独处理示波器
		{
			return RefreshOSCWidgetBox(pWidget,pTileBox);
		}
		
	}
	else if(canvasEnable)//只更新一个canvas
	{
		pTileBox->LeftBox = (pCanvas->moffsetX  )>>5;
		pTileBox->TopBox = (pCanvas->moffsetY   )>>5;
		pTileBox->RightBox = ((pCanvas->moffsetX + pCanvas->mwidth + TILESIZE - 1 )>>5) - 1 ;
		pTileBox->ButtomBox = ((pCanvas->moffsetY + pCanvas->mheight + TILESIZE - 1)>>5) - 1 ;	
		return AHMI_FUNC_SUCCESS;
	}
	else if(doubleBufferEnable) //更新一个纹理
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

	
	//更新示波器包围盒
funcStatus DynamicPageClass::RefreshOSCWidgetBox(
	WidgetClassPtr pWidget,
	TileBoxClassPtr pTileBox
)
	{
	u8   LineWidth = (pWidget->WidgetAttr >>12) & 0xf;
	u8   StepX = (pWidget->WidgetAttr>>5) & 0x3f;
	u16 Curposi = pWidget->CurPosI - 1;//当前点序号
	u16 CurposX = Curposi * StepX  + pWidget->WidgetOffsetX; //当前点X坐标


	u8 CrossTile;//对于处在Tile分界面上的线，需要把两个Tile都画了
	u8 refreshOsc; //需要刷新整个示波器

	if(Curposi!=0 && (CurposX>>5) != ((CurposX - StepX)>>5))//需要画的线跨越了2个tile
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
// 函数名： setATag
// 动画tag发生改变
// 参数列表：
//     @param1 u8 value  改变后的值
// 备注(各个版本之间的修改):
//   无
//-----------------------------
funcStatus DynamicPageClass::setATag(u8 value)
{
	s16 curPageOffsetX ; //1.11.4
	s16 curPageOffsetY ; //1.11.4
	u16 transitionParamIn; //0.10
	u16 transitionParamOut; //0.10
//	s16 nextPageZoom   ; //缩放比例，1.7.9
	TileBoxClass tileBoxTemp;  //用来保存当前的tilebox
//	myMathClass myMathTemp;   //调用数学函数
	u8 animationType = this->mAnimationType;
//	RefreshMsg refreshMsg;
	TextureClassPtr pBackgroundTexturePtr = gPagePtr[WorkingPageID].pBackgroundTexture; //background texture
//	ElementPtr curElementPtr;

	//不需要绘制动画
	if(mtotoalFrame == 0 || animationType == NO_ANIMATION)
	{
		//atag改变不发生变化
		return AHMI_FUNC_SUCCESS;
	}
	//需要绘制动画
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
				//水平平移动画
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
				//水平平移动画结束
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

				//其他情况
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
// 函数名： inoutTransitioin
// 计算缓变函数
// if(x < 0.5) y = 4*x^3
// else        y = 4(x-1)^3 + 1
// 参数列表：
//     @param1 s16 in 0.10格式输入
//     @param2 s16 out 0.10格式输出
// 备注(各个版本之间的修改):
//   无
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
// 函数名： writeBackGround
// 写入静态缓存的背景
// 参数列表：
// 备注(各个版本之间的修改):
//   无
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
	backTexture.FocusedSlice =	0;                    //focus的slice
	backTexture.SingleSliceSize =	0;              //尺寸
	backTexture.TexAddr =	START_ADDR_OF_RAM + START_ADDR_OF_DISPLAY + 3 * SIZE_OF_DISPLAY_BUFFER;          //static texture 存储空间
	backTexture.staticTexture = 1;
	backTexture.writeSourceBuffer(psourceshift, addtionalMatrix, NULL,NULL);
	return AHMI_FUNC_SUCCESS;
}

//-----------------------------
// 函数名： clearPage
// 写入一张纯黑色背景
// 参数列表：
// 备注(各个版本之间的修改):
//   无
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
	backTexture.FocusedSlice =	0;                    //focus的slice
	backTexture.SingleSliceSize =	0;              //尺寸
	backTexture.TexAddr =	0xff000000;          //static texture 存储空间
	backTexture.staticTexture = 1;
	backTexture.writeSourceBuffer(psourceshift, &matrixTemp, NULL,NULL);
	return AHMI_FUNC_SUCCESS;
}



#endif
