// MainFrm.h : CMainFrame 类的接口
//
#include "publicDefine.h"
#ifdef PC_SIM

#pragma once
#include "ShowTraceInfo.h"
#include "SerialPort.h" // add by XT, 20160912

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 属性
public:

// 操作
public:
	//CMSComm  ComPort; // changed by XT, 20160912
	CMySerialPort SerialPort; // add by XT, 20160912
	ShowTraceInfo mShowTraceInfo;
// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	
// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// 生成的消息映射函数
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