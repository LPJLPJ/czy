////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:     DynamicPage.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/12 /10 by �ڴ�Ӫ
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
* Description    : �ú���ΪModBus Slave ����������Ϊ����������.
* Input          : pvParameters
* Output         : None
* Return         : None
*******************************************************************************/
void ModBusSlaveTask(void* pvParameters)
{
	eMBErrorCode    eStatus;
	/*ģʽ	 �ӻ���ַ �˿� ������ У��λ*/
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
	
