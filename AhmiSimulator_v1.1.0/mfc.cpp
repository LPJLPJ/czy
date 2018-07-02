////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:      mfc.cpp :
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//     定义应用程序的类行为。
// 
////////////////////////////////////////////////////////////////////////////////



#include "publicDefine.h"

#ifdef PC_SIM
#include "stdafx.h"
#include "mfc.h"
#include "MainFrm.h"
#include "resource.h"
#include "mfcDoc.h"
#include "mfcView.h"
#include "CMSComm.h"
#include "UartCtrlDlg.h"
#include "getopt.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CmfcApp

BEGIN_MESSAGE_MAP(CmfcApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CmfcApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 标准打印设置命令
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_UART, &CmfcApp::OnUart)
//	ON_COMMAND(IDC_MSCOMM1, &CmfcApp::OnComm)
ON_COMMAND(ID_FILE_OPEN, &CmfcApp::OnFileOpen)
END_MESSAGE_MAP()

extern void* pvFILELoadDone;
extern CMySerialPort *SerialPortPtr;
extern char AHMIFileName[200];
extern "C"
extern void GetGlobalArgs();


// CmfcApp 构造

CmfcApp::CmfcApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CmfcApp 对象

CmfcApp theApp;


// CmfcApp 初始化

BOOL CmfcApp::InitInstance()
{

	//初始化全局参数
	GetGlobalArgs();


	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)
	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CmfcDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CmfcView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// 分析标准外壳命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);

	

	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 仅当具有后缀时才调用 DragAcceptFiles
	//  在 SDI 应用程序中，这应在 ProcessShellCommand  之后发生



	return TRUE;
}



// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CmfcApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CmfcApp 消息处理程序



void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnLButtonDown(nFlags, point);

}


void CmfcApp::OnUart()
{
	UartCtrlDlg uartDlg(SerialPortPtr);
	uartDlg.DoModal();
	// TODO: 在此添加命令处理程序代码
}


//void CmfcApp::OnComm()
//{
//	// TODO: 在此添加命令处理程序代码
//	VARIANT variant_inp;
//	COleSafeArray safearray_inp;
//	LONG len,k;
//    BYTE rxdata[2048]; //设置BYTE数组 An 8-bit integerthat is not signed.
//    CString strtemp;
//	if(COMCommPtr->get_CommEvent()==2)
//	{
//		variant_inp=COMCommPtr->get_Input(); //读缓冲区
//        safearray_inp=variant_inp; //VARIANT型变量转换为ColeSafeArray型变量
//        len=safearray_inp.GetOneDimSize(); //得到有效数据长度
//        for(k=0;k<len;k++)
//            safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组
//		
//
//	}
//}


CDocument* CmfcApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	// TODO: 在此添加专用代码和/或调用基类
	strcpy_s(AHMIFileName,lpszFileName);
	SetEvent(pvFILELoadDone);
	return CWinApp::OpenDocumentFile(lpszFileName);
}


void CmfcApp::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码
	CFileDialog dlg(true,NULL,NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT|OFN_FILEMUSTEXIST,_T("Data Files(*.acf)|*.acf|All Files (*.*)|*.*||"),NULL);
	if(dlg.DoModal() == IDOK)
	{
		CString Filename = dlg.GetFolderPath() + "\\";
		Filename = Filename + dlg.GetFileName();
		strcpy_s(AHMIFileName,Filename);
		SetEvent(pvFILELoadDone);
	}
}


#endif