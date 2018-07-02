////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2016/03/21
// File Name:     PublicVariables.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//    include all the global variables
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"
#include "UartClass.h"
#include "semphr.h"
#ifdef	EMBEDDED
	#ifdef STM32F10X_HD
		#include "usb_desc.h"
	#endif
	
	#ifdef STM32F10X_CL
		#include "usbd_conf.h"
	#endif
#endif

#ifdef AHMI_CORE

extern uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];

ConfigInfoClass ConfigData;
u16             WorkingPageID;
DynamicPageClassPtr  gPagePtr;
UartClassPtr    UartPtr;
TimerClassPtr   TimerPtr;
TagClassPtr     TagPtr;
TagUpdateClassPtr  TagUpdatePtr;
//animation
AnimationClass  gAnimationClass;

u8*					TagStringPtr;
u8*					TagStringListPtr;
u8*                 TagChangedListPtr; //record what tags have been changed, storing the tagID

//u16 startAddrOfTagChangedList;
//u16 stopAddrOfTagChangedList;

#ifdef PC_SIM
extern "C" extern QueueHandle_t   MouseQueue;
#endif
QueueHandle_t   keyboardQueue;    //键盘队列
QueueHandle_t   TimerCmdQueue;
QueueHandle_t   AnimationTimerCmdQueue;
QueueHandle_t   UartDataRecvQueue;
QueueHandle_t   RefreshQueue;
QueueHandle_t   RefreshQueueWithoutDoubleBuffer;
QueueHandle_t   ActionInstructionQueue;
QueueHandle_t   ModbusMasterRWTaskQueue;
QueueHandle_t   AnimationTaskQueue;///////////////////任务队列，存储的是消息列表的中需要刷新的信息位置信息

QueueHandle_t   CanDataRecvQueue;

void*           PageSpace;
u8*             UartBuffer;
u32             TileRenderCounter;
u32             FrameRate;

xTaskHandle ModBusMasterRWTaskHandle;
xTaskHandle ModBusSlaveTaskHandle;
xTaskHandle ModBusMasterReadWaiteHandle;
xTaskHandle	ModBusMasterTaskHandle;
xTaskHandle MouseTouchTaskHandle;
xTaskHandle KeyBoardInputTaskHandle;
xTaskHandle RTCReadTaskHanler;
xTaskHandle InterruptTaskHandle;
xTaskHandle	WidgetRefreshTaskHandle;
xTaskHandle	ActionProcessorTaskHandle;
xTaskHandle UartDataReceiveTaskHandle;
xTaskHandle TimerProcessTaskHandle;
xTaskHandle AnimationTimerProcessTaskHandle;
xTaskHandle TagUpdateTaskHandle;
xTaskHandle TagSetBindingElementTaskHandle;
xTaskHandle AnimationTaskHandle;

xTaskHandle CanTransitTaskHandle;
xTaskHandle CanTGetDataTaskHandle;
xTaskHandle RTCReadTaskHandle;

#if (defined WHOLE_TRIBLE_BUFFER) || (defined PARTIAL_TRIBLE_BUFFER)
xTaskHandle switchBufferTaskHandle;
xTaskHandle screenUpdateTaskHandle;
#endif

#ifdef MODULE_TEST_EN 
xTaskHandle moduleTestTaskHandler;
#endif

//鼠标
PIDPoint pressPoint;
PIDPoint releasePoint;
//鼠标目前状态
u8 mouseStatus = MOUSE_RELEASE;
//中断
SemaphoreHandle_t interuptSemaphore;
SemaphoreHandle_t drawSemaphore;

u8 animationDuration = 0;  //页面动画持续过程中
u8 animationExist = 0;     //有动画存在，则不启用四缓存
u8 gPageNeedRefresh;




#ifdef	EMBEDDED
//extern uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#ifdef __SRAM_SUPPORT__
static uint8_t mal_buffer[4096];
const uint8_t*  MAL_Buffer = mal_buffer;
#else
const uint8_t*  MAL_Buffer = ucHeap + 1024;
#endif	/*__SRAM_SUPPORT__*/

//const uint8_t*  MAL_Buffer = ucHeap + 1024;

//	#ifdef STM32F10X_HD
//		uint8_t  MAL_Buffer[wTransferSize * 2]; /* RAM Buffer for Downloaded Data */
//	#endif
//	
//	#ifdef STM32F10X_CL
//		uint8_t  MAL_Buffer[XFERSIZE * 2];
//	#endif
#endif

#ifdef PC_SIM
u8       sourceBuffer[20480];
#endif
#ifdef EMBEDDED
u8       sourceBuffer[2048];
#endif

#ifdef PC_SIM
u8       TextureBuffer[TEXBUFSIZE];
u8       VideoTextureBuffer[VIDEO_BUF_SIZE];
u8  GlobalFrameBuffer[ MAX_SCREEN_SIZE *2];
u8  GlobalFrameBuffer2[ MAX_SCREEN_SIZE *2]; //used for double buffer
u8  GlobalBackgroundBuffer[ MAX_SCREEN_SIZE *2 ]; //used for background texture
#endif

int flag_UartOpen = 0;
int ModbusReadEn = 0;
extern "C" uint8_t receivedata;

//ahmi
u8		 stm32info[50];
u8 addr_w = 0;
u8 addr_r = 0;

u32 startOfDynamicPage;
u32 endOfDynamicPage;
u8  gPageNeedFresh;

//screen
u16 gScreenWidth;
u16 gScreenHeight;
char* cfgString;

//video
u8 addr_cvbs = 0;
u8 video_cur_w = 2;
u8 video_cur_r = 0;
u8 video_next_w = 0;
u8 video_next_r = 1;

//profiling
#ifdef FRAME_TEST //test the frame rate
u32 gFrameCount = 0;
#endif

#endif
