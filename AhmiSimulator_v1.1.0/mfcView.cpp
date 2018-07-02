////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:     AHMIС���Ա
//
// Create Date:   2015/11/17
// File Name:      mfcView.cpp
// Project Name:  AHMISimulator
// 
// Revision:
// Revision 2.00 - File Created 2015/11/17 by...
// Additional Comments:
//     CmfcView ���ʵ��
// 
////////////////////////////////////////////////////////////////////////////////

#include "publicDefine.h"
#include "gl\glut.h" 

#ifdef PC_SIM

#include "stdafx.h"
#include "mfc.h"

#include "mfcDoc.h"
#include "mfcView.h"
#include "TouchTask.h"
#include "publicInclude.h"
#include "ahmiv3.h"
#include "JbusClass.h"
#include "Keyboard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//extern int top();
extern StructFrameBuffer  GlobalFrameBuffer[ MAX_SCREEN_SIZE ];

HWND ViewHWND;

extern"C"
{

extern void *pvMouseEvent;
extern PIDState MouseClickData;
extern struct globalArgs_t globalArgs;
}
extern u8           TextureBuffer[TEXBUFSIZE];
u16 pressX;
u16 pressY;

// CmfcView

IMPLEMENT_DYNCREATE(CmfcView, CView)

BEGIN_MESSAGE_MAP(CmfcView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_JBUS_SIM, &CmfcView::OnJbusSim)
END_MESSAGE_MAP()

// CmfcView ����/����

CmfcView::CmfcView()
{
	// TODO: �ڴ˴���ӹ������
	mouseMovingFlag = 0;
	mouseMovingXold = 0;
	mouseMovingYold = 0;

	m_pJbusClass = NULL;
}

CmfcView::~CmfcView()
{
}

BOOL CmfcView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ
cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;//Tramp
	return CView::PreCreateWindow(cs);
}

// CmfcView ����

void CmfcView::OnDraw(CDC* /*pDC*/)
{
	CmfcDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
	::glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	RenderScene();//��ͼ��������
	// Tell OpenGL to flush its pipeline
	::glFinish();
	// Now Swap the buffers
	::SwapBuffers( m_pDC->GetSafeHdc() );

}


// CmfcView ��ӡ

BOOL CmfcView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CmfcView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CmfcView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}


// CmfcView ���

#ifdef _DEBUG
void CmfcView::AssertValid() const
{
	CView::AssertValid();
}

void CmfcView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CmfcDoc* CmfcView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CmfcDoc)));
	return (CmfcDoc*)m_pDocument;
}
#endif //_DEBUG


// CmfcView ��Ϣ�������

int CmfcView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
		InitializeOpenGL();//��ʼ��openGL��ͼ
		ViewHWND = m_hWnd;
//		CreateRTOSThread();

	return 0;
}

void CmfcView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	//Make the RC non-current
	if(::wglMakeCurrent (0,0) == FALSE)
	{
		MessageBox(_T("Could not make RC non-current"));
	}

	//Delete the rendering context
	if(::wglDeleteContext (m_hRC)==FALSE)
	{
		MessageBox(_T("Could not delete RC"));
	}
	//Delete the DC
	if(m_pDC)
	{
		delete m_pDC;
	}
	//Set it to NULL
	m_pDC = NULL;

}

void CmfcView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	// TODO: �ڴ˴������Ϣ����������
	GLdouble aspect_ratio; // width/height ratio

	if ( 0 >= cx || 0 >= cy )
	{
		return;
	}
	// select the full client area
	::glViewport(0, 0, cx, cy);
	// compute the aspect ratio
	// this will keep all dimension scales equal
	aspect_ratio = (GLdouble)cx/(GLdouble)cy;
	// select the projection matrix and clear it
	::glMatrixMode(GL_PROJECTION);
	::glLoadIdentity();
	// select the viewing volume
	::gluPerspective(45.0f, aspect_ratio, .01f, 200.0f);//����ά
	//::gluOrtho2D(-10.0f, 10.0f, -10.0f, 10.0f);    //��ά


	// switch back to the modelview matrix and clear it
	::glMatrixMode(GL_MODELVIEW);
	::glLoadIdentity();

}

BOOL CmfcView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	return TRUE;
}
BOOL CmfcView::InitializeOpenGL()
{
	//Get a DC for the Client Area
	m_pDC = new CClientDC(this);
	//Failure to Get DC
	if(m_pDC == NULL)
	{
		//::MessageBox("Error Obtaining DC");
		return FALSE;
	}
	//Failure to set the pixel format
	if(!SetupPixelFormat())
	{
		return FALSE;
	}
	//Create Rendering Context
	m_hRC = ::wglCreateContext (m_pDC->GetSafeHdc ());
	//Failure to Create Rendering Context
	if(m_hRC == 0)
	{
		//	MessageBox("Error Creating RC");
		return FALSE;
	}
	//Make the RC Current
	if(::wglMakeCurrent (m_pDC->GetSafeHdc (), m_hRC)==FALSE)
	{
		//	MessageBox("Error making RC Current");
		return FALSE;
	}
	//Specify Black as the clear color
//	::glClearColor(0.0f,0.0f,0.0f,0.0f);
	//Specify the back of the buffer as clear depth
	::glClearDepth(1.0f);
	//Enable Depth Testing
	::glEnable(GL_DEPTH_TEST);
	return TRUE;

}
//�������ظ�ʽ
BOOL CmfcView::SetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW |            // support window
		PFD_SUPPORT_OPENGL |            // support OpenGL
		PFD_DOUBLEBUFFER,                // double buffered
		PFD_TYPE_RGBA,                  // RGBA type
		24,                             // 24-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		16,                             // 16-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};
	int m_nPixelFormat = ::ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);
	if ( m_nPixelFormat == 0 )
	{
		return FALSE;
	}
	if ( ::SetPixelFormat(m_pDC->GetSafeHdc(), m_nPixelFormat, &pfd) == FALSE)
	{
		return FALSE;
	}
	return TRUE;
}

void CmfcView::RenderScene()
{
	     glDrawPixels(globalArgs.ScreenWidth, globalArgs.ScreenHeigth,
        GL_RGB, GL_UNSIGNED_BYTE, (void*)&GlobalFrameBuffer);
	     //glDrawPixels(globalArgs.ScreenWidth, globalArgs.ScreenHeigth,
      //  GL_BGR_EXT, GL_UNSIGNED_BYTE, (void*)&TextureBuffer);
	// Done Drawing The Quad
}


void CmfcView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	MouseClickData.x = (u16)point.x;
	MouseClickData.y = (u16)point.y;
	MouseClickData.press = MOUSE_RELEASE ;
	mouseMovingFlag = 0;
	SetEvent(pvMouseEvent);
	CView::OnLButtonUp(nFlags, point);
	CmfcView::RenderScene();
}


//bool CmfcView::CreateRTOSThread(void)
//{
//	AfxBeginThread(RTOSThread,NULL,0,0,0,NULL);
//	return true;
//}

//UINT RTOSThread(LPVOID pParam)
//{
//	top();
//	return 0;
//}

void CmfcView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	MouseClickData.x = (u16)point.x;
	MouseClickData.y = (u16)point.y;
	mouseMovingXold = (u16)point.x;
	mouseMovingYold = (u16)point.y;
	pressX = (u16)point.x;
	pressY = (u16)point.y;
	MouseClickData.press = MOUSE_PRESS ;
	mouseMovingFlag =1;
	SetEvent(pvMouseEvent);
	CView::OnLButtonDown(nFlags, point);
	CmfcView::RenderScene();
}

void CmfcView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(mouseMovingFlag==1)
	{
		//if( ((mouseMovingXold - point.x) >10 || (point.x - mouseMovingXold) >10) )
		//{
			MouseClickData.x = (u16)point.x;
			MouseClickData.y = (u16)point.y;
			//mouseMovingXold = (u16)point.x;
			//mouseMovingYold = (u16)point.y;
			MouseClickData.press = MOUSE_HOLD;
			SetEvent(pvMouseEvent);
		//}
	}
	CView::OnMouseMove(nFlags, point);
	CmfcView::RenderScene();
}

#endif




void CmfcView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
	//MessageBox("key down");
}


void CmfcView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nChar == VK_LEFT||nChar == VK_UP)
	{
		keyboardLeft();
	}
	else if(nChar == VK_RIGHT || nChar == VK_DOWN)
	{
		keyboardRight();
	}
	else if(nChar == VK_RETURN)
	{
		keyboardEntered();
	}
	else if(nChar == VK_ESCAPE)
	{
		keyboardClear();
	}
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


void CmfcView::OnJbusSim()
{
	// TODO: �ڴ���������������
	

	// ���ָ�����m_pTipDlg��ֵΪNULL����Ի���δ��������Ҫ��̬����   
    if (NULL == m_pJbusClass)   
    {   
        // ������ģ̬�Ի���ʵ��   
        m_pJbusClass = new JbusClass();   
        m_pJbusClass->Create(IDD_JBUS_DIALOG, this);   
    }   
    // ��ʾ��ģ̬�Ի���   
    m_pJbusClass->ShowWindow(SW_SHOW);
	m_pJbusClass->GetFocus();
	//MessageBox("jbus");
}
