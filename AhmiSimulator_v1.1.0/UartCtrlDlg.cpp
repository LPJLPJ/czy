////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:       UartCtrlDly.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//     ʵ���ļ�
// 
////////////////////////////////////////////////////////////////////////////////
#include "publicDefine.h"

#ifdef PC_SIM

#include "stdafx.h"
#include "mfc.h"
#include "UartCtrlDlg.h"
#include "afxdialogex.h"


// UartCtrlDly �Ի���

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


// UartCtrlDlg ��Ϣ�������


void UartCtrlDlg::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void UartCtrlDlg::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
        NULL, KEY_READ, &hKey); // �򿪴���ע���  
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

            if( rtn == ERROR_NO_MORE_ITEMS ) // ö�ٴ���  
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

	// �쳣: OCX ����ҳӦ���� FALSE
}


void UartCtrlDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		MessageBox(_T("�ô����Ѿ���"));
		flag_UartOpen = 1;
	}
	else 
	{
	//	ComPort->put_OutBufferCount(0);
		MessageBox(_T("���ڴ�ʧ��"));
		flag_UartOpen = 0;
	}
	CString str=_T("Serial Port Open.");
	SerialPort->SendData(str,str.GetLength());
	CDialog::OnOK();
}

#endif