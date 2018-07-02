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
// Additional Comments:
//    
// 
////////////////////////////////////////////////////////////////////////////////
#include"private.h"
#include "mb.h"
#include "mbframe.h"

#include "mbcrc.h"
#include "mbport.h"
#include "mbrtu.h"

#ifdef USE_MODBUS_SLAVE
extern int flag_UartOpen;

void CommSendChar(unsigned char* data,int size);
void ModBusSlaveTask(void* pvParameters)
{
	eMBErrorCode    eStatus;

	while(flag_UartOpen == 0)
	{
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
	/*模式	 从机地址 端口 波特率 校验位*/

	eStatus = eMBInit( MB_RTU, 0x0A, 1, 115200, MB_PAR_EVEN );
	
	/* Enable the Modbus Protocol Stack. */
	eStatus = eMBEnable(  );
	
	for( ;; )
	{
	 ( void )eMBPoll(  );
	 /* Here we simply count the number of poll cycles. */
//	 usRegInputBuf[0]++;
	}

}

eMBErrorCode
eMBRTUSend( UCHAR ucSlaveAddress, const UCHAR * pucFrame, USHORT usLength )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    USHORT          usCRC16;
    ENTER_CRITICAL_SECTION(  );

    /* Check if the receiver is still in idle state. If not we where to
     * slow with processing the received frame and the master sent another
     * frame on the network. We have to abort sending the frame.
     */
    if( eRcvState == STATE_RX_IDLE )
    {
        /* First byte before the Modbus-PDU is the slave address. */
        pucSndBufferCur = ( UCHAR * ) pucFrame - 1;
        usSndBufferCount = 1;

        /* Now copy the Modbus-PDU into the Modbus-Serial-Line-PDU. */
        pucSndBufferCur[MB_SER_PDU_ADDR_OFF] = ucSlaveAddress;
        usSndBufferCount += usLength;

        /* Calculate CRC16 checksum for Modbus-Serial-Line-PDU. */
        usCRC16 = usMBCRC16( ( UCHAR * ) pucSndBufferCur, usSndBufferCount );
        ucRTUBuf[usSndBufferCount++] = ( UCHAR )( usCRC16 & 0xFF );
        ucRTUBuf[usSndBufferCount++] = ( UCHAR )( usCRC16 >> 8 );

        /* Activate the transmitter. */
        eSndState = STATE_TX_XMIT;
        vMBPortSerialEnable( FALSE, TRUE );

		CommSendChar( ucRTUBuf,usSndBufferCount);

		 xMBPortEventPost( EV_FRAME_SENT );
            /* Disable transmitter. This prevents another transmit buffer
             * empty interrupt. */
            vMBPortSerialEnable( TRUE, FALSE );
            eSndState = STATE_TX_IDLE;
    }
    else
    {
        eStatus = MB_EIO;
    }
    EXIT_CRITICAL_SECTION(  );
    return eStatus;
}
#endif
