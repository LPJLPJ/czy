////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     UART.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicInclude.h"
#include "FreeRTOS.h"
#include "timers.h"

//#ifdef PC_SIM

#ifdef USE_UART
extern TagClassPtr TagPtr;
extern UartClassPtr UartPtr;
extern QueueHandle_t   UartDataRecvQueue;
extern ConfigInfo      ConfigData;
extern QueueHandle_t   ActionInstructionQueue;

void InitUartBuffer(u8 ID, u8 size, u8 TagID)
{
	UartPtr[ID].UartBuffer = (u8*)pvPortMalloc(size);
	UartPtr[ID].LinkTag = TagID;
}

void CountDownTimerCallBackFunc(TimerHandle_t pxTimer)
{
	UartReceiveStateMachine(0,1);//复位状态机
}

void UartDataReceiveTask(void* pvParameters)
{
 	BaseType_t xStatus;
	u8 recvdata;
	TimerHandle_t CountDownTimer;
	
	CountDownTimer = xTimerCreate
		          (  /* Just a text name, not used by the RTOS kernel. */
                     "CountDownTimer",
                     /* The timer period in ticks. */
                     100,
                     /* The timers will auto-reload themselves when they expire. */
                     pdFALSE,
                     /* Assign each timer a unique id equal to its array index. */
                     ( void * ) 0,
                     /* Each timer calls the same callback when it expires. */
                     CountDownTimerCallBackFunc
                   );
 	for(;;)
 	{
 		xStatus=xQueueReceive(UartDataRecvQueue,
			                  (void*)&recvdata,
							  portMAX_DELAY
			                   );
 		if(xStatus!=pdPASS)
 		{
 			return;
 		}
		xTimerReset(CountDownTimer,0);
		UartReceiveStateMachine(recvdata,0);
  		taskYIELD();
 	}
}
//********************************************
//接收数据的状态机
//数据包格式：
//-------------------------------
//包头（0x41)
//-------------------------------
//数据属性(1字节)
//格式：
//7-5:保留
//4:数据含义，0表示指令，1表示文字数据
//3-0:写入缓冲区的编号
//------------------------------------
//有效数据长度（1字节，数量为N）
//------------------------------------
//数据(N个字节）
//------------------------------------
//校验和(1字节,前面所有数据按位异或)
//------------------------------------
//********************************************

#define  HEAD  1
#define  ATTR  2
#define  LEN   3
#define  DATA  4
#define  CHK   5

void UartReceiveStateMachine(u8 data,u8 reset)
{


	static int state=HEAD;//状态字
	static int UartBufTextPtr=0;//指向缓冲区中的第N个字节
	static int UartBufID=0;//指向第N个缓冲区
	static int UartRecvNum=0;
	static int UartAttr=0;
	static int checksum=0;
	int  ActionAddr;

	if(reset)//复位状态机
	{
		state = HEAD;
		return;
	}
	switch(state)
	{
	case(HEAD):
		if(data==0x41)
		{
			state = ATTR;
			checksum = data;
		}
		return;
	case(ATTR):
		state = LEN;
		UartAttr = (data & 0x10)?1:0;
		UartBufID = data & 0xf;
		UartBufTextPtr = 0;
		checksum = checksum ^ data;
		return;
	case(LEN):
		state = DATA;
		UartRecvNum = data;
		checksum = checksum ^ data;
		return;
	case(DATA):
		UartPtr[UartBufID].UartBuffer[UartBufTextPtr++] = data;
		if(UartBufTextPtr >= ConfigData.UartBufSizeforEach)
		{
			state = HEAD;
			return;
		}
		checksum = checksum ^ data;
		if(UartBufTextPtr == UartRecvNum)
		{
			if(UartAttr)//文字数据
			{
				(TagPtr[UartPtr[UartBufID].LinkTag]).setValue(UartRecvNum);
			}
			else//控制数据，由ActionProcessor处理
			{
				ActionAddr = (int)UartPtr[UartBufID].UartBuffer;
				xQueueSendToBack(ActionInstructionQueue,&ActionAddr,portMAX_DELAY);
			}
			state = HEAD;
		}
		return;
	//case(CHK):
	//	if (checksum == data)
	//	{
	//		if(UartAttr)//文字数据
	//		{
	//			TagSetValue(&(TagPtr[UartPtr[UartBufID].LinkTag]),UartRecvNum);
	//		}
	//		else//控制数据，由ActionProcessor处理
	//		{
	//			ActionAddr = (int)UartPtr[UartBufID].UartBuffer;
	//			xQueueSendToBack(ActionInstructionQueue,&ActionAddr,portMAX_DELAY);
	//		}
	//	}
	//	state = HEAD;
	//	return;
	default: 
		state = HEAD;
		return;
	}


}
#endif

