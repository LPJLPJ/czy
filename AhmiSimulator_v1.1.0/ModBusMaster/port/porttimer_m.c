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
 * File: $Id: porttimer_m.c,v 1.60 2013/08/13 15:07:05 Armink add Master Functions$
 */

/* ----------------------- Platform includes --------------------------------*/
//#include <windows.h>
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbport.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "projdefs.h"
#include "mbconfig.h"

#ifdef USE_MODBUS_MASTER
#if MB_MASTER_RTU_ENABLED > 0 || MB_MASTER_ASCII_ENABLED > 0
/* ----------------------- Variables ----------------------------------------*/
static USHORT usT35TimeOut50us;
TimerHandle_t MasterCountDownTimer;
//static struct rt_timer timer;
//void prvvTIMERExpiredISR(void);
//static void timer_timeout_ind(void* parameter);

/* ----------------------- static functions ---------------------------------*/
void prvvMasterTIMERExpiredISR(void);

/* ----------------------- Start implementation -----------------------------*/

void MasterCountDownTimerCallBackFunc(TimerHandle_t pxTimer)
{
	prvvMasterTIMERExpiredISR();
}

BOOL xMBMasterPortTimersInit(USHORT usTimeOut50us)
{
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//    /* backup T35 ticks */
//    usT35TimeOut50us = usTimeOut50us;
//
////    rt_timer_init(&timer, "master timer",
////                   timer_timeout_ind, /* bind timeout callback function */
////                   RT_NULL,
////                   (50 * usT35TimeOut50us) / (1000 * 1000 / RT_TICK_PER_SECOND),
////                   RT_TIMER_FLAG_ONE_SHOT); /* one shot */
//	
//	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
//	TIM_TimeBaseStructure.TIM_Period = 50*usT35TimeOut50us;    	//下一个更新事件装入活动的自动重装载寄存器周期的值。它的取值必须在0x0000和0xFFFF之间。
//  TIM_TimeBaseStructure.TIM_Prescaler = 35;					    //预分频值
//  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分割
//  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
//  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
//	TIM_ClearFlag(TIM3, TIM_FLAG_Update);

	MasterCountDownTimer = xTimerCreate
		          (  /* Just a text name, not used by the RTOS kernel. */
                     "MasterCountDownTimer",
                     /* The timer period in ticks. */
                     usTimeOut50us,
                     /* The timers will auto-reload themselves when they expire. */
                     0,
                     /* Assign each timer a unique id equal to its array index. */
                     ( void * ) 0,
                     /* Each timer calls the same callback when it expires. */
                     MasterCountDownTimerCallBackFunc
                   );



    return TRUE;
}

void vMBMasterPortTimersT35Enable()
{
	//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

 //   /* Set current timer mode, don't change it.*/
 //   vMBMasterSetCurTimerMode(MB_TMODE_T35);
	//
	//
	//TIM_TimeBaseStructure.TIM_Period = 50*usT35TimeOut50us;    	//下一个更新事件装入活动的自动重装载寄存器周期的值。它的取值必须在0x0000和0xFFFF之间。
 // TIM_TimeBaseStructure.TIM_Prescaler = 35;					    //预分频值
 // TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分割
 // TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
 // TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	//
	//TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	//
	//TIM_SetCounter(TIM3,0x00);			//清零计数器值
 // TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
 // TIM_Cmd(TIM3,ENABLE);
	 xTimerChangePeriod( MasterCountDownTimer,35, 0 );
	 xTimerReset( MasterCountDownTimer, 0 );
	
//start
	 xTimerStart(MasterCountDownTimer,0);

//    rt_timer_control(&timer, RT_TIMER_CTRL_SET_TIME, &timer_tick);

//    rt_timer_start(&timer);
}

void vMBMasterPortTimersConvertDelayEnable()
{
	//TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 //   uint32_t timer_tick = MB_MASTER_DELAY_MS_CONVERT * 100;

 //   /* Set current timer mode, don't change it.*/
 //   vMBMasterSetCurTimerMode(MB_TMODE_CONVERT_DELAY);
	//
	//TIM_TimeBaseStructure.TIM_Period = timer_tick;    	//下一个更新事件装入活动的自动重装载寄存器周期的值。它的取值必须在0x0000和0xFFFF之间。
 // TIM_TimeBaseStructure.TIM_Prescaler = 35;					    //预分频值
 // TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分割
 // TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
 // TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	//
	//TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	//
	//TIM_SetCounter(TIM3,0x00);			//清零计数器值
 // TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
 // TIM_Cmd(TIM3,ENABLE);

	xTimerChangePeriod( MasterCountDownTimer, MB_MASTER_DELAY_MS_CONVERT , 0 );
	xTimerReset( MasterCountDownTimer, 0 );

//    rt_timer_control(&timer, RT_TIMER_CTRL_SET_TIME, &timer_tick);

//    rt_timer_start(&timer);
}

void vMBMasterPortTimersRespondTimeoutEnable()
{
	//TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 //   uint32_t timer_tick = MB_MASTER_TIMEOUT_MS_RESPOND * 100;

 //   /* Set current timer mode, don't change it.*/
 //   vMBMasterSetCurTimerMode(MB_TMODE_RESPOND_TIMEOUT);
	//
	//TIM_TimeBaseStructure.TIM_Period = timer_tick;    	//下一个更新事件装入活动的自动重装载寄存器周期的值。它的取值必须在0x0000和0xFFFF之间。
 // TIM_TimeBaseStructure.TIM_Prescaler = 35;					    //预分频值
 // TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分割
 // TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
 // TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	//
	//TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	//
	//TIM_SetCounter(TIM3,0x00);			//清零计数器值
 // TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
 // TIM_Cmd(TIM3,ENABLE);

	xTimerChangePeriod( MasterCountDownTimer, MB_MASTER_TIMEOUT_MS_RESPOND , 0 );
	xTimerReset( MasterCountDownTimer, 0 );

//    rt_timer_control(&timer, RT_TIMER_CTRL_SET_TIME, &timer_tick);

//    rt_timer_start(&timer);
}

void vMBMasterPortTimersDisable()
{
////    rt_timer_stop(&timer);
//	TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE);
//  TIM_Cmd(TIM3,DISABLE);
	xTimerStop( MasterCountDownTimer, 0 );
}

void prvvMasterTIMERExpiredISR(void)
{
    (void) pxMBMasterPortCBTimerExpired();
}

//void TIM3_IRQHandler(void)
//{
//	prvvTIMERExpiredISR();
//}

//static void timer_timeout_ind(void* parameter)
//{
//    prvvTIMERExpiredISR();
//}

#endif
#endif
