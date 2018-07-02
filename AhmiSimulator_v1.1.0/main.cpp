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
#ifdef EMBEDDED
#include "stm32f10x.h"
#include "stm32f10x_flash.h"
#include "spi_if.h"
#include "i2c_aw2083.h"
#include "usb_dfu.h"
#include "port.h"
#include "ahmiv3_vd.h"
#include "bsp_usart1.h"
#include "ahmi3_function.h"
#include "GetChipInfo.h"
#endif

#ifdef AHMI_CORE


#ifdef EMBEDDED
#define Lightness               359             ////// (Lightness+1)/720*100%??????±???? 
#define EMPTYPOINTERSIZE					16
void Comm_Init(void);
#endif

funcStatus InitialAHMIStruct();


extern  ConfigInfoClass		ConfigData;
extern TimerClassPtr		TimerPtr;
extern DynamicPageClassPtr  gPagePtr;
extern QueueHandle_t		RefreshQueue;
extern QueueHandle_t        RefreshQueueWithoutDoubleBuffer;
extern TagClassPtr			TagPtr;
extern TagUpdateClassPtr    TagUpdatePtr;
extern u8*					TagStringPtr;
extern u8*					TagStringListPtr;

extern UartClassPtr			UartPtr;
extern QueueHandle_t		ActionInstructionQueue;
#ifdef PC_SIM
extern "C" extern QueueHandle_t		MouseQueue;
#endif
extern QueueHandle_t		UartDataRecvQueue;
extern QueueHandle_t		TimerCmdQueue;
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
extern xTaskHandle InterruptTaskHandle;
extern xTaskHandle WidgetRefreshTaskHandle;
extern xTaskHandle ActionProcessorTaskHandle;
extern xTaskHandle UartDataReceiveTaskHandle;
extern xTaskHandle TimerProcessTaskHandle;
extern xTaskHandle TagUpdateTaskHandle;
extern xTaskHandle AnimationTaskHandle;

#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
extern xTaskHandle switchBufferTaskHandle;
#endif
#ifdef WHOLE_TRIBLE_BUFFER
extern xTaskHandle screenUpdateTaskHandle;
#endif

extern void*    PageSpace;
extern 	u16    	WorkingPageID;
typedef u32 		pointersize;

extern u32 startOfDynamicPage;
extern u32 endOfDynamicPage;

extern SemaphoreHandle_t interuptSemaphore;
extern SemaphoreHandle_t drawSemaphore;


#ifdef EMBEDDED
extern SpiBasic Stm_Flash;
extern SpiBasic FPGA_Device;
extern uint32_t flag_Calibration;
extern u8		   	stm32info[];
#endif

#ifdef PC_SIM
extern u8				TextureBuffer[TEXBUFSIZE];
extern u32		TileRenderCounter;
extern u32		FrameRate;


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


void resetFPGA(void)
{
	int i;
	GPIO_InitTypeDef GPIOInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIOInitStructure);
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_6);
	for(i = 0; i < 100; i++);
	GPIO_SetBits(GPIOB, GPIO_Pin_6);
}

#ifdef USING_CHANGE_BRIGHTNESS
void TIM4_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; /////////GPIO7 TIM4 ch2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 

	
	TIM_TimeBaseStructure.TIM_Period = 719; //PWM ????
	TIM_TimeBaseStructure.TIM_Prescaler =199; //·???
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //????
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //??? TIMx
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //
	TIM_OC2Init(TIM4, &TIM_OCInitStructure); //
	
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable); //ch2

	TIM_Cmd(TIM4, ENABLE); //????TIM4
	
	}
#endif
	
void testscreen(void)
{
#ifdef USING_CHANGE_BRIGHTNESS
	GPIO_InitTypeDef GPIOInitStructure;
	TIM4_PWM_Init();
	TIM_SetCompare2(TIM4,Lightness);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIOInitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
#endif

#ifdef USING_NO_CHANGE_BRIGHTNESS	
	GPIO_InitTypeDef GPIOInitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIOInitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8;
	GPIOInitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOInitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIOInitStructure);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_7);
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
#endif
}

#endif


//-----------------------------
// 函数名： AHMITop
// 顶层输入接口
// 参数列表：
// 备注(各个版本之间的修改):
//-----------------------------
#ifdef PC_SIM
int AHMITop()
#endif
#ifdef EMBEDDED
int main()
#endif
{
	//char text[200];
	 portBASE_TYPE xReturn;
	 int err;
#ifdef EMBEDDED	
	 int i;
	u8 *stm32ptraddr;
	GetChipID();
	resetFPGA();
	vPortInitialiseBlocks();
	pucAlignedHeapInitialise();
	TaskInitialParam();
	croutineInitialParam();
	initialSTMFlashReadBaseAddr();
	Comm_Init();
	
	USB_EnterGPIOCfg();
	testscreen();
	if(GPIO_ReadInputDataBit(USB_Enter_GPIOX, USB_Enter_GPIO_Pin))
	{
		usb_dfumain();
	}
//	usb_dfumain();
#endif
	 ActionInstructionQueue = xQueueCreate(50,sizeof(pointersize));
	 RefreshQueue = xQueueCreate(50,sizeof(RefreshMsg));
	 RefreshQueueWithoutDoubleBuffer = xQueueCreate(3,sizeof(RefreshMsg));
#ifdef PC_SIM	
	MouseQueue = xQueueCreate(5,sizeof(PIDState)); 
#endif
	 TimerCmdQueue = xQueueCreate(10,sizeof(TimerHandle_t));
	 AnimationTaskQueue = xQueueCreate(10, sizeof(u8));
#ifdef USE_UART
	UartDataRecvQueue = xQueueCreate(100,sizeof(u8));
#endif
	keyboardQueue = xQueueCreate(5,sizeof(u8)); //键盘队列 
#ifdef USE_MODBUS_MASTER
	 ModbusMasterRWTaskQueue = xQueueCreate(50, sizeof(ModBusMasterRWMsg));

	 //xReturn = xTaskCreate(ModBusMasterReadWaiteTask,"ReadWaiteTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + 2,&ModBusMasterReadWaiteHandle);
	 //if(xReturn != pdTRUE) 
	 //{
		// ERROR_PRINT("Fail to create ModBus Task.\r\n");
		// return 0;
	 //}

		xReturn = xTaskCreate(ModBusMasterRWTask,"ModBusMasterRWTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + 1,&ModBusMasterRWTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create ModBus Task.\r\n");
		 return 0;
	 }
	 xReturn = xTaskCreate(ModBusMasterTask,"ModBusMasterTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + 2,&ModBusMasterTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create ModBus Task.\r\n");
		 return 0;
	 }	  
#endif
#ifdef USE_MODBUS_SLAVE
	 xReturn = xTaskCreate(ModBusSlaveTask,"ModBusSlaveTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + 1,&ModBusSlaveTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create ModBus Task.\r\n");
		 return 0;
	 }

#endif
#ifdef USE_TOUCHSCREEN
	 xReturn = xTaskCreate(MouseTouchTask,"MouseTouchTask",configMINIMAL_STACK_SIZE,NULL,2,&MouseTouchTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create MouseTouch Task.\r\n");
		 return 0;
	 }
#endif
#ifdef USE_UART
	 xReturn = xTaskCreate(UartDataReceiveTask,"UartDataReceiveTask",configMINIMAL_STACK_SIZE,NULL,2,&UartDataReceiveTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create UartDataReceiveTask Task.\r\n");
		 return 0;
	 }
#endif
	 xReturn = xTaskCreate(WidgetRefreshTask,"WidgetRefreshTask",0x0800,NULL,2,&WidgetRefreshTaskHandle);
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
	 
	 // add by xt, 20151130
	 xReturn  = xTaskCreate(TagUpdateTask,"TagUpdateTask",configMINIMAL_STACK_SIZE,NULL,2,&TagUpdateTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create TagUpdateTask Task.\r\n");
		 return 0;
	 }

	 //animation, add by Zhou Yuzhi
	 xReturn  = xTaskCreate(animationQueueTask,"TagUpdateTask",configMINIMAL_STACK_SIZE,NULL,2,&AnimationTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create TagUpdateTask Task.\r\n");
		 return 0;
	 }

	 //keyboard, added by zuz 20160903
	 xReturn  = xTaskCreate(keyboardReveiveTask,"keyboardReveiveTask",configMINIMAL_STACK_SIZE,NULL,2,&KeyBoardInputTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create TagUpdateTask Task.\r\n");
		 return 0;
	 }

	 //interrupt
	 //xReturn  = xTaskCreate(interuptTask,"interuptTask",configMINIMAL_STACK_SIZE,NULL,2,&InterruptTaskHandle);
	 //if(xReturn != pdTRUE) 
	 //{
		// ERROR_PRINT("Fail to create TagUpdateTask Task.\r\n");
		// return 0;
	 //}

#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
	 //change buffer
	  xReturn = xTaskCreate(swtichDispBufferTask,"swtichDispBufferTask",configMINIMAL_STACK_SIZE,NULL,2,&switchBufferTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create swtichDispBuffer Task.\r\n");
		 return 0;
	 }
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
#endif

	 err = InitialAHMIStruct();
#ifdef EMBEDDED	
	if(err != AHMI_FUNC_SUCCESS) 
	{
		stm32ptraddr = (u8 *)Stm32_interface_Addr;
		//*(stm32ptraddr + Tile_size_x) = TILE_NUM_X;
		//*(stm32ptraddr + Tile_size_y) = TILE_NUM_Y;
		//*(stm32ptraddr + Tile_x_size) = TILESIZE;
		//*(stm32ptraddr + Tile_y_size) = TILESIZE;
		
		*(stm32ptraddr + Tile_start_x) = 0;
		*(stm32ptraddr + Tile_start_y) = 0 ;
		*(stm32ptraddr + Tile_end_x)   = TILE_NUM_X - 1  ;
		*(stm32ptraddr + Tile_end_y)   = TILE_NUM_Y - 1 ;
		*(stm32ptraddr + Addr_combine) = 0x09;
		for(;;)
		{
			DrawTouchCalibPage(
				SCREEN_WIDTH,
				SCREEN_HEIGHT,
				0xFFFFFFFF, //BackgroundColor:white
				EMPTYPOINTERSIZE,
				//LeftTop position of LeftTop Point
				0, 
				0,
				0xFFFF0000, //Pointer0Color:red
				//LeftTop position of RightTop Point
				SCREEN_WIDTH - EMPTYPOINTERSIZE, 
				0,
				0xFFFF0000, //Pointer1Color:black
				//LeftTop position of LeftBottum Point
				0, 
				SCREEN_HEIGHT - EMPTYPOINTERSIZE,
				0xFFFF0000, //Pointer1Color:black
				//LeftTop position of RightBottum Point
				SCREEN_WIDTH - EMPTYPOINTERSIZE, 
				SCREEN_HEIGHT - EMPTYPOINTERSIZE,
				0xFFFF0000, //Pointer1Color:black
				//LeftTop position of Center Point
				SCREEN_WIDTH / 2, 
				SCREEN_HEIGHT / 2,
				0xFFFFFFFF //Pointer1Color:black
				);
				for(i = 0; i < 65535; i++);
			
		}
	}
#endif	
#ifdef PC_SIM
	if(err != AHMI_FUNC_SUCCESS) 
		 return 0;
#endif
	 vTaskStartScheduler();
	 ERROR_PRINT("Fail to start Scheduler.\r\n");
	 for(;;);
	
}

//-----------------------------
// 函数名： InitialAHMIStruct
// 初始化各个结构体以及页面
// 参数列表：
// 备注(各个版本之间的修改):
//-----------------------------
funcStatus InitialAHMIStruct()
{
#ifdef PC_SIM
	char text[200];
#endif
#ifdef EMBEDDED	
#ifdef USE_TOUCHSCREEN
	u8 *stm32ptraddr;
#endif
#endif
	int ID;
	int err;
	int i;
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

	//rtc
#ifdef EMBEDDED
	if(ConfigData.innerClockEn)
	{
		xReturn  = xTaskCreate(RTCReadTask,"RTCReadTask",configMINIMAL_STACK_SIZE,NULL,2,&RTCReadTaskHanler);
		if(xReturn != pdTRUE) 
		{
			ERROR_PRINT("Fail to create TagUpdateTask Task.\r\n");
			return 0;
		}
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
	}

	TagStringPtr = (u8*)pvPortMalloc(ConfigData.StringSize);
	if(TagStringPtr == NULL && ConfigData.StringSize != 0 )
	{
		ERROR_PRINT("TagStringPtr Initial failed.\r\n"); 
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
	}
	else
	{
		startOfDynamicPage = (u32)PageSpace;
		endOfDynamicPage = (u32)PageSpace + ConfigData.DynamicPageSize;
	}
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
			 newPage->enlightedWidgetID
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



	for(ID=0;ID<ConfigData.NumofTimers;ID++)
	{
		(TimerPtr+ID)->initTimer(ID);
	}
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
#ifdef USE_TOUCHSCREEN
	flag_Calibration = 0;
	if(flag_Calibration == 1)
	{
#endif
#endif
	if(gPagePtr[0].mAnimationType == NO_ANIMATION)
	{
		if(gPagePtr[WorkingPageID].loadPage(0,1) == AHMI_FUNC_FAILURE)
		{
#ifdef DEBUG
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
#ifdef DEBUG
			WriteText(text,"Reading page %d failure.\r\n", WorkingPageID);
			ERROR_PRINT(text);
#endif
			return AHMI_FUNC_FAILURE;
		}
	}
#ifdef EMBEDDED	
#ifdef USE_TOUCHSCREEN
	}
	else if((flag_Calibration == 0) || (flag_Calibration == 0xFFFFFFFF))
	{
		stm32ptraddr = (u8 *)Stm32_interface_Addr;
		//*(stm32ptraddr + Tile_size_x) = TILE_NUM_X;
		//*(stm32ptraddr + Tile_size_y) = TILE_NUM_Y;
		//*(stm32ptraddr + Tile_x_size) = TILESIZE;
		//*(stm32ptraddr + Tile_y_size) = TILESIZE;
		
		*(stm32ptraddr + Tile_start_x) = 0;
		*(stm32ptraddr + Tile_start_y) = 0 ;
		*(stm32ptraddr + Tile_end_x)   = TILE_NUM_X - 1  ;
		*(stm32ptraddr + Tile_end_y)   = TILE_NUM_Y - 1 ;
		*(stm32ptraddr + Addr_combine) = 0x09;

	}
#endif
#endif

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


void Comm_Init(void)
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
#ifdef USE_TOUCHSCREEN
	AW2083_InitDevice();
#endif
//  ahmi_start();
}
#endif

#endif