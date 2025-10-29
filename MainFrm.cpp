// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "OpenC.h"

#include "OpenCDoc.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_FILE_SALVATUTTO, OnUpdateFileSalvatutto)
	ON_WM_DROPFILES()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_WINDOW_CASCADE, OnWindowCascade)
	ON_COMMAND(ID_WINDOW_TILE_HORZ, OnWindowTileHorz)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() {

	myFont.CreateFont(14,6,0,0,FW_THIN,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH | FF_MODERN,"courier");
	}

CMainFrame::~CMainFrame() {
	
	}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	RECT rc;
	char myBuf[64];

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
	}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
	char myBuf[128];
	CRect rc;
	int n,n2;

/*	theApp.GetProfileString(S,IDS_COORDINATE,myBuf,32);
	sscanf(myBuf,"%d,%d,%d,%d",&cs.x,&cs.y,&cs.cx,&cs.cy);
	cs.cx-=cs.x;
	cs.cy-=cs.y;*/

	if(theApp.m_bLoadWindowPlacement)
		theApp.LoadWindowPlacement(rc,n,n2);

	cs.x=rc.left;
	cs.y=rc.top;
	cs.cy=rc.bottom-rc.top;
	cs.cx=rc.right-rc.left;

	return CMDIFrameWnd::PreCreateWindow(cs);
	}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
/////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnUpdateFileSalvatutto(CCmdUI* pCmdUI) {
	CWnd *v=GetWindow(GW_CHILD);
	if(v)
		v=v->GetNextWindow();
	pCmdUI->Enable(v != NULL);	
	}


void CMainFrame::OnDropFiles(HDROP hDropInfo) {
	char myBuf[256];
	CStringEx S;
	
	DragQueryFile(hDropInfo,0,myBuf,256);

	S=myBuf;
	if(S.ReverseFindNoCase(".C")>=0 || S.ReverseFindNoCase(".H")>=0) {
		theApp.OpenDocumentFile(myBuf);
		}
	DragFinish(hDropInfo);
	
	CMDIFrameWnd::OnDropFiles(hDropInfo);
	}


RECT *CMainFrame::getOutputWndRect(RECT *rc) {

	GetClientRect(rc);
//	rc->left=0;
	rc->top=rc->bottom-max(100,rc->bottom/4)-GetSystemMetrics(SM_CYCAPTION)-GetSystemMetrics(SM_CYEDGE)*4;
	rc->right=rc->right-GetSystemMetrics(SM_CXEDGE)*2;
	rc->bottom=max(100,rc->bottom/4)-GetSystemMetrics(SM_CYCAPTION)-GetSystemMetrics(SM_CYEDGE)*4;
	return rc;
	}

void CMainFrame::OnSize(UINT nType, int cx, int cy) {
	CView *v;
	RECT rc;

	CMDIFrameWnd::OnSize(nType, cx, cy);
	
	if(theApp.theOutput) {
		v=theApp.theOutput->getView();
		if(v) {
			getOutputWndRect(&rc);
			v->GetParent()->SetWindowPos(0,rc.left,rc.top,rc.right,rc.bottom,SWP_NOZORDER | SWP_NOACTIVATE);	
			v->Invalidate();
			}
		}
	}

BOOL CMainFrame::DestroyWindow() {
	RECT rc;
	char myBuf[64];
	
	return CMDIFrameWnd::DestroyWindow();
	}


void CMainFrame::OnClose() {
	char myBuf[256],myBuf1[64],*p;
	int i;
	COpenCDoc *myDoc;
	POSITION pos=theApp.pDocTemplate->GetFirstDocPosition();
	
	i=0;
	do {		// cancello tutte le sezioni dei file...
		wsprintf(myBuf1,"File%u",i);
		theApp.GetPrivateProfileString(theApp.fileApertiKey,myBuf1,myBuf,256);
		if(*myBuf) {
			p=strrchr(myBuf,'\\');
			if(p) {
				p++;
				theApp.WritePrivateProfileString(p,(char *)NULL,NULL);
				}
			}
		i++;
		} while(*myBuf);
	theApp.WritePrivateProfileString(theApp.fileApertiKey,(char *)NULL,NULL);	// ...cancello pure l'elenco...
	i=0;
	while(pos) {		// e ricreo l'elenco (le sezioni se le ricrea ogni singolo doc.
		myDoc=(COpenCDoc *)theApp.pDocTemplate->GetNextDoc(pos);
		wsprintf(myBuf,"File%u",i);
		theApp.WritePrivateProfileString(theApp.fileApertiKey,myBuf,myDoc->GetPathName());
		i++;
		}

	theApp.OnClosingMainFrame();
	
	CMDIFrameWnd::OnClose();
	}

void CMainFrame::OnWindowCascade() {
	CWnd *w;
	CString S;

	w=MDIGetActive();
	while(w) {
		w->GetWindowText(S);
		if(S=="Output")
			break;
		w=w->GetNextWindow();
		}
	if(w) {
		w->EnableWindow(FALSE);
		}
	MDICascade(MDITILE_SKIPDISABLED);	
	if(w) {
		w->EnableWindow(TRUE);
		}
	}

void CMainFrame::OnWindowTileHorz() {
	CWnd *w;
	CString S;

	w=MDIGetActive();
	while(w) {
		w->GetWindowText(S);
		if(S=="Output")
			break;
		w=w->GetNextWindow();
		}
	if(w) {
		w->EnableWindow(FALSE);
		}
	MDITile(MDITILE_SKIPDISABLED);	
	if(w) {
		w->EnableWindow(TRUE);
		}
	}


