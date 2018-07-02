// MainFrm.h : CMainFrame ��Ľӿ�
//
#include "publicDefine.h"
#ifdef PC_SIM

#pragma once
#include "ShowTraceInfo.h"
#include "SerialPort.h" // add by XT, 20160912

class CMainFrame : public CFrameWnd
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
public:

// ����
public:
	//CMSComm  ComPort; // changed by XT, 20160912
	CMySerialPort SerialPort; // add by XT, 20160912
	ShowTraceInfo mShowTraceInfo;
// ��д
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnComm();
	DECLARE_EVENTSINK_MAP()
public:

	virtual void DoDataExchange(CDataExchange* pDX);
	void StartFreeRTOSThread(void);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
int AHMITop(void);

#endif