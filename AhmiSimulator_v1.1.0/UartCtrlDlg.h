#include "publicDefine.h"
#ifdef PC_SIM

#pragma once
#include "afxwin.h"
#include "CMSComm.h"
#include "SerialPort.h"// changed by XT, 20160912

// UartCtrlDly �Ի���

struct COMID
{
	int ID;
	CString  Name;
};

class UartCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(UartCtrlDlg)
private:
//	CMSComm* ComPort; // changed by XT, 20160912
	CMySerialPort * SerialPort; // add by XT, 20160912
	COMID    mCOMID[20];
public:
	UartCtrlDlg(CMySerialPort * mSerialPort,CWnd* pParent = NULL);   // ��׼���캯��  // changed by XT, 20160912
	virtual ~UartCtrlDlg();

// �Ի�������
	enum { IDD = IDD_UARTCTRLDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnEnChangeEdit1();
 	CComboBox ComPortCombo;
	CString ComSettingValue;
	int Init(void);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};

#endif