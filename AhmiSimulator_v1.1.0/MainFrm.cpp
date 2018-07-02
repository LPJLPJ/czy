////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:     MainFrm.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//    CMainFrame 类的实现
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
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

//CMSComm *COMCommPtr;// changed by XT, 20160912
CMySerialPort *SerialPortPtr; // add by XT, 20160912
ShowTraceInfo *ConcoleDlgPtr;
// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
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
	//	TRACE0("未能创建工具栏\n");
	//	return -1;      // 未能创建
	//}

	//if (!m_wndStatusBar.Create(this) ||
	//	!m_wndStatusBar.SetIndicators(indicators,
	//	  sizeof(indicators)/sizeof(UINT)))
	//{
	//	TRACE0("未能创建状态栏\n");
	//	return -1;      // 未能创建
	//}

	//// TODO: 如果不需要工具栏可停靠，则删除这三行
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
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式.



	cs.cx = globalArgs.ScreenWidth+20;
	cs.cy = globalArgs.ScreenHeigth+62;
	cs.style |= WS_OVERLAPPEDWINDOW;
	cs.style &= ~WS_THICKFRAME;
	cs.style &= ~FWS_ADDTOTITLE;
	cs.lpszName = "AHMI Simulator";

	return TRUE;
}


// CMainFrame 诊断

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


// CMainFrame 消息处理程序

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
//		variant_inp=COMCommPtr->get_Input(); //读缓冲区
//        safearray_inp=variant_inp; //VARIANT型变量转换为ColeSafeArray型变量
//        len=safearray_inp.GetOneDimSize(); //得到有效数据长度
//        for(k=0;k<len;k++)
//            safearray_inp.GetElement(&k,Revcbuf+k);//转换为BYTE型数组
//		
//		RevcLength = len;
//		SetEvent(pvUartEvent);
//	}
}

void CMainFrame::DoDataExchange(CDataExchange* pDX)
{
	// TODO: 在此添加专用代码和/或调用基类

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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//MouseClickData.x = point.x;
	//MouseClickData.y = point.y;
	//MouseClickData.press = 1 ;
	//SetEvent(pvMouseEvent);
	CFrameWnd::OnLButtonDown(nFlags, point);
}


void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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