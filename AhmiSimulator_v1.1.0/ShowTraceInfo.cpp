////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:      ShowTraceInfo.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//    ʵ���ļ�
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicDefine.h"

#ifdef PC_SIM

#include "stdafx.h"
#include "mfc.h"
#include "ShowTraceInfo.h"
#include "afxdialogex.h"

extern ShowTraceInfo *ConcoleDlgPtr;
// ShowTraceInfo �Ի���

IMPLEMENT_DYNAMIC(ShowTraceInfo, CDialog)

ShowTraceInfo::ShowTraceInfo(CWnd* pParent /*=NULL*/)
	: CDialog(ShowTraceInfo::IDD, pParent)
	, czConcole(_T(""))
{

}

ShowTraceInfo::~ShowTraceInfo()
{
}

void ShowTraceInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LBX_PROMPT, m_lbxPrompt);
}


BEGIN_MESSAGE_MAP(ShowTraceInfo, CDialog)
END_MESSAGE_MAP()

extern"C"
{
void TraceDlgPrint(char* printdata)
{
	CString  strPrompt = printdata;
	ConcoleDlgPtr->m_lbxPrompt.AddString(strPrompt);
	int count = 0;
	count = ConcoleDlgPtr->m_lbxPrompt.GetCount ();

	ConcoleDlgPtr->m_lbxPrompt.SetCurSel (count - 1);
}
}

void TraceDlgPrint_cpp(char* printdata)
{
	CString  strPrompt = printdata;
	ConcoleDlgPtr->m_lbxPrompt.AddString(strPrompt);
	int count = 0;
	count = ConcoleDlgPtr->m_lbxPrompt.GetCount ();

	ConcoleDlgPtr->m_lbxPrompt.SetCurSel (count - 1);
}

// ShowTraceInfo ��Ϣ�������


BOOL ShowTraceInfo::OnInitDialog()
{
	CDialog::OnInitDialog();
	HWND hWnd = ::GetDesktopWindow(); 
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	RECT DlgRect;
	GetWindowRect(&DlgRect);
	SetWindowPos(&wndTop ,rect.right-DlgRect.right,rect.bottom-DlgRect.bottom,0,0,SWP_NOSIZE |SWP_SHOWWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control

}


void ShowTraceInfo::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�


	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

#endif