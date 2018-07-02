#include "publicDefine.h"
#ifdef PC_SIM

// mfc.h : mfc 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "CMSComm.h"

// CmfcApp:
// 有关此类的实现，请参阅 mfc.cpp
//

class CmfcApp : public CWinApp
{
public:
	

public:
	CmfcApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUart();
//	afx_msg void OnComm();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	afx_msg void OnFileOpen();
};

extern CmfcApp theApp;

#endif