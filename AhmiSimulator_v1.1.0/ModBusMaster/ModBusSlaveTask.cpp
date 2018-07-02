////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     DynamicPage.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/12 /10 by 于春营
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////
#include "mb.h"
#include "mbrtu.h"
#include "FreeRTOS.h"
#include "task.h"
#ifdef USE_MODBUS_SLAVE
/*******************************************************************************
* Function Name  : ModBusSlaveTask
* Description    : 该函数为ModBus Slave 的任务函数，为该任务的入口.
* Input          : pvParameters
* Output         : None
* Return         : None
*******************************************************************************/
void ModBusSlaveTask(void* pvParameters)
{
	eMBErrorCode    eStatus;
	/*模式	 从机地址 端口 波特率 校验位*/
	eStatus = eMBInit( MB_RTU, 0x01, 1, 115200, MB_PAR_NONE );
	
	/* Enable the Modbus Protocol Stack. */
	eStatus = eMBEnable();
	
	for( ;; )
	{
	 ( void )eMBPoll();
	 	vTaskDelay(2 / portTICK_RATE_MS);
	}

}
#endif
	
