#include "publicDefine.h"
#ifdef PC_SIM

#pragma once
#include "afxwin.h"


// ShowTraceInfo �Ի���

class ShowTraceInfo : public CDialog
{
	DECLARE_DYNAMIC(ShowTraceInfo)

public:
	ShowTraceInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~ShowTraceInfo();

// �Ի�������
	enum { IDD = IDD_SHOWTRACEINFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString czConcole;
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEdit1();
	CListBox m_lbxPrompt;
};

#endif
