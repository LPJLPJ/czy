/***********************
Private Function:
DisplayExec();

**************************/





/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbframe.h"
#include "mbproto.h"
#include "mbconfig.h"
//#include "private.h"

#ifdef USE_MODBUS_SLAVE

extern TagClassPtr TagPtr;
extern UartClassPtr UartPtr;
extern QueueHandle_t   UartDataRecvQueue;
extern ConfigInfo      ConfigData;
extern QueueHandle_t   ActionInstructionQueue;

/* ----------------------- Defines ------------------------------------------*/
#define MB_PDU_FUNC_EXEC_ATTR_ADDR_OFF           ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_EXEC_LEN_ADDR_OFF            ( MB_PDU_DATA_OFF + 1 )
#define MB_PDU_FUNC_EXEC_DATA_ADDR_OFF           ( MB_PDU_DATA_OFF + 2 )


eMBException
eMBFuncDisplayExec( UCHAR * pucFrame, USHORT * usLen )
{
	eMBException    eStatus = MB_EX_NONE;
//  eMBErrorCode    eRegStatus;
	int 						mbAttr = 0;
	int  						ActionAddr;
	int							DataLen = 0;
	int 						UartBufID = 0;
	int 						i = 0;
	
	
	mbAttr = (USHORT)(pucFrame[MB_PDU_FUNC_EXEC_ATTR_ADDR_OFF] & 0x10) ? 1 : 0;
	DataLen = (USHORT)pucFrame[MB_PDU_FUNC_EXEC_LEN_ADDR_OFF];
	UartBufID = (USHORT)pucFrame[MB_PDU_FUNC_EXEC_ATTR_ADDR_OFF] & 0x0F;
	for(i = 0;i < DataLen;i++)
	{
		UartPtr[UartBufID].UartBuffer[i] = (USHORT)(pucFrame[MB_PDU_FUNC_EXEC_DATA_ADDR_OFF + i]);
	}
	if(mbAttr)
	{
		TagSetValue(&(TagPtr[UartPtr[UartBufID].LinkTag]),DataLen);
	}
	else
	{
		ActionAddr = (int)UartPtr[UartBufID].UartBuffer;
		xQueueSendToBack(ActionInstructionQueue,&ActionAddr,NULL);
	}
	
	return eStatus;
}
#endif
