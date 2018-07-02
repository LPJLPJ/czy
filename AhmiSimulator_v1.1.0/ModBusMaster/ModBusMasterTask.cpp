////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     DynamicPage.c
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/12 /02 by 于春营
// Revision 2.01 - File modify 2016/03 /02 by 于春营
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////

//#include "stm32f10x.h"
//#include <windows.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "publicInclude.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mb_m.h"
#include "mbframe.h"
#include "mbcrc.h"
#include "mbport.h"
#include "mbrtu.h"
#include "user_mb_app.h"

#ifdef USE_MODBUS_MASTER

extern QueueHandle_t   ModbusMasterRWTaskQueue;
extern ConfigInfoClass      ConfigData;
extern TagUpdateClassPtr    TagUpdatePtr;
extern unsigned int   usMRegHoldBuf[MB_MASTER_TOTAL_SLAVE_NUM][M_REG_HOLDING_NREGS];
#ifdef PC_SIM
extern "C" void CommSendChar(unsigned char* data,int size);
#endif

extern int flag_UartOpen;
extern int ModbusReadEn;

void ModBusMasterReadWaiteTask(void* pvParameters)
{
//	int i,count,write_count;
	//eMBMasterReqErrCode    errorCode = MB_MRE_NO_ERR;
//	ModBusMasterRWMsg mModBusRWMsg;
//	int delayCountMS;
//	int tempRemainderDelay;
//	int previousRegID;

	while(flag_UartOpen == 0)
	{
		vTaskDelay(1000 / portTICK_RATE_MS);
	}

	while(1)
	{
		
			vTaskDelay(1000 / portTICK_RATE_MS);

	}
}

void ModBusMasterRWTask(void* pvParameters)
{

	BaseType_t xStatus; 
	ModBusMasterRWMsg ModBusMasterRWMsgRecv;

	while(flag_UartOpen == 0)
	{
		vTaskDelay(1000 / portTICK_RATE_MS);
	}

	while(1)
	{
		xStatus=xQueueReceive(ModbusMasterRWTaskQueue,
 							(void*)&ModBusMasterRWMsgRecv,
 							portMAX_DELAY);
		if(xStatus!=pdPASS)
 		{
 			return;
 		}

		if(ModBusMasterRWMsgRecv.opCode == AHMI_READ_CODE)
		{
			
		  AHMI_READ(ModBusMasterRWMsgRecv.regID,ModBusMasterRWMsgRecv.numofReg);
		}
		else if(ModBusMasterRWMsgRecv.opCode == AHMI_WRITE_CODE)
		{
		
			AHMI_WRITE(ModBusMasterRWMsgRecv.regID,ModBusMasterRWMsgRecv.numofReg,&usMRegHoldBuf[0][ModBusMasterRWMsgRecv.regID]);
			
		}
		eMBMasterPoll();
		vTaskDelay(50 / portTICK_RATE_MS);
			
	}

}

void ModBusMasterTask(void* pvParameters)
{
	while(flag_UartOpen == 0)
	{
		vTaskDelay(10 / portTICK_RATE_MS);
	}

	eMBMasterInit(MB_RTU, 2, 115200,  MB_PAR_EVEN);
	eMBMasterEnable();
	ModbusReadEn = 1;
	
#ifdef EMBEDDED 
	flag_UartOpen = 1;
#endif

	while (1)
	{
	
		eMBMasterPoll();
		vTaskDelay(1 / portTICK_RATE_MS);
	
	}

}

#ifdef PC_SIM
eMBErrorCode
eMBMasterRTUSend( UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;

	//int i;

    if ( ucSlaveAddress > MB_MASTER_TOTAL_SLAVE_NUM ) return MB_EINVAL;

    ENTER_CRITICAL_SECTION(  );


    if( eRcvState == STATE_M_RX_IDLE )
    {
        /* First byte before the Modbus-PDU is the slave address. */
        pucMasterSndBufferCur = ( UCHAR * ) pucFrame - 1;
        usMasterSndBufferCount = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pucMasterSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        usMasterSndBufferCount += usLength;

        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) pucMasterSndBufferCur, usMasterSndBufferCount );
        ucMasterRTUSndBuf[usMasterSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        ucMasterRTUSndBuf[usMasterSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );

        /* Activate the transmitter. */
        eSndState = STATE_M_TX_XMIT;

//	m_strCtrlLightBL = str;

		//ucMasterRTUSndBuf[usMasterSndBufferCount] = "0";
	/*COleVariant m_OleVariant = ucMasterRTUSndBuf;*/

		CommSendChar((unsigned char*)ucMasterRTUSndBuf,usMasterSndBufferCount);


	//	for(i = 0;i < usMasterSndBufferCount;i++)
	//	{
	//		array.SetAt(i,ucMasterRTUSndBuf[i]);
	//	}
	//COMCommPtr->put_Output(COleVariant(array));
        vMBMasterPortSerialEnable( FALSE, TRUE );

		usMasterSndBufferCount = 0;

		xFrameIsBroadcast = ( ucMasterRTUSndBuf[MB_SER_PDU_ADDR_OFF] == MB_ADDRESS_BROADCAST ) ? TRUE : FALSE;
            /* Disable transmitter. This prevents another transmit buffer
             * empty interrupt. */
            vMBMasterPortSerialEnable( TRUE, FALSE );
            eSndState = STATE_M_TX_XFWR;
            /* If the frame is broadcast ,master will enable timer of convert delay,
             * else master will enable timer of respond timeout. */
            if ( xFrameIsBroadcast == TRUE )
            {
            	vMBMasterPortTimersConvertDelayEnable( );
            }
            else
            {
            	vMBMasterPortTimersRespondTimeoutEnable( );
            }
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}
#endif
#endif

