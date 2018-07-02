/*
 * FreeModbus Libary: RT-Thread Port
 * Copyright (C) 2013 Armink <armink.ztl@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions $
 */
//#include <windows.h>
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbconfig.h"
//#include "rtdevice.h"
//#include "bsp.h"
#ifdef USE_MODBUS_MASTER
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Static variables ---------------------------------*/
//ALIGN(RT_ALIGN_SIZE)
/* software simulation serial transmit IRQ handler thread stack */
//static rt_uint8_t serial_soft_trans_irq_stack[512];
///* software simulation serial transmit IRQ handler thread */
//static struct rt_thread thread_serial_soft_trans_irq;
///* serial event */
//static struct rt_event event_serial;
///* modbus master serial device */
//static rt_serial_t *serial;

/* ----------------------- Defines ------------------------------------------*/
/* serial transmit event */
#define EVENT_SERIAL_TRANS_START    (1<<0)

/* ----------------------- static functions ---------------------------------*/
//static void prvvUARTTxReadyISR(void);
//static void prvvUARTRxISR(void);
//static rt_err_t serial_rx_ind(rt_device_t dev, rt_size_t size);
//static void serial_soft_trans_irq(void* parameter);

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBMasterPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{

//	BOOL    bInitialized = TRUE;
//	GPIO_InitTypeDef GPIO_InitStruct; 
//	USART_InitTypeDef USART_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD
//		| RCC_APB2Periph_AFIO, ENABLE);
//	
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
//	
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA,&GPIO_InitStruct);
//	
////	RCC_APB2PeriphClockCmd(RCC_APB1Periph_UART4 | RCC_APB2Periph_GPIOC 
////		| RCC_APB2Periph_AFIO, ENABLE);
////	
////	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
////	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
////	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_Init(GPIOB,&GPIO_InitStruct);
////	
////	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
////	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
////	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_Init(GPIOB,&GPIO_InitStruct);
//	
//	USART_InitStructure.USART_BaudRate = ulBaudRate;
//	switch ( eParity )
//	{
//	case MB_PAR_NONE:
//		USART_InitStructure.USART_Parity = USART_Parity_No;
//		break;
//	case MB_PAR_ODD:
//		USART_InitStructure.USART_Parity = USART_Parity_Odd;
//		break;
//	case MB_PAR_EVEN:
//		USART_InitStructure.USART_Parity = USART_Parity_Even;
//		break;
//	}
//	switch ( ucDataBits )
//	{
//	case 8:
//		if(eParity==MB_PAR_NONE)
//		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
//		else
//		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
//		break;
//	case 7:
//		break;
//	default:
//		bInitialized = FALSE;
//	}
//	if( bInitialized )
//	{
//		ENTER_CRITICAL_SECTION(  );
//		USART_InitStructure.USART_StopBits = USART_StopBits_1;
//		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//		USART_Init(USART1, &USART_InitStructure);
//		USART_Cmd(USART1, ENABLE);
//		EXIT_CRITICAL_SECTION(  );
//	}

  return TRUE;
}

void vMBMasterPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
//    rt_uint32_t recved_event;
//    if (xRxEnable)
//    {
//        /* enable RX interrupt */
//        serial->ops->control(serial, RT_DEVICE_CTRL_SET_INT, (void *)RT_DEVICE_FLAG_INT_RX);
//        /* switch 485 to receive mode */
//        rt_pin_write(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, PIN_LOW);
//    }
//    else
//    {
//        /* switch 485 to transmit mode */
//        rt_pin_write(MODBUS_MASTER_RT_CONTROL_PIN_INDEX, PIN_HIGH);
//        /* disable RX interrupt */
//        serial->ops->control(serial, RT_DEVICE_CTRL_CLR_INT, (void *)RT_DEVICE_FLAG_INT_RX);
//    }
//    if (xTxEnable)
//    {
//        /* start serial transmit */
//        rt_event_send(&event_serial, EVENT_SERIAL_TRANS_START);
//    }
//    else
//    {
//        /* stop serial transmit */
//        rt_event_recv(&event_serial, EVENT_SERIAL_TRANS_START,
//                RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 0,
//                &recved_event);
//    }
	/*	ENTER_CRITICAL_SECTION(  );*/
   /* if( xRxEnable )
    {
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    }
    else
    {
       USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    }
    if( xTxEnable )
    {
       USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
    else
    {
       USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }*/
  /*  EXIT_CRITICAL_SECTION(  );*/
}

//void vMBMasterPortClose(void)
//{
//    serial->parent.close(&(serial->parent));
//}

BOOL xMBMasterPortSerialPutByte(CHAR ucByte)
{
//    serial->parent.write(&(serial->parent), 0, &ucByte, 1);
	//USART_SendData(USART1, ucByte);
	//while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == 0);  //等待发送完成
    return TRUE;
}

BOOL xMBMasterPortSerialGetByte(CHAR * pucByte)
{
//    serial->parent.read(&(serial->parent), 0, pucByte, 1);
	//USART_ClearFlag(USART1, USART_IT_RXNE) ;
	//*pucByte = (u8)USART_ReceiveData(USART1);	//获取接收BUFFER的数据
    return TRUE;
}

/* 
 * Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(void)
{
    pxMBMasterFrameCBTransmitterEmpty();
}

/* 
 * Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
    pxMBMasterFrameCBByteReceived();
}


//void USART1_IRQHandler(void)
//{
//
//	if(USART_GetITStatus(USART1,USART_IT_TXE))
//	{
//	    pxMBMasterFrameCBTransmitterEmpty(  );
//	}
//	else if(USART_GetITStatus(USART1,USART_IT_RXNE))
//	{
//		pxMBMasterFrameCBByteReceived(  );
//	}
//}
/**
 * Software simulation serial transmit IRQ handler.
 *
 * @param parameter parameter
 */
//static void serial_soft_trans_irq(void* parameter) {
//    rt_uint32_t recved_event;
//    while (1)
//    {
//        /* waiting for serial transmit start */
//        rt_event_recv(&event_serial, EVENT_SERIAL_TRANS_START, RT_EVENT_FLAG_OR,
//                RT_WAITING_FOREVER, &recved_event);
//        /* execute modbus callback */
//        prvvUARTTxReadyISR();
//    }
//}

/**
 * This function is serial receive callback function
 *
 * @param dev the device of serial
 * @param size the data size that receive
 *
 * @return return RT_EOK
 */
//static rt_err_t serial_rx_ind(rt_device_t dev, rt_size_t size) {
//    prvvUARTRxISR();
//    return RT_EOK;
//}

#endif
#endif
