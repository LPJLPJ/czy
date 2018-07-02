////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:      mfc.cpp :
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//     ����Ӧ�ó��������Ϊ��
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
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// ��׼��ӡ��������
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


// CmfcApp ����

CmfcApp::CmfcApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CmfcApp ����

CmfcApp theApp;


// CmfcApp ��ʼ��

BOOL CmfcApp::InitInstance()
{

	//��ʼ��ȫ�ֲ���
	GetGlobalArgs();


	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CmfcDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(CmfcView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);

	

	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand  ֮����



	return TRUE;
}



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CmfcApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CmfcApp ��Ϣ�������



void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnLButtonDown(nFlags, point);

}


void CmfcApp::OnUart()
{
	UartCtrlDlg uartDlg(SerialPortPtr);
	uartDlg.DoModal();
	// TODO: �ڴ���������������
}


//void CmfcApp::OnComm()
//{
//	// TODO: �ڴ���������������
//	VARIANT variant_inp;
//	COleSafeArray safearray_inp;
//	LONG len,k;
//    BYTE rxdata[2048]; //����BYTE���� An 8-bit integerthat is not signed.
//    CString strtemp;
//	if(COMCommPtr->get_CommEvent()==2)
//	{
//		variant_inp=COMCommPtr->get_Input(); //��������
//        safearray_inp=variant_inp; //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
//        len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
//        for(k=0;k<len;k++)
//            safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
//		
//
//	}
//}


CDocument* CmfcApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	// TODO: �ڴ����ר�ô����/����û���
	strcpy_s(AHMIFileName,lpszFileName);
	SetEvent(pvFILELoadDone);
	return CWinApp::OpenDocumentFile(lpszFileName);
}


void CmfcApp::OnFileOpen()
{
	// TODO: �ڴ���������������
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