/*
 * FreeModbus Libary: MSP430 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
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
 * File: $Id: porttimer.c,v 1.3 2007/06/12 06:42:01 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "private.h"
#include "timers.h"

/* ----------------------- Defines ------------------------------------------*/
/* Timer ticks are counted in multiples of 50us. Therefore 20000 ticks are
 * one second.
 */
#ifdef USE_MODBUS_SLAVE
#define MB_TIMER_TICKS          ( 20000L )

TimerHandle_t SlaveCountDownTimer;
/* ----------------------- Start implementation -----------------------------*/
void SlaveCountDownTimerCallBackFunc(TimerHandle_t pxTimer)
{
	( void )pxMBPortCBTimerExpired();
}

BOOL
xMBPortTimersInit( USHORT usTim1Timeout50us )
{  
//  SysTick_ITConfig(ENABLE);
//  SysTick_SetReload(1000*usTim1Timeout50us);
//  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

//TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
////TIM_OCInitTypeDef  TIM_OCInitStructure;
//
//   /* TIM6 clock enable */
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//  /* Time base configuration */
//  TIM_TimeBaseStructure.TIM_Period = 1000*usTim1Timeout50us;    	//下一个更新事件装入活动的自动重装载寄存器周期的值。它的取值必须在0x0000和0xFFFF之间。
//  TIM_TimeBaseStructure.TIM_Prescaler = 55;					    //预分频值
//  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟分割
//  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
//  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
//
//  TIM_ClearFlag(TIM2, TIM_FLAG_Update); //清除溢出中断标志
////  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
//  TIM_Cmd(TIM2,ENABLE);
	SlaveCountDownTimer = xTimerCreate
		          (  /* Just a text name, not used by the RTOS kernel. */
                     "SlaveCountDownTimer",
                     /* The timer period in ticks. */
                     usTim1Timeout50us,
                     /* The timers will auto-reload themselves when they expire. */
                     0,
                     /* Assign each timer a unique id equal to its array index. */
                     ( void * ) 0,
                     /* Each timer calls the same callback when it expires. */
                     SlaveCountDownTimerCallBackFunc
                   );


  return 1;
}

void
vMBPortTimersEnable( void )
{
//	SysTick_CounterCmd(SysTick_Counter_Clear);
//	SysTick_CounterCmd(SysTick_Counter_Enable);

  //TIM_ClearFlag(TIM2, TIM_FLAG_Update); //清除溢出中断标志
  //TIM_SetCounter(TIM2,0x00);			//清零计数器值
  //TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
  //TIM_Cmd(TIM2,ENABLE);

  //GPIO_SetBits(GPIOC, GPIO_Pin_0);
	xTimerChangePeriod( SlaveCountDownTimer,35, 0 );
	xTimerReset( SlaveCountDownTimer, 0 );
}

void
vMBPortTimersDisable( void )
{
//	SysTick_CounterCmd(SysTick_Counter_Disable);

  /*TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);
  TIM_Cmd(TIM2,DISABLE);

  GPIO_ResetBits(GPIOC, GPIO_Pin_0);*/
	xTimerStop( SlaveCountDownTimer, 0 );
}


//void prvvTIMERExpiredISR( void )
//{
//	( void )pxMBPortCBTimerExpired();
//}
#endif

