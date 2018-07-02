////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     main.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Revision 2.02 - add the modbus definition
// Additional Comments:
//    in 2.00, add the canvas initiation
// 
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "publicInclude.h"
#include "mb.h"
#include "UartClass.h"
#include "AHMIBasicDefine.h"
#include "trace.h"
#include "TouchTask.h"
#include "FileLoad.h"
#include "keyboard.h"
#include "semphr.h"
#include "interruptTask.h"
#include "ahmirtc.h"
#include "AHMIInterface.h"
#include "task.h"
#include "semphr.h"
#ifdef EMBEDDED
#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "spi_if.h"
#include "i2c_aw2083.h"
#include "usb_dfu.h"
#include "port.h"
#include "ahmiv3_vd.h"
#include "bsp_usart.h"
#include "ahmi3_function.h"
#include "GetChipInfo.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_exti.h"
#include "IOConfig.h"
#include "stm32f10x_pwr.h"
#include "location.h"
#include "spi_nand_flash.h"
#endif
#ifdef MODULE_TEST_EN
#include "moduleTest.h"
#endif

#ifdef AHMI_CORE


#ifdef EMBEDDED
#define Lightness               359             ////// (Lightness+1)/720*100%??????±???? 
#define EMPTYPOINTERSIZE					16
void Comm_Init(AHMIPinCfg* AHMIpincfg);
#endif

funcStatus InitialAHMIStruct(AHMIPinCfg* AHMIpincfg);

#ifdef EMBEDDED
extern PRIVILEGED_DATA TCB_t * volatile pxCurrentTCB;

extern uint32_t __get_PSP(void);
extern uint32_t __get_MSP(void);
#endif

extern  ConfigInfoClass		ConfigData;
extern TimerClassPtr		TimerPtr;
extern DynamicPageClassPtr  gPagePtr;
extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t        RefreshQueueWithoutDoubleBuffer;
extern TagClassPtr			TagPtr;
extern TagUpdateClassPtr    TagUpdatePtr;
extern u8*					TagStringPtr;
extern u8*					TagStringListPtr;
extern u8*                  TagChangedListPtr;
extern u16 gScreenWidth;
extern u16 gScreenHeight;
extern u8 animationDuration;  //animation duration
extern AnimationClass  gAnimationClass;

extern UartClassPtr			UartPtr;
extern QueueHandle_t		ActionInstructionQueue;
#ifdef PC_SIM
extern "C" extern QueueHandle_t		MouseQueue;
extern"C" extern struct globalArgs_t globalArgs;
#endif
extern QueueHandle_t		UartDataRecvQueue;
extern QueueHandle_t		TimerCmdQueue;
extern QueueHandle_t		AnimationTimerCmdQueue;
extern QueueHandle_t		ModbusMasterRWTaskQueue;
extern QueueHandle_t		AnimationTaskQueue;//任务队列，存储的是消息列表的中需要刷新的信息位置信息
extern QueueHandle_t        keyboardQueue;


extern xTaskHandle ModBusMasterRWTaskHandle;
extern xTaskHandle ModBusSlaveTaskHandle;
extern xTaskHandle ModBusMasterReadWaiteHandle;
extern xTaskHandle ModBusMasterTaskHandle;
extern xTaskHandle MouseTouchTaskHandle;
extern xTaskHandle KeyBoardInputTaskHandle;
extern xTaskHandle RTCReadTaskHanler;
#ifdef VIDEO_EN
extern xTaskHandle InterruptTaskHandle;
#endif
extern xTaskHandle WidgetRefreshTaskHandle;
extern xTaskHandle ActionProcessorTaskHandle;
extern xTaskHandle UartDataReceiveTaskHandle;
extern xTaskHandle TimerProcessTaskHandle;
extern xTaskHandle AnimationTimerProcessTaskHandle;
extern xTaskHandle TagUpdateTaskHandle;
extern xTaskHandle TagSetBindingElementTaskHandle;
extern xTaskHandle AnimationTaskHandle;
#ifdef MODULE_TEST_EN 
extern xTaskHandle moduleTestTaskHandler;
#endif


xTaskHandle ServerTaskHandle;
xTaskHandle TestFPGAResetTaskHandle;
#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
extern xTaskHandle switchBufferTaskHandle;
#endif
#ifdef WHOLE_TRIBLE_BUFFER
extern xTaskHandle screenUpdateTaskHandle;
#endif

extern void*    PageSpace;
extern 	u16    	WorkingPageID;
extern u16 startAddrOfTagChangedList;
extern u16 stopAddrOfTagChangedList;
typedef u32 		pointersize;

extern u32 startOfDynamicPage;
extern u32 endOfDynamicPage;

extern SemaphoreHandle_t interuptSemaphore;
extern SemaphoreHandle_t drawSemaphore;

//config information string
extern char* cfgString;
#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
extern u16     addr_combine;
#endif

uint8_t runningControl = 0;
uint8_t testRunningControl = 0;
SemaphoreHandle_t xFPGASemaphore = NULL;

#ifdef EMBEDDED

extern uint32_t flag_Calibration;
extern u8		   	stm32info[];

extern u8 		initAHMI;
#endif

#ifdef PC_SIM
extern u8				TextureBuffer[TEXBUFSIZE];
extern u32		TileRenderCounter;
extern u32		FrameRate;
extern u8 animationDuration;


void TileRenderCNTTask(void* pvParameters)
{
	char log[100];
	FrameRate=0;
	TileRenderCounter=0;
	for(;;)
	{
		vTaskDelay(1000);
		WriteText(log,"Tile Render Rate:%d.\n\r",TileRenderCounter);
		ERROR_PRINT(log);
		TileRenderCounter = 0;
		WriteText(log,"AHMI Call Rate:%d.\n\r",FrameRate);
		ERROR_PRINT(log);
		FrameRate = 0;
	}
}
#endif

#ifdef EMBEDDED
#ifdef LOW_POWER_TEST
void OpenAHMI()
{
	OpenPower();
	vTaskDelay(10 / portTICK_RATE_MS);
//	ScreenPowerOn();
	initAHMI = 0;
	resetFPGA();
	ahmi_init(stm32info);
	runningControl = 0;
}

void CloseAHMI(void)
{
	runningControl = 1;
	vTaskDelay(20 / portTICK_RATE_MS);
	ScreenPowerOff();
	ClosePower();
}
#endif
#endif

#ifdef EMBEDDED
#ifdef LOW_POWER_TEST
void TestFPGAResetTask(void* pvParameters)
{
	testRunningControl = 0;
	for(;;)
	{
		testRunningControl = 0;
//		OpenPower();
//		vTaskDelay(20 / portTICK_RATE_MS);		
//		runningControl = 0;
		vTaskDelay(5000 / portTICK_RATE_MS);
//		vTaskDelay(20000 / portTICK_RATE_MS);
//		
//		vTaskDelay(20000 / portTICK_RATE_MS);
//		vTaskDelay(20000 / portTICK_RATE_MS);

//		testRunningControl = 1;
//		runningControl = 1;
//		vTaskDelay(20 / portTICK_RATE_MS);
//		ClosePower();
//		resetFPGA();
//		ahmi_init(stm32info);
		vTaskDelay(5000 / portTICK_RATE_MS);
//		vTaskDelay(20000 / portTICK_RATE_MS);
//		
//		vTaskDelay(20000 / portTICK_RATE_MS);
//		vTaskDelay(20000 / portTICK_RATE_MS);

	}
}


void ServerTask(void* pvParameters)
{
	runningControl = 0;
	for(;;)
	{
		if((testRunningControl == 0) && (runningControl == 1))
		{
			OpenAHMI();
		}
		
//		OpenPower();
//		vTaskDelay(20 / portTICK_RATE_MS);		
//		runningControl = 0;
//		vTaskDelay(10000 / portTICK_RATE_MS);
//		vTaskDelay(20000 / portTICK_RATE_MS);
//		
//		vTaskDelay(20000 / portTICK_RATE_MS);
//		vTaskDelay(20000 / portTICK_RATE_MS);
		else if(testRunningControl == 1)
		{
			if(xSemaphoreTake( xFPGASemaphore, portMAX_DELAY ) == pdTRUE)
				CloseAHMI();
			xSemaphoreGive( xFPGASemaphore );
		}
		
//		runningControl = 1;
//		vTaskDelay(20 / portTICK_RATE_MS);
//		ClosePower();
//		resetFPGA();
//		ahmi_init(stm32info);
		vTaskDelay(20 / portTICK_RATE_MS);
//		vTaskDelay(20000 / portTICK_RATE_MS);
//		
//		vTaskDelay(20000 / portTICK_RATE_MS);
//		vTaskDelay(20000 / portTICK_RATE_MS);

	}
}
#endif
#endif

//-----------------------------
// 函数名： initAHMIHardware
// 顶层输入接口
// 参数列表：
// 备注(各个版本之间的修改):
//-----------------------------

void initAHMIHardware(AHMIPinCfg* AHMIpincfg)
{
#ifdef EMBEDDED
	int i;	
	PowerIOConfig();
	OpenPower();
	//for(i = 0; i < 2000000;)
	//{
	//	i++;
	//}
	GetChipID();
	resetFPGA();
	vPortInitialiseBlocks();
	pucAlignedHeapInitialise();
	TaskInitialParam();
	croutineInitialParam();
	initialSTMFlashReadBaseAddr();
	testscreen();
	Comm_Init(AHMIpincfg);
#endif
}

void initQueue()
{
	 ActionInstructionQueue = xQueueCreate(50,sizeof(pointersize));
	 RefreshQueue = xQueueCreate(50,sizeof(RefreshMsg));
	 RefreshQueueWithoutDoubleBuffer = xQueueCreate(3,sizeof(RefreshMsg));
	keyboardQueue = xQueueCreate(5,sizeof(u8)); //the queue for keyboard
#ifdef PC_SIM	
	MouseQueue = xQueueCreate(5,sizeof(PIDState)); 
#endif
	 TimerCmdQueue = xQueueCreate(10,sizeof(TimerHandle_t));
	 AnimationTimerCmdQueue = xQueueCreate(10,sizeof(TimerHandle_t));
	 AnimationTaskQueue = xQueueCreate(10, sizeof(AnimationMsg));
}

int startAHMI(AHMIPinCfg* AHMIpincfg)
{
	int err;
#ifdef EMBEDDED	
	int i;
	u8 *stm32ptraddr;
#endif
	portBASE_TYPE xReturn;
	
#ifdef EMBEDDED
	xFPGASemaphore = xSemaphoreCreateMutex();
#endif
	
	if(AHMIpincfg->USETOUCH == 1)
	{
		xReturn = xTaskCreate(MouseTouchTask,"MouseTouchTask",configMINIMAL_STACK_SIZE,NULL,2,&MouseTouchTaskHandle);
		if(xReturn != pdTRUE) 
		{
			ERROR_PRINT("Fail to create MouseTouch Task.\r\n");
			return 0;
		}
	}
#ifdef EMBEDDED
	#ifdef LOW_POWER_TEST
	 xReturn = xTaskCreate(TestFPGAResetTask,"TestFPGAResetTask",configMINIMAL_STACK_SIZE,NULL,2,&TestFPGAResetTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create WidgetRefreshTask Task.\r\n");
		 return 0;
	 }

	 xReturn = xTaskCreate(ServerTask,"ServerTask",configMINIMAL_STACK_SIZE,NULL,3,&ServerTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create WidgetRefreshTask Task.\r\n");
		 return 0;
	 }
	 #endif
#endif
	 xReturn = xTaskCreate(WidgetRefreshTask,"WidgetRefreshTask",0x1000,NULL,2,&WidgetRefreshTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create WidgetRefreshTask Task.\r\n");
		 return 0;
	 }
	 xReturn =  xTaskCreate(ActionProcessorTask,"ActionProcessorTask",configMINIMAL_STACK_SIZE,NULL,2,&ActionProcessorTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create ActionProcessorTask Task.\r\n");
		 return 0;
	 }
	 xReturn =  xTaskCreate(TimerProcessTask,"TimerProcessTask",configMINIMAL_STACK_SIZE,NULL,2,&TimerProcessTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create TimerProcessTask Task.\r\n");
		 return 0;
	 }

	  xReturn =  xTaskCreate(AnimationTimerProcessTask,"TimerProcessTask",configMINIMAL_STACK_SIZE,NULL,2,&AnimationTimerProcessTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create AnimationTimerProcessTask Task.\r\n");
		 return 0;
	 }
	 
	 // add by xt, 20151130
	 /*xReturn  = xTaskCreate(TagUpdateTask,"TagUpdateTask",configMINIMAL_STACK_SIZE,NULL,2,&TagUpdateTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create TagUpdateTask Task.\r\n");
		 return 0;
	 }
*/
	 //animation, add by Zhou Yuzhi
	 xReturn  = xTaskCreate(animationQueueTask,"TagUpdateTask",configMINIMAL_STACK_SIZE,NULL,2,&AnimationTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create TagUpdateTask Task.\r\n");
		 return 0;
	 }

#ifdef MODULE_TEST_EN
	xReturn  = xTaskCreate(moduleTestTask,"moduleTestTask",configMINIMAL_STACK_SIZE,NULL,2,&moduleTestTaskHandler);
	if(xReturn != pdTRUE) 
	{
		 ERROR_PRINT("Fail to create moduleTestTask Task.\r\n");
		 return 0;
	}
#endif

	 //keyboard, added by zuz 20160903
	/* xReturn  = xTaskCreate(keyboardReveiveTask,"keyboardReveiveTask",configMINIMAL_STACK_SIZE,NULL,2,&KeyBoardInputTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create TagUpdateTask Task.\r\n");
		 return 0;
	 }*/

	 //interrupt
#ifdef VIDEO_EN
	 xReturn  = xTaskCreate(interuptTask,"interuptTask",configMINIMAL_STACK_SIZE,NULL,2,&InterruptTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create TagUpdateTask Task.\r\n");
		 return 0;
	 }
#endif

#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
#ifdef EMBEDDED
	 //change buffer
	  xReturn = xTaskCreate(swtichDispBufferTask,"swtichDispBufferTask",configMINIMAL_STACK_SIZE,NULL,2,&switchBufferTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create swtichDispBuffer Task.\r\n");
		 return 0;
	 }
#endif
#endif

#ifdef WHOLE_TRIBLE_BUFFER
	 //screen update
	 xReturn = xTaskCreate(screenUpdateTask,"screenUpdateTask",configMINIMAL_STACK_SIZE,NULL,2,&screenUpdateTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create swtichDispBuffer Task.\r\n");
		 return 0;
	 }
#endif

	 //update changed tag, added by zuz
	 //xReturn = xTaskCreate(TagSetBindingElementTask,"TagSetBindingElementTask",configMINIMAL_STACK_SIZE,NULL,2,&TagSetBindingElementTaskHandle);
	 //if(xReturn != pdTRUE) 
	 //{
	//	 ERROR_PRINT("Fail to create TagSetBindingElementTask Task.\r\n");
	//	 return 0;
	 //}


	//semaphore, added by zuz
	interuptSemaphore = xSemaphoreCreateCounting( 10, 0 ); 
	if(interuptSemaphore == NULL)
	{
		ERROR_PRINT("Fail to create semaphore interuptSemaphore.\r\n");
		 return 0;
	}

#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
	drawSemaphore = xSemaphoreCreateCounting( 10, 0 );
	if(drawSemaphore == NULL)
	{
		ERROR_PRINT("Fail to create semaphore drawSemaphore.\r\n");
		 return 0;
	}
	//double buffer, added by zuz
	 //addr_combine = 0; //[7:6] addr_none, [5:3] addr_display [2:0] addr_w
	//addr_combine = 0x0;
#endif



	 err = InitialAHMIStruct(AHMIpincfg);
#ifdef EMBEDDED	
	
#ifndef	DDR_TEST
	if(err != AHMI_FUNC_SUCCESS) 
	{
#endif
		stm32ptraddr = (u8 *)Stm32_interface_Addr;
		//*(stm32ptraddr + Tile_size_x) = TILE_NUM_X;
		//*(stm32ptraddr + Tile_size_y) = TILE_NUM_Y;
		//*(stm32ptraddr + Tile_x_size) = TILESIZE;
		//*(stm32ptraddr + Tile_y_size) = TILESIZE;
		
		*(stm32ptraddr + Tile_start_x) = 0;
		*(stm32ptraddr + Tile_start_y) = 0 ;
		*(stm32ptraddr + Tile_end_x)   = TILE_NUM_X_CFG - 1  ;
		*(stm32ptraddr + Tile_end_y)   = TILE_NUM_Y_CFG - 1 ;
		#ifdef EMBEDDED
		display_start();
		#endif
		//*(stm32ptraddr + Addr_combine) = 0x0;
		for(;;)
		{
			DrawTouchCalibPage(
				SCREEN_WIDTH_CFG,
				SCREEN_HEIGHT_CFG,
				0xFFFFFFFF, //BackgroundColor:white
				EMPTYPOINTERSIZE,
				//LeftTop position of LeftTop Point
				0, 
				0,
				0xFFFF0000, //Pointer0Color:red
				//LeftTop position of RightTop Point
				SCREEN_WIDTH_CFG - EMPTYPOINTERSIZE, 
				0,
				0xFFFF0000, //Pointer1Color:black
				//LeftTop position of LeftBottum Point
				0, 
				SCREEN_HEIGHT_CFG - EMPTYPOINTERSIZE,
				0xFFFF0000, //Pointer1Color:black
				//LeftTop position of RightBottum Point
				SCREEN_WIDTH_CFG - EMPTYPOINTERSIZE, 
				SCREEN_HEIGHT_CFG - EMPTYPOINTERSIZE,
				0xFFFF0000, //Pointer1Color:black
				//LeftTop position of Center Point
				SCREEN_WIDTH_CFG / 2, 
				SCREEN_HEIGHT_CFG / 2,
				0xFFFF0000 //Pointer1Color:black
				);
				for(i = 0; i < 65535; i++)
				{
					i++;
				}
			
		}
#ifndef	DDR_TEST
	}
#endif
#endif	
#ifdef PC_SIM
	if(err != AHMI_FUNC_SUCCESS) 
		return AHMI_FUNC_FAILURE;
	else
		return AHMI_FUNC_SUCCESS;
#endif
#ifdef EMBEDDED		
	return 0;
#endif
	
}

//-----------------------------
// 函数名： InitialAHMIStruct
// 初始化各个结构体以及页面
// 参数列表：
// 备注(各个版本之间的修改):
//-----------------------------
funcStatus InitialAHMIStruct(AHMIPinCfg* AHMIpincfg)
{
#ifdef PC_SIM
	char text[200];
#endif
#ifdef EMBEDDED	
	u8 *stm32ptraddr;
#endif
	int ID;
	int err;
	int i;
	int numOfTagChangedList;
	DynamicPageCfgClass tempDynamicPageCfgClass;
	DynamicPageCfgClass* newPage = &tempDynamicPageCfgClass;

#ifdef PC_SIM	
	if( OpenACFFile() == AHMI_FUNC_FAILURE)
	{
		return AHMI_FUNC_FAILURE;
	}
#endif
	err = LoadConfigData(&ConfigData);
	if(err == AHMI_FUNC_FAILURE)
	{
		ERROR_PRINT("File read error.\r\n");
		return AHMI_FUNC_FAILURE;
	}

	//screen

	gScreenWidth = ConfigData.AHMIInfo.ScreenWidth;
	gScreenHeight = ConfigData.AHMIInfo.ScreenHeight;

	//rtc
#ifdef EMBEDDED
	if(ConfigData.innerClockEn)
	{
//		err  = xTaskCreate(RTCReadTask,"RTCReadTask",configMINIMAL_STACK_SIZE,NULL,2,&RTCReadTaskHanler);
//		if(err != pdTRUE) 
//		{
//			ERROR_PRINT("Fail to create TagUpdateTask Task.\r\n");
//			return 0;
//		}
	}
#endif

	TimerPtr= (TimerClassPtr)pvPortMalloc(sizeof(TimerClass)* ConfigData.NumofTimers);
	if(TimerPtr == NULL && ConfigData.NumofTimers != 0 )
	{
		ERROR_PRINT("Timer Class Initial failed.\r\n");
	}
	gPagePtr = (DynamicPageClassPtr)pvPortMalloc(sizeof(DynamicPageClass) * ConfigData.NumofPages);
	if(gPagePtr == NULL && ConfigData.NumofPages != 0 )
	{
		ERROR_PRINT("Page Initial failed.\r\n");
	}
	TagPtr  = (TagClassPtr)pvPortMalloc(sizeof(TagClass) * ConfigData.NumofTags);
	if(TagPtr == NULL && ConfigData.NumofTags != 0 )
	{
		ERROR_PRINT("Tag Class Initial failed.\r\n"); 
	}

	// change by xt, 20151130
	TagUpdatePtr = (TagUpdateClassPtr)pvPortMalloc(sizeof(TagUpdateClass) * ConfigData.NumofTagUpdateQueue);
	if(TagUpdatePtr == NULL && ConfigData.NumofTagUpdateQueue != 0 )
	{
		ERROR_PRINT("Tag Update Queue Initial failed.\r\n");
	}

	TagStringListPtr = (u8*)pvPortMalloc(ConfigData.TagStringLength);
	if(TagStringListPtr == NULL && ConfigData.TagStringLength != 0 )
	{
		ERROR_PRINT("TagStringListPtr Initial failed.\r\n"); 
		return AHMI_FUNC_FAILURE;
	}

	TagStringPtr = (u8*)pvPortMalloc(ConfigData.StringSize);
	if(TagStringPtr == NULL && ConfigData.StringSize != 0 )
	{
		ERROR_PRINT("TagStringPtr Initial failed.\r\n"); 
		return AHMI_FUNC_FAILURE;
	}
	if(ConfigData.NumofTags % 8 == 0)
		numOfTagChangedList = ConfigData.NumofTags / 8;
	else 
		numOfTagChangedList = ConfigData.NumofTags / 8 + 1;

	TagChangedListPtr = (u8*)pvPortMalloc(numOfTagChangedList * sizeof(u8));
	if(TagChangedListPtr == NULL && ConfigData.NumofTags != 0)
	{
		ERROR_PRINT("TagChangedListPtr Initial failed.\r\n"); 
		return AHMI_FUNC_FAILURE;
	}
	
	for(ID = 0; ID < numOfTagChangedList; ID ++)
	{
		TagChangedListPtr[ID] = 0;
	}
#ifdef USE_UART
	UartPtr = (UartClassPtr)pvPortMalloc(sizeof(UartClass) * ConfigData.NumofUartBuf);
	if(UartPtr == NULL && ConfigData.NumofUartBuf != 0 )
	{
		ERROR_PRINT("Uart Buffer Initial failed.\r\n");
	}
#endif
	PageSpace = pvPortMalloc(ConfigData.DynamicPageSize);
	if(PageSpace == NULL && ConfigData.DynamicPageSize != 0 )
	{
		ERROR_PRINT("PageSpace Initial failed.\r\n");
		return AHMI_FUNC_FAILURE;
	}
	else
	{
		startOfDynamicPage = (u32)PageSpace;
		endOfDynamicPage = (u32)PageSpace + ConfigData.DynamicPageSize;
	}

	animationDuration = 0;

	//page
	for(ID =0;ID<ConfigData.NumofPages;ID++)
	{
#ifdef PC_SIM
			if(ReadPageDataFromFile(newPage) == AHMI_FUNC_FAILURE)
			return AHMI_FUNC_FAILURE;
#endif
#ifdef EMBEDDED
		ReadDataFromSTMFlash(newPage,sizeof(DynamicPageCfgClass));
		if(newPage == NULL)
		{
			return AHMI_FUNC_FAILURE;
		}
#endif
		if(gPagePtr[ID].InitPage(
			 PageSpace,
			 newPage->StoreOffset,
			 newPage->PageAttr,
			 newPage->NumOfWidget,
			 newPage->NumOfCanvas,
			 newPage->NumOfSubCanvas,
			 newPage->ActionInstructionsSize,
			 newPage->totalNumOfTexture,
			 newPage->NumofTouchWidget,
			 newPage->NumOfPageAction,
			 newPage->NumOfCanvasAction,
			 newPage->AnimationType,
			 newPage->enlightedWidgetID,
			 newPage->animationFrame,
			 newPage->mNunOfAnimationActions
		    ) == AHMI_FUNC_FAILURE)
		{
			return AHMI_FUNC_FAILURE;
		}
#ifdef PC_SIM
		sprintf_s(text,200, "Reading %d Page:\r\nStoreOffset is %d.\r\n",
			ID,newPage->StoreOffset);
		ERROR_PRINT(text);
		WriteText(text,"Widget Number is %d.\r\nInstruction size is %d.\r\nTexture number is %d.\r\n",
			newPage->NumOfWidget,newPage->ActionInstructionsSize,newPage->totalNumOfTexture);
		ERROR_PRINT(text);
#endif
	}
	
	for(ID=0;ID<ConfigData.NumofTags;ID++)
	{
#ifdef PC_SIM
		if(ReadTagDataFromFile(&TagPtr[ID], ID) == AHMI_FUNC_FAILURE)
#endif
#ifdef EMBEDDED
		if(ReadTagData(&TagPtr[ID], ID) == AHMI_FUNC_FAILURE)
#endif
			return AHMI_FUNC_FAILURE;
#ifdef PC_SIM		
		WriteText(text,"Reading %d Tag \r\nInit value is %d.\r\n",
			ID,(TagPtr+ID)->mValue);
		ERROR_PRINT(text);
		WriteText(text,"element Link number is %d.\r\n",
			(TagPtr+ID)->mNumOfElementLinker);
		ERROR_PRINT(text);
#endif
	}

	ReadTagStringList(TagStringListPtr,ConfigData.TagStringLength);

	ReadInitString(TagStringPtr, ConfigData.StringSize);

	i = 0;
	for(ID = 0;ID < ConfigData.TagStringLength;ID++)
	{
		TagPtr[TagStringListPtr[ID]].mValue = (u32)&TagStringPtr[i];
		i += (TagPtr[TagStringListPtr[ID]].mTagType >> 1) & 0x1f;
	}

	//initial animation array
	if(gAnimationClass.initAnimationMsg() == AHMI_FUNC_FAILURE)
		return AHMI_FUNC_FAILURE;

	//initial timer
	for(ID=0;ID<ConfigData.NumofTimers;ID++)
	{
		(TimerPtr+ID)->initTimer(ID);
	}
	gAnimationClass.initAnimationTimer(ID);
#ifdef USE_UART
	for(ID=0 ;ID<ConfigData.NumofUartBuf;ID++)
	{
		InitUartBuffer(ID,ConfigData.UartBufSizeforEach,ConfigData.NumofTimers*6+ID);
	}
#endif
	initTagUpdateQueue(ConfigData.NumofTagUpdateQueue, ConfigData.NumofTags, TagPtr, TagUpdatePtr);

#ifdef PC_SIM
	LoadTextureToSDRAM(TextureBuffer,TEXBUFSIZE,ConfigData.TotalTextureSize);
#endif		
#ifdef EMBEDDED	
	if(AHMIpincfg->USETOUCH == 1 )
	{

//		flag_Calibration = 0;
		if((flag_Calibration == 0) || (flag_Calibration == 0xFFFFFFFF))
		{
			stm32ptraddr = (u8 *)Stm32_interface_Addr;
			
			*(stm32ptraddr + Tile_start_x) = 0;
			*(stm32ptraddr + Tile_start_y) = 0 ;
			*(stm32ptraddr + Tile_end_x)   =  TILE_NUM_X - 1  ;
			*(stm32ptraddr + Tile_end_y)   =  TILE_NUM_Y - 1 ;
			*( (u32*)(stm32ptraddr + addr_ahmi_0) ) = START_ADDR_OF_DISPLAY ;
	    *( (u32*)(stm32ptraddr + addr_disp_0) )	= START_ADDR_OF_DISPLAY ;
			
			while(AW2083_Calibration() == 0);	
		}
		else
		{
			if(gPagePtr[0].mAnimationType == NO_ANIMATION)
			{
				if(gPagePtr[WorkingPageID].loadPage(0) == AHMI_FUNC_FAILURE)
				{
#ifdef AHMI_DEBUG
					WriteText(text,"Reading page %d failure.\r\n", WorkingPageID);
					ERROR_PRINT(text);
#endif
					return AHMI_FUNC_FAILURE;
				}
			}
			else
			{
				if(gPagePtr[WorkingPageID].loadPageWithAnimation(0,0,0,Animation_Frame,gPagePtr[0].mAnimationType) == AHMI_FUNC_FAILURE)
				{
#ifdef AHMI_DEBUG
					WriteText(text,"Reading page %d failure.\r\n", WorkingPageID);
					ERROR_PRINT(text);
#endif
					return AHMI_FUNC_FAILURE;
				}
			}
		}

	}
	else
	{
#endif
//		if(gPagePtr[0].mAnimationType == NO_ANIMATION)
//		{
			if(gPagePtr[WorkingPageID].loadPage(0,1) == AHMI_FUNC_FAILURE)
			{
#ifdef AHMI_DEBUG
				WriteText(text,"Reading page %d failure.\r\n", WorkingPageID);
				
				(text);
#endif
				return AHMI_FUNC_FAILURE;
			}
//		}
//		else
//		{
//			if(gPagePtr[WorkingPageID].loadPageWithAnimation(0,0,0,Animation_Frame,gPagePtr[0].mAnimationType) == AHMI_FUNC_FAILURE)
//			//if(gPagePtr[WorkingPageID].loadPage(0,1) == AHMI_FUNC_FAILURE)
//			{
//#ifdef DEBUG
//				WriteText(text,"Reading page %d failure.\r\n", WorkingPageID);
//				ERROR_PRINT(text);
//#endif
//				return AHMI_FUNC_FAILURE;
//			}
//		}
#ifdef EMBEDDED
	}
#endif
	//setRegValue(1,50);
	return AHMI_FUNC_SUCCESS;
}

#ifdef EMBEDDED
void MCO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	// config PA8 for MCO 
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;     
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;     
 GPIO_Init(GPIOA, &GPIO_InitStructure);   
 RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);
}

void RCC_Configuration(void)
{

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
        RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
  
}

void InitIWDG(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		/* Enable the TIM6 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	

	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 5000;    	//下一个更新事件装入活动的自动重装载寄存器周期的值。它的取值必须在0x0000和0xFFFF之间。
  TIM_TimeBaseStructure.TIM_Prescaler = 719;					    //预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分割
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
  TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM6, TIM_FLAG_Update);
	
	  /* Check if the system has resumed from IWDG reset */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
  {
    /* IWDGRST flag set */
    /* Clear reset flags */
    RCC_ClearFlag();
  }
	RCC_LSICmd(ENABLE);
	
	/* Wait till LSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {}
		
	/* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_32);
		
	/* Set counter reload value to obtain 250ms IWDG TimeOut.
     Counter Reload Value = 250ms/IWDG counter clock period
                          = 250ms / (LSI/32)
                          = 0.25s / (LsiFreq/32)
                          = LsiFreq/(32 * 4)
                          = LsiFreq/128
   */
	
	
	IWDG_SetReload(319);
		
	/* Reload IWDG counter */
  IWDG_ReloadCounter();
		
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();	
		
	TIM_SetCounter(TIM6,0x00);			//清零计数器值
  TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIM6,ENABLE);

}



void Comm_Init(AHMIPinCfg* AHMIpincfg)
{
	RCC_Configuration();
	MCO_Init();
	USART2_Config();
	NVIC_Configuration();
//	USART1_Config();
	STM_SPI_Init();
	FPGA_SPI_Init();
//	 Sd_fs_init();         
//	init_frame_buffer();	
	ahmi_init(stm32info);
//	ADV7180Init();
	if(AHMIpincfg->USETOUCH == 1)
	{
		AW2083_InitDevice();
	}
	//InitIWDG();
}

void AHMISystemReset(void)
{
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}

void AHMIEnterStopMode(void)
{
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
}
#endif

unsigned int getTagValue(unsigned int ID)
{
	return TagPtr[ID].getValue();
}
void setTagValue(unsigned int ID, unsigned int value)
{
	TagPtr[ID].setValue(value,ID);
}
void setRegValue(unsigned int ID, unsigned int value)
{
	unsigned int i;
	for(i = 0; i < ConfigData.NumofTags; i++)
	{
		if(TagPtr[i].mRegID == ID)
		{
			TagPtr[i].setValue(value,i);
			return;
		}
	}
}

#endif
