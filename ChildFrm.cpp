// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "OpenC.h"

#include "MainFrm.h"
#include "OpenCDoc.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame() {
	// TODO: add member initialization code here
	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::OnCreateClient( LPCREATESTRUCT lpcs,
	CCreateContext* pContext) {
	int i;

	i=m_wndSplitter.Create( this,
		2, 1,                 // TODO: adjust the number of rows, columns
		CSize( 100, 40 ),      // TODO: adjust the minimum pane size
		pContext );

	return i;
	}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CMDIChildWnd::PreCreateWindow(cs);
	}

BOOL CChildFrame::CreateView( int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext ) {
	int i=1;

	AfxMessageBox("cv");
	// non gliene puo' fregare di meno!
	return i;
	}

BOOL CChildFrame::SplitRow( int cyBefore ) {
	int i=1;

	AfxMessageBox("split");
	// non gliene puo' fregare di meno!
	return i;
	}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers


IMPLEMENT_DYNCREATE(CChildFrame2, CMDIChildWnd  /*CMiniFrameWnd*/)


CChildFrame2::CChildFrame2() {
	LOGBRUSH lb;

	lb.lbColor=RGB(240,240,240);
	lb.lbHatch=0;
	lb.lbStyle=BS_SOLID;
	myBKBrush=CreateBrushIndirect(&lb);
	}

CChildFrame2::~CChildFrame2() {
	}

BEGIN_MESSAGE_MAP(CChildFrame2, CMDIChildWnd /*CMiniFrameWnd*/)
	//{{AFX_MSG_MAP(CChildFrame2)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_ADDTEXT,OnAddText)
	ON_MESSAGE(WM_CLSWINDOW,OnClsWindow)
END_MESSAGE_MAP()


BOOL CChildFrame2::PreCreateWindow(CREATESTRUCT& cs) {
	RECT rc;
	
	cs.style &= ~(/*WS_CAPTION | */ WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU);
	((CMainFrame *)theApp.m_pMainWnd)->getOutputWndRect(&rc);
	cs.x=rc.left;
	cs.y=rc.top;
	cs.cx=rc.right;
	cs.cy=rc.bottom;
	return CMDIChildWnd::PreCreateWindow(cs);
	}

int CChildFrame2::OnCreate(LPCREATESTRUCT lpCreateStruct) {

	if(CMDIChildWnd /*CMiniFrameWnd*/::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
	}

void CChildFrame2::OnClose() {
	// TODO: Add your message handler code here and/or call default
	
	CMDIChildWnd /*CMiniFrameWnd*/::OnClose();
	}

void CChildFrame2::OnGetMinMaxInfo(MINMAXINFO *lpMMI) {
	COpenCDoc2 *d=(COpenCDoc2 *)GetActiveDocument();
	RECT rc;

	((CMainFrame *)theApp.m_pMainWnd)->getOutputWndRect(&rc);

	if(d) {
		lpMMI->ptMaxSize.x=lpMMI->ptMinTrackSize.x=lpMMI->ptMaxTrackSize.x=rc.right;
		lpMMI->ptMaxSize.y=lpMMI->ptMinTrackSize.y=lpMMI->ptMaxTrackSize.y=rc.bottom;
		}
	}


afx_msg LRESULT CChildFrame2::OnAddText(WPARAM wParam, LPARAM lParam) {
	int i;
	char *s=(char *)lParam;
	char myBuf[64];

//wParam e' il "tipo" di stringa... indicava errore o warning o info, ma ora non lo uso...
	((CEditView *)GetWindow(GW_CHILD))->GetEditCtrl().ReplaceSel(s);
	myBuf[0]=13;
	myBuf[1]=10;
	myBuf[2]=0;
	((CEditView *)GetWindow(GW_CHILD))->GetEditCtrl().ReplaceSel(myBuf);
	((CEditView *)GetWindow(GW_CHILD))->GetDocument()->SetModifiedFlag(0);
	i=1;
	GlobalFree(s);
	return i;
	}

afx_msg LRESULT CChildFrame2::OnClsWindow(WPARAM wParam, LPARAM lParam) {
	
	((CEditView *)GetWindow(GW_CHILD))->GetEditCtrl().SetSel(0,-1);
	((CEditView *)GetWindow(GW_CHILD))->GetEditCtrl().Clear();
	((CEditView *)GetWindow(GW_CHILD))->GetDocument()->SetModifiedFlag(0);
	return 1;
	}


HBRUSH CChildFrame2::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) {
	// questa deve stare qua, e non in OutputView...

	HBRUSH hbr = CMDIChildWnd::OnCtlColor(pDC, pWnd, nCtlColor);
	
	pDC->SetBkColor(RGB(240,240,240));
	
	return myBKBrush;
	}


void CChildFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) {
	
	lpMMI->ptMaxSize.y=500;		// non funzia, volevo evitare che andasse sopra la output...
	
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
	}

