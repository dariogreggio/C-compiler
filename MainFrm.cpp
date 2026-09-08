// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "OpenC.h"

#include "OpenCDoc.h"
#include "OpenCView.h"
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
	ON_MESSAGE(WM_ADDTEXT,OnAddText)
	ON_MESSAGE(WM_CLSWINDOW,OnClsWindow)
END_MESSAGE_MAP()

static UINT indicators[] = {
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_POS,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame() {

	myFont.CreateFont(14,6,0,0,FW_THIN,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH | FF_MODERN,"courier");
	numErrors=numWarnings=0;
	}

CMainFrame::~CMainFrame() {
	
	}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	RECT rc;
	char myBuf[64];

	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
		}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
		}
	m_wndStatusBar.SetPaneInfo(m_wndStatusBar.CommandToIndex(ID_INDICATOR_POS), 
                           ID_INDICATOR_POS, 
                           SBPS_NORMAL, 
                           80); // Larghezza in pixel

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// 1. Creazione della DialogBar ancorata in basso (CBRS_BOTTOM) gemini 9/2026
    if (!m_wndOutputBar.Create(this, IDD_OUTPUT_BAR,
        CBRS_BOTTOM | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_OUTPUT_BAR))
    {
        TRACE0("Impossibile creare la finestra di Output\n");
        return -1;
    }

    // 2. Collega l'Edit Control interno alla variabile CEdit
    m_wndOutputEdit.SubclassDlgItem(IDC_TXT_OUTPUT, &m_wndOutputBar);

    // Imposta un font monospaziato stile IDE (Courier New)
//	myFont.CreateFont(16,8,0,0,FW_THIN,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_MODERN,"arial");
    CFont* pFont = CFont::FromHandle((HFONT)::GetStockObject(ANSI_FIXED_FONT));
    m_wndOutputEdit.SetFont(pFont);
/*	lb.lbColor=RGB(240,240,240);
	lb.lbHatch=0;
	lb.lbStyle=BS_SOLID;
	myBKBrush=CreateBrushIndirect(&lb);
*/
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

	if (m_wndOutputBar.GetSafeHwnd())    {
    CRect rcBar;
    m_wndOutputBar.GetClientRect(&rcBar);

    // Ridimensiona il controllo CEdit interno per occupare l'intera barra
    if (m_wndOutputEdit.GetSafeHwnd())        {
        m_wndOutputEdit.MoveWindow(0, 0, rcBar.Width(), rcBar.Height());
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



void CMainFrame::AddOutputText(LPCTSTR lpszText) {

  if (!m_wndOutputEdit.GetSafeHwnd())
      return;

  // Posiziona il cursore a fine testo
  int nLen = m_wndOutputEdit.GetWindowTextLength();
  m_wndOutputEdit.SetSel(nLen, nLen);

  // Inserisci il testo e vai a capo
  m_wndOutputEdit.ReplaceSel(lpszText);
  m_wndOutputEdit.ReplaceSel(_T("\r\n"));
	}

void CMainFrame::ClearOutputText() {

  if(m_wndOutputEdit.GetSafeHwnd()) 
    m_wndOutputEdit.SetWindowText(_T(""));
	}

int CMainFrame::AddText(const char *s,int m) {
	int i=0;
	char myBuf[64],*p;

  if(s) {
    AddOutputText(s); // Accoda il testo nella CDialogBar
    GlobalFree((void*)s);    // O libera la memoria come facevi prima
    }
		
	switch(m) {
		case 1:
			numErrors++;
			break;
		case 2:
			numWarnings++;
			break;
		}

	return i;
	}

int CMainFrame::Cls() {
	int i=0;

	ClearOutputText(); // Pulisce l'edit control nella CDialogBar
		
	numErrors=numWarnings=0;
  return 1;
	}

afx_msg LRESULT CMainFrame::OnAddText(WPARAM wParam, LPARAM lParam) {
	int i;
	char *s=(char *)lParam;
	char myBuf[64];

//wParam e' il "tipo" di stringa... indicava errore o warning o info, ma ora non lo uso...
	AddText((LPCTSTR)lParam,wParam);
	i=1;
	return i;
	}

afx_msg LRESULT CMainFrame::OnClsWindow(WPARAM wParam, LPARAM lParam) {
	
	Cls();
	return 1;
	}



void CMainFrame::GoToRichEditLine(int lineNum) {
	HWND hRichEdit;

	COpenCView *w=(COpenCView*)MDIGetActive();

  // I controlli RichEdit usano indici a base 0, quindi convertiamo lineNum (1-based)
  int targetLine = lineNum - 1;

  // 1. Ottiene l'indice di carattere iniziale della riga richiesta
  LRESULT charIndex = w->GetRichEditCtrl().SendMessage(EM_LINEINDEX, (WPARAM)targetLine, 0);

  // Se la riga specificata esiste
  if (charIndex != -1) {
      // 2. Posiziona il cursore all'inizio di quella riga (inizio == fine per non selezionare testo)
		w->GetRichEditCtrl().SendMessage(EM_SETSEL, (WPARAM)charIndex, (LPARAM)charIndex);

      // 3. (Opzionale) Fa scorrere il controllo affinché la riga e il cursore siano ben visibili
    w->GetRichEditCtrl().SendMessage(EM_SCROLLCARET, 0, 0);
      
      // Assicura che il controllo prenda il focus per mostrare il cursore attivo
//      SetFocus(hRichEdit);
    }
	}

