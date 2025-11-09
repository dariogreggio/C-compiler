// OpenCEditView.cpp : implementation of the COpenCEditView class
//

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

BEGIN_MESSAGE_MAP(COpenCView, CRichEditView)
	//{{AFX_MSG_MAP(COpenCView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_CHAR()
	ON_COMMAND(ID_EDIT_TROVASELEZIONE, OnEditTrovaselezione)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TROVASELEZIONE, OnUpdateEditTrovaselezione)
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRichEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenCView construction/destruction

COpenCView::COpenCView() {

	m_bDelayUpdateItems = FALSE;
	
	}

COpenCView::~COpenCView() {
	}

BOOL COpenCView::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs


	return CRichEditView::PreCreateWindow(cs);
	}

/////////////////////////////////////////////////////////////////////////////
// COpenCEditView drawing

void COpenCView::OnDraw(CDC* pDC) {
	COpenCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

//	SetPrinterFont(&(((CMainFrame *)GetParent())->myFont));
	// TODO: add draw code for native data here
	CRichEditView::OnDraw(pDC);
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

	return 0;
	}

void COpenCView::OnDestroy() {
	COpenCDoc *d=GetDocument();
	RECT rc;
	char myBuf[64];

	CRichEditView::OnDestroy();
	
	GetParent()->GetWindowRect(&rc);	// coord. schermo della mia MDIChildFrmae...
	GetParent()->GetParent()->GetParent()->ScreenToClient(&rc);	// ... diventano coord. client rispetto al MDI padre (che NON e' theApp.m_pMainWnd !! ce n'è una un mezzo...
		// DUE GetParent perche' c'e' Splitter!!
	int correz=6;
	wsprintf(myBuf,"%d,%d,%d,%d",rc.left-correz,rc.top-correz,rc.right+correz,rc.bottom/*+correz*/);

	d->WritePrivateProfileString(IDS_COORDINATE,myBuf);
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
	RECT rc2;

	GetParent()->GetParent()->GetWindowRect(rc);
	theApp.m_pMainWnd->GetWindowRect(&rc2);
	rc2.top+=GetSystemMetrics(SM_CYMENU)+GetSystemMetrics(SM_CYSIZE)+GetSystemMetrics(SM_CYCAPTION)+
		5*GetSystemMetrics(SM_CYBORDER)+2*GetSystemMetrics(SM_CYFRAME);
	rc2.left+=2*GetSystemMetrics(SM_CXBORDER)+GetSystemMetrics(SM_CXFRAME);
	rc->top-=rc2.top;
	rc->bottom-=rc2.top;
	rc->left-=rc2.left;
	rc->right-=rc2.left;

	return TRUE;
	}


