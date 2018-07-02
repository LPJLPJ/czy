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
#include "AHMIInterface.h"
#include "publicInclude.h"
#include "mb.h"
#include "UartClass.h"
#include "AHMIBasicDefine.h"
#include "trace.h"
#include "interruptTask.h"
#include "ahmirtc.h"
#include "refreshQueueHandler.h"
#ifdef EMBEDDED
#include "usb_dfu.h"
#include "port.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_exti.h"
#include "IOConfig.h"
#include "bsp_usart.h"
#include "RX8025.h"
#include "LP3907.h"
#include "location.h"
#include "stm32f10x_can.h"
#include "CanTransit.h"
#include "checkPWR.h"
#ifdef VIDEO_EN
#include "tvp5150.h"
#endif
#endif

#ifdef AHMI_CORE


extern QueueHandle_t		ModbusMasterRWTaskQueue;
extern QueueHandle_t   CanDataRecvQueue;

extern xTaskHandle ModBusMasterRWTaskHandle;
extern xTaskHandle ModBusSlaveTaskHandle;
extern xTaskHandle ModBusMasterReadWaiteHandle;
extern xTaskHandle ModBusMasterTaskHandle;
extern xTaskHandle UartDataReceiveTaskHandle;

extern xTaskHandle CanTransitTaskHandle;
extern xTaskHandle CanTGetDataTaskHandle;
extern xTaskHandle RTCReadTaskHandle;

xTaskHandle GetPosiCDMATaskHandle;
xTaskHandle GetPosiWIFITaskHandle;

xTaskHandle CheckPWRTaskHandle;

AHMIPinCfg AHMIpincfg = {      //引脚配置	
	1                         //使用触屏为1
};

#ifdef EMBEDDED
void StartBee(
								TIM_TypeDef * TIMx, 				//定时器：TIM2，TIM3，TIM4，TIM5，TIM6，TIM7
								uint8_t PrePriority,        //中断的主优先级：0~3
								uint8_t SubPriority         //中断的子优先级：0~3
							);
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
#ifdef VIDEO_EN
	int delay1, delay2;
#endif
	int i = 0;
#ifdef PC_SIM
	int err;
#endif
	portBASE_TYPE xReturn;
	
#ifdef EMBEDDED
	portBASE_TYPE xReturn;
	PWRGoodIOConfig();
	STM32LoadIOConfig();
	GPIO_SetBits(STM32LOAD_GPIOX, STM32LOAD_GPIOPin);
	
	
	PowerIOConfig();
	
	//if(GetPWRStatus()!= 0)
	if(1)
	{
		
		PowerIOConfig();		//config and set power control pin.
//		testLP3907();
		
		RTCINTAEXTIConfig();
		RTCINTBEXTIConfig();
		
		initAHMIHardware(&AHMIpincfg);
		
		USB_EnterGPIOCfg();
		if(GPIO_ReadInputDataBit(USB_Enter_GPIOX, USB_Enter_GPIO_Pin))
		{
			usb_dfumain();
		}
//		usb_dfumain();
		#ifdef FLASH_TEST
		usb_dfumain();
		#endif

#endif
	#ifdef VIDEO_EN
	FPGA_IRQ_Init();
	#endif
	
	initQueue();

#ifdef EMBEDDED	 
	 CanDataRecvQueue = xQueueCreate(10,sizeof(CanRxMsg));
	 ///////add by smaller
	 xReturn = xTaskCreate(CanTransitTask,"CanTransitTask",512,NULL,2,&CanTransitTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create UartDataReceiveTask Task.\r\n");
		 return 0;
	 }
	 ///////add by smaller
	 xReturn = xTaskCreate(CanTGetDataTask,"CanTGetDataTask",150,NULL,2,&CanTGetDataTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create UartDataReceiveTask Task.\r\n");
		 return 0;
	 }
	 
	 xReturn = xTaskCreate(CheckPWRTask,"CheckPWRTask",64,NULL,tskIDLE_PRIORITY + 1,&CheckPWRTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create CheckPWRTask Task.\r\n");
		 return 0;
	 }
	 
//	 xReturn = xTaskCreate(RTCReadTask,"RTCReadTask",64,NULL,tskIDLE_PRIORITY + 1,&RTCReadTaskHandle);
//	 if(xReturn != pdTRUE) 
//	 {
//		 ERROR_PRINT("Fail to create RTCReadTask Task.\r\n");
//		 return 0;
//	 }
	 
	 xReturn = xTaskCreate(GetPosiWithGPSAndCDMATask,"GetPosiCDMATask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + 1,&GetPosiCDMATaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create GetPosiCDMATask Task.\r\n");
		 return 0;
	 }
	 
//	 xReturn = xTaskCreate(GetPosiGPSTask,"GetPosiWIFITask",256,NULL,tskIDLE_PRIORITY + 1,&GetPosiWIFITaskHandle);
//	 if(xReturn != pdTRUE) 
//	 {
//		 ERROR_PRINT("Fail to create UartDataReceiveTask Task.\r\n");
//		 return 0;
//	 }
	 
		startAHMI(&AHMIpincfg);
	 
	}
	else
	{
		PWRGoodEXTIConfig();
		xReturn = xTaskCreate(GetPosiWithGPSAndCDMATask,"GetPosiCDMATask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + 1,&GetPosiCDMATaskHandle);
		if(xReturn != pdTRUE) 
		{
			ERROR_PRINT("Fail to create GetPosiCDMATask Task.\r\n");
			return 0;
		}
	}
#endif
		

#ifdef USE_UART
	UartDataRecvQueue = xQueueCreate(100,sizeof(u8));
#endif
#ifdef USE_MODBUS_MASTER
	 ModbusMasterRWTaskQueue = xQueueCreate(50, sizeof(ModBusMasterRWMsg));

	 xReturn = xTaskCreate(ModBusMasterReadWaiteTask,"ReadWaiteTask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + 2,&ModBusMasterReadWaiteHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create ModBus Task.\r\n");
		 return 0;
	 }

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
#ifdef USE_UART
	 xReturn = xTaskCreate(UartDataReceiveTask,"UartDataReceiveTask",configMINIMAL_STACK_SIZE,NULL,2,&UartDataReceiveTaskHandle);
	 if(xReturn != pdTRUE) 
	 {
		 ERROR_PRINT("Fail to create UartDataReceiveTask Task.\r\n");
		 return 0;
	 }
#endif
	 
#ifdef EMBEDDED	 	 
//	xReturn = xTaskCreate(GetPosiCDMATask,"GetPosiCDMATask",configMINIMAL_STACK_SIZE,NULL,tskIDLE_PRIORITY + 1,&GetPosiCDMATaskHandle);
//	 if(xReturn != pdTRUE) 
//	 {
//		 ERROR_PRINT("Fail to create ModBus Task.\r\n");
//		 return 0;
//	 }
	 
//	 xReturn = xTaskCreate(GetPosiWIFITask,"GetPosiWIFITask",256,NULL,tskIDLE_PRIORITY + 1,&GetPosiWIFITaskHandle);
//	 if(xReturn != pdTRUE) 
//	 {
//		 ERROR_PRINT("Fail to create ModBus Task.\r\n");
//		 return 0;
//	 }

#endif
	 
#ifdef PC_SIM
	err = startAHMI(&AHMIpincfg);
	if(err != AHMI_FUNC_SUCCESS) 
		 return 0;
#endif
#ifdef VIDEO_EN
	for(delay1 = 0; delay1 < 100; delay1++)
		for(delay2 = 0; delay2 < 36000;)
			delay2++;
	 Tvp5150Init();
#endif
	 vTaskStartScheduler();
	 ERROR_PRINT("Fail to start Scheduler.\r\n");
	 for(;;);
}

#endif

#ifdef EMBEDDED	 
void StartBee(
								TIM_TypeDef * TIMx, 				//定时器：TIM2，TIM3，TIM4，TIM5，TIM6，TIM7
								uint8_t PrePriority,        //中断的主优先级：0~3
								uint8_t SubPriority         //中断的子优先级：0~3
							)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	SpeakerIOConfig();
	
	if(TIMx == TIM2)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	}
	else if(TIMx == TIM3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	}
	else if(TIMx == TIM4)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	}
	else if(TIMx == TIM5)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	}
	else if(TIMx == TIM6)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	}
	else if(TIMx == TIM7)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	}	
	else
		return;
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	/* Enable the TIM2 gloabal Interrupt */	
	if(TIMx == TIM2)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	}
	else if(TIMx == TIM3)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	}
	else if(TIMx == TIM4)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	}
	else if(TIMx == TIM5)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	}
	else if(TIMx == TIM6)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	}
	else if(TIMx == TIM7)
	{
		NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	}	
	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PrePriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = SubPriority;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	
 	TIM_ITConfig(TIMx, TIM_IT_Update,DISABLE);
  TIM_Cmd(TIMx, DISABLE);


	TIM_TimeBaseStructure.TIM_Period = 2;    	//下一个更新事件装入活动的自动重装载寄存器周期的值。它的取值必须在0x0000和0xFFFF之间。
  TIM_TimeBaseStructure.TIM_Prescaler = 35;					    //预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分割
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
  TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	
	TIM_ClearFlag(TIMx, TIM_FLAG_Update);
	
	TIM_SetCounter(TIMx,0x00);			//清零计数器值
  TIM_ITConfig(TIMx,TIM_IT_Update,ENABLE);
  TIM_Cmd(TIMx,ENABLE);
	
	GPIO_SetBits(SPEAKER_GPIOX, SPEAKER_GPIOPin);
}
#endif
