#include "publicDefine.h"
#ifdef PC_SIM

// mfc.h : mfc Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "CMSComm.h"

// CmfcApp:
// �йش����ʵ�֣������ mfc.cpp
//

class CmfcApp : public CWinApp
{
public:
	

public:
	CmfcApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnUart();
//	afx_msg void OnComm();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	afx_msg void OnFileOpen();
};

extern CmfcApp theApp;

#endif