// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "OpenC.h"

#include "OpenCDoc.h"
#include "OpenCView.h"
#include "MainFrm.h"
#include <mmsystem.h> // Per i suoni di sistema (es. MessageBeep)
//#include <commctrl.h>

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
	ON_COMMAND(ID_VISUALIZZA_FINESTRADIOUTPUT, OnVisualizzaFinestradioutput)
	ON_UPDATE_COMMAND_UI(ID_VISUALIZZA_FINESTRADIOUTPUT, OnUpdateVisualizzaFinestradioutput)
	ON_COMMAND(ID_VISUALIZZA_FINESTRAPROGETTO, OnVisualizzaFinestraprogetto)
	ON_UPDATE_COMMAND_UI(ID_VISUALIZZA_FINESTRAPROGETTO, OnUpdateVisualizzaFinestraprogetto)
	ON_COMMAND(ID_FINESTRA_CHIUDITUTTE, OnFinestraChiuditutte)
	ON_UPDATE_COMMAND_UI(ID_FINESTRA_CHIUDITUTTE, OnUpdateFinestraChiuditutte)
	ON_COMMAND(ID_TREE_OPEN, OnTreeOpen)
	ON_COMMAND(ID_TREE_IMPOSTAZIONI, OnTreeImpostazioni)
	ON_COMMAND(ID_TREE_ESCLUDI, OnTreeEscludi)
	ON_COMMAND(ID_TREE_PROPRIET, OnTreePropriet)
	ON_UPDATE_COMMAND_UI(ID_TREE_ESCLUDI, OnUpdateTreeEscludi)
	ON_COMMAND(ID_TREE_ADD, OnTreeAdd)
	ON_COMMAND(ID_TREE_ELIMINA, OnTreeElimina)
	ON_UPDATE_COMMAND_UI(ID_TREE_ELIMINA, OnUpdateTreeElimina)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_PROJECT, OnTreeDoubleClick)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_PROJECT, OnTreeRightClick)
	ON_WM_ENDSESSION()
	ON_COMMAND(ID_FILE_SALVATUTTO, OnFileSalvatutto)
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
	ON_MESSAGE(WM_ADDTEXT,OnAddText)
	ON_MESSAGE(WM_CLSWINDOW,OnClsWindow)
	ON_MESSAGE(WM_GOTO_OUTPUT_LINE, OnGotoOutputLine)
	ON_COMMAND(ID_NEXT_ERROR, OnNextError)
	ON_COMMAND(ID_PREV_ERROR, OnPrevError)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE_PROJECT, OnTreeItemExpanded)
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

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	// 1. Creazione della DialogBar (m_wndOutputBar)
	if(!m_wndOutputBar.Create(this, IDD_OUTPUT_BAR,
			CBRS_BOTTOM | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_OUTPUT_BAR)) {
		TRACE0("Impossibile creare la finestra di Output\n");
		return -1;
		}

	// 2. Creazione del TabCtrl *DENTRO* m_wndOutputBar (passiamo &m_wndOutputBar come parent!)
	if(!m_wndOutputBar.m_wndOutputTab.Create(WS_CHILD | WS_VISIBLE | TCS_BOTTOM | TCS_FORCELABELLEFT | TCS_TABS | WS_CLIPSIBLINGS, 
																						CRect(0,0,0,0), &m_wndOutputBar, IDC_OUTPUT_TAB)) {
    return -1;
		}

	// 3. Aggiunta schede e font al TabCtrl
	m_wndOutputBar.m_wndOutputTab.InsertItem(0, _T("Build"));
	m_wndOutputBar.m_wndOutputTab.InsertItem(1, _T("Debug"));
	m_wndOutputBar.m_wndOutputTab.InsertItem(2, _T("Cerca nei file"));

	CFont *pFont = CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	m_wndOutputBar.m_wndOutputTab.SetFont(pFont);

	// 4. Subclassing dell'Edit control (che fa già parte del template IDD_OUTPUT_BAR)
	m_wndOutputBar.m_wndOutputEdit.SubclassDlgItem(IDC_TXT_OUTPUT, &m_wndOutputBar);
	pFont = CFont::FromHandle((HFONT)::GetStockObject(ANSI_FIXED_FONT));
	m_wndOutputBar.m_wndOutputEdit.SetFont(pFont);

/*	lb.lbColor=RGB(240,240,240);
	lb.lbHatch=0;
	lb.lbStyle=BS_SOLID;
	myBKBrush=CreateBrushIndirect(&lb);
*/

  if(!m_wndProjectBar.Create(this, IDD_TREE_PROJECT,
    CBRS_LEFT | CBRS_TOOLTIPS | CBRS_FLYBY, IDD_TREE_PROJECT))    {
    TRACE0("Impossibile creare la finestra tree\n");
    return -1;
    }
  m_wndProjectTree.SubclassDlgItem(IDC_TREE_PROJECT, &m_wndProjectBar);

	HTREEITEM tp0,tp1;

	il.Create(IDB_PROJECTREE,16,0,RGB(255,255,255));
	m_wndProjectTree.SetImageList(&il,TVSIL_NORMAL);

	projectTreeRoot=m_wndProjectTree.InsertItem("Progetto",6,6);
/*//	m_wndProjectTree.SetItemImage(tp1,0,1);
	tp1=m_wndProjectTree.InsertItem("Source",0,1,projectTreeRoot);
	tp1=m_wndProjectTree.InsertItem("Header",0,1,projectTreeRoot);
	tp1=m_wndProjectTree.InsertItem("Altro",0,1,projectTreeRoot);*/

	// Puoi associare una stringa allocata o un ID univoco
		//CString* pPath = new CString(_T("C:\\MioProgetto\\ARROWS.C"));
		//m_wndTree.SetItemData(hItem, (DWORD_PTR)pPath);

//	m_wndProjectTree.Expand(projectTreeRoot,TVE_EXPAND);

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

void CMainFrame::OnFileSalvatutto() {
	
	theApp.SaveAllModified();
	}

void CMainFrame::OnUpdateFileSalvatutto(CCmdUI* pCmdUI) {
  CFrameWnd* pActiveFrame = MDIGetActive();

	pCmdUI->Enable(pActiveFrame != NULL);
//	pCmdUI->Enable(!theApp.nomeProgetto.IsEmpty());		// vedere quale è meglio...
	}


void CMainFrame::OnDropFiles(HDROP hDropInfo) {
	char myBuf[256];
	CStringEx S;
	
	DragQueryFile(hDropInfo,0,myBuf,255);

	S.SplitPath(myBuf,4);
	if(!S.CompareNoCase(".C") || !S.CompareNoCase(".H")) {		// .INC? ASM?
		ActivateViewByTitle/*theApp.OpenDocumentFile*/(myBuf);
		}
/*	else if(!S.CompareNoCase(".INC") || !S.CompareNoCase(".ASM")) {
		ActivateViewByTitle(myBuf);
		}*/
	else if(!S.CompareNoCase(".MAK")) {
		CStringEx S,S1;

		S=myBuf;
		theApp.nomeProgetto=S;
		theApp.updateWindowTitle(S);

		S="Progetto "+S+" aperto correttamente.";
		((CMainFrame*)theApp.m_pMainWnd)->m_wndStatusBar.SetWindowText(S);
		theApp.LoadProject(myBuf);
		}
	else
		MessageBeep(-1);
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
  CRect rcBar,rcBar2;

	CMDIFrameWnd::OnSize(nType, cx, cy);

	if(m_wndProjectBar.GetSafeHwnd())    {
    m_wndProjectBar.GetClientRect(&rcBar2);

    // Ridimensiona il controllo 
    if(m_wndProjectTree.GetSafeHwnd())        {
      m_wndProjectTree.MoveWindow(0, 0, rcBar2.Width(), rcBar2.Height() /*-rcBar.Height()*/);
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
	
	if(theApp.progettoModified) {
		i=AfxMessageBox("Il progetto è stato modificato: chiudere comunque?",MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION);
		if(i != IDOK)
			return;
		}

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

/*	w=MDIGetActive(); era per Output window
	while(w) {
		w->GetWindowText(S);
		w=w->GetNextWindow();
		}
	if(w) {
		w->EnableWindow(FALSE);
		}*/
	MDICascade();	
/*	if(w) {
		w->EnableWindow(TRUE);
		}*/
	}

void CMainFrame::OnWindowTileHorz() {
	CWnd *w;
	CString S;

/*	w=MDIGetActive();era per Output window
	while(w) {
		w->GetWindowText(S);
		w=w->GetNextWindow();
		}
	if(w) {
		w->EnableWindow(FALSE);
		}*/
	MDITile(MDITILE_HORIZONTAL);	
/*	if(w) {
		w->EnableWindow(TRUE);
		}*/
	}

void CMainFrame::OnFinestraChiuditutte() {
	CWnd* pWnd = MDIGetActive();

  while(pWnd)    {
    // 1. SALVIAMO la finestra successiva PRIMA di chiudere quella corrente
    CWnd* pNextWnd = pWnd->GetNextWindow();

    // 2. Inviamo WM_CLOSE per attivare la chiusura pulita MFC (salvataggio, OnClose, ecc.)
    pWnd->SendMessage(WM_CLOSE, 0, 0);

    // 3. Se l'utente ha premuto "Annulla" sulla richiesta di salvataggio,
    // la finestra esiste ancora: interrompiamo il ciclo per non chiudere le altre.
    if (::IsWindow(pWnd->GetSafeHwnd()))        {
        break;
    }

    // Passiamo alla prossima finestra salvata in precedenza
    pWnd = pNextWnd;
    }	
	}

void CMainFrame::OnUpdateFinestraChiuditutte(CCmdUI* pCmdUI) {
	
	}

void CMainFrame::OnVisualizzaFinestradioutput() {
	BOOL bVisible = m_wndOutputBar.IsWindowVisible();

// Passando FALSE come terzo parametro, eviti l'animazione lenta di delay.
  ShowControlBar(&m_wndOutputBar, !bVisible, FALSE);

  // 3. Ricalcola il layout del Frame per riadattare le MDI Child
  RecalcLayout();
	}

void CMainFrame::OnUpdateVisualizzaFinestradioutput(CCmdUI* pCmdUI) {
	BOOL bVisible = m_wndOutputBar.IsWindowVisible();

	pCmdUI->SetCheck(bVisible);
	}

void CMainFrame::OnVisualizzaFinestraprogetto() {
	BOOL bVisible = m_wndProjectBar.IsWindowVisible();

  ShowControlBar(&m_wndProjectBar, !bVisible, FALSE);
  RecalcLayout();
	}

void CMainFrame::OnUpdateVisualizzaFinestraprogetto(CCmdUI* pCmdUI) {
	BOOL bVisible = m_wndProjectBar.IsWindowVisible();

	pCmdUI->SetCheck(bVisible);
	}


BOOL CMainFrame::OnQueryEndSession() {
	int i;
	
	if(theApp.progettoModified) {
		i=AfxMessageBox("Il progetto è stato modificato: uscire comunque?",MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION);
		if(i == IDOK)
			return TRUE;
		else 
			return FALSE;
		}

	}


void CMainFrame::OnNextError() {
  m_wndOutputBar.m_wndOutputEdit.ProcessNextError(TRUE);
	}

void CMainFrame::OnPrevError() {
  m_wndOutputBar.m_wndOutputEdit.ProcessNextError(FALSE);
	}

void CMainFrame::AddOutputText(LPCTSTR lpszText) {

  if(!m_wndOutputBar.m_wndOutputEdit.GetSafeHwnd())
      return;

  // Posiziona il cursore a fine testo
  int nLen = m_wndOutputBar.m_wndOutputEdit.GetWindowTextLength();
  m_wndOutputBar.m_wndOutputEdit.SetSel(nLen, nLen);

  // Inserisci il testo e vai a capo
  m_wndOutputBar.m_wndOutputEdit.ReplaceSel(lpszText);
  m_wndOutputBar.m_wndOutputEdit.ReplaceSel(_T("\r\n"));
	}

void CMainFrame::ClearOutputText() {

  if(m_wndOutputBar.m_wndOutputEdit.GetSafeHwnd()) 
    m_wndOutputBar.m_wndOutputEdit.SetWindowText(_T(""));
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

LRESULT CMainFrame::OnGotoOutputLine(WPARAM wParam, LPARAM lParam) {
  int nLineNum = (int)wParam;
	const char *lpszFileName = (LPCTSTR)lParam;

  if(nLineNum > 0) {
    GoToRichEditLine(nLineNum,lpszFileName,TRUE);
    }
  return 0;
	}

/* per gestire anche file...

  if(!lpszFileName || nLineNum <= 0)
    return 0;

  // 1. Risolvi il percorso completo se abbiamo solo il nome del file (es. "SKYBASIC.C")
  CString strFullPath = lpszFileName;
  if (m_pCurrentProject )    {
    // Metodo helper del tuo progetto per ottenere il path assoluto
    strFullPath = m_pCurrentProject->GetFullPathForFile(lpszFileName);
    }

  // 2. Apri o attiva il documento MDI tramite l'App
  CWinApp* pApp = AfxGetApp();
  CDocument* pDoc = pApp->OpenDocumentFile(strFullPath);

	ActivateViewByTitle(strFullPath);


  if(!pDoc)
      return 0;

  // 3. Trova la vista attiva associata al documento
  POSITION pos = pDoc->GetFirstViewPosition();
  if(pos) {
    CView* pView = pDoc->GetNextView(pos);
    if(pView)        {
      // Attiva la finestra MDI MDIChild
      CFrameWnd* pChildFrame = pView->GetParentFrame();
      if(pChildFrame)
        pChildFrame->MDIActivate();

        // 4. Se la vista è un'editor di testo (es. CEditView o Scintilla/custom)
        if (pView->IsKindOf(RUNTIME_CLASS(CEditView)))            {
          CEditView* pEditView = (CEditView*)pView;
          CEdit& editCtrl = pEditView->GetEditCtrl();

          // Calcola l'indice del carattere a inizio riga (nLineNum è 1-based, LineIndex è 0-based)
          int nCharIndex = editCtrl.LineIndex(nLineNum - 1);
          if(nCharIndex != -1)                {
            editCtrl.SetSel(nCharIndex, nCharIndex);
            editCtrl.ScrollToCaret();
            }
          }
        // Se usi una tua classe di vista personalizzata, puoi invocare un tuo metodo dedicato:
        // else if (pView->IsKindOf(RUNTIME_CLASS(CMySourceView)))
        // {
        //     ((CMySourceView*)pView)->GotoLine(nLineNum);
        // }
      }
    }

  return 1;
	}
*/

void CMainFrame::GoToRichEditLine(int lineNum,const char *fileName,bool bSelect) {

	if(fileName) {
		ActivateViewByTitle(fileName);
/*	  CMDIChildWnd* pChild = (CMDIChildWnd*)MDIGetActive();
		if(!pChild) {
			// Nessuna finestra aperta, procedi ad aprire il documento/vista
			theApp.OpenDocumentFile(fileName);
	    }
		else {

			// 2. Scorri tutte le MDI Child aperte tramite la catena di finestre
			CMDIChildWnd* pStartChild = pChild;
			bool bFound = false;
				COpenCDoc *pDoc = (COpenCDoc*)pChild->GetActiveDocument();
				POSITION pos = pDoc->GetFirstViewPosition();
				if(pos) {
					CView* pView = pDoc->GetNextView(pos);
					if(pView)        {
						// Attiva la finestra MDI MDIChild
						CMDIFrameWnd* pChildFrame = (CMDIFrameWnd*)pView->GetParentFrame();
						if(pChildFrame)
							//pChildFrame->
							MDIActivate(pChildFrame);

							// 4. Se la vista è un'editor di testo 
							if(pView->IsKindOf(RUNTIME_CLASS(CEditView))) {
								CEditView* pEditView = (CEditView*)pView;
								CEdit& editCtrl = pEditView->GetEditCtrl();
						}
					}
				}
			}*/
		}

  // MDI view attiva
  CFrameWnd* pActiveFrame = MDIGetActive();
  if(!pActiveFrame) 
		return;

  COpenCView* pView = (COpenCView*)pActiveFrame->GetActiveView();
  if(!pView || !::IsWindow(pView->GetRichEditCtrl().GetSafeHwnd())) 
		return;

  CRichEditCtrl& rich = pView->GetRichEditCtrl();

  int targetLine = lineNum - 1; // 0-based index

  // 1. Calcola l'offset del primo e dell'ultimo carattere della riga
  LPARAM startChar = (LPARAM)rich.SendMessage(EM_LINEINDEX, (WPARAM)targetLine, 0);
  if (startChar == -1) 
		return;

  LRESULT lineLength = rich.SendMessage(EM_LINELENGTH, startChar, 0);
  LPARAM endChar = startChar + lineLength;

  // 2. Seleziona ed evidenzia l'intera riga nel file sorgente
	rich.SendMessage(EM_SETSEL, (WPARAM)startChar, endChar);
	if(!bSelect)
		rich.SendMessage(EM_SETSEL, (WPARAM)startChar, startChar);

  // 3. Porta la riga visibile al centro dello schermo e dai il focus alla finestra
  rich.SendMessage(EM_SCROLLCARET, 0, 0);
  //pActiveFrame->MDIActivate();
  rich.SetFocus();
	}

/*
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
*/


void CMainFrame::OnTreeDoubleClick(NMHDR* pNMHDR, LRESULT* pResult) {
  // 1. Converti il puntatore alla struttura specifica del Tree
  LPNMITEMACTIVATE pItemActivate = (LPNMITEMACTIVATE)pNMHDR;
  
  // 2. Ottieni l'elemento selezionato dell'albero
  HTREEITEM hItem = m_wndProjectTree.GetSelectedItem();
  if(hItem && hItem != projectTreeRoot) {
		HTREEITEM tp=m_wndProjectTree.GetNextItem(hItem,TVGN_PARENT);
		if(tp && tp != projectTreeRoot) {
			CString S=m_wndProjectTree.GetItemText(tp);

			if(S.Compare("Source"))
				;

			CString strText = m_wndProjectTree.GetItemText(hItem);
			// Esegui l'azione (es. apri il documento/file corrispondente)
			ActivateViewByTitle(strText);
			}
		}

  *pResult = 0;
	}

void CMainFrame::OnTreeRightClick(NMHDR* pNMHDR, LRESULT* pResult) {
	CMenu menu;

	*pResult = 0; // Permette la gestione standard successiva

  // 1. Prendi la posizione corrente del cursore dello schermo
  CPoint ptScreen;
  ::GetCursorPos(&ptScreen);

  // 2. Converti le coordinate per il controllo TreeView
  CPoint ptClient = ptScreen;
  CTreeCtrl& tree = m_wndProjectTree;
  tree.ScreenToClient(&ptClient);

  // 3. Esegui l'HitTest per capire su quale nodo ha cliccato l'utente
  UINT uFlags = 0;
  HTREEITEM hItem = tree.HitTest(ptClient, &uFlags);

  // Se il clic è avvenuto esattamente sopra un elemento o sopra la sua icona
  if(hItem && (uFlags & TVHT_ONITEM))  {
    // Forza la selezione dell'elemento cliccato col tasto destro!
    tree.SelectItem(hItem);

    // (Opzionale) Salva l'elemento corrente se ti serve nei comandi
    // m_hClickedItem = hItem;

		HTREEITEM tp=m_wndProjectTree.GetNextItem(hItem,TVGN_PARENT);
		if(tp == projectTreeRoot) {
			CString S=m_wndProjectTree.GetItemText(hItem);

			if(!S.Compare("Source")) {
				if(menu.LoadMenu(IDR_PROJECTTREE1)) {
					CMenu* pSubMenu = menu.GetSubMenu(0);
					if(pSubMenu) {
						pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
							ptScreen.x,ptScreen.y,
							this 
							);
						}
					}
				}
			else if(!S.Compare("Header")) {
				if(menu.LoadMenu(IDR_PROJECTTREE1)) {
					CMenu* pSubMenu = menu.GetSubMenu(0);
					if(pSubMenu) {
						pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
							ptScreen.x,ptScreen.y,
							this 
							);
						}
					}
				}
			else if(!S.Compare("Altro")) {		// :)
				if(menu.LoadMenu(IDR_PROJECTTREE1)) {
					CMenu* pSubMenu = menu.GetSubMenu(0);
					if(pSubMenu) {
						pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
							ptScreen.x,ptScreen.y,
							this 
							);
						}
					}
				}
			}

		else if(tp) {
			// 4. Carica il menu risorsa
			if(menu.LoadMenu(IDR_PROJECTTREE)) {
				// Prendi il primo sottomenu (quello che contiene le voci vere e proprie)
				CMenu* pSubMenu = menu.GetSubMenu(0);
				if(pSubMenu) {
					// Mostra il menu popup alle coordinate dello schermo
					pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
						ptScreen.x,ptScreen.y,
						this // La finestra che riceverà i messaggi COMMAND dei menu (ON_COMMAND)
						);
					}
				}
			}
    }
	}

void CMainFrame::OnTreeItemExpanded(NMHDR* pNMHDR, LRESULT* pResult) {
  LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)pNMHDR;
  *pResult = 0;

  // Recuperiamo il puntatore al CTreeCtrl dentro la CDialogBar
  CTreeCtrl* pTree = (CTreeCtrl*)m_wndProjectBar.GetDlgItem(IDC_TREE_PROJECT);
  if(!pTree)
    return;

  HTREEITEM hItem = pNMTreeView->itemNew.hItem;

  // Verifichiamo se l'elemento espanso/chiuso ha sotto-elementi (è una cartella)
  if(hItem != projectTreeRoot && pTree->ItemHasChildren(hItem))  {
    // Indici delle icone nella tua CImageList:
    int nFolderClosedImg = 0; // Indice dell'icona cartella CHIUSA
    int nFolderOpenImg   = 1; // Indice dell'icona cartella APERTA
		HTREEITEM tp0;

    if(pNMTreeView->action == TVE_EXPAND)      {
      // Il nodo è stato espanso -> Impostiamo l'icona cartella APERTA
      pTree->SetItemImage(hItem, nFolderOpenImg, nFolderOpenImg);
	    }
    else if (pNMTreeView->action == TVE_COLLAPSE)      {
      // Il nodo è stato chiuso -> Impostiamo l'icona cartella CHIUSA
      pTree->SetItemImage(hItem, nFolderClosedImg, nFolderClosedImg);
      }
		}
	}

void CMainFrame::OnTvnDeleteitemTree(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMTREEVIEW pNMTreeView = (LPNMTREEVIEW)pNMHDR;

  /* no, passo l'elemento dell'array!  // Se il nodo eliminato aveva un ItemData, liberiamo la memoria
  char *pFile = (char *)pNMTreeView->itemOld.lParam;
  if(pFile && pFile>(char*)10)		// marker per i nodi principali!
    delete pFile; // Libera la CString e la struttura!
	*/

  *pResult = 0;
	}

void CMainFrame::ActivateViewByTitle(const CString& strTargetTitle) {

  // 1. Ottieni la prima finestra Child MDI
  CMDIChildWnd* pChild = (CMDIChildWnd*)MDIGetActive();
  if(!pChild) {
    // Nessuna finestra aperta, procedi ad aprire il documento/vista
    theApp.OpenDocumentFile(strTargetTitle);
//		MessageBeep(0);
    return;
    }

  // 2. Scorri tutte le MDI Child aperte tramite la catena di finestre
  CMDIChildWnd* pStartChild = pChild;
  bool bFound = false;

  do {
    // Ottieni la vista o il documento associato al frame
    COpenCDoc *pDoc = (COpenCDoc*)pChild->GetActiveDocument();
    
    CString strDocTitle;
    if(pDoc)
      strDocTitle = pDoc->GetTitle(); // oppure pDoc->GetPathName()
    else
      pChild->GetWindowText(strDocTitle); // fallback sul titolo della finestra

    // 3. Confronto (senza distinzione tra maiuscole/minuscole)
    if(strDocTitle.CompareNoCase(strTargetTitle) == 0) {
      // Trovata! Portala in primo piano e attiva la vista
      MDIActivate(pChild);
      
      if(pChild->IsIconic()) // Se era minimizzata, ripristinala
        pChild->ShowWindow(SW_RESTORE);
          
      bFound = true;
      break;
      }

    // Passa alla MDI Child successiva
    pChild = (CMDIChildWnd*)pChild->GetWindow(GW_HWNDNEXT);

		} while (pChild && pChild != pStartChild);

  // 4. Se non è stata trovata tra quelle aperte, la apri ex novo
  if(!bFound) {
    theApp.OpenDocumentFile(strTargetTitle);
//			MessageBeep(0);
    }
	}


void CMainFrame::OnTreeOpen() {
	CString strText;

  HTREEITEM hItem = m_wndProjectTree.GetSelectedItem();
  if(hItem && hItem != projectTreeRoot) {
		HTREEITEM tp=m_wndProjectTree.GetNextItem(hItem,TVGN_PARENT);
		if(tp && tp != projectTreeRoot) {
			CString S=m_wndProjectTree.GetItemText(tp);

			strText=m_wndProjectTree.GetItemText(hItem);
			ActivateViewByTitle(strText);
			}
		}
	}

void CMainFrame::OnTreeImpostazioni() {
	
	
	}

void CMainFrame::OnTreeEscludi() {
//	TV_ITEM tv;
	struct PROGETTO_ENTRY *pe;
  CTreeCtrl* pTree = (CTreeCtrl*)m_wndProjectBar.GetDlgItem(IDC_TREE_PROJECT);
	pe=(struct PROGETTO_ENTRY*)pTree->GetItemData(pTree->GetSelectedItem());

	if((DWORD)pe > 10) {		// marker
		pe->flag= !pe->flag;
		pTree->SetItemImage(pTree->GetSelectedItem(),pe->flag ? 2 : 3,pe->flag ? 2 : 3);
		theApp.progettoModified=TRUE;
		}
	
	}

void CMainFrame::OnTreePropriet() {
//	TV_ITEM tv;
	struct PROGETTO_ENTRY *pe;
  CTreeCtrl* pTree = (CTreeCtrl*)m_wndProjectBar.GetDlgItem(IDC_TREE_PROJECT);
//	pTree->GetItem(&tv);
//	pTree->GetItemText(pTree->GetSelectedItem());
	pe=(struct PROGETTO_ENTRY*)pTree->GetItemData(pTree->GetSelectedItem());
	CStringEx filename;

	if((DWORD)pe > 10) {		// marker
	  CFileStatus status;
    CString strInfo;

		if(!theApp.pathProgetto.IsEmpty())
			filename=theApp.pathProgetto+'\\'+pe->nomefile;
		else
			filename=pe->nomefile;

    // Se il file esiste su disco, leggiamo le sue caratteristiche
    if(CFile::GetStatus(filename, status)) {
      // Formattiamo la data di ultima modifica (es. DD/MM/YYYY HH:MM)
      CString strDate = status.m_mtime.Format(_T("%d/%m/%Y %H:%M:%S"));

      // Formattiamo la dimensione
      CString strSize;
      if(status.m_size < 1024)
        strSize.Format(_T("%lu Bytes"), status.m_size);
      else
        strSize.Format(_T("%.2f KB (%lu Bytes)"), status.m_size / 1024.0, status.m_size);

      strInfo.Format(_T("Nome file: %s\n") _T("Percorso completo: %s\n")
        _T("Dimensione: %s\n") _T("Ultima modifica: %s\n") _T("Stato build: %s"),
        (LPCTSTR)pe->nomefile,status.m_szFullName,
        strSize,strDate,
        !pe->flag ? _T("Escluso dalla compilazione") : _T("Incluso nella compilazione")
				);
			}
    else  {
      // File presente nel progetto ma non trovato su disco
      strInfo.Format(_T("File: %s\n\n")
        _T("ATTENZIONE: Il file non è stato trovato su disco!"),
        pe->nomefile
        );
			}

    AfxMessageBox(strInfo, MB_ICONINFORMATION | MB_OK, 0);
    }
	
	}

void CMainFrame::OnUpdateTreeEscludi(CCmdUI* pCmdUI) {
    CTreeCtrl* pTree = (CTreeCtrl*)m_wndProjectBar.GetDlgItem(IDC_TREE_PROJECT);
    if (!pTree) 
			return;

    HTREEITEM hSelected = pTree->GetSelectedItem();
    if(!hSelected) {
      pCmdUI->Enable(FALSE);
      return;
			}

    PROGETTO_ENTRY* pe = (PROGETTO_ENTRY*)pTree->GetItemData(hSelected);

    // Se è un nodo file valido
    if (pe && (DWORD_PTR)pe > 10) {
        pCmdUI->Enable(TRUE);

        // Se pe->flag indica che il file è escluso, proponiamo "Includi"
				pCmdUI->SetText(_T(pe->flag ? "&Escludi" :  "&Includi"));
	    }
    else    {
        // Disabilitiamo la voce di menu se l'utente ha cliccato su una cartella (Source/Header)
      pCmdUI->Enable(FALSE);
    }
	}

void CMainFrame::OnTreeAdd() {
	CStringEx S,S1;
	CFileDialog myDlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST | OFN_SHOWHELP | OFN_HIDEREADONLY,
		"File sorgente (*.c,*.h)|*.c;*.h|Tutti i file (*.*)|*.*||"
		);

	if(myDlg.DoModal() == IDOK) {
		theApp.AddFileToProject(myDlg.GetPathName(),TRUE);
		theApp.progettoModified=TRUE;
		}
		
	}

void CMainFrame::OnTreeElimina() {
	int i;
	CStringEx S;
	struct PROGETTO_ENTRY *pe;
  CTreeCtrl* pTree = (CTreeCtrl*)m_wndProjectBar.GetDlgItem(IDC_TREE_PROJECT);
	pe=(struct PROGETTO_ENTRY*)pTree->GetItemData(pTree->GetSelectedItem());
	S=pe->nomefile;

	if(AfxMessageBox("Eliminare questo file dal progetto?",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) {

		for(i=0; i<theApp.fileProgetto.GetSize(); i++) {
			if(!theApp.fileProgetto[i].nomefile.Compare(S)) {
				theApp.fileProgetto.RemoveAt(i);
				theApp.ReparseProgetto();
//	:)			if(AfxMessageBox("Eliminare fisicamente il file?",MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION)) {
	//				}

				break;
				}
			}

		theApp.progettoModified=TRUE;
		}

	
	}

void CMainFrame::OnUpdateTreeElimina(CCmdUI* pCmdUI) {
  CTreeCtrl* pTree = (CTreeCtrl*)m_wndProjectBar.GetDlgItem(IDC_TREE_PROJECT);
	
	pCmdUI->Enable(pTree->GetSelectedItem() ? TRUE : FALSE);
	}




// ------------------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(COutputBar, CDialogBar)

COutputBar::COutputBar() { }

COutputBar::~COutputBar() { }

BEGIN_MESSAGE_MAP(COutputBar, CDialogBar)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_OUTPUT_TAB, OnTabSelChange)
END_MESSAGE_MAP()

BOOL COutputBar::PreTranslateMessage(MSG* pMsg) {

  // Verifichiamo se è un tasto premuto con CTRL
  if(pMsg->message == WM_KEYDOWN && (GetKeyState(VK_CONTROL) & 0x8000)) {
    // Se il focus è dentro la nostra CEdit di output
    CWnd* pFocusWnd = CWnd::GetFocus();
    if(pFocusWnd && pFocusWnd->GetDlgCtrlID() == IDC_TXT_OUTPUT) {
      if(pMsg->wParam == 'C' || pMsg->wParam == 'c') {
        // Invia direttamente il messaggio di COPY alla CEdit
        m_wndOutputEdit.Copy();
        return TRUE; // Tasto gestito, non inoltrare alla MainFrame!
				}
      else if(pMsg->wParam == 'A' || pMsg->wParam == 'a') {
        // Un bel "Seleziona Tutto" (Ctrl+A) che non fa mai male!
        m_wndOutputEdit.SetSel(0, -1);
        return TRUE;
        }
      }
    }

  return CDialogBar::PreTranslateMessage(pMsg);
	}

void COutputBar::OnSize(UINT nType, int cx, int cy) {
  CDialogBar::OnSize(nType, cx, cy);

  if(cx <= 0 || cy <= 0)
    return;

  if(m_wndOutputTab.GetSafeHwnd()) {
    // 1. Il TabControl occupa tutto lo spazio della barra
    m_wndOutputTab.MoveWindow(0, 0, cx, cy);

    // 2. Calcoliamo l'area utile visibile per la finestra di testo
    CRect rcDisplay(0, 0, cx, cy);
    
    // AdjustRect con FALSE calcola l'area interna escludendo i tab
    m_wndOutputTab.AdjustRect(FALSE, &rcDisplay);

    if(m_wndOutputEdit.GetSafeHwnd())        {
      // 3. Posizioniamo l'Edit Box nell'area interna esatta
      m_wndOutputEdit.MoveWindow(&rcDisplay);

      // 4. IMPORTANTE per Win32: Il Tab Control DEVE stare sopra in Z-Order 
      // affinché le linguette in basso restino visibili e cliccabili!
      m_wndOutputTab.SetWindowPos(&CWnd::wndTop, 0, 0, 0, 0, 
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
      
      m_wndOutputEdit.SetWindowPos(&m_wndOutputTab, 0, 0, 0, 0, 
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
      }
		}
  else if (m_wndOutputEdit.GetSafeHwnd())
    m_wndOutputEdit.MoveWindow(0, 0, cx, cy);
	}

void COutputBar::OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult) {
  int nTab = m_wndOutputTab.GetCurSel();

  // Nascondiamo tutto
  m_wndOutputEdit.ShowWindow(SW_HIDE);
  m_wndFindInFilesDlg.ShowWindow(SW_HIDE);

  // Mostriamo solo quello selezionato
  switch (nTab) {
		case 0: // Build
		case 1: // Debug
			m_wndOutputEdit.ShowWindow(SW_SHOW);
			break;

		case 2: // Find in Files 
			m_wndFindInFilesDlg.ShowWindow(SW_SHOW);
			break;
	  }

  *pResult = 0;
	}



// ------------------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(COutputEdit, CEdit)

COutputEdit::COutputEdit()
{
}

COutputEdit::~COutputEdit()
{
}

BEGIN_MESSAGE_MAP(COutputEdit, CEdit)
  ON_WM_LBUTTONDBLCLK()
  ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void COutputEdit::OnLButtonDblClk(UINT nFlags, CPoint point) {

  CEdit::OnLButtonDblClk(nFlags, point);
  ProcessCurrentLine();
	}

void COutputEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {

  if(nChar == VK_F4) {
    ProcessCurrentLine();
    return; // Intercettato F4
		}
  else if(nChar == VK_DELETE) {
    SetWindowText("");
    return; // 
		}

  CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}

void COutputEdit::ProcessCurrentLine() {

  // 1. Identifica l'indice della riga correntemente selezionata/cliccata
  int nSelStart, nSelEnd;
  GetSel(nSelStart, nSelEnd);
  int nLineIndex = LineFromChar(nSelStart);

  // 2. Estrae il testo della riga
  int nLen = LineLength(LineIndex(nLineIndex));
  if(nLen <= 0) 
		return;

  CString strText;
  LPTSTR pBuffer = strText.GetBufferSetLength(nLen + 1);
  GetLine(nLineIndex, pBuffer, nLen);
  pBuffer[nLen] = _T('\0');
  strText.ReleaseBuffer();

// 1. Estrazione del nome file (tutto ciò che sta prima dei primi ':')
  CString strFileName;
  int nColonPos = strText.Find(_T(':'));
  if(nColonPos != -1) {
    strFileName = strText.Left(nColonPos);
    strFileName.TrimLeft();
    strFileName.TrimRight();
    }
		
	// 3. Estrae il numero di riga dal messaggio di errore (es: "main.c(42): error...")
  int nLineNum = 0;
  
  // Parsing del pattern " filename(linea) " o ": line X"
  int nFirstParen = strText.Find(_T('('));
  int nSecondParen = strText.Find(_T(')'));

  if(nFirstParen != -1 && nSecondParen > nFirstParen)    {
    CString strLine = strText.Mid(nFirstParen + 1, nSecondParen - nFirstParen - 1);
    nLineNum = _ttoi(strLine);
		}
  else  {
    // Fallback: se la riga contiene direttamente solo un numero o un formato libero
    // Modifica la logica in base al formato dei messaggi dell'output
    sscanf((LPCTSTR)strText, "%d", &nLineNum);
		}

  // 4. Invia il messaggio con il numero di riga al CMainFrame
  if(nLineNum > 0)
    theApp.m_pMainWnd->SendMessage(WM_GOTO_OUTPUT_LINE, (WPARAM)nLineNum, (LPARAM)(LPCTSTR)strFileName);
	}

void COutputEdit::SelectLine(int nLineIndex) {
  int nStartChar = LineIndex(nLineIndex);

  if(nStartChar != -1)    {
    int nLineLen = LineLength(nStartChar);
    SetSel(nStartChar, nStartChar + nLineLen);
//        ScrollCaret(); // Assicura che la riga selezionata sia visibile
    }
	}



void COutputEdit::ProcessNextError(BOOL bForward) {
  int nTotalLines = GetLineCount();

  if (nTotalLines <= 0) 
		return;

  // 1. Determina la riga da cui partire
  int nSelStart, nSelEnd;
  GetSel(nSelStart, nSelEnd);
  int nCurrentLine = LineFromChar(nSelStart);

  // Incrementa o decrementa la riga di partenza
  int nStartSearchLine = bForward ? (nCurrentLine + 1) : (nCurrentLine - 1);
  
  BOOL bWrapped = FALSE;

  // Gestione dei limiti e del wrap-around (ricomincia da capo o dal fondo)
  if (nStartSearchLine >= nTotalLines)    {
    nStartSearchLine = 0;
    bWrapped = TRUE;
    }
  else if (nStartSearchLine < 0)    {
    nStartSearchLine = nTotalLines - 1;
    bWrapped = TRUE;
    }

    int nTargetLine = -1;

  // 2. Ciclo di ricerca della riga con il formato errore "(linea)"
  for (int i = 0; i < nTotalLines; ++i)    {
    int nCheckLine;
    if (bForward)        {
      nCheckLine = (nStartSearchLine + i) % nTotalLines;
	    }
    else        {
      nCheckLine = (nStartSearchLine - i + nTotalLines) % nTotalLines;
		  }

    // Se durante il ciclo passiamo dal limite dell'elenco, segnamo il wrap
    if (i > 0 && nCheckLine == (bForward ? 0 : nTotalLines - 1))        {
      bWrapped = TRUE;
	    }

    int nLen = LineLength(LineIndex(nCheckLine));
    if (nLen <= 0) 
			continue;

    CString strText;
    LPTSTR pBuffer = strText.GetBufferSetLength(nLen + 1);
    GetLine(nCheckLine, pBuffer, nLen);
    pBuffer[nLen] = _T('\0');
    strText.ReleaseBuffer();

    // Controllo della presenza del pattern "(X)"
    int nFirstParen = strText.Find(_T('('));
    int nSecondParen = strText.Find(_T(')'), nFirstParen);

    if(nFirstParen != -1 && nSecondParen > nFirstParen)        {
      nTargetLine = nCheckLine;
      break; // Trovato!
      }
    }

  // 3. Risultato della ricerca
  if(nTargetLine != -1) {
    // Se la ricerca è dovuta ripartire dall'inizio/fine, emetti un suono
    if(bWrapped) 
      ::MessageBeep(MB_ICONASTERISK);

    SelectLine(nTargetLine);
    ProcessCurrentLine();
    }
	}

