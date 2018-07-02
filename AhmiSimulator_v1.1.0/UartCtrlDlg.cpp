////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMI小组成员
//
// Create Date:   2015/11/17
// File Name:       UartCtrlDly.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//     实现文件
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicDefine.h"

#ifdef PC_SIM

#include "stdafx.h"
#include "mfc.h"
#include "UartCtrlDlg.h"
#include "afxdialogex.h"


// UartCtrlDly 对话框

extern int flag_UartOpen ;

IMPLEMENT_DYNAMIC(UartCtrlDlg, CDialog)

	UartCtrlDlg::UartCtrlDlg(CMySerialPort* mSerialPort,CWnd* pParent /*=NULL*/)
	: CDialog(UartCtrlDlg::IDD, pParent)
	, ComSettingValue(_T(""))
{
	//ComPort = mUartPtr;// changed by XT, 20160912
	SerialPort = mSerialPort;// changed by XT, 20160912
}

UartCtrlDlg::~UartCtrlDlg()
{
}

void UartCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//  DDX_CBString(pDX, IDC_COMBO1, Com_Setting_value);
	//  DDX_Control(pDX, IDC_EDIT1, ComPortCombo);
	DDX_Control(pDX, IDC_COMBO1, ComPortCombo);
	DDX_Text(pDX, IDC_EDIT1, ComSettingValue);
}


BEGIN_MESSAGE_MAP(UartCtrlDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, &UartCtrlDlg::OnCbnSelchangeCombo1)
	ON_EN_CHANGE(IDC_EDIT1, &UartCtrlDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDOK, &UartCtrlDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// UartCtrlDlg 消息处理程序


void UartCtrlDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
}


void UartCtrlDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


int UartCtrlDlg::Init(void)
{
		

//	UpdateData(0);
	return 0;
}


BOOL UartCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    int rtn;  
	HKEY hKey;  
    //m_cmbComm.ResetContent();  
    rtn = RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"),  
        NULL, KEY_READ, &hKey); // 打开串口注册表  
	if(rtn == ERROR_SUCCESS)
	{
        int i=0;  
        CHAR portName[256],commName[256];  
        DWORD dwLong,dwSize;  
		ComPortCombo.ResetContent();
        while(1)  
        {  
            dwSize = sizeof(portName);  
            dwLong = dwSize;  
            rtn = RegEnumValue( hKey, i, portName, &dwLong,  
                NULL, NULL, (PUCHAR)commName, &dwSize );  

            if( rtn == ERROR_NO_MORE_ITEMS ) // 枚举串口  
            {
                break;  
            }
			mCOMID[i].ID = i;
			mCOMID[i].Name = commName;
            ComPortCombo.AddString(commName);
            i++;  
        }
    }
    RegCloseKey(hKey);  
	ComPortCombo.SetCurSel (0); 
	ComSettingValue = "115200";
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control

	// 异常: OCX 属性页应返回 FALSE
}


void UartCtrlDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	int Pos = ComPortCombo.GetCurSel();
	int COMNumber;	
	int bund;
	
	
	sscanf_s(mCOMID[Pos].Name,"COM%d",&COMNumber);
	sscanf_s(ComSettingValue,"%d",&bund);

	//ComPort->put_CommPort(COMNumber);
	//ComPort->put_Settings(ComSettingValue);
	//ComPort->put_InBufferSize(1024);
	//ComPort->put_OutBufferSize(1024);
	//ComPort->put_InputLen(0);
	//ComPort->put_InputMode(1);
	//ComPort->put_RThreshold(1);
	if(SerialPort->Open(COMNumber,bund))
	{
		//ComPort->put_PortOpen(TRUE);
		MessageBox(_T("该串口已经打开"));
		flag_UartOpen = 1;
	}
	else 
	{
	//	ComPort->put_OutBufferCount(0);
		MessageBox(_T("串口打开失败"));
		flag_UartOpen = 0;
	}
	CString str=_T("Serial Port Open.");
	SerialPort->SendData(str,str.GetLength());
	CDialog::OnOK();
}

#endif