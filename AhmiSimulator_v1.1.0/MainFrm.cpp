////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:     MainFrm.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//    CMainFrame ���ʵ��
// 
////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "mfc.h"
#include "CMSComm.h"
#include "MainFrm.h"
#include "UartCtrlDlg.h"
#include "publicInclude.h"
#include "JbusClass.h"

#ifdef PC_SIM
//#include "port.h"





//#include "portmacro.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern"C"
{
extern unsigned char Revcbuf[256];
extern int  RevcLength;
extern void *pvUartEvent;
extern long xPortStartScheduler( void );
extern struct globalArgs_t globalArgs;
}
extern char AHMIFileName[200];

void* pvFILELoadDone = NULL;
void* pvSerilPortReceivedByte = NULL;// changed by XT, 20160912
static DWORD WINAPI  FreeRTOSThread(LPVOID lpParameter);
static DWORD WINAPI  SerialPortRecvByteThread(LPVOID lpParameter);// changed by XT, 20160912
static DWORD WINAPI  SerialPortListenThread(LPVOID lpParameter);// changed by XT, 20160912


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(IDC_MSCOMM1, &CMainFrame::OnComm)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

//CMSComm *COMCommPtr;// changed by XT, 20160912
CMySerialPort *SerialPortPtr; // add by XT, 20160912
ShowTraceInfo *ConcoleDlgPtr;
// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("δ�ܴ���������\n");
	//	return -1;      // δ�ܴ���
	//}

	//if (!m_wndStatusBar.Create(this) ||
	//	!m_wndStatusBar.SetIndicators(indicators,
	//	  sizeof(indicators)/sizeof(UINT)))
	//{
	//	TRACE0("δ�ܴ���״̬��\n");
	//	return -1;      // δ�ܴ���
	//}

	//// TODO: �������Ҫ��������ͣ������ɾ��������
	//::ShowWindow(m_wndToolBar,SW_HIDE);

//	ComPort.Create(NULL,WS_VISIBLE|WS_CHILD,CRect(0,0,0,0),this,IDC_MSCOMM1);  // changed by XT, 20160912
//	COMCommPtr = &(this->ComPort); // changed by XT, 20160912

	SerialPortPtr = &SerialPort;

	ConcoleDlgPtr = &(this->mShowTraceInfo);
	mShowTraceInfo.Create(IDD_SHOWTRACEINFO);
	mShowTraceInfo.ShowWindow(TRUE);
	StartFreeRTOSThread();
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ.



	cs.cx = globalArgs.ScreenWidth+20;
	cs.cy = globalArgs.ScreenHeigth+62;
	cs.style |= WS_OVERLAPPEDWINDOW;
	cs.style &= ~WS_THICKFRAME;
	cs.style &= ~FWS_ADDTOTITLE;
	cs.lpszName = "AHMI Simulator";

	return TRUE;
}


// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame ��Ϣ�������

BEGIN_EVENTSINK_MAP(CMainFrame, CFrameWnd)
    //{{AFX_EVENTSINK_MAP(CFMCCommDlg)
	ON_EVENT(CMainFrame, IDC_MSCOMM1, 1 /* OnComm */, OnComm, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// changed by XT 20160912
void CMainFrame::OnComm()
{
//	VARIANT variant_inp;
//	COleSafeArray safearray_inp;
//	LONG len,k;
//    CString strtemp;
//	if(COMCommPtr->get_CommEvent()==2)
//	{
//		variant_inp=COMCommPtr->get_Input(); //��������
//        safearray_inp=variant_inp; //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
//        len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
//        for(k=0;k<len;k++)
//            safearray_inp.GetElement(&k,Revcbuf+k);//ת��ΪBYTE������
//		
//		RevcLength = len;
//		SetEvent(pvUartEvent);
//	}
}

void CMainFrame::DoDataExchange(CDataExchange* pDX)
{
	// TODO: �ڴ����ר�ô����/����û���

	CFrameWnd::DoDataExchange(pDX);
}

// changed by XT 20160912
void CMainFrame::StartFreeRTOSThread(void)
{
	void *pvHandle;
	pvFILELoadDone = CreateEvent(NULL,FALSE,FALSE,NULL);
	pvHandle = CreateThread( NULL, 0, FreeRTOSThread, NULL, 0, NULL );
	if( pvHandle != NULL )
	{
		SetThreadPriority( pvHandle, THREAD_PRIORITY_BELOW_NORMAL );
		SetThreadPriorityBoost( pvHandle, TRUE );
		SetThreadAffinityMask( pvHandle, 0x01 );
	}
	if(globalArgs.FileName != NULL)
	{
		
		strcpy_s(AHMIFileName,globalArgs.FileName);
		SetEvent(pvFILELoadDone);
	}

	// create serialport listen thread
	pvHandle = CreateThread( NULL, 0, SerialPortListenThread, NULL, 0, NULL );
	if( pvHandle != NULL )
	{
		SetThreadPriority( pvHandle, THREAD_PRIORITY_BELOW_NORMAL );
		SetThreadPriorityBoost( pvHandle, TRUE );
		SetThreadAffinityMask( pvHandle, 0x01 );
	}



}

// changed by XT 20160912
static DWORD WINAPI  SerialPortListenThread(LPVOID lpParameter)
{
	void *pvHandle;

	pvSerilPortReceivedByte = CreateEvent(NULL,FALSE,FALSE,NULL);

	while(!SerialPortPtr->IsOpened()) Sleep(10);// wait serialport open

	//create recv byte thread

	pvHandle = CreateThread( NULL, 0, SerialPortRecvByteThread, NULL, 0, NULL );
	if( pvHandle != NULL )
	{
		SetThreadPriority( pvHandle, THREAD_PRIORITY_BELOW_NORMAL );
		SetThreadPriorityBoost( pvHandle, TRUE );
		SetThreadAffinityMask( pvHandle, 0x01 );
	}

	
	// listening
	while(1)
	{
		if(SerialPortPtr->ReadDataWaiting())
		{
			SetEvent(pvSerilPortReceivedByte);
		}
		Sleep(10);

	}
}


// changed by XT 20160912
static DWORD WINAPI  SerialPortRecvByteThread(LPVOID lpParameter)
{
	while(1)
	{
		WaitForSingleObject(pvSerilPortReceivedByte,INFINITE);
		RevcLength = SerialPortPtr->ReadData(Revcbuf,256);
		SetEvent(pvUartEvent);
	}
}

static DWORD WINAPI  FreeRTOSThread(LPVOID lpParameter)
{
	WaitForSingleObject(pvFILELoadDone,INFINITE);
	AHMITop();
	return 0;
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//MouseClickData.x = point.x;
	//MouseClickData.y = point.y;
	//MouseClickData.press = 1 ;
	//SetEvent(pvMouseEvent);
	CFrameWnd::OnLButtonDown(nFlags, point);
}


void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//MouseClickData.x = point.x;
	//MouseClickData.y = point.y;
	//MouseClickData.press = 0 ;
	//SetEvent(pvMouseEvent);
	CFrameWnd::OnLButtonUp(nFlags, point);
}

// changed by XT 20160912
extern "C" void CommSendChar(unsigned char* data,int size)
{
//	CString m_strCtrlLightBL;

	//conver char to string
//	int i;

//	m_strCtrlLightBL = str;

	//CByteArray array;

	//for(i = 0;i < size;i++)
	//{
	//	array.Add(data[i]);
	//}
	SerialPortPtr->SendData((char*)data,size);


}

#endif