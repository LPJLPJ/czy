#include "publicDefine.h"
#ifdef PC_SIM

#pragma once
#include "afxwin.h"


// ShowTraceInfo 对话框

class ShowTraceInfo : public CDialog
{
	DECLARE_DYNAMIC(ShowTraceInfo)

public:
	ShowTraceInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ShowTraceInfo();

// 对话框数据
	enum { IDD = IDD_SHOWTRACEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString czConcole;
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEdit1();
	CListBox m_lbxPrompt;
};

#endif
