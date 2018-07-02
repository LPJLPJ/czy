////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
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
	UartReceiveStateMachine(0,1);//��λ״̬��
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
//�������ݵ�״̬��
//���ݰ���ʽ��
//-------------------------------
//��ͷ��0x41)
//-------------------------------
//��������(1�ֽ�)
//��ʽ��
//7-5:����
//4:���ݺ��壬0��ʾָ�1��ʾ��������
//3-0:д�뻺�����ı��
//------------------------------------
//��Ч���ݳ��ȣ�1�ֽڣ�����ΪN��
//------------------------------------
//����(N���ֽڣ�
//------------------------------------
//У���(1�ֽ�,ǰ���������ݰ�λ���)
//------------------------------------
//********************************************

#define  HEAD  1
#define  ATTR  2
#define  LEN   3
#define  DATA  4
#define  CHK   5

void UartReceiveStateMachine(u8 data,u8 reset)
{


	static int state=HEAD;//״̬��
	static int UartBufTextPtr=0;//ָ�򻺳����еĵ�N���ֽ�
	static int UartBufID=0;//ָ���N��������
	static int UartRecvNum=0;
	static int UartAttr=0;
	static int checksum=0;
	int  ActionAddr;

	if(reset)//��λ״̬��
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
			if(UartAttr)//��������
			{
				(TagPtr[UartPtr[UartBufID].LinkTag]).setValue(UartRecvNum);
			}
			else//�������ݣ���ActionProcessor����
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
	//		if(UartAttr)//��������
	//		{
	//			TagSetValue(&(TagPtr[UartPtr[UartBufID].LinkTag]),UartRecvNum);
	//		}
	//		else//�������ݣ���ActionProcessor����
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

