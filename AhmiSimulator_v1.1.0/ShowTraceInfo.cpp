////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:      ShowTraceInfo.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//    实现文件
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicDefine.h"

#ifdef PC_SIM

#include "stdafx.h"
#include "mfc.h"
#include "ShowTraceInfo.h"
#include "afxdialogex.h"

extern ShowTraceInfo *ConcoleDlgPtr;
// ShowTraceInfo 对话框

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

// ShowTraceInfo 消息处理程序


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
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。


	// TODO:  在此添加控件通知处理程序代码
}

#endif