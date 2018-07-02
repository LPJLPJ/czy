#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// JbusClass 对话框

class JbusClass : public CDialogEx
{
	DECLARE_DYNAMIC(JbusClass)

public:
	JbusClass(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~JbusClass();

// 对话框数据
	enum { IDD = IDD_JBUS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_jbusListCtrl;
	CEdit m_jbusEdit;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkJbusList(NMHDR *pNMHDR, LRESULT *pResult);
	int hitRow;
	int hitCol;
	afx_msg void OnNMClickJbusList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownJbusList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReturnJbusList(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_sendButton;
	afx_msg void OnBnClickedSendButton();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLvnItemchangedJbusList(NMHDR *pNMHDR, LRESULT *pResult);
	CEdit m_reg_edit;
	afx_msg void OnEnKillfocusRegEdit();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int regID;
	int tagID;
	int tagValue;
	afx_msg void OnBnClickedSendButton2();
};
