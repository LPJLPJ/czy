/**
  ******************************************************************************
  * @�ļ�   
  * @���� 
  * @�汾
  * @����
  * @��Ҫ
  ******************************************************************************
  * @ע������
  *
  *
  *
  ******************************************************************************
  */  

/* ͷ�ļ�		--------------------------------------------------------------*/
#include "mb.h"
#include "mbutils.h"
#include "private.h"
/* ˽���������� --------------------------------------------------------------*/
/* ˽�ж��� 	--------------------------------------------------------------*/
/* ˽�к궨�� 	--------------------------------------------------------------*/
/* ˽�б��� 	--------------------------------------------------------------*/
/* ˽�к������� --------------------------------------------------------------*/

/**
  * @����
  * @����
  * @����ֵ
  */ 
#ifdef USE_MODBUD_SLAVE
/* ----------------------- Defines ------------------------------------------*/
//#define REG_INPUT_START 1000
//#define REG_INPUT_NREGS 4
#define DISCRETE_INPUT_START        1
#define DISCRETE_INPUT_NDISCRETES   96
#define COIL_START                  1
#define COIL_NCOILS                 96
#define REG_INPUT_START             1
#define REG_INPUT_NREGS             100
#define REG_HOLDING_START           1
#define REG_HOLDING_NREGS           100
#define FALSH_USER_START_ADDRESS    (uint32_t)0x08010000           //�����û�������ʹ�õ��ڲ��������ʼ��ַ


/* ----------------------- Static variables ---------------------------------*/
//static USHORT   usRegInputStart = REG_INPUT_START;
//static USHORT   usRegInputBuf[REG_INPUT_NREGS];

static USHORT   usDiscreteInputStart                             = DISCRETE_INPUT_START;
static UCHAR    usDiscreteInputBuf[DISCRETE_INPUT_NDISCRETES/8]  ;
static USHORT   usCoilStart                                      = COIL_START;
static UCHAR    usCoilBuf[COIL_NCOILS/8]                         ;
static USHORT   usRegInputStart                                  = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS]                   ;
static USHORT   usRegHoldingStart                                = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS]               ;

//void ModBusTask(void* pvParameters)
//{
//	eMBErrorCode    eStatus;
//	/*ģʽ	 �ӻ���ַ �˿� ������ У��λ*/
//	eStatus = eMBInit( MB_RTU, 0x0A, 1, 115200, MB_PAR_EVEN );
//	
//	/* Enable the Modbus Protocol Stack. */
//	eStatus = eMBEnable(  );
//	
//	for( ;; )
//	{
//	 ( void )eMBPoll(  );
//	 /* Here we simply count the number of poll cycles. */
////	 usRegInputBuf[0]++;
//	}
//
//}
	
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
   eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
    if( ( usAddress >= REG_HOLDING_START ) &&
        ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
   eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex , iRegBitIndex , iNReg;
    iNReg =  usNCoils / 8 + 1;        //ռ�üĴ�������
    if( ( usAddress >= COIL_START ) &&
        ( usAddress + usNCoils <= COIL_START + COIL_NCOILS ) )
    {
        iRegIndex    = ( int )( usAddress - usCoilStart ) / 8 ;    //ÿ���Ĵ�����8��
		iRegBitIndex = ( int )( usAddress - usCoilStart ) % 8 ;	   //����ڼĴ����ڲ���λ��ַ
        switch ( eMode )
        {
            /* Pass current coil values to the protocol stack. */
        case MB_REG_READ:
            while( iNReg > 0 )
            {
				*pucRegBuffer++ = xMBUtilGetBits(&usCoilBuf[iRegIndex++] , iRegBitIndex , 8);
                iNReg --;
            }
			pucRegBuffer --;
			usNCoils = usNCoils % 8;                        //���µ���Ȧ��	
			*pucRegBuffer = *pucRegBuffer <<(8 - usNCoils); //��λ����
			*pucRegBuffer = *pucRegBuffer >>(8 - usNCoils);
            break;

            /* Update current coil values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while(iNReg > 1)									 //���������������������
            {
				xMBUtilSetBits(&usCoilBuf[iRegIndex++] , iRegBitIndex  , 8 , *pucRegBuffer++);
                iNReg--;
            }
			usNCoils = usNCoils % 8;                            //���µ���Ȧ��
			xMBUtilSetBits(&usCoilBuf[iRegIndex++] , iRegBitIndex  , usNCoils , *pucRegBuffer++);
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    /*static uint8_t flag = 0;
	
	flag == 0 ? GPIO_SetBits(GPIOC,GPIO_Pin_4) : GPIO_ResetBits(GPIOC,GPIO_Pin_4);  
	flag ^= 1;*/
	return MB_ENOREG;
}

/**
  * @}
  */


/*******************�ļ���β**************************************************/
#endif
