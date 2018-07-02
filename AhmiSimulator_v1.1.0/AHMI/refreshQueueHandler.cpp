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
//    the original structure in C
// 
////////////////////////////////////////////////////////////////////////////////
#include "AHMICfgDefinition.h"
#ifdef PC_SIM
#include "stdafx.h"
#endif
#include "FreeRTOS.h"
#include "queue.h"
#include "publicInclude.h"
#include "ahmiv3.h"
#include "ahmiv3_vd.h"
#ifdef EMBEDDED
#include "ahmi3_function.h"
#include "semphr.h"
#include "spi_if.h"
#include "IOConfig.h"
extern u8 initAHMI;
#endif

#ifdef AHMI_CORE

extern QueueHandle_t   RefreshQueue;
extern QueueHandle_t   RefreshQueueWithoutDoubleBuffer;
extern u16             WorkingPageID;
extern ConfigInfoClass   ConfigData;
extern u8 sourceBuffer[2048];
u8 ahmi_init_int = 0;
u8 ahmi_init2 = 0;
u8 ahmi_init3 = 0;
u8 staticRefresh = 0;
static u8 staticRefresh2 = 0;
static u32 screenUpdateCount = 0;
extern u8 animationDuration;

extern u8		stm32info[];
TextureClass    doubleBufferTexture;
#ifdef PARTIAL_TRIBLE_BUFFER//部分三缓存
static u8 addr_n = 1;
#endif

#ifdef WHOLE_TRIBLE_BUFFER//部分三缓存
static u8 addr_n = 0;

#endif

extern u8 gPageNeedRefresh;
extern u8 animationExist;

#ifdef PC_SIM
extern StructFrameBuffer  GlobalFrameBuffer[ MAX_SCREEN_SIZE *2];
extern StructFrameBuffer  GlobalFrameBuffer2[ MAX_SCREEN_SIZE *2]; //used for double buffer
ahmi *myahmi;
extern HWND ViewHWND;
#endif
extern DynamicPageClassPtr  gPagePtr;
extern TagClassPtr     TagPtr;
extern u16             WorkingPageID;

extern void SetWriteBuffer(u32);

#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
//extern u16     addr_combine;
#ifdef EMBEDDED
extern SemaphoreHandle_t drawSemaphore;
extern SemaphoreHandle_t xFPGASemaphore;
#ifdef FRAME_TEST
extern u32 gFrameCount ;
#endif
#endif
static u32 i = 0;

#endif

//static u8 i = 0;
u8 ahmi_initial = 0;
u8 ahmi_first = 0;

extern	u8 addr_w;
extern	u8 addr_r;

#ifdef VIDEO_EN
extern  u8 addr_cvbs;             
extern  u8 video_cur_w;           
extern  u8 video_cur_r ;          
#endif

extern uint8_t runningControl;


funcStatus sendToRefreshQueue(RefreshMsg* refreshMsg)
{
#ifdef SOURCE_BOX_COMBINE
	TileBoxClass SourceBox;
	TileBoxClass sourceBoxRead;
	AnimationMsg animationMsgRead;
	
	matrixClass matrixTemp;
	u8 numOfRefreshQueue;
	u8 i;
	//禁止切换线程
	taskENTER_CRITICAL();

	computingTileBoxFromAnimationMsg(pAnimationMsg, &SourceBox);
	if(SourceBox.LeftBox > (s8)(TILE_NUM_X-1))
	{		
		//允许切换线程
		taskEXIT_CRITICAL();
		return AHMI_FUNC_SUCCESS;
	}
	if(SourceBox.TopBox > (s8)(TILE_NUM_Y-1))
	{		
		//允许切换线程
		taskEXIT_CRITICAL();
		return AHMI_FUNC_SUCCESS;
	}
	SourceBox.adjustSourceBuffer();

	//判断包围盒是否包含
	if(pAnimationMsg->Total_Frame == 0) //有动画的情况下，不合并
	{
		numOfRefreshQueue = (u8)uxQueueMessagesWaiting(RefreshQueue);
		for(i = 0; i < numOfRefreshQueue; i++)
		{
			arthasReadDataFromQueue(RefreshQueue,&animationMsgRead,i);
			computingTileBoxFromAnimationMsg(&animationMsgRead, &sourceBoxRead);
			if(animationMsgRead.Total_Frame != 0)
				continue;
			else if(sourceBoxRead.sourceBoxContain(&SourceBox)) //存在
			{
				if(animationMsgRead.RefreshType == ANIMATION_REFRESH_PAGE) //原先是页面更新，可以合并一切
				{
					taskEXIT_CRITICAL();
					return AHMI_FUNC_SUCCESS;
				}
				else if(animationMsgRead.RefreshType == ANIMATION_REFRESH_CANVAS) //原先是canvas更新，只能合并canvas或者widget
				{
					if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_WIDGET || pAnimationMsg->RefreshType == ANIMATION_REFRESH_CANVAS)
					{
						taskEXIT_CRITICAL();
						return AHMI_FUNC_SUCCESS;
					}
				}
				else if(animationMsgRead.RefreshType == ANIMATION_REFRESH_WIDGET) //原先是widget，只能合并widget
				{
					if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_WIDGET)
					{
						taskEXIT_CRITICAL();
						return AHMI_FUNC_SUCCESS;
					}
				}
			}
			else if(SourceBox.sourceBoxContain(&sourceBoxRead)) //输入的sourcebox包含原先存在的sourcebox
			{
				//widget只能吞并widget
				if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_WIDGET)
				{
					if(animationMsgRead.RefreshType != ANIMATION_REFRESH_WIDGET)
						continue;
				}
				//canvas可以吞并widget或者canvas
				else if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_CANVAS)
				{
					if(animationMsgRead.RefreshType != ANIMATION_REFRESH_WIDGET || animationMsgRead.RefreshType != ANIMATION_REFRESH_CANVAS)
						continue;
				}
				//page可以吞并一切,其他一切情况不能吞并
				else if(pAnimationMsg->RefreshType != ANIMATION_REFRESH_PAGE) 
					continue;
				arthasCopyDataToQueueInPos(RefreshQueue, pAnimationMsg, i);
				taskEXIT_CRITICAL();
				return AHMI_FUNC_SUCCESS;
			}
		}
	}
	
	
#endif
#if (defined PARTIAL_DOUBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
	//如果没有合并，则发送包围盒
	xQueueSendToBack(RefreshQueue,refreshMsg,portMAX_DELAY);
#endif
#ifdef STATIC_BUFFER_EN

	//taskENTER_CRITICAL();
	if(animationExist == 0 && refreshMsg->mElementType == ANIMAITON_REFRESH_STATIC_BUFFER && staticRefresh == 0 && animationDuration == 0)
	{
		
		staticRefresh = 1;
		staticRefresh2 = 1;
		//taskEXIT_CRITICAL();
#ifdef AHMI_DEBUG
	char text[100];
	sprintf(text,"current size of queue %d", uxQueueMessagesWaiting(RefreshQueue) );
	ERROR_PRINT(text);
#endif
		xQueueSendToFront(RefreshQueue,refreshMsg,portMAX_DELAY);
	}
	else if(refreshMsg->mElementType != ANIMAITON_REFRESH_STATIC_BUFFER)
	{
		//taskEXIT_CRITICAL();
		xQueueSendToBack(RefreshQueue,refreshMsg,portMAX_DELAY);
	}
#endif
#ifdef SOURCE_BOX_COMBINE
	//允许切换线程
	taskEXIT_CRITICAL();
#endif
	
	return AHMI_FUNC_SUCCESS;
}

funcStatus computingTileBoxFromAnimationMsg(AnimationMsg* pAnimationMsg, TileBoxClassPtr SourceBox)
{
	CanvasClassPtr drawingCanvas;
	if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_PAGE) //页面则更新整个包围盒
	    return pAnimationMsg->Old_ElementPtr.pageptr->refreshWidgetBox(NULL, NULL, NULL,0, 0, 0, 0, SourceBox);
	else if(pAnimationMsg->RefreshType ==ANIMATION_REFRESH_CANVAS) //更新canvas
	{
		drawingCanvas = gPagePtr[WorkingPageID].pCanvasList+pAnimationMsg->Old_ElementPtr.scptr->attachCanvas;
		return gPagePtr[WorkingPageID].refreshWidgetBox(NULL, drawingCanvas,NULL, 0, 0, 1, 0, SourceBox);
	}
	else if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_WIDGET) //绘制widget包围盒
		return gPagePtr[WorkingPageID].refreshWidgetBox(pAnimationMsg->Old_ElementPtr.wptr,NULL, NULL, 0,1,0, 0, SourceBox);
	//else if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_TEXTURE) //绘制texture,仍然使用widget包围盒
	//	return gPagePtr[WorkingPageID].refreshWidgetBox(pAnimationMsg->Old_ElementPtr.wptr,NULL, NULL,1,0,0,0, SourceBox);
	else if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_DOUBLE_BUFFER) //更新双缓存，包围盒为纹理的包围盒
		return gPagePtr[WorkingPageID].refreshWidgetBox(NULL,NULL, pAnimationMsg->Old_ElementPtr.tptr, 0, 0, 0, 1, SourceBox);
	else
		return AHMI_FUNC_FAILURE;
}

//处理refresh队列数据
void WidgetRefreshTask(void* pvParameters)
 {
 	RefreshMsg refreshMsg;
 	BaseType_t xStatus; 
//#ifdef PARTIAL_TRIBLE_BUFFER
	u8 numOfRefreshQueue;
//#endif
#ifdef PC_SIM
	ahmi mahmi(gScreenWidth, gScreenHeight); 
	myahmi = &mahmi; 
#endif
 	for(;;) 
 	{
#ifdef LOW_POWER_TEST
		if(runningControl > 0)
			continue;
		xSemaphoreTake(xFPGASemaphore, portMAX_DELAY );
#endif
#ifdef PARTIAL_TRIBLE_BUFFER
		numOfRefreshQueue = (u8)uxQueueMessagesWaiting(RefreshQueue);
		while(numOfRefreshQueue > 0)
		{
			while(uxQueueMessagesWaiting(RefreshQueueWithoutDoubleBuffer) != 0)
			{
				xStatus=xQueueReceive(RefreshQueueWithoutDoubleBuffer,
 								(void*)&refreshMsg,
 								portMAX_DELAY);
 				if(xStatus!=pdPASS)
 				{
 					return;
 				}
				drawimmediately(&refreshMsg); //不需要判断双缓存
			}
#endif
		if(RefreshQueue == NULL)
		{
			ERROR_PRINT("ERROR in WidgetRefreshTask: refresh queeu corruption");
			vTaskDelay( SCREEN_UPDATE_TIME / portTICK_RATE_MS );
			continue;
		}
		numOfRefreshQueue = (u8)uxQueueMessagesWaiting(RefreshQueue);
#ifdef AHMI_DEBUG
		char text[100];
		sprintf(text,"drawing one frame. now refresh queue %d", numOfRefreshQueue);
		ERROR_PRINT(text);
#endif
		//if(numOfRefreshQueue != 0)
		//{
 			xStatus=xQueueReceive(RefreshQueue,
 								(void*)&refreshMsg,
 								portMAX_DELAY);
 			if(xStatus!=pdPASS)
 			{
 				return;
 			}
		//}
		//else 
		//{
		//	refreshMsg.mElementType = ANIMATION_REFRESH_DOUBLE_BUFFER;
		//	refreshMsg.mElementPtr.pageptr = gPagePtr + WorkingPageID;
			
		//}
		
		//update all the tags
		UpdateAllTags();
		if(refreshMsg.mElementPtr.pageptr == NULL)
		{
			ERROR_PRINT("ERROR in widgetRefreshTask: no element pointer given");
			continue;
		}
		drawimmediately(&refreshMsg);
		

		//if(numOfRefreshQueue == 0)
		//	vTaskDelay( SCREEN_UPDATE_TIME / portTICK_RATE_MS );
#ifndef WHOLE_TRIBLE_BUFFER
			numOfRefreshQueue --;

#endif
#ifdef PC_SIM
			InvalidateRect(ViewHWND,NULL,FALSE);
#endif
#ifndef WHOLE_TRIBLE_BUFFER
		}
#endif
//		ScreenPowerOn();
#ifdef LOW_POWER_TEST
        xSemaphoreGive( xFPGASemaphore );
#endif
 	}
 }

//-----------------------------
// 函数名：  drawimmediately
// 绘图更新函数
// 参数列表：
// @param1 AnimationMsg* pAnimationMsg    绘图信息
// @param2 u8 refreshType                 绘图类型，1表示不需要判断双缓存，直接绘图。0表示需要判断双缓存
// 备注(各个版本之间的修改):
// ...
//-----------------------------
#ifdef PARTIAL_DOUBLE_BUFFER
funcStatus drawimmediately(RefreshMsg* refreshMsg)
{
	
	TileBoxClass SourceBox;
	u8 *stm32ptraddr;
	u8 *sourcebufferaddr;
	TextureClassPtr  currenttexture;
	u32 sourceshift = 0;
	CanvasClassPtr drawingCanvas;
	//u16 animationType;
	matrixClass matrixTemp;
	ElementPtr curPtr;
//	AnimationMsg animationMsg;
	u8 doubleBufferEn = 0;

	stm32ptraddr = (u8 *)Stm32_interface_Addr;
	//*(stm32ptraddr + Addr_combine) = 0x09; 
#ifdef PC_SIM
	//*(stm32ptraddr + BUFFER_WIDTH) = gScreenWidth / 32;   
#endif
#ifdef EMBEDDED
	*(stm32ptraddr + BUFFER_WIDTH) = TILE_NUM_X;
#endif

	//计算包围盒
	if(refreshMsg->mElementType == ANIMATION_REFRESH_PAGE) //页面则更新整个包围盒
	{
		curPtr.pageptr = gPagePtr;
	    SourceBox.sourceReCompute(curPtr,ANIMATION_REFRESH_PAGE, &(gPagePtr[WorkingPageID].mPageMatrix) );
	}
	else if(refreshMsg->mElementType ==ANIMATION_REFRESH_CANVAS) //更新canvas
	{
		drawingCanvas = refreshMsg->mElementPtr.cptr;

	}
	else if(refreshMsg->mElementType == ANIMATION_REFRESH_WIDGET) //绘制widget包围盒
	{
		gPagePtr[WorkingPageID].refreshWidgetBox(refreshMsg->mElementPtr.wptr,NULL, NULL, 0,1,0, 0, &SourceBox);
		curPtr.pageptr = gPagePtr;
		SourceBox.sourceReCompute(curPtr,ANIMATION_REFRESH_PAGE, &(gPagePtr[WorkingPageID].mPageMatrix) );
	}
	//else if(refreshMsg->mElementType == ANIMATION_REFRESH_TEXTURE) //绘制texture,仍然使用widget包围盒
	//	gPagePtr[WorkingPageID].refreshWidgetBox(refreshMsg->mElementPtr.wptr,NULL, NULL,1,0,0,0, &SourceBox);
	else if(refreshMsg->mElementType == ANIMATION_REFRESH_DOUBLE_BUFFER) //更新双缓存，包围盒为纹理的包围盒
		gPagePtr[WorkingPageID].refreshWidgetBox(NULL,NULL, refreshMsg->mElementPtr.tptr, 0, 0, 0, 1, &SourceBox);
	else
		return AHMI_FUNC_FAILURE;

	SourceBox.adjustSourceBuffer();

	//写入背景
	sourcebufferaddr = (u8 *)SoureBufferAddr;
	if(refreshMsg->mElementType != ANIMATION_REFRESH_PAGE && refreshMsg->mElementType != ANIMATION_REFRESH_DOUBLE_BUFFER) //动画切换的时候在函数内部写背景
	{
		currenttexture = gPagePtr[WorkingPageID].pBackgroundTexture;
		currenttexture->writeSourceBuffer( &sourceshift, &(gPagePtr[WorkingPageID].mPageMatrix), 0, NULL );
	}

	
	if(refreshMsg->mElementType == ANIMATION_REFRESH_PAGE) //页面更新
	{
		refreshMsg->mElementPtr.pageptr->drawPageWithAnimation(
			refreshMsg->mElementPtr.pageptr,
			&SourceBox,
			&sourceshift
			);
	}
	else if(refreshMsg->mElementType ==ANIMATION_REFRESH_CANVAS) //更新canvas
	{
		drawingCanvas = refreshMsg->mElementPtr.cptr;
		drawingCanvas->drawCanvasWithAnimation(
			&SourceBox,
			&sourceshift
			);
	}
	else if(refreshMsg->mElementType ==ANIMATION_REFRESH_WIDGET) //更新控件
	{
		//animationType = pAnimationMsg->Old_ElementPtr.wptr->ANIMATION_TYPE; //低字节表示动画类型
		refreshMsg->mElementPtr.wptr->drawWidgetWithAnimation(
			0,
			0,
			&SourceBox,
			&sourceshift
			);
		//gPagePtr[WorkingPageID].drawPageWithAnimation(
		//	&gPagePtr[WorkingPageID],
		//	&SourceBox,
		//	&sourceshift
		//	);
	}
	//else if(refreshMsg->mElementType == ANIMATION_REFRESH_TEXTURE)
	//{
	//	//animationType = pAnimationMsg->Old_ElementPtr.wptr->ANIMATION_TYPE;
	//	refreshMsg->mElementPtr.wptr->drawTextureWithAnimation(
	//		0,
	//		0,
	//		&SourceBox,
	//		&sourceshift
	//		);
	//}
	//else if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_DOUBLE_BUFFER)
	//{
	//	
	//	matrixTemp.A = (1 << 9);
	//	matrixTemp.B = 0;
	//	matrixTemp.C = 0;
	//	matrixTemp.D = (1 << 9);
	//	matrixTemp.E = 0;
	//	matrixTemp.F = 0;
	//	pAnimationMsg->Old_ElementPtr.tptr->writeSourceBuffer(&sourceshift, &matrixTemp, 0, &SourceBox);
	//}
	taskEXIT_CRITICAL();

	if(SourceBox.LeftBox > (s8)(TILE_NUM_X - 1)) return AHMI_FUNC_SUCCESS;
	if(SourceBox.RightBox > (s8)(TILE_NUM_X - 1)) SourceBox.RightBox = (s8)(TILE_NUM_X - 1);
	if(SourceBox.TopBox > (s8)(TILE_NUM_Y - 1)) return AHMI_FUNC_SUCCESS;
	if(SourceBox.ButtomBox > (s8)(TILE_NUM_Y - 1)) SourceBox.ButtomBox = (s8)(TILE_NUM_Y - 1);

	*(stm32ptraddr + Tile_start_x) = 0 ;
	*(stm32ptraddr + Tile_start_y) = 0;
	*(stm32ptraddr + Tile_end_x)   = TILE_NUM_X - 1;
	*(stm32ptraddr + Tile_end_y)   = TILE_NUM_Y - 1 ;

	//判断是否启用双缓存
	//if(pAnimationMsg->RefreshType != ANIMATION_REFRESH_DOUBLE_BUFFER)
	//	judgeDoubleBuffer(&sourceshift, stm32ptraddr, &doubleBufferEn);

	//绘图
	if (sourceshift<SoureBufferSize-1)//sourcebuffer剩余不小于16its时，加结束符
	{//建议重新设置头的排序，可以将end放在第一个bit，可以避免多余的判断
		*(sourcebufferaddr + sourceshift++) = ENDFLAG;//texsecondheader = \0010 0000
	}

	AHMI_draw(&sourceshift);

	//发送一次立即绘制队列
	//if(pAnimationMsg->RefreshType != ANIMATION_REFRESH_DOUBLE_BUFFER && doubleBufferEn)
	//{
	//	animationMsg.Now_Frame = 0;
	//	animationMsg.Total_Frame = 0;
	//	animationMsg.Old_ElementPtr.tptr = &doubleBufferTexture;
	//	animationMsg.New_ElementPtr.tptr = &doubleBufferTexture;
	//	animationMsg.RefreshType = ANIMATION_REFRESH_DOUBLE_BUFFER;
	//	xQueueSendToBack(RefreshQueueWithoutDoubleBuffer,&animationMsg,portMAX_DELAY);
	//}

	
	return AHMI_FUNC_SUCCESS;
}
#endif

#ifdef PC_SIM
funcStatus drawimmediately(RefreshMsg* refreshMsg)
{
	
	TileBoxClass SourceBox;
	u8 *stm32ptraddr;
	u8 *sourcebufferaddr;
	TextureClassPtr  currenttexture;
	u32 sourceshift = 0;
	matrixClass matrixTemp;
	
	u8 doubleBufferEn = 0;

	stm32ptraddr = (u8 *)Stm32_interface_Addr;

	//data check
	if(refreshMsg == NULL)
	{
		ERROR_PRINT("ERROR in drawimmediately: element pointer corruption");
		return AHMI_FUNC_FAILURE;
	}
	if(refreshMsg->mElementType == ANIMATION_REFRESH_PAGE || 
		refreshMsg->mElementType == ANIMATION_REFRESH_DOUBLE_BUFFER || 
		refreshMsg->mElementType == ANIMAITON_REFRESH_STATIC_BUFFER)
	{
		if(refreshMsg->mElementPtr.pageptr == NULL || refreshMsg->mElementPtr.pageptr - gPagePtr >= ConfigData.NumofPages ||  refreshMsg->mElementPtr.pageptr - gPagePtr < 0)
		{
			ERROR_PRINT("ERROR in drawimmediately: element pointer out of boundary");
			return AHMI_FUNC_FAILURE;
		}
	}
	else
	{
		ERROR_PRINT("ERROR in drawimmediately: element type not supported");
		return AHMI_FUNC_FAILURE;
	}

	//计算包围盒
	//animationExist = 1;//debug
	if(refreshMsg->mElementType != ANIMAITON_REFRESH_STATIC_BUFFER || animationExist != 0)
	{
		if(gPageNeedRefresh == 1)
		{
			refreshMsg->mElementType = ANIMATION_REFRESH_PAGE;
			gPageNeedRefresh = 0;
		}
	}
	if(refreshMsg->mElementType == ANIMAITON_REFRESH_STATIC_BUFFER && animationExist == 0)
		staticRefresh2 = 0;
	
	if(refreshDrawingTileBox(refreshMsg, &SourceBox) == AHMI_FUNC_FAILURE)
	{
#ifdef AHMI_DEBUG
		ERROR_PRINT("ERROR when computing drawing source box");
#endif
		return AHMI_FUNC_FAILURE;
	}


	

	//写入背景
	taskENTER_CRITICAL();
//	if(refreshMsg->mElementType == ANIMAITON_REFRESH_STATIC_BUFFER && animationExist == 0) //更新静态buffer，地址为静态buffer地址
//	{
//		addr_w = 3;
//		*( (u32*)(stm32ptraddr + addr_ahmi_0) ) = START_ADDR_OF_DISPLAY + addr_w * (SIZE_OF_DISPLAY_BUFFER);
//#ifdef AHMI_DEBUG
//		ERROR_PRINT("refresh the static buffer");
//#endif
//	}
//	else 
//	{
		addr_w = 0;
		*( (u32*)(stm32ptraddr + addr_ahmi_0) ) = START_ADDR_OF_DISPLAY + addr_w * (SIZE_OF_DISPLAY_BUFFER);
//	}

	sourcebufferaddr = (u8 *)SoureBufferAddr;

	//写背景
	currenttexture = gPagePtr[WorkingPageID].pBackgroundTexture;
	if(gPagePtr[WorkingPageID].mAnimationType != ANIMATION_REFRESH_NULL && animationExist != 0)
		gPagePtr[WorkingPageID].clearPage(&sourceshift);
	matrixTemp.matrixInit();
	matrixTemp.A =gPagePtr[WorkingPageID].mPageMatrix.A;
	matrixTemp.D = gPagePtr[WorkingPageID].mPageMatrix.D;
	matrixTemp.E = gPagePtr[WorkingPageID].mPageMatrix.E;
	matrixTemp.F = gPagePtr[WorkingPageID].mPageMatrix.F;
	currenttexture->writeSourceBuffer( &sourceshift, &(matrixTemp), NULL, NULL);
	
	if(refreshMsg->mElementType == ANIMATION_REFRESH_PAGE  || animationExist != 0) //页面更新
	{
		gPagePtr[WorkingPageID].drawPage(&SourceBox,&sourceshift,1,ANIMATION_REFRESH_PAGE,0);
	}
	else if(refreshMsg->mElementType == ANIMATION_REFRESH_DOUBLE_BUFFER)
	{
		gPagePtr[WorkingPageID].drawPage(&SourceBox,&sourceshift,0,ANIMATION_REFRESH_PAGE,0);
	}
	else if(refreshMsg->mElementType == ANIMAITON_REFRESH_STATIC_BUFFER)
		gPagePtr[WorkingPageID].drawPage(&SourceBox,&sourceshift,0,ANIMATION_REFRESH_PAGE,1);
	taskEXIT_CRITICAL();

	*(stm32ptraddr + Tile_start_x) = 0 ;
	*(stm32ptraddr + Tile_start_y) = 0;
	*(stm32ptraddr + Tile_end_x)   = TILE_NUM_X - 1;
	*(stm32ptraddr + Tile_end_y)   = TILE_NUM_Y - 1 ;


	//绘图
	if (sourceshift<SoureBufferSize-1)//sourcebuffer剩余不小于16its时，加结束符
	{//建议重新设置头的排序，可以将end放在第一个bit，可以避免多余的判断
		*(sourcebufferaddr + sourceshift++) = ENDFLAG;//texsecondheader = \0010 0000
	}

	AHMI_draw(&sourceshift);
	



	
	return AHMI_FUNC_SUCCESS;
}

#endif

#ifdef EMBEDDED
#ifdef WHOLE_TRIBLE_BUFFER
//处理refresh队列数据
void WidgetRefreshTask(void* pvParameters)
{
	RefreshMsg refreshMsg;
	BaseType_t xStatus; 
#ifndef WHOLE_TRIBLE_BUFFER
	u8 numOfRefreshQueue;
#endif
#ifdef PC_SIM
	ahmi mahmi(gScreenWidth, gScreenHeight); 
	myahmi = &mahmi; 
#endif
	for(;;) 
	{
		//禁止切换线程
		//taskENTER_CRITICAL();
#ifndef WHOLE_TRIBLE_BUFFER
		numOfRefreshQueue = (u8)uxQueueMessagesWaiting(RefreshQueue);
		while(numOfRefreshQueue > 0)
		{
			while(uxQueueMessagesWaiting(RefreshQueueWithoutDoubleBuffer) != 0)
			{
				xStatus=xQueueReceive(RefreshQueueWithoutDoubleBuffer,
					(void*)&refreshMsg,
					portMAX_DELAY);
				if(xStatus!=pdPASS)
				{
					return;
				}
				drawimmediately(&refreshMsg); //不需要判断双缓存
			}
#endif
			xStatus=xQueueReceive(RefreshQueue,
				(void*)&refreshMsg,
				portMAX_DELAY);
			if(xStatus!=pdPASS)
			{
				return;
			}

			drawimmediately(&refreshMsg);
			//taskEXIT_CRITICAL();
#ifndef WHOLE_TRIBLE_BUFFER
			numOfRefreshQueue --;

#endif
#ifdef PC_SIM
			InvalidateRect(ViewHWND,NULL,FALSE);
#endif
#ifndef WHOLE_TRIBLE_BUFFER
		}
#endif
		//允许切换线程
		//vTaskDelay(SCREEN_UPDATE_TIME / portTICK_RATE_MS);
	}
}


//-----------------------------
// 函数名：  drawimmediately
// 绘图更新函数
// 参数列表：
// @param1 AnimationMsg* pAnimationMsg    绘图信息
// @param2 u8 refreshType                 绘图类型，1表示不需要判断双缓存，直接绘图。0表示需要判断双缓存
// 备注(各个版本之间的修改):
// ...
//-----------------------------
funcStatus drawimmediately(RefreshMsg* refreshMsg)
{

	TileBoxClass SourceBox;
	u8 *stm32ptraddr;
	u8 *sourcebufferaddr;
	TextureClassPtr  currenttexture;
	u32 sourceshift = 0;
	//	CanvasClassPtr drawingCanvas;
	//u16 animationType;
	matrixClass matrixTemp;
	//	AnimationMsg animationMsg;
	//	u8 doubleBufferEn = 0;
	ElementPtr curPtr;

	u8 addr_w;
	//	u8 addr_n;
	u8 addr_r;
	u8 ahmi_init = 0;

	if(addr_combine == 0)
	{
		addr_combine = 0x102; // r->0, w->2, n->4
		ahmi_init = 1;
	}
	else 
	{
		addr_w = addr_combine & 0x7;
		//addr_n = (addr_combine & 0x1c0) >> 6;
		addr_r = (addr_combine & 0x38) >> 3;
		if(1 == addr_n) //not ready
			return AHMI_FUNC_SUCCESS;
		if(addr_w == addr_r)
			addr_w = (addr_w + 4) % 6; //w += 4
		else 
			addr_w = (addr_w + 2) % 6; //w +=2
		//addr_n = (addr_combine & 0x38) >> 3; //n -> r
		addr_combine = (addr_combine & 0x38) + ( addr_w);
		ahmi_init = 0;
	}
	stm32ptraddr = (u8 *)Stm32_interface_Addr;
	if(ahmi_init == 0)
		*(stm32ptraddr + Addr_combine) = (u8)addr_combine;
	else 
		*(stm32ptraddr + Addr_combine) = 0x0; //r->0, w->0
	//#ifdef PC_SIM
	//	*(stm32ptraddr + BUFFER_WIDTH) = globalArgs.ScreenWidth / 32;   
	//#endif
	//#ifdef EMBEDDED
	//	*(stm32ptraddr + BUFFER_WIDTH) = TILE_NUM_X;
	//#endif

	//计算包围盒
	if(refreshMsg->mElementType == ANIMATION_REFRESH_PAGE || refreshMsg->mElementType == ANIMATION_REFRESH_DOUBLE_BUFFER) //页面则更新整个包围盒
	{
		curPtr.pageptr = gPagePtr;
		SourceBox.sourceReCompute(curPtr,ANIMATION_REFRESH_PAGE, &(gPagePtr[WorkingPageID].mPageMatrix) );
	}
	else 
	{
		taskEXIT_CRITICAL();
		return AHMI_FUNC_FAILURE;
	}

	SourceBox.adjustSourceBuffer();

	//写入背景
	sourcebufferaddr = (u8 *)SoureBufferAddr;
	//if(refreshMsg->mElementType != ANIMATION_REFRESH_PAGE) //动画切换的时候在函数内部写背景
	//{
	currenttexture = gPagePtr[WorkingPageID].pBackgroundTexture;
	currenttexture->writeSourceBuffer( &sourceshift, &(gPagePtr[WorkingPageID].mPageMatrix) );
	//}

	taskENTER_CRITICAL();
	if(refreshMsg->mElementType == ANIMATION_REFRESH_PAGE) //页面更新
	{
		refreshMsg->mElementPtr.pageptr->drawPage(&SourceBox,&sourceshift,1,ANIMATION_REFRESH_PAGE);
	}
	else if(refreshMsg->mElementType == ANIMATION_REFRESH_DOUBLE_BUFFER)
	{
		refreshMsg->mElementPtr.pageptr->drawPage(&SourceBox,&sourceshift,0,ANIMATION_REFRESH_PAGE);
	}
	taskEXIT_CRITICAL();

	if(SourceBox.LeftBox > (s8)(TILE_NUM_X-1)) return AHMI_FUNC_SUCCESS;
	if(SourceBox.RightBox > (s8)(TILE_NUM_X-1)) SourceBox.RightBox = (s8)(TILE_NUM_X-1);
	if(SourceBox.TopBox > (s8)(TILE_NUM_Y-1)) return AHMI_FUNC_SUCCESS;
	if(SourceBox.ButtomBox > (s8)(TILE_NUM_Y-1)) SourceBox.ButtomBox = (s8)(TILE_NUM_Y-1);

	*(stm32ptraddr + Tile_start_x) = SourceBox.LeftBox ;
	*(stm32ptraddr + Tile_start_y) = SourceBox.TopBox ;
	*(stm32ptraddr + Tile_end_x)   = SourceBox.RightBox;
	*(stm32ptraddr + Tile_end_y)   = SourceBox.ButtomBox ;


	//»æÍ¼
	if (sourceshift<SoureBufferSize-1)//sourcebufferÊ£Óà²»Ð¡ÓÚ16itsÊ±£¬¼Ó½áÊø·û
	{//½¨ÒéÖØÐÂÉèÖÃÍ·µÄÅÅÐò£¬¿ÉÒÔ½«end·ÅÔÚµÚÒ»¸öbit£¬¿ÉÒÔ±ÜÃâ¶àÓàµÄÅÐ¶Ï
		*(sourcebufferaddr + sourceshift++) = ENDFLAG;//texsecondheader = \0010 0000
	}

	if( refreshMsg->mElementType == ANIMATION_REFRESH_PAGE || refreshMsg->mElementType == ANIMATION_REFRESH_DOUBLE_BUFFER)
		AHMI_draw(&sourceshift);


	//send semaphore
	if( (refreshMsg->mElementType == ANIMATION_REFRESH_PAGE || refreshMsg->mElementType == ANIMATION_REFRESH_DOUBLE_BUFFER) && ahmi_init != 1)
	{
		//addr_disp = (addr_combine & 0x38) >> 3;
		addr_n = 1;
		while( xSemaphoreGive( drawSemaphore ) != pdTRUE )  //switch display buffer
		{  
			;//return AHMI_FUNC_FAILURE;
		} 
	}


	return AHMI_FUNC_SUCCESS;
}

#endif

#ifdef PARTIAL_TRIBLE_BUFFER//partial trible buffer

//widget refresh task
void WidgetRefreshTask(void* pvParameters)
 {
 	AnimationMsg animationMsg;
 	BaseType_t xStatus; 
#ifndef WHOLE_TRIBLE_BUFFER
	u8 numOfRefreshQueue;
#endif
#ifdef PC_SIM
	ahmi mahmi(gScreenWidth, gScreenHeight); 
	myahmi = &mahmi; 
#endif
 	for(;;) 
 	{
		//??????
		//taskENTER_CRITICAL();
#ifndef WHOLE_TRIBLE_BUFFER
		//numOfRefreshQueue = (u8)uxQueueMessagesWaiting(RefreshQueue);
		//while(numOfRefreshQueue > 0)
		//{
			while(uxQueueMessagesWaiting(RefreshQueueWithoutDoubleBuffer) != 0)
			{
				xStatus=xQueueReceive(RefreshQueueWithoutDoubleBuffer,
 								(void*)&animationMsg,
 								portMAX_DELAY);
 				if(xStatus!=pdPASS)
 				{
 					return;
 				}
				drawimmediately(&animationMsg); //????????
			}
#endif
 			xStatus=xQueueReceive(RefreshQueue,
 								(void*)&animationMsg,
 								portMAX_DELAY);
 			if(xStatus!=pdPASS)
 			{
 				return;
 			}
			drawimmediately(&animationMsg);
#ifndef WHOLE_TRIBLE_BUFFER
		//	numOfRefreshQueue --;
#endif
#ifdef PC_SIM
			InvalidateRect(ViewHWND,NULL,FALSE);
#endif
		//}
  		//??????
		//vTaskDelay(SCREEN_UPDATE_TIME / portTICK_RATE_MS);
 	}
 }
//-----------------------------
// ???:  drawimmediately
// ??????
// ????:
// @param1 AnimationMsg* pAnimationMsg    ????
// @param2 u8 refreshType                 ????,1??????????,?????0?????????
// ??(?????????):
// ...
//-----------------------------
funcStatus drawimmediately(AnimationMsg* pAnimationMsg)
{
	
	TileBoxClass SourceBox;
	u8 *stm32ptraddr;
	u8 *sourcebufferaddr;
	TextureClassPtr  currenttexture;
	u32 sourceshift = 0;
	CanvasClassPtr drawingCanvas;
	//u16 animationType;
	matrixClass matrixTemp;
	AnimationMsg animationMsg;
	u8 doubleBufferEn = 0;
	//TextureClass    doubleBufferTexture;
	//u8 addr_w;
	//u8 addr_n;
	//u8 ahmi_init = 0;

	//if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_PAGE && ahmi_first != 0)
	//	return AHMI_FUNC_SUCCESS;
	//else if(pAnimationMsg->RefreshType != ANIMATION_REFRESH_DOUBLE_BUFFER && pAnimationMsg->RefreshType != ANIMATION_REFRESH_PAGE)
	//	return AHMI_FUNC_SUCCESS;
	
	if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_DOUBLE_BUFFER) //need to switch write buffer
	{
		//addr_w = 0;
		
		if(ahmi_first == 0)
		{
			ahmi_first = 1; // r->2, w->1, n->0
			ahmi_initial = 0;
			addr_n = 0;
			addr_w = 1;
			addr_r = 0;
		}
		else 
		{
			if(addr_w == addr_r)
				addr_w = (addr_w + 2) % 3; //w += 2
			else if( (addr_w % 3) == ((addr_r + 1)%3) )
				addr_w = (addr_w + 1) % 3; //w += 1
			//addr_n = addr_r; //n -> r
 			ahmi_initial = 1;
		}
		
	}
	stm32ptraddr = (u8 *)Stm32_interface_Addr;
	*( (u32*)(stm32ptraddr + addr_ahmi_0) ) = addr_w * (SIZE_OF_DISPLAY_BUFFER);
	*( (u32*)(stm32ptraddr + addr_disp_0) )	= addr_r * (SIZE_OF_DISPLAY_BUFFER);

	//计算包围盒
	if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_PAGE) //页面则更新整个包围盒
	    pAnimationMsg->Old_ElementPtr.pageptr->refreshWidgetBox(NULL, NULL, NULL,0, 0, 0, 0, &SourceBox);
	else if(pAnimationMsg->RefreshType ==ANIMATION_REFRESH_CANVAS) //更新canvas
	{
		drawingCanvas = gPagePtr[WorkingPageID].pCanvasList+pAnimationMsg->Old_ElementPtr.scptr->attachCanvas;
		gPagePtr[WorkingPageID].refreshWidgetBox(NULL, drawingCanvas,NULL, 0, 0, 1, 0, &SourceBox);
	}
	else if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_WIDGET) //绘制widget包围盒
		gPagePtr[WorkingPageID].refreshWidgetBox(pAnimationMsg->Old_ElementPtr.wptr,NULL, NULL, 0,1,0, 0, &SourceBox);
	else if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_TEXTURE) //绘制texture,仍然使用widget包围盒
		gPagePtr[WorkingPageID].refreshWidgetBox(pAnimationMsg->Old_ElementPtr.wptr,NULL, NULL,1,0,0,0, &SourceBox);
	else if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_DOUBLE_BUFFER) //更新双缓存，包围盒为纹理的包围盒
	{
		SourceBox.LeftBox = 0;
		SourceBox.TopBox = 0;
		SourceBox.RightBox =  TILE_NUM_X - 1;
		SourceBox.ButtomBox = TILE_NUM_Y - 1;
	}
	else
		return AHMI_FUNC_FAILURE;

	SourceBox.adjustSourceBuffer();

	//写入背景
	sourcebufferaddr = (u8 *)SoureBufferAddr;
	if(pAnimationMsg->RefreshType != ANIMATION_REFRESH_PAGE && pAnimationMsg->RefreshType != ANIMATION_REFRESH_DOUBLE_BUFFER) //动画切换的时候在函数内部写背景
	{
		currenttexture = gPagePtr[WorkingPageID].pBackgroundTexture;
		currenttexture->writeSourceBuffer( &sourceshift, &(gPagePtr[WorkingPageID].mPageMatrix), 0, NULL );
	}

	if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_PAGE) //页面更新
	{
		pAnimationMsg->Old_ElementPtr.pageptr->drawPageWithAnimation(
			pAnimationMsg->Old_ElementPtr.pageptr,
			pAnimationMsg->New_ElementPtr.pageptr,
			pAnimationMsg->Now_Frame,
			pAnimationMsg->Total_Frame,
			&SourceBox,
			&sourceshift
			);
	}
	else if(pAnimationMsg->RefreshType ==ANIMATION_REFRESH_CANVAS) //更新canvas
	{
		drawingCanvas = gPagePtr[WorkingPageID].pCanvasList+pAnimationMsg->Old_ElementPtr.scptr->attachCanvas;
		drawingCanvas->drawCanvasWithAnimation(
			pAnimationMsg->Old_ElementPtr.scptr,
			pAnimationMsg->New_ElementPtr.scptr,
			pAnimationMsg->Now_Frame,
			pAnimationMsg->Total_Frame,
			&SourceBox,
			&sourceshift
			);
	}
	else if(pAnimationMsg->RefreshType ==ANIMATION_REFRESH_WIDGET) //更新控件
	{
		//animationType = pAnimationMsg->Old_ElementPtr.wptr->ANIMATION_TYPE; //低字节表示动画类型
		pAnimationMsg->Old_ElementPtr.wptr->drawWidgetWithAnimation(
			pAnimationMsg->Now_Frame,
			pAnimationMsg->Total_Frame,
			&SourceBox,
			&sourceshift
			);
	}
	else if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_TEXTURE)
	{
		//animationType = pAnimationMsg->Old_ElementPtr.wptr->ANIMATION_TYPE;
		pAnimationMsg->Old_ElementPtr.wptr->drawTextureWithAnimation(
			pAnimationMsg->Now_Frame,
			pAnimationMsg->Total_Frame,
			&SourceBox,
			&sourceshift
			);
	}
	else if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_DOUBLE_BUFFER)
	{
		
		matrixTemp.A = (1 << 9);
		matrixTemp.B = 0;
		matrixTemp.C = 0;
		matrixTemp.D = (1 << 9);
		matrixTemp.E = 0;
		matrixTemp.F = 0;
		
			doubleBufferTexture.OffsetX = 0;//(*(stm32ptraddr + Tile_start_x)) << 9;//(4 + 5);
			doubleBufferTexture.OffsetY = 0;//(*(stm32ptraddr + Tile_start_y)) << 9;//(4 + 5);
#ifdef PC_SIM
			doubleBufferTexture.TexWidth  = gScreenWidth;//( *(stm32ptraddr + Tile_end_x) - *(stm32ptraddr + Tile_start_x) + 1 ) << 5;
			doubleBufferTexture.TexHeight = gScreenHeight;//( *(stm32ptraddr + Tile_end_y) - *(stm32ptraddr + Tile_start_y) + 1) << 5;
#endif
#ifdef EMBEDDED
			doubleBufferTexture.TexWidth  = SCREEN_WIDTH;//( *(stm32ptraddr + Tile_end_x) - *(stm32ptraddr + Tile_start_x) + 1 ) << 5;
			doubleBufferTexture.TexHeight = SCREEN_HEIGHT;//( *(stm32ptraddr + Tile_end_y) - *(stm32ptraddr + Tile_start_y) + 1) << 5;
#endif
			doubleBufferTexture.FocusedSlice = 0;
			doubleBufferTexture.RotateAngle = 0;
			doubleBufferTexture.ScalerX = 512;
			doubleBufferTexture.ScalerY = 512;
			doubleBufferTexture.ShearAngleX = 0;
			doubleBufferTexture.SingleSliceSize = 0;
#ifdef PC_SIM
			doubleBufferTexture.TexAddr = (0x8000000) + ADDR_MULTI_BASE;
#endif
#ifdef EMBEDDED
			if(addr_w == 0)
				doubleBufferTexture.TexAddr = (0x8000000) + (SIZE_OF_DISPLAY_BUFFER << 1);
			else if(addr_w == 1)
				doubleBufferTexture.TexAddr = (0x8000000) ;
			else if(addr_w == 2)
				doubleBufferTexture.TexAddr = (0x8000000) + (SIZE_OF_DISPLAY_BUFFER);
			else 
				return AHMI_FUNC_FAILURE;
#endif
			doubleBufferTexture.TexAttr = RGBA8888 | NONMASKTEX | ADDRTEXTURE;
			doubleBufferTexture.TexLeftTileBox = 0;
			doubleBufferTexture.TexRightTileBox =TILE_NUM_X - 1;
			doubleBufferTexture.TexTopTileBox = 0;
			doubleBufferTexture.TexButtomTileBox = TILE_NUM_Y - 1;
		
		doubleBufferTexture.writeSourceBuffer(&sourceshift, &matrixTemp, 0, &SourceBox);
	}


	if(SourceBox.LeftBox > (s8)(TILE_NUM_X-1)) return AHMI_FUNC_SUCCESS;
	if(SourceBox.RightBox > (s8)(TILE_NUM_X-1)) SourceBox.RightBox = (s8)(TILE_NUM_X-1);
	if(SourceBox.TopBox > (s8)(TILE_NUM_Y-1)) return AHMI_FUNC_SUCCESS;
	if(SourceBox.ButtomBox > (s8)(TILE_NUM_Y-1)) SourceBox.ButtomBox = (s8)(TILE_NUM_Y-1);

	*(stm32ptraddr + Tile_start_x) = SourceBox.LeftBox ;
	*(stm32ptraddr + Tile_start_y) = SourceBox.TopBox ;
	*(stm32ptraddr + Tile_end_x)   = SourceBox.RightBox;
	*(stm32ptraddr + Tile_end_y)   = SourceBox.ButtomBox ;

	//判断是否启用双缓存
//	if( pAnimationMsg->RefreshType != ANIMATION_REFRESH_DOUBLE_BUFFER )
//	{
//		judgeDoubleBuffer(&doubleBufferEn);
//		//doubleBufferEn = 1;
//		if(doubleBufferEn)
//		{
//			//addr_w = addr_combine & 0x3;
//			doubleBufferTexture.OffsetX = 0;//(*(stm32ptraddr + Tile_start_x)) << 9;//(4 + 5);
//			doubleBufferTexture.OffsetY = 0;//(*(stm32ptraddr + Tile_start_y)) << 9;//(4 + 5);
//#ifdef PC_SIM
//			doubleBufferTexture.TexWidth  = gScreenWidth;//( *(stm32ptraddr + Tile_end_x) - *(stm32ptraddr + Tile_start_x) + 1 ) << 5;
//			doubleBufferTexture.TexHeight = gScreenHeight;//( *(stm32ptraddr + Tile_end_y) - *(stm32ptraddr + Tile_start_y) + 1) << 5;
//#endif
//#ifdef EMBEDDED
//			doubleBufferTexture.TexWidth  = SCREEN_WIDTH;//( *(stm32ptraddr + Tile_end_x) - *(stm32ptraddr + Tile_start_x) + 1 ) << 5;
//			doubleBufferTexture.TexHeight = SCREEN_HEIGHT;//( *(stm32ptraddr + Tile_end_y) - *(stm32ptraddr + Tile_start_y) + 1) << 5;
//#endif
//			doubleBufferTexture.FocusedSlice = 0;
//			doubleBufferTexture.RotateAngle = 0;
//			doubleBufferTexture.ScalerX = 512;
//			doubleBufferTexture.ScalerY = 512;
//			doubleBufferTexture.ShearAngleX = 0;
//			doubleBufferTexture.SingleSliceSize = 0;
//#ifdef PC_SIM
//			doubleBufferTexture.TexAddr = (0x8000000) + ADDR_MULTI_BASE;
//#endif
//#ifdef EMBEDDED
//			if(addr_w == 0)
//				doubleBufferTexture.TexAddr = (0x8000000);// + (ADDR_MULTI_BASE << 1);
//			else if(addr_w == 1)
//				doubleBufferTexture.TexAddr = (0x8000000) + (SIZE_OF_DISPLAY_BUFFER);
//			else if(addr_w == 2)
//				doubleBufferTexture.TexAddr = (0x8000000) + (SIZE_OF_DISPLAY_BUFFER << 1);
//			else 
//				return AHMI_FUNC_FAILURE;
//#endif
//			doubleBufferTexture.TexAttr = RGBA8888 | NONMASKTEX | ADDRTEXTURE;
//			doubleBufferTexture.TexLeftTileBox = 0;
//			doubleBufferTexture.TexRightTileBox =TILE_NUM_X - 1;
//			doubleBufferTexture.TexTopTileBox = 0;
//			doubleBufferTexture.TexButtomTileBox = TILE_NUM_Y - 1;
//		}
//	}

	//绘图
	if (sourceshift<SoureBufferSize-1)//sourcebuffer剩余不小于16its时，加结束符
	{//建议重新设置头的排序，可以将end放在第一个bit，可以避免多余的判断
		*(sourcebufferaddr + sourceshift++) = ENDFLAG;//texsecondheader = \0010 0000
	}
	//taskENTER_CRITICAL();
	AHMI_draw(&sourceshift);
	//taskEXIT_CRITICAL();

		ahmi_first_frame = 1;
	
	//发送一次立即绘制队列
//	if(pAnimationMsg->RefreshType != ANIMATION_REFRESH_DOUBLE_BUFFER && doubleBufferEn)
//	{
//		animationMsg.Now_Frame = 0;
//		animationMsg.Total_Frame = 0;
//		animationMsg.Old_ElementPtr.tptr = &doubleBufferTexture;
//		animationMsg.New_ElementPtr.tptr = &doubleBufferTexture;
//		animationMsg.RefreshType = ANIMATION_REFRESH_DOUBLE_BUFFER;
//		xQueueSendToBack(RefreshQueueWithoutDoubleBuffer,&animationMsg,portMAX_DELAY);
//		
//	}
	
	
	//send semaphore
	
	if(pAnimationMsg->RefreshType == ANIMATION_REFRESH_DOUBLE_BUFFER && ahmi_initial != 0)
	{
		if( xSemaphoreGive( drawSemaphore ) != pdTRUE )  //switch display buffer
		{  
         return AHMI_FUNC_FAILURE;
		} 
	}

	
	return AHMI_FUNC_SUCCESS;
}
#endif

#endif

//-----------------------------
// 函数名：  freshen
// 绘图更新函数
// 参数列表：
// @param1 u32*   sourceshift            sourcebuffer偏移量
// @param2 8 *pflag                       无用
// 备注(各个版本之间的修改):
// ...
//-----------------------------
void AHMI_draw(u32 * sourceshift)
{
#ifdef PC_SIM
#ifdef TEST_VECTOR_GEN
	FILE* fp;
	u32 i;
	fp =fopen("sourcebuffer.hex","w");
	for(i=0;i<* sourceshift;i+=2)
	{
		fprintf(fp,"%04X\n",*(u16*)(&sourceBuffer[i]));
	}
	fclose(fp);
#endif
	*sourceshift = 0;
	//u32 addr_ahmi = ( (stm32info[Addr_combine] & 0x07) * (ADDR_MULTI_BASE) );//Addr_combine[2:0] * 0x200000
	//if(addr_ahmi == ADDR_MULTI_BASE)
		myahmi->DrawFrame(GlobalFrameBuffer,0,0);
	//else 
	//	myahmi->DrawFrame(GlobalFrameBuffer2,1,0);  //将数据绘制到另一块缓存中
#endif
#ifdef EMBEDDED	
	AHMIDraw(sourceshift);
#endif
}

//-----------------------------
// 函数名：  adjustDoubleBuffer
// 绘图更新函数
// 参数列表：
// @param1 u32*   sourceshift            sourcebuffer偏移量
// 备注(各个版本之间的修改):
//     created by zuz 20160822
//-----------------------------
#ifdef PARTIAL_DOUBLE_BUFFER
#ifdef EMBEDDED
funcStatus judgeDoubleBuffer(u32* sourceshift, u8 *stm32ptraddr, u8 *doubleBufferEn)
{
	u32 i = 0;
	u32 textureCount = 0;
	u16 texAttr;
	u8 SB_Matrix;
	u8 SB_AddrType;
	u8 shiftSize = 0;
	u8 *sourcebufferaddr = (u8 *)SoureBufferAddr;

	
	while( i < (*sourceshift) )
	{
		texAttr = *(u16 *)(sourcebufferaddr + i);
		SB_Matrix  = (texAttr & ABCDEFMATRIX) >> 1;
		SB_AddrType = (texAttr & ADDRTEXTURE) >> 8;
		shiftSize = TexMinSize
		+ (((SB_Matrix+1)>>1) << 2)
		+ (SB_AddrType << 2);//用以记录指针的偏移字节总数
		textureCount ++;
		if(textureCount >= DOUBULE_BUFFER_TEXTURE_SIZE)
			break;
		i += shiftSize;
	}

	if(textureCount >= DOUBULE_BUFFER_TEXTURE_SIZE)
	{
		//写入缓存中
//		*(stm32ptraddr + Addr_combine) = 0x0A ;
//		*(stm32ptraddr + BUFFER_WIDTH) =  ( *(stm32ptraddr + Tile_end_x) - *(stm32ptraddr + Tile_start_x) + 1) ;
		*doubleBufferEn = 1;

		//发送一次立即绘制
		doubleBufferTexture.OffsetX = 0;//(*(stm32ptraddr + Tile_start_x)) << 9;//(4 + 5);
		doubleBufferTexture.OffsetY = 0;//(*(stm32ptraddr + Tile_start_y)) << 9;//(4 + 5);
#ifdef PC_SIM
		doubleBufferTexture.TexWidth  = globalArgs.ScreenWidth;//( *(stm32ptraddr + Tile_end_x) - *(stm32ptraddr + Tile_start_x) + 1 ) << 5;
		doubleBufferTexture.TexHeight = globalArgs.ScreenHeigth;//( *(stm32ptraddr + Tile_end_y) - *(stm32ptraddr + Tile_start_y) + 1) << 5;
#endif
#ifdef EMBEDDED
		doubleBufferTexture.TexWidth  = SCREEN_WIDTH;//( *(stm32ptraddr + Tile_end_x) - *(stm32ptraddr + Tile_start_x) + 1 ) << 5;
		doubleBufferTexture.TexHeight = SCREEN_HEIGHT;//( *(stm32ptraddr + Tile_end_y) - *(stm32ptraddr + Tile_start_y) + 1) << 5;
#endif
		doubleBufferTexture.FocusedSlice = 0;
		doubleBufferTexture.RotateAngle = 0;
		doubleBufferTexture.ScalerX = 512;
		doubleBufferTexture.ScalerY = 512;
		doubleBufferTexture.ShearAngleX = 0;
		doubleBufferTexture.SingleSliceSize = 0;
#ifdef PC_SIM
		doubleBufferTexture.TexAddr = (0x8000000) + ADDR_MULTI_BASE;
#endif
#ifdef EMBEDDED
		doubleBufferTexture.TexAddr = (0x8000000) + (ADDR_MULTI_BASE << 1);
#endif
		doubleBufferTexture.TexAttr = RGB565 | NONMASKTEX | ADDRTEXTURE;
		doubleBufferTexture.TexLeftTileBox = *(stm32ptraddr + Tile_start_x);
		doubleBufferTexture.TexRightTileBox = *(stm32ptraddr + Tile_end_x);
		doubleBufferTexture.TexTopTileBox = *(stm32ptraddr + Tile_start_y);
		doubleBufferTexture.TexButtomTileBox = *(stm32ptraddr + Tile_end_y);

	}
	else 
	{
		*doubleBufferEn = 0;
	}

	return AHMI_FUNC_SUCCESS;
}
#endif

#endif

#ifdef EMBEDDED
#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)


//-----------------------------
// 函数名  swtichDispBufferTask
// 交换缓存的任务
// 参数列表:
// 备注:
//     created by zuz 20160822
//     used to switch display buffer
//-----------------------------
void swtichDispBufferTask(void* pvParameters)
{

	u32 WaitDisplayDoneCount = 0;
	for(;;)
	{
		if(runningControl > 0)
			continue;
		
//		xSemaphoreTake( xFPGASemaphore, portMAX_DELAY );
		
		if(xSemaphoreTake(drawSemaphore, portMAX_DELAY) == pdTRUE) //switch disp buffer
		{
			
			//taskENTER_CRITICAL();
			//addr_disp = (addr_combine & 0x38) >> 3;
			//
			//addr_disp = (addr_disp + 2) % 6;
			//addr_none = addr_disp;
			//
			//addr_combine = (addr_combine & 0x07) + (addr_none << 6) + (addr_disp << 3);
			
			
			addr_r = (addr_r + 1) % 3;
			

			//SPI_FPGA_Burst_Send_REG_data(AHMICFG_ADDR_COMBO,(u16*)(stm32info + AHMICFG_ADDR_COMBO),1);
			//stm32_regs
//			*(stm32info + Addr_combine) = addr_combine;
      *( (u32*)(stm32info + addr_disp_0) ) = START_ADDR_OF_DISPLAY + addr_r * (SIZE_OF_DISPLAY_BUFFER);
			
			taskENTER_CRITICAL();
			SPI_FPGA_Burst_Send_REG_data(AHMICFG_DISPADDRBASE0,(u16*)&(stm32info[addr_disp_0]), 4 / 2);
			taskEXIT_CRITICAL();
			
			//taskENTER_CRITICAL();
			while(WaitDisplayDone() == 0)
			{
				WaitDisplayDoneCount++;
				vTaskDelay( 1 / portTICK_RATE_MS );
				if(WaitDisplayDoneCount > 2000)
				{
					resetFPGA();
					initAHMI = 0;
					//i = *(int *)0xFFFFFFFF;
					ahmi_init(stm32info);
					addr_n = 0;
					WaitDisplayDoneCount = 0;
					break;
				}
			}
			//taskEXIT_CRITICAL();
			addr_n = 0;
			WaitDisplayDoneCount = 0;
			
			
		}
//		xSemaphoreGive( xFPGASemaphore );
		//else 
		//	return;
	}
}

#endif
#endif

#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
//-----------------------------
//     screenUpdateTask
//     created by zuz 20160822
//     used to update the whole screen
//-----------------------------
void screenUpdateTask(void* pvParameters)
{
	RefreshMsg refreshMsg;
	for(;;)
	{
#ifdef WHOLE_TRIBLE_BUFFER
		refreshMsg.mElementType = ANIMATION_REFRESH_DOUBLE_BUFFER;
		refreshMsg.mElementPtr.pageptr = &(gPagePtr[WorkingPageID]);
		if(uxQueueMessagesWaiting(RefreshQueue) < 10)
			xQueueSendToBack(RefreshQueue,&refreshMsg,0);
		
		if(screenUpdateCount < 10 && staticRefresh == 1)
		{
			screenUpdateCount ++;
		}
		else if(screenUpdateCount >= 10 && staticRefresh == 1)
		{
			screenUpdateCount = 0;
			staticRefresh = 0;
		}
#endif	
#ifdef PARTIAL_TRIBLE_BUFFER
		if(addr_n == 0 && 	ahmi_first_frame == 1)
		{
			addr_n = 1;
			animationMsg.Now_Frame = 0;
			animationMsg.Total_Frame = 0;
			animationMsg.Old_ElementPtr.tptr = NULL;
			animationMsg.New_ElementPtr.tptr = NULL;
			animationMsg.RefreshType = ANIMATION_REFRESH_DOUBLE_BUFFER;
			xQueueSendToBack(RefreshQueueWithoutDoubleBuffer,&animationMsg,portMAX_DELAY);
		}
#endif
		vTaskDelay( SCREEN_UPDATE_TIME / portTICK_RATE_MS );

	}
}
#endif


//refresh the drawing source box
funcStatus refreshDrawingTileBox(RefreshMsg* refreshMsg, TileBoxClassPtr pSourceBox)
{
	ElementPtr curPtr;
	if(refreshMsg->mElementType == ANIMATION_REFRESH_PAGE || refreshMsg->mElementType == ANIMATION_REFRESH_DOUBLE_BUFFER || refreshMsg->mElementType == ANIMAITON_REFRESH_STATIC_BUFFER) //页面则更新整个包围盒
	{
		curPtr.pageptr = gPagePtr;
	    pSourceBox->sourceReCompute(curPtr,ANIMATION_REFRESH_PAGE, &(gPagePtr[WorkingPageID].mPageMatrix) );
	}
	else 
	{
		return AHMI_FUNC_FAILURE;
	}
	pSourceBox->adjustSourceBuffer();
	return AHMI_FUNC_SUCCESS;
}





#endif
