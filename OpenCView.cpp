// OpenCEditView.cpp : implementation of the COpenCEditView class
//
OCCHIO con v. 41 di richedit non va più Find !! e finire ctrl-f3 cmq


#include "stdafx.h"
#include "OpenC.h"
#include "Mainfrm.h"

#include "OpenCDoc.h"
#include "OpenCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// COpenCEditView

IMPLEMENT_DYNCREATE(COpenCView, CRichEditView)
// https://www.codeproject.com/articles/A-Very-Simple-Way-to-Use-Richedit-5-0-in-VC6-and-o?msg=3389216#comments-section

static UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

BEGIN_MESSAGE_MAP(COpenCView, CRichEditView)
	//{{AFX_MSG_MAP(COpenCView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CHAR()
	ON_COMMAND(ID_EDIT_TROVASELEZIONE, OnEditTrovaselezione)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TROVASELEZIONE, OnUpdateEditTrovaselezione)
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_REPEAT, OnEditRepeat)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPEAT, OnUpdateEditRepeat)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRichEditView::OnFilePrintPreview)
  ON_UPDATE_COMMAND_UI(ID_INDICATOR_POS, OnUpdatePosIndicator)
	ON_WM_CONTEXTMENU()
// 2. Mappa il messaggio speciale verso la funzione di gestione MFC
//	ON_COMMAND(ID_EDIT_FIND, OnEditFindCustom)               // Sovrascrive il comando Trova di MFC
  ON_COMMAND(ID_EDIT_REPEAT, OnEditTrovaselezione)        // Il tuo Ctrl+F3
  ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplaceCmd)  // Messaggi dalla Dialog
	ON_COMMAND(ID_OPEN_INCLUDE_FILE, OnOpenIncludeFile)
	ON_MESSAGE(WM_MY_FILE_CHANGED, OnFileChangedExternally)
	//ON_EN_CHANGE(AFX_IDW_PANE_FIRST, OnEnChange)
	ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenCView construction/destruction

COpenCView::COpenCView() {

	m_strClass=TEXT("RichEdit50W");
  //Must be m_strClass, it is a member of CCtrlView

	m_bDelayUpdateItems = FALSE;

	m_pFindDlg = NULL;
  m_bMatchCase = FALSE;
  m_bWholeWord = FALSE;

	m_uTimerID=0;
	m_bInitialUpdateDone=FALSE;
	}

COpenCView::~COpenCView() {
	}

BOOL COpenCView::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
		
// Rimuovi gli stili di scrollbar dal frame/vista per evitare il raddoppio
  cs.style &= ~(WS_VSCROLL | WS_HSCROLL);
	
	return CRichEditView::PreCreateWindow(cs);
	}

/////////////////////////////////////////////////////////////////////////////
// COpenCEditView drawing

void COpenCView::OnPaint() {
	COpenCView *w;

	CRichEditView::OnPaint();
	//Default();

  CPaintDC dc(this);
//	GetRichEditCtrl().SendMessage(WM_PRINT, (WPARAM)dc.GetSafeHdc(), PRF_CLIENT | PRF_CHILDREN | PRF_ERASEBKGND);

	CSplitterWnd* pSplitter = (CSplitterWnd*)GetParent();

	//GetRichEditCtrl().Invalidate();

/*	w=(COpenCView*)((CMainFrame*)GetParent()->GetParent())->GetActiveView();
  if(!w) 
		return;*/

	CWnd* pPaneWnd = pSplitter->GetPane(0, 0);
	if(pPaneWnd) {
    // Handle della finestra (HWND)
//    HWND hWndSubWindow = pPaneWnd->GetSafeHwnd();

    // Se vuoi il cast alla tua CView specifica:
    CGutterWnd* pOtherView = DYNAMIC_DOWNCAST(CGutterWnd, pPaneWnd);
    if(pOtherView)    {
      pOtherView->Invalidate();
			}
		}

	pPaneWnd = pSplitter->GetPane(1, 0);
	if(pPaneWnd) {
    CGutterWnd* pOtherView = DYNAMIC_DOWNCAST(CGutterWnd, pPaneWnd);
    if(pOtherView)    {
      pOtherView->Invalidate();
			}
		}
	}


/////////////////////////////////////////////////////////////////////////////
// COpenCView printing

BOOL COpenCView::OnPreparePrinting(CPrintInfo* pInfo) {

	// default preparation
	return DoPreparePrinting(pInfo);
	}

void COpenCView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
	// TODO: add extra initialization before printing
	}

void COpenCView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/) {
	// TODO: add cleanup after printing
	}



void COpenCView::OnEnChange() { // Mappato su EN_CHANGE
  CRichEditCtrl& edit = GetRichEditCtrl();

	// Rinviamo la gestione base al CRichEditView
//    CRichEditView::OnChange();

  COpenCDoc* pDoc = (COpenCDoc*)GetDocument();
  if (pDoc)    {
      // Se il documento non risultava ancora modificato, forza il flag!
//       if (!pDoc->IsModified())        
          pDoc->SetModifiedFlag(TRUE); // Questo farà scattare UpdateFrameTitle()!
      
    }

  int nNewLineCount = edit.GetLineCount();
  
  // m_nPrevLineCount lo avevi salvato precedentemente (es. all'apertura o al cambio precedente)
  int nDelta = nNewLineCount - pDoc->m_nDocLines;

  if(nDelta != 0)    {
    // Ricaviamo la riga corrente del cursore
    long nStartChar, nEndChar;
    edit.GetSel(nStartChar, nEndChar);
    int nCurrentLine = edit.LineFromChar(nStartChar);

    // Aggiorniamo la lista dei breakpoint ecc
    pDoc->UpdateMarkers(nCurrentLine, nDelta);

    // Aggiorniamo il conteggio precedente per il prossimo EN_CHANGE
    pDoc->m_nDocLines = nNewLineCount;
    
    // Forza il ridisegno del margine sinistro (dove ci sono i pallini dei breakpoint)
		CSplitterWnd* pSplitter = (CSplitterWnd*)GetParent();		// v. OnPaint
		CWnd* pPaneWnd = pSplitter->GetPane(0, 0);
		if(pPaneWnd) {
			CGutterWnd* pOtherView = DYNAMIC_DOWNCAST(CGutterWnd, pPaneWnd);
			if(pOtherView)    {
				pOtherView->Invalidate();
				}
			}
		pPaneWnd = pSplitter->GetPane(1, 0);
		if(pPaneWnd) {
			CGutterWnd* pOtherView = DYNAMIC_DOWNCAST(CGutterWnd, pPaneWnd);
			if(pOtherView)    {
				pOtherView->Invalidate();
				}
			}
    }

// Riavviamo il timer ad ogni tasto premuto (100 ms di ritardo)  PER COLORAZIONE
  SetTimer(1, 100, NULL);
	TRACE("en_change\n");
	}

void COpenCView::OnTimer(UINT_PTR nIDEvent) {

	if(nIDEvent == 1)    {
		KillTimer(1); // Spegniamo il timer
		HighlightVisibleRange();        // Ricoloriamo la porzione visibile!
		}
	else    {
		CRichEditView::OnTimer(nIDEvent);
    }
	}

/////////////////////////////////////////////////////////////////////////////
// COpenCEditView diagnostics

#ifdef _DEBUG
void COpenCView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void COpenCView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

COpenCDoc* COpenCView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenCDoc)));
	return (COpenCDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenCEditView message handlers

int COpenCView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	PARAFORMAT pf;
	PARAFORMAT pf2;
	LONG rgxTabs[32];
	int i,n;

	if(CRichEditView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	GetRichEditCtrl().SetFont(&(((CMainFrame *)theApp.m_pMainWnd)->myFont),TRUE);
	GetRichEditCtrl().ModifyStyle(WS_VSCROLL | WS_HSCROLL,0);		// altrimenti mi becco pure le barre dell'Edit Ctrl...
// Assicurati che lo scrollbar appartenga solo ed esclusivamente all'Edit Control
  DWORD dwStyle = GetRichEditCtrl().GetStyle();
  dwStyle |= (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_DISABLENOSCROLL);
  ::SetWindowLong(GetRichEditCtrl().GetSafeHwnd(), GWL_STYLE, dwStyle);

	pf.cbSize = sizeof(PARAFORMAT);
	pf.dwMask = PFM_ALIGNMENT | PFM_TABSTOPS;
	pf.wAlignment = PFA_RIGHT;
	pf.cTabCount = 32;		// boh...
	for(i=0; i<pf.cTabCount; i++)
		pf.rgxTabs[i]=i*4;

	// NON FA NULLA, verificare... ma su Richedit
//	((CRichEditCtrl*)this)->SetParaFormat(pf);
//	GetEditCtrl().SetParaFormat(pf2);

//https://learn.microsoft.com/en-us/cpp/mfc/reference/ceditview-class?view=msvc-170#settabstops
	CFont *pFont = GetFont();
	TEXTMETRIC tm;
	CDC *pDC = GetDC();
	CFont *pOldFont = pDC->SelectObject(pFont);
	pDC->GetTextMetrics(&tm);
	pDC->SelectObject(pOldFont);
	CRect rect(0, 0, 200, 1);
	::MapDialogRect((HWND)this, rect);
	SetTabStops((4 * tm.tmAveCharWidth * 100) / rect.Width());		// 4 char così
	ReleaseDC(pDC);

	::RevokeDragDrop(m_hWnd);		// non va...  https://stackoverflow.com/questions/2476589/how-to-disable-dragging-from-a-rich-edit-control
	// per evitare drop di file nel testo
	// v.sotto! messaggio a richedit

	return 0;
	}

void COpenCView::OnDestroy() {
	COpenCDoc *d=GetDocument();
	RECT rc;
	char myBuf[64];

  //CRichEditView::OnDestroy(); <------the MFC App Wiz put the codes here,
  // but it shouldn't.
  // Deactivate the item on destruction; this is important
  // when a splitter view is being used.
  COleClientItem* pActiveItem = GetDocument()->
               GetInPlaceActiveItem(this); //If OnDestroy() were still up there, the app would CRASH here!

	theApp.UnregisterMonitoredFile(m_hWnd);

  if(pActiveItem != NULL && pActiveItem->GetActiveView() == this) {
    pActiveItem->Deactivate();
    ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
		}
  CRichEditView::OnDestroy();
  // this codes should be here, so, everything is fine!

	GetParent()->GetWindowRect(&rc);	// coord. schermo della mia MDIChildFrmae...
	GetParent()->GetParent()->GetParent()->ScreenToClient(&rc);	// ... diventano coord. client rispetto al MDI padre (che NON e' theApp.m_pMainWnd !! ce n'è una un mezzo...
		// DUE GetParent perche' c'e' Splitter!!
	int correz=6;
	wsprintf(myBuf,"%d,%d,%d,%d",rc.left-correz,rc.top-correz,rc.right+correz,rc.bottom/*+correz*/);

	d->WritePrivateProfileString(IDS_COORDINATE,myBuf);
	}

BOOL COpenCView::CreateView(int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext) {
	int i=1;

	AfxMessageBox("cv");
	// non gliene puo' fregare di meno! forse arriva alla splitter??
	return i;
	}

void COpenCView::OnInitialUpdate() {

  CRichEditView::OnInitialUpdate(); // Oppure la tua classe base

	if(m_bInitialUpdateDone)		// serve secondo gemini da sempre :D mah...
    return;

  m_bInitialUpdateDone = TRUE;

	CRect rect;
  GetRichEditCtrl().GetClientRect(&rect);
//  rect.left += 80; // Riserva 20px sulla sinistra per il margine segnalibri  UN CAZZO faccio con splitter
//  GetRichEditCtrl().SetRect(&rect);

	// Imposta un margine sinistro di 30 pixel (in DP/Pixel)
    // EC_LEFTMARGIN indica di modificare solo il margine sinistro
//    GetRichEditCtrl().SendMessage(EM_SETMARGINS, EC_LEFTMARGIN, MAKELONG(300, 0));
		
// 1. Diciamo a CRichEditView di formattare rispetto alla finestra
//    m_nWordWrap = WrapToWindow; // oppure NoWrap, a seconda delle tue esigenze
    
    // 2. Impostiamo il margine sinistro in Twip (1 pixel = ~15 twip a 96 DPI)
    // 30 pixel * 15 = 450 twip
  //  m_rectMargin.left = 450;

    // 3. Applichiamo la modifica al layout
    //WrapChanged();

		/*PARAFORMAT2 pf;
    ZeroMemory(&pf, sizeof(pf));
    pf.cbSize = sizeof(PARAFORMAT2);
    pf.dwMask = PFM_STARTINDENT;
    pf.dxStartIndent = 300; // Rientro in Twip (~30 pixel)

    // Imposta la selezione su tutto il testo ed applica il rientro predefinito
    GetRichEditCtrl().SetSel(0, -1);
    GetRichEditCtrl().SendMessage(EM_SETPARAFORMAT, 0, (LPARAM)&pf);
    
    // Ripristina la selezione a inizio documento
    GetRichEditCtrl().SetSel(0, 0);
*/

		// Disabilita il Word Wrap (nessun dispositivo di target, larghezza linea = 0)
//  GetRichEditCtrl().SendMessage(EM_SETTARGETDEVICE, (WPARAM)NULL, 10000000);	fa impazzire scrollbar 
// Imposta il wrap su "nessun wrap"
	// non c'è pd SetWrapMode(CRichEditView::WrapNone);


	// Disabilita la gestione nativa di Drag & Drop OLE sul RichEdit
    //::RevokeDragDrop(GetRichEditCtrl().GetSafeHwnd()); non fa nulla
	GetRichEditCtrl().ModifyStyleEx(WS_EX_ACCEPTFILES, 0);

// Ottiene la maschera eventi attuale e aggiunge ENM_CHANGE (compreso il tasto destro NM_RCLICK
  long lMask = GetRichEditCtrl().GetEventMask();
  GetRichEditCtrl().SetEventMask(lMask | ENM_CHANGE | ENM_MOUSEEVENTS);

	// 1. Aggiungi gli stili obbligatori per lo scorrimento orizzontale illimitato  C'è GIA
//    rich.ModifyStyle(0, WS_HSCROLL | ES_AUTOHSCROLL);

    // 2. Disabilita il wrapping avanzato e imposta l'ampiezza riga a un valore enorme (o 0)
    // Passando 1 come cxLineWidth disattiva il wrap sulla larghezza del window DC
  GetRichEditCtrl().SendMessage(EM_SETTARGETDEVICE, (WPARAM)NULL, 1);

// Disabilita il Word Wrap nativo di RichEdit 4.1 / 5.0
		//rich.SendMessage(EM_SETWORDWRAPMODE, (WPARAM)WOF_NOREPEAT, 0);		non c'è, frocio google

    // 3. (Fondamentale per RichEdit 4.1 / 5.0): Notifica il ricalcolo del layout  CAZZATA gemini cmq ok
    // Forza la dimensione del testo a non essere vincolata dal rect della finestra
  GetRichEditCtrl().SendMessage(EM_SETRECTNP, 0, 0);

// non c'è	GetRichEditCtrl().SetTextMode(TM_RICHTEXT | TM_SINGLELEVELUNDO);		per impedire Paste di immagini
	// non va cmq GetRichEditCtrl().SendMessage(EM_SETTEXTMODE, TM_PLAINTEXT | TM_MULTILEVELUNDO, 0);
	GetRichEditCtrl().SendMessage(EM_SETOLECALLBACK, 0, (LPARAM)NULL);

    // 4. Aggiorna l'interfaccia
  //  rich.Invalidate();

  COpenCDoc* pDoc = GetDocument();
  if(pDoc && GetRichEditCtrl().GetSafeHwnd()) {
    // Se la vista è appena stata creata ed è vuota
    if(GetRichEditCtrl().GetTextLength() == 0) {
      POSITION pos = pDoc->GetFirstViewPosition();
      while(pos) {
        CView* pView = pDoc->GetNextView(pos);
        if (pView != this && pView->IsKindOf(RUNTIME_CLASS(COpenCView))) {
          CString strText;		// a che serviva sta roba??
          //((COpenCView*)pView)->GetRichEditCtrl().GetWindowText(strText);
          //GetRichEditCtrl().SetWindowText(strText);
 //         GetRichEditCtrl().SetFocus();
          break;
          }
        }
      }
    }

	if(!theApp.nomeProgetto.IsEmpty())
		theApp.LoadProject(theApp.nomeProgetto,pDoc);

	WIN32_FILE_ATTRIBUTE_DATA wfd;
  if (GetFileAttributesEx(pDoc->GetPathName(), GetFileExInfoStandard, &wfd)) {
    // Registra la finestra corrente presso il monitor globale
    theApp.RegisterMonitoredFile(pDoc->GetPathName(), m_hWnd, wfd.ftLastWriteTime);
    }
				
	//SetFocus();
	GetParentFrame()->SetTitle(pDoc->GetTitle());
	((CMainFrame*)theApp.m_pMainWnd)->MDIActivate(GetParentFrame());
	GetParentFrame()->SetActiveView(this);

	//SetTimer(1, 100, NULL);		// colorazione
	PostMessage(WM_TIMER, 1, 0); // O chiama direttamente HighlightVisibleRange();

	}

void COpenCView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) {
  CRichEditView::OnActivateView(bActivate, pActivateView, pDeactiveView);

  // Eseguiamo solo se la vista viene effettivamente ATTIVATA 
  // e se la vista disattivata è DIVERSA da se stessa
  if(bActivate && (pActivateView != pDeactiveView))    {

        // Evita riesecuzioni ridondanti, cazzata cmq, initialupdate arriva sempre 2 volte SEMBRAVA!
	  }
	}

BOOL COpenCView::PreTranslateMessage(MSG* pMsg) {

  if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F12)    {
    CRichEditCtrl& ctrl = GetRichEditCtrl();

    // Ricaviamo la posizione del cursore di testo corrente
    CHARRANGE cr;
    ctrl.GetSel(cr);

    // Convertiamo l'indice in coordinate client per riusare GetWordAtPoint
		CPoint ptCaret ; //= ctrl.PosFromChar(cr.cpMin);
    CString strInclude = GetWordAtPoint(ptCaret);

    if(!strInclude.IsEmpty())        {
      COpenCDoc* pDoc = GetDocument();
      if(pDoc) {
        pDoc->OpenIncludeFile(strInclude);
        return TRUE; // Messaggio gestito
        }
      }
    }

    return CRichEditView::PreTranslateMessage(pMsg);
	}

void COpenCView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {

// probabilmente bisogna aggiornare le sub-panes tra loro ogni volta che una viene modificata... boh?	
	CRichEditView::OnChar(nChar, nRepCnt, nFlags);
	}

BOOL COpenCView::SetTabStops(const int& cxEachStop) {

	return GetRichEditCtrl().SendMessage(EM_SETTABSTOPS,1,(uint32_t)&cxEachStop);
	}

uint32_t COpenCView::CharFromPos(POINT pt) {
	POINTL pt2;

	return GetRichEditCtrl().SendMessage(EM_CHARFROMPOS,0,(uint32_t)&pt);
	}

uint32_t COpenCView::GetLineCount() {

	return GetRichEditCtrl().GetLineCount();
	}

DWORD CALLBACK COpenCView::MyStreamInCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb) {
  CFile* pFile = (CFile*) dwCookie;

  *pcb = pFile->Read(pbBuff, cb);
  return 0;
	}

DWORD CALLBACK COpenCView::MyStreamOutCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb) {
  CFile* pFile = (CFile*) dwCookie;

  pFile->Write(pbBuff, cb);
  *pcb = cb;
  return 0;
	}

long COpenCView::StreamIn(EDITSTREAM es) {

	es.pfnCallback = COpenCView::MyStreamInCallback;
	return GetRichEditCtrl().StreamIn(SF_TEXT, es);
	}

long COpenCView::StreamOut(EDITSTREAM es) {

	es.pfnCallback = COpenCView::MyStreamOutCallback;
	GetRichEditCtrl().SetModify(FALSE);
	return GetRichEditCtrl().StreamOut(SF_TEXT, es);
	}

void COpenCView::OnEditTrovaselezione() {

// gemini 2026

  CRichEditCtrl& ctrl = GetRichEditCtrl();

  CHARRANGE cr;
  ctrl.GetSel(cr);

	// 1. Se il cursore è fermo, trova i confini della parola rispettando i margini di riga
  if (cr.cpMin == cr.cpMax)    {
    long nPos = cr.cpMin;

    // Ricaviamo l'indice del primo carattere della riga corrente
    long nLineIndex = (long)ctrl.SendMessage(EM_LINEINDEX, -1, 0);
    long nStart = 0;
    long nEnd = 0;

// Gestione speciale per INIZIO FILE (Posizione 0)
    if (nPos == 0)        {
      nStart = 0;
      nEnd = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_RIGHT, 0);

      // Se la prima parola non è stata trovata correttamente con WB_RIGHT,
      // usiamo WB_RIGHTSTART per saltare ad esempio eventuali spazi/caratteri iniziali
      if (nEnd <= 0) {
        nStart = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_RIGHTSTART, 0);
        nEnd   = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_RIGHT, nStart);
        }
     }

    else if (nPos == nLineIndex)        {
      // Inizio riga generico
      nStart = nPos;
      nEnd = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_RIGHT, nStart);
      }

     else        {
        // Resto del codice preesistente per l'interno della riga...
        nStart = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_LEFT, nPos);

        if (nStart < nLineIndex) 
          nStart = nLineIndex;

        nEnd = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_RIGHT, nStart);
        }

       
      // Se l'intervallo non è valido (es. cursore su spazi a fine riga), proviamo ad avanzare
      if (nEnd <= nPos)        {
        nStart = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_RIGHTSTART, nPos);
        nEnd   = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_RIGHT, nStart);
	      }


      // Selezioniamo la parola trovata

      if (nEnd > nStart)
        ctrl.SetSel(nStart, nEnd);

      } 

#if 0		// fa cagare cmq, provare SelectWordAtCaret ecc sotto

// 1. Se il cursore è fermo, trova i confini esatti della parola
    if (cr.cpMin == cr.cpMax)    {
			long nPos = cr.cpMin;

   // WB_ISDELIMITER: verifica se il carattere alla posizione nPos è un delimitatore (spazio, tab, punteggiatura)
        BOOL bOnDelimiter = (BOOL)ctrl.SendMessage(EM_FINDWORDBREAK, WB_ISDELIMITER, nPos);

        long nStart = nPos;
        long nEnd = nPos;

        if (bOnDelimiter)        {
            // Se il cursore si trova su uno spazio o delimitatore, avanza alla prossima parola
            nStart = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_RIGHTSTART, nPos);
            nEnd   = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_RIGHT, nStart);
        }
        else        {
            // Se siamo già sopra una parola:
            // WB_MOVEWORDLEFT si sposta all'inizio della parola corrente SENZA saltare indietro se siamo già all'inizio
            // WB_LEFTSTART trova l'inizio esatto della parola contenente nPos
            nStart = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_LEFTSTART, nPos);
            
            // Se nStart fallisce o restituisce nPos quando non dovrebbe, proviamo a retrocedere fino al delimitatore
            if (nStart < 0 || nStart > nPos)            {
                nStart = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_LEFT, nPos);
            }

            nEnd = (long)ctrl.SendMessage(EM_FINDWORDBREAK, WB_RIGHT, nStart);
        }

        // Seleziona la parola trovata
        if (nEnd > nStart)        {
            ctrl.SetSel(nStart, nEnd);
        }
    }
#endif


    // 2. Estrazione testo con il metodo sicuro che abbiamo creato
    CString strFind = GetRichTextSelection();
    strFind.TrimLeft();
    strFind.TrimRight();

    if (strFind.IsEmpty())
        return;

    // Salviamo il testo per i successivi "Trova Successivo"
    m_strLastSearch = strFind;

    // 3. Esegui la ricerca
    DoSearchText(m_strLastSearch, TRUE /* Avanti */, m_bMatchCase, m_bWholeWord);


#if 0
	char *lpszFind;
	int nStartChar,nEndChar;
	CHARRANGE cha;
	int oldSel;
	GetRichEditCtrl().GetSel(cha);
	uint8_t foundState=0;
// astratto...	IDataObject ido;

	nStartChar=CharFromPos(GetRichEditCtrl().GetCaretPos());
//	GetEditCtrl().GetSel(nStartChar,nEndChar);
	nStartChar=LOWORD(nStartChar);

	nEndChar=nStartChar+1;

rifo:
	GetRichEditCtrl().SetSel(nStartChar,nEndChar);
	GetRichEditCtrl().Copy();

	// o anche GetTextRange()


	// selezionare la parola completa... NON c'è un metodo automatico :( senza passare da clipboard
/*	if(OpenClipboard()) {
//		HRESULT h=GetClipboardData( &cha,RECO_PASTE, &ido, &iddo);
		GetRichEditCtrl().Copy();

	  //lpszFind= (char*)GlobalLock(h); 
		CloseClipboard();
		}*/
	switch(foundState) {
		case 0:
			if(isgraph(*lpszFind)) {
				foundState++;
				nStartChar--;
				goto rifo;
				}
			else {
				GetRichEditCtrl().SetSel(cha);
				MessageBeep(0);
				return;
				}
			break;
		case 1:
			if(isgraph(*lpszFind)) {
				nStartChar--;
				goto rifo;
				}
			else {
				nStartChar++;
				foundState++;
				nEndChar++;
				goto rifo;
				}
			break;
		case 2:
			if(strchr(lpszFind,' ')) {
				nEndChar--;
				foundState++;
				goto rifo;
				}
			else {
				nEndChar++;
				if(nEndChar<65535  /*GetEditCtrl().GetLimitText()*/)		// non va... TROVARE
					goto rifo;
				}
			break;
		case 3:
			break;
		}

	if(!FindText(lpszFind,TRUE,FALSE))		// RIPARTIRE DA INIZIO
		;
#endif
	}

void COpenCView::OnUpdateEditTrovaselezione(CCmdUI* pCmdUI) {
	
	}



BOOL COpenCView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	int m;

	m=GetAsyncKeyState(VK_CONTROL) & 0x8000;

	if(zDelta>0) {
		SendMessage(WM_VSCROLL,MAKEWORD(m ? SB_PAGEUP : SB_LINEUP,0),0);
		}
	else {
		SendMessage(WM_VSCROLL,MAKEWORD(m ? SB_PAGEDOWN : SB_LINEDOWN,0),0);
		}
	
	return CRichEditView::OnMouseWheel(nFlags, zDelta, pt);
	}

BOOL COpenCView::GetWindowPos(RECT *rc) {		// restituisce coordinate relative al parent/FrameWnd
	RECT rc2,rc3;

	if(GetParent() && GetParent()->GetParent()) {
		GetParent()->GetParent()->GetWindowRect(rc);
		theApp.m_pMainWnd->GetWindowRect(&rc2);
		rc2.top+=GetSystemMetrics(SM_CYMENU)+GetSystemMetrics(SM_CYSIZE)+GetSystemMetrics(SM_CYCAPTION)+
			GetSystemMetrics(SM_CYBORDER)+GetSystemMetrics(SM_CYFRAME) +15 /*toolbar*/;
	// non più valida in cihiuseura porcamadonna 	((CMainFrame*)GetParent())->getToolbarRect(&rc3);
		rc2.left+=2*GetSystemMetrics(SM_CXBORDER)+GetSystemMetrics(SM_CXFRAME);
		rc->top-=rc2.top;
		rc->bottom-=rc2.top;
		rc->left-=rc2.left;		// (tree project
		rc->right-=rc2.left;
		return TRUE;
		}

	return FALSE;
	}


void COpenCView::OnUpdatePosIndicator(CCmdUI* pCmdUI) {
  CRichEditCtrl& ctrl = GetRichEditCtrl();
  
  CHARRANGE cr;
  ctrl.GetSel(cr);

  long nLine = ctrl.LineFromChar(cr.cpMin);
  long nLineStart = ctrl.LineIndex(nLine);
  long nCol = cr.cpMin - nLineStart;

  CString strPos;
  strPos.Format(_T("Ln %d, Col %d"), nLine + 1, nCol + 1);

  pCmdUI->Enable(TRUE);
  pCmdUI->SetText(strPos);
	}



#define KEY_TAG_START	'<'
#define KEY_TAG_END		'>'
#define KEY_DBL_QUOTE	'"'
#define COMMENT_START	_T("/*")
#define COMMENT_END		_T("*/")

#define TIMER_BACKGROUNDCOLORING	100

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx
/////////////////////////////////////////////////////////////////////////////

// Callback procedure that reads a files contents into rich edit control.
static DWORD CALLBACK StreamInCallback(DWORD dwCookie, LPBYTE pbBuff, 
									   LONG cb, LONG *pcb) {
  CFile* pFile = (CFile*)dwCookie;
  *pcb = pFile->Read(pbBuff, cb);
  return 0;
	}




BOOL COpenCView::DoSearchText(LPCTSTR lpszFind, BOOL bDown, BOOL bCase, BOOL bWholeWord) {

  if(!lpszFind || lpszFind[0] == _T('\0'))
      return FALSE;

  CRichEditCtrl& ctrl = GetRichEditCtrl();

  CHARRANGE cr;
  ctrl.GetSel(cr);

  FINDTEXTEXW ft;
  ::ZeroMemory(&ft, sizeof(ft));

  // Buffer fisso per la stringa Unicode
  WCHAR szUnicodeFind[1024];

#ifdef _UNICODE
  // Se il progetto è compilato in Unicode, copia direttamente
  lstrcpynW(szUnicodeFind, lpszFind, 1024);
  //ft.lpstrText = szUnicodeFind;
#else
  // Se il progetto è compilato in ANSI (MBCS), converti manualmente tramite Win32 API
//  ::MultiByteToWideChar(CP_ACP, 0, lpszFind, -1, szUnicodeFind, 1024);
	CStringEx Sw(lpszFind /*"culo"*/);		//test
	WCHAR szUnicode[256];
	ft.lpstrText=Sw.GetUnicode(szUnicode);
#endif


  // Imposta i flag di ricerca
  DWORD dwFlags = 0;
  if(bDown)      dwFlags |= FR_DOWN;
  if(bCase)      dwFlags |= FR_MATCHCASE;
  if(bWholeWord) dwFlags |= FR_WHOLEWORD;

  if(bDown) {
    ft.chrg.cpMin = cr.cpMax;
    ft.chrg.cpMax = -1; // Cerca fino alla fine del documento
	  }
  else {
    ft.chrg.cpMin = cr.cpMin;
    ft.chrg.cpMax = 0;  // Cerca verso l'inizio
		}

  // Invio del messaggio nativo Unicode EM_FINDTEXTEXW
  long nFound = (long)ctrl.SendMessage(EM_FINDTEXTEXW, (WPARAM)dwFlags, (LPARAM)&ft);

  if(nFound != -1) {
    // Seleziona il testo trovato e centra la vista
    ctrl.SetSel(ft.chrgText);
    ctrl.SendMessage(EM_HIDESELECTION, FALSE, FALSE);
    ctrl.SendMessage(EM_SCROLLCARET, 0, 0);
    return TRUE;
		}

  AfxMessageBox(_T("Testo non trovato."), MB_OK | MB_ICONINFORMATION);
  return FALSE;
	}

CString COpenCView::GetRichTextSelection() {
  CRichEditCtrl& ctrl = GetRichEditCtrl();
  
  CHARRANGE cr;
  ctrl.GetSel(cr);

  long nLen = cr.cpMax - cr.cpMin;
  if (nLen <= 0)
      return _T("");

  // Allocazione di sicurezza: raddoppiamo la dimensione in byte per gestire 
  // l'eventuale terminatore Unicode a 16-bit che RichEdit scrive nel buffer
  int nBufferChars = (nLen + 2) * 2; 
  TCHAR* pBuffer = new TCHAR[nBufferChars];
  ::ZeroMemory(pBuffer, sizeof(TCHAR) * nBufferChars);

  // Invia EM_GETSELTEXT nativo
  ctrl.SendMessage(EM_GETSELTEXT, 0, (LPARAM)pBuffer);

  CString strResult;

#ifdef _UNICODE
    strResult = pBuffer;
#else
    // Se il controllo ha risposto in WCHAR (Unicode), convertiamo in ANSI
    if (pBuffer[1] == '\0' && pBuffer[0] != '\0') {
        // Il buffer contiene una stringa WCHAR (Unicode)
        WCHAR* pwstr = (WCHAR*)pBuffer;
        int nAnsiLen = ::WideCharToMultiByte(CP_ACP, 0, pwstr, -1, NULL, 0, NULL, NULL);
        if (nAnsiLen > 0) {
            char* pAnsiBuf = new char[nAnsiLen + 1];
            ::ZeroMemory(pAnsiBuf, nAnsiLen + 1);
            ::WideCharToMultiByte(CP_ACP, 0, pwstr, -1, pAnsiBuf, nAnsiLen, NULL, NULL);
            strResult = pAnsiBuf;
            delete[] pAnsiBuf;
        }
			}
    else {
        // Il buffer contiene già caratteri ANSI standard
        strResult = pBuffer;
    }
#endif

  delete[] pBuffer; // Ora la memoria viene liberata in modo sicuro
  return strResult;
	}


void COpenCView::SelectWordAtCaret() {
  CRichEditCtrl& ctrl = GetRichEditCtrl();

  CHARRANGE cr;
  ctrl.GetSel(cr);

  if(cr.cpMin != cr.cpMax)
    return; // C'è già una selezione

  long nPos = cr.cpMin;
  long nLen = ctrl.GetTextLength();

  if(nLen == 0)
    return;

  // Leggiamo un piccolo blocco di testo intorno al cursore (es. 128 caratteri)
  long nStartBuf = (nPos > 64) ? (nPos - 64) : 0;
  long nEndBuf   = (nPos + 64 < nLen) ? (nPos + 64) : nLen;
  long nBufSize  = nEndBuf - nStartBuf;

/*  TCHAR* pBuf = new TCHAR[nBufSize + 1];
  ::ZeroMemory(pBuf, sizeof(TCHAR) * (nBufSize + 1));

  TEXTRANGE tr;
  tr.chrg.cpMin = nStartBuf;
  tr.chrg.cpMax = nEndBuf;
  tr.lpstrText  = pBuf;

  ctrl.SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr);*/

	CString pBuf;
	GetTextRange(nStartBuf,nEndBuf,pBuf);

  // Indice relativo al buffer
  long nRelPos = nPos - nStartBuf;

  // Se siamo su uno spazio/delimitatore, avanziamo fino alla prima lettera valida
  while(nRelPos < nBufSize && _istspace(pBuf[(int)nRelPos]))
    nRelPos++;

  if(nRelPos >= nBufSize) {
//    delete[] pBuf;
    return;
		}

  // Troviamo l'inizio della parola andando a sinistra
  long nSelStartRel = nRelPos;
  while(nSelStartRel > 0 && (_istalnum(pBuf[(int)nSelStartRel - 1]) || pBuf[(int)nSelStartRel - 1] == _T('_'))) {
    nSelStartRel--;
	  }

  // Troviamo la fine della parola andando a destra
  long nSelEndRel = nRelPos;
  while(nSelEndRel < nBufSize && (_istalnum(pBuf[(int)nSelEndRel]) || pBuf[(int)nSelEndRel] == _T('_'))) {
    nSelEndRel++;
		}

  // Convertiamo gli indici relativi in posizioni assolute del documento
  long nFinalStart = nStartBuf + nSelStartRel;
  long nFinalEnd   = nStartBuf + nSelEndRel;

  if(nFinalEnd > nFinalStart) {
    ctrl.SetSel(nFinalStart, nFinalEnd);
		}

//  delete[] pBuf;
	}


// testo colorato
void COpenCView::HighlightVisibleRange() {
  CRichEditCtrl& edit = GetRichEditCtrl();
  COpenCDoc *pDoc = GetDocument();
	CStringEx S;

	if(!theApp.TestoColorato)
		return;
	S.SplitPath(pDoc->GetTitle(),4);
	if(S.CompareNoCase(".C") && S.CompareNoCase(".H"))		// per ora :) poi ampliare e gestire
		return;

	DWORD dwOldEventMask = edit.SetEventMask(edit.GetEventMask() & ~ENM_CHANGE);

	// 1. Blocco del Rendering visivo
  edit.SetRedraw(FALSE);
// 1. Salviamo lo stato di modifica reale del documento
  BOOL bOriginallyModified = pDoc->IsModified();

  // 2. Salvataggio della selezione corrente e dello scroll per ripristinarli dopo
  CHARRANGE crOriginal;
  edit.GetSel(crOriginal);
  
  int nFirstVisibleLine = edit.GetFirstVisibleLine();

  // 3. Calcolo dell'intervallo di caratteri VISIBILI
/*  int nStartChar = edit.LineIndex(nFirstVisibleLine);
  int nLastVisibleLine = nFirstVisibleLine + GetVisibleLineCount();
  int nEndChar = edit.LineIndex(nLastVisibleLine + 1);
  
  if(nEndChar == -1) // Se siamo alla fine del documento
    nEndChar = edit.GetTextLength();*/

// 3. Calcolo dell'intervallo di caratteri VISIBILI
	int nStartChar = edit.LineIndex(nFirstVisibleLine);
	if (nStartChar == -1) nStartChar = 0;

	// Prendiamo qualche riga in più di margine (buffer di sicurezza)
	int nLastVisibleLine = nFirstVisibleLine + GetVisibleLineCount() + 2; 
	int nTotalLines = edit.GetLineCount();

	if(nLastVisibleLine >= nTotalLines)
			nLastVisibleLine = nTotalLines - 1;

	int nLastLineStart = edit.LineIndex(nLastVisibleLine);
	int nEndChar = edit.GetTextLength();

	if(nLastLineStart != -1) {
    // L'ultimo carattere è l'inizio dell'ultima riga visibile + la sua lunghezza
  nEndChar = nLastLineStart + edit.LineLength(nLastLineStart);
	}

  // 4. Estrazione del buffer di testo visibile
  CString strText;
  
  // Legge soltanto la porzione visibile per il parsing
// 4. Estrazione del buffer di testo visibile
	GetTextRange(nStartChar, nEndChar, strText);
//  edit.GetTextRange(nStartChar, nEndChar, strText.GetBuffer(nEndChar - nStartChar + 1)); non c'è
//  strText.ReleaseBuffer();

  // 5. Reset del colore base del blocco visibile (es. Nero per testo normale)
  CHARFORMAT2 cfDefault;
  ZeroMemory(&cfDefault, sizeof(cfDefault));
  cfDefault.cbSize = sizeof(cfDefault);
  cfDefault.dwMask = CFM_COLOR;
  cfDefault.crTextColor = RGB(0, 0, 0); // Colore default
  
  edit.SetSel(nStartChar, nEndChar);
  edit.SetSelectionCharFormat(cfDefault);

  // 6. Esecuzione del Parser Lexer (C/C++)
  ParseAndApplyHighlighting(strText, nStartChar);

  // 7. Ripristino della selezione iniziale e dello Scroll
  edit.SetSel(crOriginal);
  edit.LineScroll(nFirstVisibleLine - edit.GetFirstVisibleLine());

  // 8. Sblocco e Redraw finale in un unico frame
  edit.SetRedraw(TRUE);
  edit.Invalidate();

  // Ripristina la maschera eventi
  edit.SetEventMask(dwOldEventMask);

	// Ripristiniamo il flag originale! 
  // Se il file era pulito, TORNA pulito senza asterisco '*'
  pDoc->SetModifiedFlag(bOriginallyModified);

	}

void COpenCView::ParseAndApplyHighlighting(const CString& strText, int nGlobalOffset) {
  CRichEditCtrl& edit = GetRichEditCtrl();

  // Tabella Parole Chiave C/C++
  static const LPCTSTR szKeywords[] = {
    _T("if"), _T("else"), _T("for"), _T("while"), _T("return"), _T("void"),
    _T("int"), _T("char"), _T("float"), _T("double"), _T("struct"), _T("class"),
    _T("const"), _T("static"), _T("switch"), _T("case"), _T("typedef"), 
		_T("defined"), // andrebbe viola pure questa??
		NULL
    };

  // Stili per i Token
  CHARFORMAT2 cfKeyword = CreateColorFormat(RGB(0, 0, 255));      // Blu
  CHARFORMAT2 cfComment = CreateColorFormat(RGB(0, 128, 0));     // Verde
  CHARFORMAT2 cfString  = CreateColorFormat(RGB(163, 21, 21));   // Rosso/Marrone
  CHARFORMAT2 cfPrep    = CreateColorFormat(RGB(128, 0, 128));   // Viola (#include, #define)

  int nLen = strText.GetLength();
  int i = 0;

  while(i < nLen)   {
    // A. Gestione Commenti //
    if(strText[i] == _T('/') && i + 1 < nLen && strText[i + 1] == _T('/')) {
      int nStart = i;
      while(i < nLen && strText[i] != _T('\n') && strText[i] != _T('\r'))
        i++;

      ApplyStyleToRange(nGlobalOffset + nStart, nGlobalOffset + i, cfComment);
      continue;
			}

    // B. Gestione Stringhe "..."
    if(strText[i] == _T('"')) {
      int nStart = i++;
      while(i < nLen && strText[i] != _T('"') && strText[i] != _T('\n')) {
        if(strText[i] == _T('\\') && i + 1 < nLen) 
					i++; // Escape \"
        i++;
        }
      if(i < nLen && strText[i] == _T('"')) 
				i++;

      ApplyStyleToRange(nGlobalOffset + nStart, nGlobalOffset + i, cfString);
      continue;
	    }

    // C. Preprocessore #include / #define
    if(strText[i] == _T('#')) {
      int nStart = i;
      while(i < nLen && (_istalnum(strText[i]) || strText[i] == _T('#')))
        i++;

      ApplyStyleToRange(nGlobalOffset + nStart, nGlobalOffset + i, cfPrep);
      continue;
			}

    // D. Parole Chiave (Keywords) e Identificatori
    if(_istalpha(strText[i]) || strText[i] == _T('_')) {
      int nStart = i;
	    while(i < nLen && (_istalnum(strText[i]) || strText[i] == _T('_')))
        i++;

      CString strWord = strText.Mid(nStart, i - nStart);
      if(IsKeyword(strWord, szKeywords))
        ApplyStyleToRange(nGlobalOffset + nStart, nGlobalOffset + i, cfKeyword);
      continue;
	    }

    i++;
    }
	}

CHARFORMAT2 COpenCView::CreateColorFormat(COLORREF color) {
  CHARFORMAT2 cf;

  ZeroMemory(&cf, sizeof(cf));
  cf.cbSize = sizeof(cf);
  cf.dwMask = CFM_COLOR;
  cf.crTextColor = color;
  return cf;
	}

void COpenCView::ApplyStyleToRange(int nStart, int nEnd, const CHARFORMAT2& cf) {

  GetRichEditCtrl().SetSel(nStart, nEnd);
  GetRichEditCtrl().SetSelectionCharFormat((CHARFORMAT2&)cf);
	}

int COpenCView::GetVisibleLineCount() {
  CRichEditCtrl& edit = GetRichEditCtrl();

  CRect rect;
  edit.GetClientRect(&rect);

  if(rect.Height() <= 0)
    return 0;

  // Prendiamo il primo carattere in alto a sinistra
  int nFirstChar = CharFromPos(CPoint(0, 0));
  // Prendiamo il carattere nell'angolo in basso a destra
  int nLastChar = CharFromPos(CPoint(rect.right - 1, rect.bottom - 1));

  if(nFirstChar < 0) 
		return 0;
  if(nLastChar < 0) 
		nLastChar = edit.GetTextLength();

  int nFirstLine = edit.LineFromChar(nFirstChar);
  int nLastLine = edit.LineFromChar(nLastChar);

  // Il numero di righe visibili è la differenza tra l'ultima e la prima riga + 1
  int nVisible = nLastLine - nFirstLine + 1;
  
  // Un piccolo margine di sicurezza (+1 riga) per non tagliare mai l'ultima riga parziale
  return (nVisible > 0) ? (nVisible + 1) : 1;
	}
/*int COpenCView::GetVisibleLineCount() {		// secondo gemini il font non è sempre valido... bah
  CRichEditCtrl& edit = GetRichEditCtrl();

  CRect rect;
  edit.GetClientRect(&rect);

  // Otteniamo l'altezza in pixel di una riga di testo usando le metriche dei font (TEXTMETRIC)
  CClientDC dc(&edit);
  CFont* pOldFont = dc.SelectObject(edit.GetFont());

  TEXTMETRIC tm;
  dc.GetTextMetrics(&tm);
  dc.SelectObject(pOldFont);

  int nLineHeight = tm.tmHeight + tm.tmExternalLeading;
  if(nLineHeight <= 0)
      nLineHeight = 16; // Valore di fallback di sicurezza

  return rect.Height() / nLineHeight;
	}*/

BOOL COpenCView::IsKeyword(const CString& strWord, const LPCTSTR szKeywords[]) {

  for(int i=0; szKeywords[i] != NULL; i++) {
    // In C/C++ le parole chiave sono case-sensitive (es. "if", non "IF")
    if(!strWord.Compare(szKeywords[i]))
      return TRUE;
    }
  return FALSE;
	}

void COpenCView::GetTextRange(int nStart, int nEnd, CString& strText) {

  if(nEnd <= nStart) {
    strText.Empty();
    return;
		}

  int nLen = nEnd - nStart;
  
  // Allocazione del buffer temporaneo (+1 per il carattere di fine stringa \0)
  wchar_t *pBuffer = new wchar_t[nLen + 1];
  ZeroMemory(pBuffer, (nLen + 1) * sizeof(TCHAR));

  TEXTRANGEW tr;
  tr.chrg.cpMin = nStart;
  tr.chrg.cpMax = nEnd;
  tr.lpstrText  = pBuffer;

  // Invia il messaggio nativo Win32 al controllo RichEdit
  GetRichEditCtrl().SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr);
	CStringEx Sw(pBuffer);		//test
	strText=Sw.GetASCII();

  strText = pBuffer;
  delete[] pBuffer;
	}

// ------------------------------------------------------------------
// GESTORE DEL MESSAGGIO INVIATO DALLA DIALOG (Trova Successivo / Chiusura)
// ------------------------------------------------------------------
LRESULT COpenCView::OnFindReplaceCmd(WPARAM wParam, LPARAM lParam) {

  CFindReplaceDialog* pDlg = CFindReplaceDialog::GetNotifier(lParam);

  if(!pDlg)
      return 0;

  // Se l'utente ha chiuso la finestra
  if(pDlg->IsTerminating())    {
      m_pFindDlg = NULL;
      return 0;
		}

  // Se l'utente ha premuto "Trova Successivo"
  if(pDlg->FindNext())    {
    m_strLastSearch = pDlg->GetFindString();
    m_bMatchCase = pDlg->MatchCase();
    m_bWholeWord = pDlg->MatchWholeWord();
    BOOL bDown = pDlg->SearchDown();

    DoSearchText(m_strLastSearch, bDown, m_bMatchCase, m_bWholeWord);
		}

  return 0;
	}

// ------------------------------------------------------------------
// COMANDO "TROVA" PERSONALIZZATO (Ctrl+F o da Menu)
// ------------------------------------------------------------------
void COpenCView::OnEditFind() {

  // Se la dialog è già aperta, portala in primo piano
  if(m_pFindDlg) {
    m_pFindDlg->SetActiveWindow();
    return;
    }

  // Se c'è del testo selezionato, usalo come testo predefinito nella Dialog
  CString strInitText = GetRichEditCtrl().GetSelText();
  strInitText.TrimLeft();
  strInitText.TrimRight();
  if (!strInitText.IsEmpty())    {
    m_strLastSearch = strInitText;
	  }

  // Crea e mostra la finestra di dialogo modello di ricerca
  m_pFindDlg = new CFindReplaceDialog();
  m_pFindDlg->Create(TRUE, m_strLastSearch, NULL, FR_DOWN, this);
	}


void COpenCView::OnEditRepeat() {

// Se non è mai stata fatta una ricerca e m_strLastSearch è vuota, 
  // proviamo prima a prendere il testo eventualmente selezionato
  if (m_strLastSearch.IsEmpty())    {
    m_strLastSearch = GetRichTextSelection();
    m_strLastSearch.TrimLeft();
    m_strLastSearch.TrimRight();
		}

  // Se abbiamo una stringa di ricerca valida, cerchiamo l'occorrenza successiva
  if (!m_strLastSearch.IsEmpty())    {
    DoSearchText(m_strLastSearch, TRUE /* Down */, m_bMatchCase, m_bWholeWord);
		}
  else {
    // Nessun testo da cercare disponibile: apri la dialog o avvisa
    OnEditFind();
    }	
	}

void COpenCView::OnUpdateEditRepeat(CCmdUI* pCmdUI) {
	// TODO: Add your command update UI handler code here
	
	}

void COpenCView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  BOOL bCtrlPressed  = (::GetKeyState(VK_CONTROL) < 0);
  BOOL bShiftPressed = (::GetKeyState(VK_SHIFT) < 0);

  if(bCtrlPressed && (nChar == VK_RIGHT || nChar == VK_LEFT)) {
    CRichEditCtrl& ctrl = GetRichEditCtrl();
    
    CHARRANGE cr;
    ctrl.GetSel(cr);

    long nPos = (nChar == VK_RIGHT) ? cr.cpMax : cr.cpMin;
    long nLen = ctrl.GetTextLength();

    if(nLen == 0) 
			return;

    // Blocco di lettura (128 caratteri prima e dopo)
    long nStartBuf = (nPos > 128) ? (nPos - 128) : 0;
    long nEndBuf   = (nPos + 128 < nLen) ? (nPos + 128) : nLen;
    long nBufSize  = nEndBuf - nStartBuf;

    // Allocazione WCHAR esplicita per evitare corruzione della memoria con RichEdit 5.0
    WCHAR* pBuf = new WCHAR[nBufSize + 1];
    ::ZeroMemory(pBuf, sizeof(WCHAR) * (nBufSize + 1));

    TEXTRANGEW tr;
    tr.chrg.cpMin = nStartBuf;
    tr.chrg.cpMax = nEndBuf;
    tr.lpstrText  = pBuf;

    // Usiamo il messaggio nativo Unicode per non sforare nei buffer
    ctrl.SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&tr);

    long nRelPos = nPos - nStartBuf;

    if(nChar == VK_RIGHT)        {
      // 1. Consuma prima tutti i caratteri della parola/identificatore su cui ci troviamo
      while(nRelPos < nBufSize && (iswalnum(pBuf[nRelPos]) || pBuf[nRelPos] == L'_'))
        nRelPos++;

      // 2. Consuma gli spazi o delimitatori successivi per fermarsi ALL'INIZIO della parola dopo
      while (nRelPos < nBufSize && !(iswalnum(pBuf[nRelPos]) || pBuf[nRelPos] == L'_'))
        nRelPos++;
	    }
    else // VK_LEFT
    {
        // Retrocedi se siamo su uno spazio/delimitatore
      while(nRelPos > 0 && !(iswalnum(pBuf[nRelPos - 1]) || pBuf[nRelPos - 1] == L'_'))
        nRelPos--;

        // Retrocedi finché trova caratteri alfanumerici OPPURE '_'
      while(nRelPos > 0 && (iswalnum(pBuf[nRelPos - 1]) || pBuf[nRelPos - 1] == L'_'))
        nRelPos--;
      }

    long nNewPos = nStartBuf + nRelPos;

    delete[] pBuf; // Deallocazione sicura

    // Gestione selezione (Ctrl+Shift+Freccia) o semplice movimento del cursore
    if (bShiftPressed)
      ctrl.SetSel(cr.cpMin, nNewPos);
    else
      ctrl.SetSel(nNewPos, nNewPos);

    return; // Intercetta l'evento ed evita la gestione di default di RichEdit
	  }
// 2. Tasti che causano scorrimento o cambio di riga
  else if(nChar == VK_UP || nChar == VK_DOWN || 
    nChar == VK_PRIOR || nChar == VK_NEXT || // PageUp / PageDown
    nChar == VK_HOME || nChar == VK_END) {
		HighlightVisibleRange(); // Ricoloriamo il blocco visibile appena cambia lo scroll
    }

  // Per tutti gli altri tasti, lascia la gestione standard
  CRichEditView::OnKeyDown(nChar, nRepCnt, nFlags);
	}


void COpenCView::OnSize(UINT nType, int cx, int cy) {

  // Lasciamo che la View ridimensioni il RichEdit normalmente
  CRichEditView::OnSize(nType, cx, cy);
	}

// Quando lo schermo scorre o il testo cambia, ridisegniamo la gutter
void COpenCView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {

	CSplitterWnd* pSplitter = (CSplitterWnd*)GetParent();

	CWnd* pPaneWnd = pSplitter->GetPane(0,0);
  CGutterWnd *w = DYNAMIC_DOWNCAST(CGutterWnd, pPaneWnd);
  w->Invalidate(); // Richiede un nuovo WM_PAINT per aggiornare la gutter
	pPaneWnd = pSplitter->GetPane(1,0);
  w = DYNAMIC_DOWNCAST(CGutterWnd, pPaneWnd);
  w->Invalidate(); // Richiede un nuovo WM_PAINT per aggiornare la gutter

  CRichEditView::OnVScroll(nSBCode, nPos, pScrollBar);
	HighlightVisibleRange(); // Ricoloriamo il blocco visibile appena cambia lo scroll
	}

void COpenCView::OnEnVScroll() {

  Invalidate(); // Richiede un nuovo WM_PAINT per aggiornare la gutter
	}	

void COpenCView::OnContextMenu(CWnd* pWnd, CPoint point){

  // Se scatenato da tastiera (Shift+F10 / tasto menu), usiamo la posizione del caret
  if (point.x == -1 && point.y == -1)    {
    POINT pt;
    ::GetCaretPos(&pt);
    ClientToScreen(&pt);
    point = pt;
		}

  // Convertiamo le coordinate di schermo in coordinate client per il RichEditCtrl
  CPoint ptClient = point;
  GetRichEditCtrl().ScreenToClient(&ptClient);

  // Estragg il nome del file / parola sotto il punto cliccato
  CString strWord = GetWordAtPoint(ptClient);

  CMenu menu;
  if (menu.CreatePopupMenu())    {
    // 1. Comandi di modifica standard
    CHARRANGE cr;
    GetRichEditCtrl().GetSel(cr);
    BOOL bHasSelection = (cr.cpMin != cr.cpMax);

    menu.AppendMenu(MF_STRING | (GetRichEditCtrl().CanUndo() ? MF_ENABLED : MF_GRAYED), ID_EDIT_UNDO, _T("&Annulla"));
    menu.AppendMenu(MF_SEPARATOR);
    menu.AppendMenu(MF_STRING | (bHasSelection ? MF_ENABLED : MF_GRAYED), ID_EDIT_CUT, _T("Ta&glia"));
    menu.AppendMenu(MF_STRING | (bHasSelection ? MF_ENABLED : MF_GRAYED), ID_EDIT_COPY, _T("&Copia"));
    menu.AppendMenu(MF_STRING | (GetRichEditCtrl().CanPaste() ? MF_ENABLED : MF_GRAYED), ID_EDIT_PASTE, _T("&Incolla"));

    // 2. Opzione custom per l'include
    if (!strWord.IsEmpty())        {
        if (strWord.Right(2).CompareNoCase(_T(".h")) == 0 || 
            strWord.Right(4).CompareNoCase(_T(".hpp")) == 0 ||
            strWord.Right(2).CompareNoCase(_T(".c")) == 0 ||
            strWord.Right(4).CompareNoCase(_T(".cpp")) == 0)
        {
            menu.AppendMenu(MF_SEPARATOR);
            CString strLabel;
            strLabel.Format(_T("Apri '%s'"), (LPCTSTR)strWord);
						m_strSelectedInclude=strWord;
            
            // ID_OPEN_INCLUDE_FILE da definire in resource.h
            menu.AppendMenu(MF_STRING, ID_OPEN_INCLUDE_FILE, strLabel);
        }
			}

    // Mostra il menu contestuale
    menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    }
	}

CString COpenCView::GetWordAtPoint(CPoint ptClient) {
  CRichEditCtrl& ctrl = GetRichEditCtrl();

  int nCharIndex = CharFromPos(ptClient);
  if (nCharIndex < 0)
      return _T("");

  int nLineIndex = ctrl.LineFromChar(nCharIndex);
  int nLineStart = ctrl.LineIndex(nLineIndex);
  int nLineLen = ctrl.LineLength(nCharIndex);

  if (nLineLen <= 0)
      return _T("");

  CString strLine;
  ctrl.GetLine(nLineIndex, strLine.GetBuffer(nLineLen + 1), nLineLen);
  strLine.ReleaseBuffer(nLineLen);

  int nPosInLine = nCharIndex - nLineStart;
  if (nPosInLine < 0 || nPosInLine >= strLine.GetLength())
      return _T("");

  // --- Espansione del controllo senza la lambda ---
  int nStart = nPosInLine;
  while (nStart > 0)  {
      TCHAR c = strLine[nStart - 1];
      if (_istalnum(c) || c == _T('.') || c == _T('_') || c == _T('\\') || c == _T('/'))
          nStart--;
      else
          break;
		}

  int nEnd = nPosInLine;
  while (nEnd < strLine.GetLength())  {
      TCHAR c = strLine[nEnd];
      if (_istalnum(c) || c == _T('.') || c == _T('_') || c == _T('\\') || c == _T('/'))
          nEnd++;
      else
          break;
		}

  return strLine.Mid(nStart, nEnd - nStart);
	}

void COpenCView::OnOpenIncludeFile() {
  COpenCDoc* pDoc = GetDocument();
  ASSERT_VALID(pDoc);

  if(pDoc && !m_strSelectedInclude.IsEmpty()) {
    pDoc->OpenIncludeFile(m_strSelectedInclude);
    }
	}

LRESULT COpenCView::OnFileChangedExternally(WPARAM wParam, LPARAM lParam) {
  COpenCDoc* pDoc = GetDocument();
  if(!pDoc)
		return 0;

	if(((::GetTickCount() - pDoc->m_dwLastSelfSaveTime) > 1000) /*!pDoc->m_bIsSavingSelf*/) {
		CString strMsg;
		strMsg.Format(_T("Il file '%s' è stato modificato all'esterno.\nRicaricarlo?"), pDoc->GetTitle());

		if (AfxMessageBox(strMsg, MB_YESNO | MB_ICONQUESTION) == IDYES) {
					// Ora siamo nel thread GUI nativo, OnOpenDocument è sicuro al 100%!
			pDoc->OnOpenDocument(pDoc->GetPathName());
			}
		}

  return 0;
	}


#if 0 // uso manager globale, v. OpenCApp
struct SMonThreadParams {
  HWND hWndDoc;          // Handle HWND della vista o del frame
  CString strPath;       // Copia del percorso del file
  HANDLE hNotify;        // Handle di FindFirstChangeNotification
  HANDLE hStopEvent;     // Evento per dire al thread di chiudersi pulito
  FILETIME ftLastWrite;  // Data/ora conosciuta del file
	};

UINT FileMonTask(LPVOID param) {
  SMonThreadParams* pParams = (SMonThreadParams*)param;
  HANDLE hEvents[2] = { pParams->hNotify, pParams->hStopEvent };

  WIN32_FILE_ATTRIBUTE_DATA mywfd;

  while (TRUE)    {
    // Aspetta O una modifica del file, O il segnale di chiusura del thread
    DWORD dwWait = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);

    if (dwWait == WAIT_OBJECT_0) // Il file è cambiato!
    {
      // Verifichiamo la data (usando l'API nativa direttamente)
      if (GetFileAttributesEx(pParams->strPath, GetFileExInfoStandard, &mywfd))            {
        if (CompareFileTime(&mywfd.ftLastWriteTime, &pParams->ftLastWrite) > 0)                {
          // Aggiorniamo la data interna
          pParams->ftLastWrite = mywfd.ftLastWriteTime;

          // NON usiamo AfxMessageBox né OnOpenDocument qui!
          // Mandiamo un messaggio asincrono al thread GUI principale
          ::PostMessage(pParams->hWndDoc, WM_MY_FILE_CHANGED, 0, 0);
          }
        }

      FindNextChangeNotification(pParams->hNotify);
	    }
    else        {
      // Chiesto lo stop del thread o errore -> usciamo dal ciclo puliti
      break;
      }
    }

  delete pParams;
  return 0;
	}
#endif


/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CGutterWnd, CWnd)

BEGIN_MESSAGE_MAP(CGutterWnd, CWnd)
  ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CGutterWnd::CGutterWnd() {}
CGutterWnd::~CGutterWnd() { TRACE(_T("CGutterWnd distrutto!\n")); }

BOOL CGutterWnd::OnEraseBkgnd(CDC* pDC) {
  return TRUE; // Evita lo sfarfallio (flicker)
	}

void CGutterWnd::OnPaint() {
  CPaintDC dc(this);

	CSplitterWnd* pSplitter = (CSplitterWnd*)GetParent();

	CWnd* pPaneWnd = pSplitter->GetPane(0, 1);
  COpenCView *w = DYNAMIC_DOWNCAST(COpenCView, pPaneWnd);

/*	if(pPaneWnd) {

		w=(COpenCView*)((CMainFrame*)GetParent()->GetParent())->GetActiveView();*/
  if(!w) 
		return;

  CRect clientRect;
  GetClientRect(&clientRect);

  // Sfondo della gutter (grigio chiaro da editor)
  dc.FillSolidRect(&clientRect, RGB(240, 240, 240));

  // Linea divisoria a destra
  //dc.FillSolidRect(clientRect.right - 1, clientRect.top, 1, clientRect.Height(), RGB(210, 210, 210));

  CRichEditCtrl& ctrl = w->GetRichEditCtrl();
  COpenCDoc* pDoc = w->GetDocument();
  if(!pDoc) 
		return;

  int firstLine = ctrl.GetFirstVisibleLine();
  int lineCount = ctrl.GetLineCount();

  for(int i = firstLine; i < lineCount; ++i)    {
    int charIndex = ctrl.LineIndex(i);
    CPoint pt = ctrl.GetCharPos(charIndex);

    // Se la riga esce dalla parte inferiore della finestra, interrompiamo
    if(pt.y > clientRect.bottom)
      break;

    // Disegna il segnalibro se la riga è presente in CUIntArray
    if(pDoc->HasBookmark(i+1)) {				// il contatore parte da 0, i bookmark e i brk da 1
      CBrush brush(RGB(0, 215, 120)); // verde Windows
      CBrush* pOldBrush = dc.SelectObject(&brush);
      CPen pen(PS_SOLID, 1, RGB(0, 180, 90));
      CPen* pOldPen = dc.SelectObject(&pen);

			dc.RoundRect(2, pt.y + 2, 20, pt.y + 14,6,10);

      dc.SelectObject(pOldBrush);
      dc.SelectObject(pOldPen);
      }

    if(pDoc->HasBreakpoint(i+1))        {
      CBrush brush(RGB(215, 20, 0)); // rosso Windows
      CBrush* pOldBrush = dc.SelectObject(&brush);
      CPen pen(PS_SOLID, 1, RGB(180, 10, 0));
      CPen* pOldPen = dc.SelectObject(&pen);

      // Centra un cerchietto da 12px di diametro
      dc.Ellipse(5, pt.y + 2, 17, pt.y + 14);

      dc.SelectObject(pOldBrush);
      dc.SelectObject(pOldPen);
      }
    }
	}

BOOL CGutterWnd::OnSetCursor() {

	SetCursor(LoadCursor(theApp.m_hInstance,MAKEINTRESOURCE(IDC_RIGHT_CURSOR)));

	return TRUE;

	}

void CGutterWnd::OnLButtonDblClk(UINT nFlags, CPoint point) {
  CRect rc;
	int i;

  GetClientRect(&rc);

	CSplitterWnd* pSplitter = (CSplitterWnd*)GetParent();

	CWnd* pPaneWnd = pSplitter->GetPane(0, 1);
  COpenCView *w = DYNAMIC_DOWNCAST(COpenCView, pPaneWnd);

  if(!w) 
		return;

  CRichEditCtrl& ctrl = w->GetRichEditCtrl();
  COpenCDoc* pDoc = w->GetDocument();
  if(!pDoc) 
		return;

	i=(point.y-rc.top)/13;		// cmq 0! e usare RichEditCtrl fontsize
	pDoc->ToggleBreakpoint(i+1);
	Invalidate();
	}

void CGutterWnd::OnLButtonDown(UINT nFlags, CPoint point) {
// Supponiamo che 'nLine' sia l'indice della riga zero-based da selezionare
// (es. ricavata da LineFromChar / Y coordinate)
	CSplitterWnd* pSplitter = (CSplitterWnd*)GetParent();

	CWnd* pPaneWnd = pSplitter->GetPane(0, 1);
  COpenCView *w = DYNAMIC_DOWNCAST(COpenCView, pPaneWnd);

  if(!w) 
		return;

  CRichEditCtrl& ctrl = w->GetRichEditCtrl();

  // Convertiamo la coordinata Y del mouse nel numero di riga
  int nCharIndex = w->CharFromPos(point);
  int nLine = ctrl.LineFromChar(nCharIndex);
  
  int nStartChar = ctrl .LineIndex(nLine);
  if (nStartChar != -1) {
    int nLineLength = ctrl .LineLength(nStartChar);
    
    // Selezioniamo la riga
    ctrl .SetSel(nStartChar, nStartChar + nLineLength);
		}
  
  CWnd::OnLButtonDown(nFlags, point);
	}

void CGutterWnd::PostNcDestroy() {		// serve perché non è una CView ma è usta dentro lo splitter!
  // Chiama prima la classe base
  CWnd::PostNcDestroy(); 
  
  // Forziamo la liberazione della memoria dell'oggetto C++
  delete this; 
	}


