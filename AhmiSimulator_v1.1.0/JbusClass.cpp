// JbusClass.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mfc.h"
#include "JbusClass.h"
#include "afxdialogex.h"
#include "publicInclude.h"

extern  ConfigInfoClass		ConfigData;
extern  TagUpdateClassPtr    TagUpdatePtr;
extern TagClassPtr			TagPtr;
extern u16             WorkingPageID;

// JbusClass �Ի���

IMPLEMENT_DYNAMIC(JbusClass, CDialogEx)

JbusClass::JbusClass(CWnd* pParent /*=NULL*/)
	: CDialogEx(JbusClass::IDD, pParent)
	, hitRow(0)
	, hitCol(0)
	, regID(-1)
	, tagID(-1)
	, tagValue(0)
{

}

JbusClass::~JbusClass()
{
}

void JbusClass::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_JBUS_LIST, m_jbusListCtrl);
	DDX_Control(pDX, IDC_SEND_BUTTON, m_sendButton);
	DDX_Control(pDX, IDC_REG_EDIT, m_reg_edit);
}


BEGIN_MESSAGE_MAP(JbusClass, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_JBUS_LIST, &JbusClass::OnNMDblclkJbusList)
	ON_NOTIFY(NM_CLICK, IDC_JBUS_LIST, &JbusClass::OnNMClickJbusList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_JBUS_LIST, &JbusClass::OnLvnKeydownJbusList)
	ON_NOTIFY(NM_RETURN, IDC_JBUS_LIST, &JbusClass::OnNMReturnJbusList)
	ON_BN_CLICKED(IDC_SEND_BUTTON, &JbusClass::OnBnClickedSendButton)
	ON_BN_CLICKED(IDCANCEL, &JbusClass::OnBnClickedCancel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_JBUS_LIST, &JbusClass::OnLvnItemchangedJbusList)
	ON_EN_KILLFOCUS(IDC_REG_EDIT, &JbusClass::OnEnKillfocusRegEdit)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_BN_CLICKED(IDC_SEND_BUTTON2, &JbusClass::OnBnClickedSendButton2)
END_MESSAGE_MAP()


// JbusClass ��Ϣ�������


BOOL JbusClass::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��


	CRect rect;   
    // ��ȡ��������б���ͼ�ؼ���λ�úʹ�С   
    m_jbusListCtrl.GetClientRect(&rect);
	// Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����   
    m_jbusListCtrl.SetExtendedStyle(m_jbusListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES |  LVS_EX_FULLROWSELECT ); 
	 // Ϊ�б���ͼ�ؼ����4��   
	 m_jbusListCtrl.InsertColumn(0, _T("����"), LVCFMT_CENTER, rect.Width()/4, 0);  
    m_jbusListCtrl.InsertColumn(1, _T("TAG���"), LVCFMT_CENTER, rect.Width()/4, 0);   
    m_jbusListCtrl.InsertColumn(2, _T("�Ĵ�����"), LVCFMT_CENTER, rect.Width()/4, 1);   
    m_jbusListCtrl.InsertColumn(3, _T("��ǰTAGֵ"), LVCFMT_CENTER, rect.Width()/4, 2); 
	m_jbusListCtrl.InsertColumn(4, _T("�Ĵ���ֵ"), LVCFMT_CENTER, rect.Width()/4, 3); 
	m_jbusListCtrl.DeleteColumn(0);
	// ���ó�ʼֵ   
   // m_jbusListCtrl.InsertItem(0, _T("Java"));   
   // m_jbusListCtrl.SetItemText(0, 1, _T("1"));   
   // m_jbusListCtrl.SetItemText(0, 2, _T("1"));   
   // m_jbusListCtrl.InsertItem(1, _T("C"));   
   // m_jbusListCtrl.SetItemText(1, 1, _T("2"));   
   // m_jbusListCtrl.SetItemText(1, 2, _T("2"));   
   // m_jbusListCtrl.InsertItem(2, _T("C#"));   
   // m_jbusListCtrl.SetItemText(2, 1, _T("3"));   
   // m_jbusListCtrl.SetItemText(2, 2, _T("6"));   
   // m_jbusListCtrl.InsertItem(3, _T("C++"));   
   // m_jbusListCtrl.SetItemText(3, 1, _T("4"));   
   // m_jbusListCtrl.SetItemText(3, 2, _T("3"));   
	for(int ID = 0; ID < ConfigData.NumofTagUpdateQueue; ID++)
	{
		CString test;
		 
		int regID = TagUpdatePtr[ID].mRegID;
		int tagID = TagUpdatePtr[ID].mTagID;

		test.Format(_T("%d"), tagID);
		m_jbusListCtrl.InsertItem(ID,test); //reg ID
		test.Format(_T("%d"), regID);
		m_jbusListCtrl.SetItemText(ID, 1, test); //tagID
		if(tagID == SYSTEM_PAGE_TAG)
			test.Format(_T("%d"), WorkingPageID + 1);
		else 
			test.Format(_T("%d"), TagPtr[tagID].mValue);
		m_jbusListCtrl.SetItemText(ID, 2, test); //tag��ǰֵ
	}

	m_reg_edit.ShowWindow(SW_HIDE);

	//��ֹδ��ʼ������
	hitRow = -1;
	hitCol = -1;

	//m_jbusListCtrl.GetSubItemRect(0,4,LVIR_LABEL,rect);
	//POSITION pos = m_jbusListCtrl.GetFirstSelectedItemPosition(); 

	//m_jbusEdit.Create(ES_AUTOHSCROLL|WS_CHILD|ES_LEFT|ES_WANTRETURN|WS_BORDER,CRect(0,0,0,0),this,99);
    //m_jbusEdit.ShowWindow(SW_SHOW);
	//m_jbusEdit.MoveWindow(&rect);

	//���ý���
	GetDlgItem(IDC_SEND_BUTTON)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//˫��
void JbusClass::OnNMDblclkJbusList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LVHITTESTINFO info;
    info.pt  =  pNMItemActivate -> ptAction;
  
     if (m_jbusListCtrl.SubItemHitTest( & info)  !=   - 1  )
    {
      hitRow  =  info.iItem;
      hitCol  =  info.iSubItem;

	  if(hitCol != 3) //ֻ�е����п��ԸĶ�
	  {
		  *pResult = 0;
		  return;
	  }
 
        if (m_jbusEdit.m_hWnd  ==  NULL) // editItemΪһ�����ؼ��� 
       {
           RECT rect;
           rect.left  =   0 ;
           rect.top  =   0 ;
           rect.bottom  =   30 ;
           rect.right  =   200 ;

		   m_reg_edit.ModifyStyle(0,WS_CHILD  |  ES_CENTER  |  WS_BORDER  |  ES_AUTOHSCROLL  |  ES_WANTRETURN  |  ES_MULTILINE);
		   m_reg_edit.MoveWindow(&rect);
		   //m_reg_edit.setwin
           //m_jbusEdit.Create(WS_CHILD  |  ES_CENTER  |  WS_BORDER  |  ES_AUTOHSCROLL  |  ES_WANTRETURN  |  ES_MULTILINE, rect,  this ,  101 );
          // m_jbusEdit.SetFont( this -> GetFont(), FALSE);
		   m_reg_edit.SetFont( this -> GetFont(), FALSE);
       }
       CRect rect;
       m_jbusListCtrl.GetSubItemRect(info.iItem, info.iSubItem, LVIR_BOUNDS, rect);
       rect.top  +=   12 ;
       rect.left  +=   13 ;
       rect.right  +=   13 ;
       rect.bottom  +=   17 ;
 
       //m_jbusEdit.SetWindowText(m_jbusListCtrl.GetItemText( info .iItem,  info .iSubItem));
       //m_jbusEdit.MoveWindow( & rect, TRUE);
       //m_jbusEdit.ShowWindow( 1 );
       //m_jbusEdit.SetFocus();
	   //����tagID
		CString csTagID, csRegID;
		csTagID = m_jbusListCtrl.GetItemText(hitRow, 0);
		csRegID = m_jbusListCtrl.GetItemText(hitRow, 1);
		tagID = _ttoi(csTagID);
		regID = _ttoi(csRegID);

	   m_reg_edit.SetWindowText(m_jbusListCtrl.GetItemText( info .iItem,  info .iSubItem));
	   m_reg_edit.MoveWindow( & rect, TRUE);
	   m_reg_edit.ShowWindow( 1 );
	   m_reg_edit.SetFocus();
    }
	*pResult = 0;
}

//����
void JbusClass::OnNMClickJbusList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_reg_edit.m_hWnd  !=  NULL)
    {
		m_reg_edit.ShowWindow( 0 );
		if (hitRow  !=   - 1 && hitCol == 3)
		{
		    CString text;
		    m_reg_edit.GetWindowText(text);
			tagValue = _ttoi(text);
			if(text.GetLength() == 0)
			{
				tagID = -1;
				regID = -1;
			}
		    m_jbusListCtrl.SetItemText(hitRow, hitCol, text);
		}
    }
    hitCol  =  hitRow  =   - 1 ;
	//���ý���
	//GetDlgItem(IDC_SEND_BUTTON)->SetFocus();
	m_jbusListCtrl.SetFocus();
	*pResult = 0;
}


void JbusClass::OnLvnKeydownJbusList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}


void JbusClass::OnNMReturnJbusList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_reg_edit.m_hWnd  !=  NULL)
    {
		m_reg_edit.ShowWindow( 0 );
		if (hitRow  !=   - 1 )
		{
		    CString text;
		    m_reg_edit.GetWindowText(text);
		    m_jbusListCtrl.SetItemText(hitRow, hitCol, text);
		}
    }
    hitCol  =  hitRow  =   - 1 ;
	*pResult = 0;
}

//��tag��ֵ���͸��Ĵ���
void JbusClass::OnBnClickedSendButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	for(int ID = 0; ID < ConfigData.NumofTagUpdateQueue; ID++)
	{
		CString test;
		test = m_jbusListCtrl.GetItemText(ID,2);
		m_jbusListCtrl.SetItemText(ID,3,test);
	}
}


void JbusClass::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}


void JbusClass::OnLvnItemchangedJbusList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

//ʧȥ���㣬����tag
void JbusClass::OnEnKillfocusRegEdit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//MessageBox("ʧȥ����");
	CString text;
	m_reg_edit.GetWindowText(text);
	int i = _ttoi(text);
	if(text.GetLength() != 0 && tagID != -1 && regID != -1) //��ʾ����ֵ����
	{
		TagPtr[tagID].setValue(i,tagID);

		tagID = -1;
		regID = -1;
	}
}


void JbusClass::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void JbusClass::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//����жԻ����򽫶Ի��򽹵�ת��list control
	LRESULT a = 0;
	OnNMClickJbusList(NULL,&a);
	//CDialogEx::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL JbusClass::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) {OnKeyDown(0,0,0);return TRUE;}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) {OnKeyUp(0,0,0); return TRUE;}
	else
		return CDialog::PreTranslateMessage(pMsg);
	return CDialogEx::PreTranslateMessage(pMsg);
}


void JbusClass::OnBnClickedSendButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	for(int ID = 0; ID < ConfigData.NumofTagUpdateQueue; ID++)
	{
		CString test;
		int mTagID = TagUpdatePtr[ID].mTagID;
		int mRegID = TagUpdatePtr[ID].mRegID;
		int mValue = TagPtr[mTagID].mValue;
		test.Format(_T("%d"), mValue);
		m_jbusListCtrl.SetItemText(ID,2,test);
	}
}
