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
// https://www.codeproject.com/articles/A-Very-Simple-Way-to-Use-Richedit-5-0-in-VC6-and-o?msg=3389216#comments-section

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

	m_strClass=TEXT("RichEdit50W");
  //Must be m_strClass, it is a member of CCtrlView

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

	::RevokeDragDrop(m_hWnd);		// non va...  https://stackoverflow.com/questions/2476589/how-to-disable-dragging-from-a-rich-edit-control
	// per evitare drop di file nel testo

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

/////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////

// Default constructor
CRichEditCtrlEx::CRichEditCtrlEx() {
	// Common values
	UINT	uiSize			= sizeof(CHARFORMAT);
	DWORD	dwMask			= CFM_COLOR | CFM_FACE | CFM_SIZE; /* | CFM_CHARSET;*/
	LONG	lHeight			= 160;	// 8 point => 160 * (1/20)

	// Initialise the Tags CHARFORMAT
	m_cfTags.cbSize			= uiSize;
	m_cfTags.dwMask			= dwMask;
	m_cfTags.dwEffects		= 0;
	m_cfTags.yHeight		= lHeight;
	m_cfTags.crTextColor	= RGB(128, 0, 0);
	m_cfTags.bCharSet		= ANSI_CHARSET;
	_tcscpy(m_cfTags.szFaceName,	_T("Courier New"));

	// Initialise the Text CHARFORMAT
	m_cfText.cbSize			= uiSize;
	m_cfText.dwMask			= dwMask;
	m_cfText.dwEffects		= 0;
	m_cfText.yHeight		= lHeight;
	m_cfText.crTextColor	= RGB(0, 0, 0);
	m_cfText.bCharSet		= ANSI_CHARSET;
	_tcscpy(m_cfText.szFaceName,	_T("Courier New"));

	// Initialise the Quoted Text CHARFORMAT
	m_cfQuoted.cbSize		= uiSize;
	m_cfQuoted.dwMask		= dwMask;
	m_cfQuoted.dwEffects	= 0;
	m_cfQuoted.yHeight		= lHeight;
	m_cfQuoted.crTextColor	= RGB(0, 128, 128);
	m_cfQuoted.bCharSet		= ANSI_CHARSET;
	_tcscpy(m_cfQuoted.szFaceName, _T("Courier New"));

	// Initialise the Comment CHARFORMAT
	m_cfComment.cbSize		= uiSize;
	m_cfComment.dwMask		= dwMask;
	m_cfComment.dwEffects	= 0;
	m_cfComment.yHeight		= lHeight;
	m_cfComment.crTextColor	= RGB(0, 128, 0);
	m_cfComment.bCharSet		= ANSI_CHARSET;
	_tcscpy(m_cfComment.szFaceName, _T("Courier New"));

	// For tracking the color state at the end of every line: 
	m_nLineCount = 0;
	m_pLinesEndState = NULL;

	m_bOnEnVscrollDisabled = false;	// Disable OnEnVscroll coloring during ColorVisibleLines and OnKeyDown
	m_nOnChangeCharPosition = -1;	// OnKeyDown defers coloring to OnChange when overriding selected text

	// Background coloring timer:
	m_uiBckgdTimerInterval = 100;
	m_nBckgdTimerNumOfLines = 10;
	m_bBckgdTimerActivated = false;
	}

// Default destructor
CRichEditCtrlEx::~CRichEditCtrlEx() {

	// Free the allocated vector:
	if(m_pLinesEndState)	{
		free(m_pLinesEndState);	
		m_pLinesEndState = NULL;
		}
	}

/////////////////////////////////////////////////////////////////////////////
// Character format functions
/////////////////////////////////////////////////////////////////////////////

// Sets the character format to be used for Tags
void CRichEditCtrlEx::SetTagCharFormat(int nFontHeight, COLORREF clrFontColour, 
											CString strFontFace, bool bParse) {

	m_cfTags.yHeight		= 20 * nFontHeight;
	m_cfTags.crTextColor	= clrFontColour;
	_tcscpy(m_cfTags.szFaceName, strFontFace);

	if(bParse)	
		ParseAllLines();
	}

// Sets  the character format to be used for Tags
void CRichEditCtrlEx::SetTagCharFormat(CHARFORMAT& cfTags, bool bParse) {

	m_cfTags = cfTags;
	if(bParse)	
		ParseAllLines();
	}

// Sets the character format to be used for Quoted text
void CRichEditCtrlEx::SetQuoteCharFormat(int nFontHeight,COLORREF clrFontColour, 
											  CString strFontFace, bool bParse) {

	m_cfQuoted.yHeight		= 20 * nFontHeight;
	m_cfQuoted.crTextColor	= clrFontColour;
	
	_tcscpy(m_cfQuoted.szFaceName, strFontFace);

	if(bParse)	
		ParseAllLines();
	}

// Sets  the character format to be used for Quoted text
void CRichEditCtrlEx::SetQuoteCharFormat(CHARFORMAT& cfQuoted, bool bParse) {

	m_cfQuoted = cfQuoted;
	if(bParse)	
		ParseAllLines();
	}

// Sets the character format to be used for Comments
void CRichEditCtrlEx::SetCommentCharFormat(int nFontHeight,COLORREF clrFontColour, 
												CString strFontFace, bool bParse) {

	m_cfComment.yHeight		= 20 * nFontHeight;
	m_cfComment.crTextColor	= clrFontColour;

	_tcscpy(m_cfComment.szFaceName, strFontFace);

	if(bParse)	
		ParseAllLines();
	}

// Sets  the character format to be used for Comments
void CRichEditCtrlEx::SetCommentCharFormat(CHARFORMAT& cfComments, bool bParse) {
	m_cfComment = cfComments;

	if(bParse)	
		ParseAllLines();
	}

// Sets the character format to be used for Normal Text
void CRichEditCtrlEx::SetTextCharFormat(int nFontHeight, COLORREF clrFontColour, 
											 CString strFontFace, bool bParse) {

	m_cfText.yHeight		= 20 * nFontHeight;
	m_cfText.crTextColor	= clrFontColour;

	_tcscpy(m_cfText.szFaceName, strFontFace);

	if(bParse)	
		ParseAllLines();
	}

// Sets  the character format to be used for Normal Text
void CRichEditCtrlEx::SetTextCharFormat(CHARFORMAT& cfText, bool bParse) {

	m_cfText = cfText;
	if(bParse)	
		ParseAllLines();
	}

/////////////////////////////////////////////////////////////////////////////
// Parsing functions
/////////////////////////////////////////////////////////////////////////////

// Parses all lines in the control, coloring each line accordingly.
void CRichEditCtrlEx::ParseAllLines() {
	// Get control's text (send WM_GETTEXT  message):
	CString strCtrlText;
	GetWindowText(strCtrlText);
	
	// Allocate the vector for holding the color states of all lines: 
	m_nLineCount = GetLineCount();
	free(m_pLinesEndState);
	m_pLinesEndState = (BYTE*)malloc(m_nLineCount * sizeof(BYTE));	ASSERT(m_pLinesEndState != NULL);
	ZeroMemory(m_pLinesEndState, m_nLineCount * sizeof(BYTE));
	
	// Go over HTML line by line and calculate the color state (Comment/Quoted/Tag/Normal)
	// of the last char.
	// NOTE: This calulation is quite fast as we do it in one pass over HTML without any coloring.
	const TCHAR* pCtrlText = (LPCTSTR)strCtrlText;
	ParseLines(pCtrlText, -1 , false, 0);
	
	// Disable redraw to prevent flickering
	SetRedraw(FALSE);

	// Store the current selection and the first visible line
	CHARRANGE crCurrent;
	GetSel(crCurrent);

	// Color the all the text as Text initially
	SetSel(0, -1);
	SetWordCharFormat(m_cfText);

	// Get the control's visible range
	int nFirstLine = GetFirstVisibleLine();	// Send the control EM_GETFIRSTVISIBLELINE message
	int nLastLine = GetLastVisibleLine();	// No such message as EM_GETLASTVISIBLELINE - use our own algorithm
	for(int i = nFirstLine; i <= nLastLine; i++) {
		CString strLine;
		int nLineLength = GetLineHelper(i, strLine);

		if(nLineLength > 0)	{
			ParseLines((LPCTSTR)strLine, -1, true, i);	// Color the line (only one line)
			m_pLinesEndState[i] |= LINE_COLORED;
			}
		}

	// Restore the original selection
	SetSel(crCurrent);

	// Restore the original view position
	LineScroll(-nFirstLine, 0);

	SetRedraw(TRUE);
	Invalidate(FALSE);

	// Activate the background coloring timer
	StartColoringTimer();
	}

/////////////////////////////////////////////////////////////////////////////
// Miscellaneous functions
/////////////////////////////////////////////////////////////////////////////

// Loads the contents of the specified file into the control.
// Replaces the existing contents. To parse lines ParseAllLines must be called
void CRichEditCtrlEx::LoadFile(CString& strPath) {

	if(strPath.GetLength() > 0)	{
		CFile file(strPath, CFile::modeRead);
		EDITSTREAM es;

		es.dwCookie = (DWORD)&file;
		es.pfnCallback = StreamInCallback;
		StreamIn(SF_TEXT, es);
		}
	}

void CRichEditCtrlEx::SetBckgdColorTimer(UINT uiInterval /*= 1000*/, int nNumOfLines /*= 10*/) {

	if(uiInterval == 0 || nNumOfLines <= 0 && m_bBckgdTimerActivated) {
		// Disable current background coloring timer:
		BOOL bRes = KillTimer(TIMER_BACKGROUNDCOLORING);	
		ASSERT(bRes);
		m_bBckgdTimerActivated = false;
		}
	
	m_uiBckgdTimerInterval = uiInterval;
	m_nBckgdTimerNumOfLines = nNumOfLines;
	}

/////////////////////////////////////////////////////////////////////////////
// Overrides
/////////////////////////////////////////////////////////////////////////////

// PreSubclassWindow override. Ensures that the control is registered to 
// receive ENM_CHANGE notifications.
// Also register to receive ENM_SCROLL notifications when a keyboard event causes a change 
// in the view area of the edit control, for example, pressing HOME, END, PAGE UP, PAGE DOWN, UP ARROW, or DOWN ARROW.
void CRichEditCtrlEx::PreSubclassWindow() {
	CRichEditCtrl::PreSubclassWindow();

	// Set the event mask to include ENM_CHANGE
	long lMask = GetEventMask();
	lMask |= ENM_CHANGE | ENM_SCROLL;
	SetEventMask(lMask);

	// Set the default character format to be m_cfText
	SetDefaultCharFormat(m_cfText);

	// The CRichEditCtrl always starts with LineCount == 1, so you can start typing text immediately
	// NOTE: When CRichEditCtrl used from resource, OnCreate isn't called and GetLineCount returns 1.
	// When used as a member and Create() is explicitly called, GetLineCount here return 0 and in OnCreate returns 1.
	m_nLineCount = 1;
	m_pLinesEndState = (BYTE*)malloc(sizeof(BYTE));	ASSERT(m_pLinesEndState != NULL);
	m_pLinesEndState[0] = epsInNormalText;	// Until changed by <, " and so, we're in state "text"
	}

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx message handlers
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CRichEditCtrlEx, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRichEditCtrlEx)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_CONTROL_REFLECT(EN_VSCROLL, OnEnVscroll)
	ON_WM_HSCROLL()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

/**
 * WM_CHAR handler traps "interesting chars" (chars that change the color state, for example ‘<’ or ‘”)
 * and updates the colors accordingly.
 * Sometimes we invalidate up to 3 preceding chars to take care of 
 * comments, as pressing the ‘-‘ char might complete a comment start combination. 
 * NOTE that OnChar is too late for handling VK_BACK and VK_DELETE,
 * as chars have already been deleted at this stage
 */
void CRichEditCtrlEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	TRACE(_T("OnChar\n"));

	// Get current caret position. Not sure if GetSel the best choice???
	// NOTE: We call GetSel before base class to get the correct caret position
	long lStart = 0, lEnd	= 0;
	GetSel(lStart, lEnd);
	int nCharPosition = lStart;

	CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);

	// After CRichEditCtrl has added the char, we can get the following:
	long lCharStart	= LineIndex();	// Retrieves the character index of the current line (the line that contains the caret)
	long lCharFromLineStart = nCharPosition - lCharStart;
	int nLineIndex = LineFromChar(lCharStart);
	
	// DEBUG:
	int nLineIndex1 = LineFromChar(-1);
	CString strLine;
	int nLineLength = GetLineHelper(nLineIndex, strLine);

	// Is it a special char? If not, basically the RichEditCtrl maintains himself the current WordCharFormat.
	// However this may change when pressing a key breaks a comment combination or after the insertion point
	// was moved just passed an ending tag or ending double quotes. This will be handled
	// by the 'default' clause
	switch(nChar)	{
	case KEY_TAG_START:
		{// Invalide the colors of this and next lines as we might move to epsInTag
			InvalidateColorStates(nCharPosition);
			break;
		}
	case KEY_TAG_END:
		{// Invalide the colors of this and next lines as we might move to InNormalText (or stay in DblQuotes\Comment)
		// Recalculate the color state of this line because after '>' we might move to InNormalText or stay in DblQuotes\Comment.
		 // However the color of this '>' stays the same as the previous char (Tag\DblQuotes\Comment\NormalText)	
			InvalidateColorStates(nCharPosition);
			break;
		}
	case KEY_DBL_QUOTE:
		{// Recalculate the color state of this line because after '"' we might move to DblQuotes\Tag or stay in Comment\NormalText.
		 // The color of this '" changes if me move to DblQuotes and affects the following lines as well, so invalide the colors of this and next lines
			InvalidateColorStates(nCharPosition);
			if((m_pLinesEndState[nLineIndex] &~ LINE_COLORED) == epsInTag)	// These were ending double quotes so now we're in Tag
				{
				BOOL bRes = SetWordCharFormat(m_cfTags);	ASSERT(bRes);
				}
			break;
		}
	case '-':
		{// Trap the Comment start ("<!--") and Comment end combinations, so we can color correctly in these case:
			// Check if we move to epsInComment:
			int nCommentStart = FindCommentStartHelper(nCharPosition);
			if(nCommentStart != -1)
				InvalidateColorStates(nCommentStart);
			else {
				// Check if we move to epsInNormalText
				CString strTmp;
				int nChars = GetTextRange(nCharPosition - 1, nCharPosition + 2, strTmp);	// Search for "-->" and this char is the middle '-'
				if(strTmp != _T("*/"))	{
					nChars = GetTextRange(nCharPosition, nCharPosition + 3, strTmp);
					}	// Search for "-->" and this char is the leftmost '-'
				if(strTmp == _T("*/"))	// Search for "-->" and this char is the leftmost '-'
					{
					InvalidateColorStates(nCharPosition);
					}
			}
			break;
		}
	case '!':	// Trap the Comment start ("<!--"), so we can color correctly when state moves to epsInComment:
		{
			int nCommentStart = FindCommentStartHelper(nCharPosition);
			if(nCommentStart != -1)	
				InvalidateColorStates(nCommentStart);
			break;
		}
	case VK_RETURN:
		{
			UpdateLinesArraySize();	// Since a new line has just been added

			// After VK_RETURN we must explicitly set the correct color:
			SetFirstLineCharColor(nLineIndex);
			break;
		}
	default:
		{
			if(nChar != VK_BACK)	// VK_BACK was already handled by OnKeyDown
			{// Pressing a key might break comment combination.
			 // Pressing a key after the insertion point might result in a wrong color when we're
	         // just passed an ending tag or ending double quotes.
				int nCommentStart = FindCommentStartHelper(nCharPosition);
				if(nCommentStart != -1)	// Just broke comment start combination
					InvalidateColorStates(nCommentStart);
				else {
					CString test;
					GetWindowText(test);
					
					int nCommentEnd = FindCommentEndHelper(nCharPosition);	// +1 to pass the currently added char 
					if(nCommentEnd != -1)	// Just broke comment end combination
						InvalidateColorStates(nCommentStart);
					else {
						CString prevChar;
						int nChars = GetTextRange(nCharPosition - 1 , nCharPosition, prevChar);
						if(prevChar == _T('>') || prevChar == _T('\"'))	// We just past ending tag/ending quotes)
							InvalidateColorStates(nCharPosition - 1);
						}
					}
				}
			}
		}
	
	return;
	}

// WM_GETDLGCODE message handler. Ensures that the edit control
// handles the TAB key.
UINT CRichEditCtrlEx::OnGetDlgCode() {
	return DLGC_WANTALLKEYS;
	}

/**
 * OnChange is used to handle the cases of overriding selected text.
 * In these cases OnChange is called after OnKeyDown, thus OnKeyDown can't
 * see the updated text and defers the coloring to OnChange.
 */
void CRichEditCtrlEx::OnChange() {
	TRACE(_T("OnChange\n"));

	if(m_nOnChangeCharPosition != -1) {
		// OnKeyDown defers the call to OnChange when overriding selected text
		UpdateLinesArraySize();
		InvalidateColorStates(m_nOnChangeCharPosition);
		m_nOnChangeCharPosition = -1;
		}
	}

// WM_CREATE message handler. Ensures that the control is registered to
// receive ENM_CHANGE messages.
// NOTE that no need for initialization here since alrady done in PreSubclassWindow
int CRichEditCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct) {

	if(CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Set the default character format to be m_cfText
	SetDefaultCharFormat(m_cfText);

	return 0;
	}

/**
 * This helper was found somewhere on the net
 */
BOOL CRichEditCtrlEx::IsWindowCompletelyObscured() {
	RECT clip, winrect; 

	// Get Clipping box for window area (client not enough since I want to include the scrolls) 
	HDC hdc = ::GetWindowDC(m_hWnd);
	int cliptype = GetClipBox(hdc, &clip);	// In logical units
	::ReleaseDC(m_hWnd, hdc);

	// Check clipbox type
	if(cliptype == NULLREGION)
		return TRUE;	// Completely covered
	else if(cliptype == COMPLEXREGION)
		return FALSE;	// Partially covered
	else if(cliptype == SIMPLEREGION)	{
		GetWindowRect(&winrect);
		// Normalize coordinates:
		winrect.bottom -= winrect.top;
		winrect.top = 0;
		winrect.right -= winrect.left;
		winrect.left = 0;
    if(EqualRect(&clip,&winrect))
			return FALSE;	//completely exposed 
		else
			return TRUE;	//completely covered
		}
	return FALSE;
	}

/**
 * WM_TIMER message handler.
 * This is the background coloring timer. It runs as long as there are uncolored lines
 * After investigating, found that in order to prevent flickers:
 * 1. RichEditCtrl must have the focus to prevent vertical scrollbar flickers (Otherwise the vertical scrollbar thumb jumps and flickers)
 * 2. Caret must be positioned at the beginning of line with no selection. Otherwise the horizontal scrollbar shakes! (works only for class RichEdit20W, not RICHEDIT)
 * NOTE that when control is completely obscured - there're no restrictions!
 */
void CRichEditCtrlEx::OnTimer(UINT nIDEvent) {
	CRichEditCtrl::OnTimer(nIDEvent);

	if(nIDEvent == TIMER_BACKGROUNDCOLORING) {
		// Color next m_nBckgdTimerNumOfLines uncolored lines:
		TRACE(_T("OnTimer\n"));
		bool bColorEnabled = false;

		// To prevent flickering:
		CHARRANGE crCurrent;
		GetSel(crCurrent);
		int nCharPosition = crCurrent.cpMin;
		if((GetFocus() == this) && (crCurrent.cpMin == crCurrent.cpMax) &&
			(nCharPosition == LineIndex(LineFromChar(nCharPosition))) )	{
			// 1. Control is in focus, caret is at beginning of line and no selection is made
			bColorEnabled = true;
			}
		else if(IsWindowCompletelyObscured())	{
			// 2. Control is not in focus and its window completely obscured
			bColorEnabled = true;
			}
		if(!bColorEnabled)	
			return;	// To prevent flickering give up background coloring at this stage
		
		int nFirstVisibleLine = GetFirstVisibleLine();
		
		// Prepare for paint (disable redraw to prevent flickering):
		SetRedraw(FALSE);

		CString strLine;
		int nColored = 0;

		m_bOnEnVscrollDisabled = true;	// Clicking the last visible line would cause OnEnVscroll when line only partially visible	
		for (int i = 0; i < m_nLineCount && nColored < m_nBckgdTimerNumOfLines; i++) {
			if(!(m_pLinesEndState[i] & LINE_COLORED)) {
				// Colour the all the text as Text initially, use nCharPosition for optimization:
				int nLineLength = GetLineHelper(i, strLine);
				long lCharStart	= LineIndex(i);

				SetSel(lCharStart, lCharStart + nLineLength);
				SetSelectionCharFormat(m_cfText);

				ParseLines((LPCTSTR)strLine, -1 , true, i);
				m_pLinesEndState[i] |= LINE_COLORED;

				nColored++;
				}
			}
		if(nColored < m_nBckgdTimerNumOfLines) {
			// All lines are colored? The background coloring timer can be turned off:
			BOOL bRes = KillTimer(TIMER_BACKGROUNDCOLORING);	ASSERT(bRes);
			m_bBckgdTimerActivated = false;
			}

		// Restore after painting:
		SetSel(crCurrent);
		int nCurrentFirstVisibleLine = GetFirstVisibleLine();	
		if(nCurrentFirstVisibleLine != nFirstVisibleLine) {
			// Coloring might scroll the control, so restore original visible line.
		 // OnEnVscroll is disabled at this stage because otherwise we get into endless recurssion
			LineScroll(nFirstVisibleLine - nCurrentFirstVisibleLine, 0);
			}
		m_bOnEnVscrollDisabled = false;

		SetRedraw(TRUE);
		Invalidate(FALSE);
		}
	}

/////////////////////////////////////////////////////////////////////////////
// Helper functions
/////////////////////////////////////////////////////////////////////////////
int CRichEditCtrlEx::GetLastVisibleLine() {
	// The EM_GETRECT message retrieves the formatting rectangle of an edit control:
	RECT rfFormattingRect = {0};

	GetRect(&rfFormattingRect);
	rfFormattingRect.left++;
	rfFormattingRect.bottom -= 2;

	// The EM_CHARFROMPOS message retrieves information about the character
    // closest to a specified point in the client area of an edit control
	int nCharIndex =  CharFromPos(CPoint(rfFormattingRect.left, rfFormattingRect.bottom));

	//The EM_EXLINEFROMCHAR message determines which
    //line contains the specified character in a rich edit control
	 return LineFromChar(nCharIndex);
	}

/**
 * Changing/Adding text to a line invalidates the color state of this line and
 * the ones who follow. For example if a comment start ("<!--") is added, following
 * lines should be colored as Comment until comment end ("<--") is reached.
 * Note that previous lines are unaffected
 */
void CRichEditCtrlEx::InvalidateColorStates(int nCharPosition) {
	// Invalidate all lines starting from nLineIndex:
	int nLineIndex = LineFromChar(nCharPosition);
	long lCharStart	= LineIndex(nLineIndex);
	
	CString strCtrlText;
	int nChars = GetTextRange(lCharStart, GetTextLength(), strCtrlText);
	ParseLines((LPCTSTR)strCtrlText, -1, false, nLineIndex);
	
	// Color visible lines between [nLineIndex..m_nLineCount) that are not colored already, one line at a time:	
	ColorVisibleLines(nCharPosition);

	// Activate the background coloring timer
	StartColoringTimer();
	}

/**
 * Color chars within the visible line range, that are not colored already.
 * Coloring starts from char nCharPosition, assuming previous chars are already colored.
 * If nCharPosition is omitted - all lines within visible range are colored if not already.
 */
void CRichEditCtrlEx::ColorVisibleLines(int nCharPosition /*= -1*/) {
	// Color visible lines between [nLineIndex..m_nLineCount) that are not colored already, one line at a time:	
	CString strLine;
	int nFirstLine = GetFirstVisibleLine();	// Send the control EM_GETFIRSTVISIBLELINE message
	int nLastLine = GetLastVisibleLine();	// No such message as EM_GETLASTVISIBLELINE - use our own algorithm
	int nOrigFirstVisibleLine = nFirstLine;
	long nCharPositionLine = -1;

	if(nCharPosition != -1) {
		// Adjust nFirstLine to the line containing nCharPosition
		nCharPositionLine	= LineFromChar(nCharPosition);
		if(nFirstLine < nCharPositionLine)	nFirstLine = nCharPositionLine;
		}

	// Prepare for paint (disable redraw to prevent flickering):
	CHARRANGE crCurrent;
	SetRedraw(FALSE);
	GetSel(crCurrent);
	
	m_bOnEnVscrollDisabled = true;	// Clicking the last visible line would cause OnEnVscroll when line only partially visible
	for (int i = nFirstLine; i <= nLastLine; i++)	{
		if(!(m_pLinesEndState[i] & LINE_COLORED)) {
			int nFirstLineDEBUG2 = GetFirstVisibleLine();

			// Colour the all the text as Text initially, use nCharPosition for optimization:
			int nLineLength = GetLineHelper(i, strLine);
			long lCharStart	= LineIndex(i);
			if(i == nCharPositionLine)	{
				nLineLength -= (nCharPosition - lCharStart);
				lCharStart = nCharPosition;
			}
			SetSel(lCharStart, lCharStart + nLineLength);
			SetWordCharFormat(m_cfText);

			ParseLines((LPCTSTR)strLine, (i == nCharPositionLine) ? nCharPosition : -1 , true, i);
			m_pLinesEndState[i] |= LINE_COLORED;
			}
		}

	// Restore after painting:
	SetSel(crCurrent);
	int nCurrentFirstVisibleLine = GetFirstVisibleLine();	
	if(nCurrentFirstVisibleLine != nOrigFirstVisibleLine) {
		// Coloring might scroll the control, so restore original visible line.
	 // OnEnVscroll is disabled at this stage because otherwise we get into endless recurssion
		LineScroll(nOrigFirstVisibleLine - nCurrentFirstVisibleLine, 0);
		}
	m_bOnEnVscrollDisabled = false;
	SetRedraw(TRUE);
	Invalidate(FALSE);
	}

/**
 * ParseLines has two modes, depending on bColor parameter:
 * 1. Calculate color state of ending char for all lines without coloring.
 * 2. Calculate and color.
 * The ending chars color states are used for handling multiline tags/quotes/text/comments correctly.
 * nCharPosition states the chars from which coloring is required
 *	(can be in the middle of a line, but still the calculation starts from the beginning)
 * NOTE:
 *		New lines are automatically recognized.
 *		Function goes over xml in one pass, i.e. quite fast when no coloring involved.
 * The no-coloring mode is used for new xml inputs and several lines of xml simultaneously,
 * The coloring mode is uses one line at a time, for coloring visible lines only
 */
FastHtmlColorState CRichEditCtrlEx::ParseLines(LPCTSTR pLines, int nCharPosition, bool bColor, int nCurrentLine /*= -1 */) {

	if(nCurrentLine == -1)	
		nCurrentLine = LineFromChar(nCharPosition);

	// Get color state of beginning char (same as previous line's ending char).
	// For first line we use InNormalText, as chars preceding  '<' are considered as normal text.
	FastHtmlColorState currentState = (nCurrentLine == 0) ? epsInNormalText : (FastHtmlColorState)(m_pLinesEndState[nCurrentLine - 1] & (~LINE_COLORED));
	
	// Take care of empty ("") strings
	// When caret is at the end position and user presses the enter, we get a new empty line (OnChar).
	// OnTimer also calls ParseLines with an empty string in the background
	if(*pLines == NULL) {
		m_pLinesEndState[nCurrentLine] = currentState;
		if(bColor)	{// Apply color state of previous line's ending char:
			if(currentState == epsInTag)
				{BOOL bRes = SetWordCharFormat(m_cfTags);	ASSERT(bRes);}
			else if(currentState == epsInDblQuotes)
				{BOOL bRes = SetWordCharFormat(m_cfQuoted);	ASSERT(bRes);}
			else if(currentState == epsInComment)
				{BOOL bRes = SetWordCharFormat(m_cfComment);	ASSERT(bRes);}
			else
				{BOOL bRes = SetWordCharFormat(m_cfText);	ASSERT(bRes);}
			m_pLinesEndState[nCurrentLine] |= LINE_COLORED;
			}
		return currentState;
		}

	TCHAR* pCurChar = (TCHAR*)pLines;
	long lCharStart	= LineIndex(nCurrentLine);
	
	int nColorStart = -1;

	// loop while not whole line has been coloured:
	while (*pCurChar)	{
		if(*pCurChar == _T('\r') || *pCurChar == _T('\n')) {// EOL is reached? Set the ending-char color state:
			if(*pCurChar == _T('\r') && *(pCurChar+1) == _T('\n'))	pCurChar++;	// Take care of \r\n pattern
			if((m_pLinesEndState[nCurrentLine] &~ LINE_COLORED) == currentState)
				break;	// If ending-char color state of this line hasn't changed - no point of recalculating next lines 
			m_pLinesEndState[nCurrentLine++] = currentState;
			}
		else if(currentState == epsInComment) {// Inside Comment all chars are acceptable. The state is only changed by the "-->" combination:
			if((*pCurChar == KEY_TAG_END) && (*(pCurChar - 1) == _T('-')) && (*(pCurChar - 2) == _T('-'))) {
				if(bColor)	// Colourise the Comment (If no Comment in this line - we found a Comment end of a previous line):
					nColorStart = ColorRangeHelper((nColorStart == -1) ? lCharStart : nColorStart, lCharStart + pCurChar - pLines + 1, m_cfComment, nCharPosition);
				currentState = epsInNormalText;	// After leaving InComment state, all chars till '<' are considered normal text chars
				}	
			}
		else if(*pCurChar == KEY_TAG_START) {// '<' can start a Tag or a Comment block:
			if(currentState == epsInTag || currentState == epsInDblQuotes)
			{// IE doesn't allow '<' to be inside quotes, thus "d<d" is illegal. However I'll handle it as moving to epsInNormalText state:
				currentState = epsInNormalText;
				}
			else if(*(pCurChar + 1) == _T('!') && *(pCurChar + 2) == _T('-') && *(pCurChar + 3) == _T('-')) {
				// If we reach "<!--" we're staring a Comment
				currentState = epsInComment;
				}
			else
				{currentState = epsInTag;}
			nColorStart = pCurChar - pLines + lCharStart;
			}
		else if(*pCurChar == _T(KEY_DBL_QUOTE)) {
			// Double quotes ('"') can be starting or ending quotes.
		 // However quotes are applicable in Tag only (in InNormalText we'll treat them as regular chars)
			if(currentState == epsInDblQuotes) {
				// These are ending quotes
				if(bColor)	// Colourise the string (If no staring quotes found in this line, we're ending a Quoted Text of a previous line):
					nColorStart = ColorRangeHelper((nColorStart == -1) ? lCharStart : nColorStart, lCharStart + pCurChar - pLines + 1, m_cfQuoted, nCharPosition);
				currentState = epsInTag;	// Assumption: Before we entered the InDblQuotes state we were in a Tag:
				}
			else if(currentState == epsInTag)	// Starting quotes
			{// These are beginning quotes:
				if(bColor)	// Colourise the Tag before the starting quotes (If no staring Tag found in this line, we're ending a Tag of a previous line)
					nColorStart = ColorRangeHelper((nColorStart == -1) ? lCharStart : nColorStart, lCharStart + pCurChar - pLines, m_cfTags, nCharPosition);
				currentState = epsInDblQuotes;
				}
			}
		else if(*pCurChar == _T(KEY_TAG_END)) {// Ending tag ('>'):
			if(currentState != epsInNormalText)	// '>' in normal text has no meaning, for example >"va>lue" as text is valid
			{
				if(currentState == epsInTag)	{
					if(bColor)	// Colourise the Tag:
						nColorStart = ColorRangeHelper((nColorStart == -1) ? lCharStart : nColorStart, lCharStart + pCurChar - pLines + 1, m_cfTags, nCharPosition);
					currentState = epsInNormalText;	// After leaving Tag state, all chars till '<' are considered normal text chars
					}
				else	// If '>' is part of a string, for example ("d>d"), leave the InDblQuotes state
					ASSERT(currentState == epsInDblQuotes);	//It can also be part of a string, for example ("d>d")
				}
			}
		pCurChar++;
		}
	
	if(bColor && (pCurChar - 1 - pLines) >= (nColorStart - lCharStart))	// The = is because nColorStart position should be colored as well
	{// Then there are uncolored chars left till end of line. These are part of a multiline Tag/DblQuotes/Comment/NormalText
		if(nColorStart == -1)	nColorStart = lCharStart;	// Haven't found any interesting keys - color whole line according to previous state
		SetSel(nColorStart, lCharStart + pCurChar - pLines);
		if(currentState == epsInTag)		SetWordCharFormat(m_cfTags);
		if(currentState == epsInDblQuotes)	SetWordCharFormat(m_cfQuoted);
		if(currentState == epsInComment)	SetWordCharFormat(m_cfComment);
	}

	if(nCurrentLine < m_nLineCount) {// Set color state of last line:	 
		m_pLinesEndState[nCurrentLine] = currentState;
	}
	else {// NOTE: When called by UpdateLinesArraySize or InvalidateColorStates, ParseLines
	 // is called with a range of lines, starting with current line till the end.
	 // UpdateLinesArraySize or InvalidateColorStates do not remove trailing \n because
	 // this \n might belong to previous line if whole line was just \n.
	 // Thefore if we have a trailing \n, it is alrady handled by the if(*pCurChar == _T('\n')
	 // and nCurrentLine now equals m_nLineCount
		ASSERT(nCurrentLine == m_nLineCount);
		}
	return currentState;
	}

/**
 * This helper is used by ParseLines when called with bColor = "true".
 * When recoloring is required as a reaction to user-editing, nColorFromChar states the position
 * from which recoloring is required (caret position)
 * Return value is the updated nColorStart value (which is nColorEnd)
 */
int CRichEditCtrlEx::ColorRangeHelper(int nColorStart, int nColorEnd, CHARFORMAT charFormat, int nColorFromChar/* = -1 */) {

	if(nColorStart < nColorFromChar)	
		nColorStart = nColorFromChar;
	if(nColorStart < nColorEnd) {
		SetSel(nColorStart, nColorEnd);
		SetWordCharFormat(charFormat);
		}
	return nColorEnd;
	}

// Upon OnChar or OnChange the control's line count might have been changed
// (pressing enter, pasting text, deleting text)
void CRichEditCtrlEx::UpdateLinesArraySize() {
	int nLineCount = GetLineCount();

	// If new lines have been added to the control - expand the color state array:
	if(m_nLineCount < nLineCount) {// Reallocate the m_pLinesEndState buffer:
		int nPrevLineCount = m_nLineCount;
		m_pLinesEndState = (BYTE*)realloc(m_pLinesEndState, nLineCount * sizeof(BYTE));	ASSERT(m_pLinesEndState != NULL);
		ZeroMemory(m_pLinesEndState + m_nLineCount * sizeof(BYTE), (nLineCount - m_nLineCount) * sizeof(BYTE));	// Clear color state of new lines
		m_nLineCount = nLineCount;	// Update line counter to reflect new array size

		long lCharStart	= LineIndex(nPrevLineCount);	// Get first char of new line (using previous m_nLineCount)
		
		// Get range of all new lines, starting with current line:
		CString strCtrlText;
		int nChars = GetTextRange(lCharStart, GetTextLength(), strCtrlText);

		// Parse color states of all new lines (without coloring)
		ParseLines((LPCTSTR)strCtrlText, -1, false, nPrevLineCount);
		}
	else if(m_nLineCount < nLineCount) {
		// If lines have been removed - just update the m_nLineCount member. realloc isn't necessary
		m_nLineCount = nLineCount;
		}
	}

/**
 * NOTE: When I press enter on the first line add press chars on the second line, GetLine
 * always returns empty strings???
 */
int CRichEditCtrlEx::GetLineHelper(int nLineIndex, CString& strLine, int nLineLength /* = -1 */) {

	if(nLineLength == -1)
		nLineLength = LineLength(LineIndex(nLineIndex));
	
	int nChars = GetLine(nLineIndex, strLine.GetBuffer(nLineLength + 3), nLineLength);
	strLine.ReleaseBuffer(nChars);

	// The RichEditCtrl sometimes appends \r and sometimes \r\n so remove them:
	TrimRightCrLfHelper(strLine, nLineLength);

	return strLine.GetLength();
	}

/**
 * The RichEditCtrl sometimes appends \r and sometimes \r\n which interfere with my calculations in ParseLines.
 * However I don't want to allocate a new string with SpanExcluding(_T("\r\n")), so I'm placing NULLs in original string: 
 */
void CRichEditCtrlEx::TrimRightCrLfHelper(CString& strText, int nLength /* = -1 */) {

	if(nLength == -1)	
		nLength = strText.GetLength();

	if(nLength >= 1 && (strText[nLength - 1] == _T('\r') || strText[nLength - 1] == _T('\n')))
		strText.SetAt(nLength - 1, NULL);
	if(nLength >= 2 && strText[nLength - 2] == _T('\r'))
		strText.SetAt(nLength - 2, NULL);
	}

/**
 * Pressing '-' or '!' might bring us to a comment start.
 * Pressing a key inside "<!--" breaks a comment start.
 * Deleting/Pasting inside "<!--" breaks a comment start.
 * Return value: The invalidat position (up to 3 chars before nCharPosition) or -1 of none found
 */
int CRichEditCtrlEx::FindCommentStartHelper(int nCharPosition) {
	CString str1backwards, str2backwards, str3backwards;
	if(nCharPosition > 0)	{int nChars = GetTextRange(nCharPosition - 1 , nCharPosition, str1backwards);}
	if(nCharPosition > 1)	{int nChars = GetTextRange(nCharPosition - 2 , nCharPosition, str2backwards);}
	if(nCharPosition > 2)	{int nChars = GetTextRange(nCharPosition - 3 , nCharPosition, str3backwards);}
	
	if(str1backwards == _T('<'))			
		return nCharPosition - 1;
	else if(str2backwards == _T("<!"))		
		return nCharPosition - 2;
	else if(str3backwards == _T("<!-"))	
		return nCharPosition - 3;
	else
		return -1;
	}

int CRichEditCtrlEx::FindCommentEndHelper(int nCharPosition) {
	CString str1forward, str2forward;
    int nChars = GetTextRange(nCharPosition + 1, nCharPosition + 2, str1forward);	// Search for "-->" and this char is the middle '-'

	nChars = GetTextRange(nCharPosition + 1, nCharPosition + 3, str2forward);
	if(str1forward == _T('>'))				
		return  nCharPosition + 2;
	else if(str2forward == _T("->"))		
		return  nCharPosition + 3;
	else
		return -1;
	}

/**
 * When user presses VK_RETURN or sets caret at begining of line, we must explicitly
 * set the correct color. For example if user writes <a>, then moves caret to line start
 * and presses a char, this char gets the color of the Tag instead of the InNormalText
 */
void CRichEditCtrlEx::SetFirstLineCharColor(int nLineIndex) {

	// First char of this new line gets the color state of previous line's ending char:
	FastHtmlColorState prevState = (FastHtmlColorState)(m_pLinesEndState[nLineIndex - 1] &~ LINE_COLORED);
	if(prevState == epsInTag)
		{BOOL bRes = SetWordCharFormat(m_cfTags);	ASSERT(bRes);}
	else if(prevState == epsInDblQuotes)
		{BOOL bRes = SetWordCharFormat(m_cfQuoted);	ASSERT(bRes);}
	else if(prevState == epsInComment)
		{BOOL bRes = SetWordCharFormat(m_cfComment);	ASSERT(bRes);}
	else
		{BOOL bRes = SetWordCharFormat(m_cfText);	ASSERT(bRes);}
	}

/**
 * For back I'm getting OnKeyDown, OnChange, OnChar.
 * For Delete I'm getting OnKeyDown, OnChange.
 * If a text is selected, I'm getting OnKeyDown, OnChar and OnChange.
 *		In this case when base class's OnKeyDown is done, GetWindowText doesn't
 *		return the updated text, so I'm defering the action to OnChange
 */
void CRichEditCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	TRACE(_T("OnKeyDown\n"));

	// TODO: Add your message handler code here and/or call default
	long lStart = 0, lEnd = 0;
	GetSel(lStart, lEnd);
	int nCharPosition = lStart;

	// If caret at begining of line, we must explicitly set the correct color:
	long lCharStart	= LineIndex();	// Retrieves the character index of the current line (the line that contains the caret)
	if(lCharStart == lStart)	{
		int nLineIndex = LineFromChar(lCharStart);
		SetFirstLineCharColor(nLineIndex);
	}

	bool bDeferedToOnChange = false;	// When selection is made, we have to defer the action to OnChange
	
	CString strDeleted;	// For trapping the deleted char/chars (GetTextRange must be called before base class's OnKeyDown)

	// Pressing ctrl+v invalidates all chars from nCharPosition and might also scroll the control.
	// OnKeyDown's base class calls OnEnVScroll and finally OnChar. Therefore we can query for updated
	// line count only after call to base class's OnKeyDown is made (GetWindowText after OnKeyDown returns updated text)
	bool bIsPasting = false;
	if(nChar == _T('V') && GetKeyState(VK_CONTROL) & 0x8000)	{//check state of left and right CTRL keys. If high order bit is 1 - indicates the key is down
		bIsPasting = true;
		m_bOnEnVscrollDisabled = true;	// Disable OnEnVScroll because we don't have updated line count yet
		}
	else {// Not pasted text - check for deleted chars
		if(lStart != lEnd)	{// A selection is made - pressing any "regular" char would delete the selection.
		 // However we cannot detect the changes here, so we must defer the action to OnChange
			int nChars = GetTextRange(lStart, (lEnd == lStart) ? (lEnd + 1) : lEnd, strDeleted);
			bDeferedToOnChange = true;
			}
		else {// The RichEditCtrl seems to add a "regular" char in his OnChar handler.
		 // However VK_BACK and VK_DELETE seem to be handled here (GetWindowText after OnKeyDown returns updated text)
			if(nChar == VK_DELETE) {
				// Trap the about-to-be-deleted char:
				int nChars = GetTextRange(lStart, lEnd + 1, strDeleted);	// Delete without selection is the same as delete with selecting the next char
				}
			else if(nChar == VK_BACK) {// The char just-before-caret-possition will be deleted:
				int nChars = GetTextRange(lStart - 1 , lStart, strDeleted);
				nCharPosition--;	// Because after pressing "back" the caret moves to the left
				}
			}
		}

	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	if(bIsPasting) {
		// At this point the chars have already been pasted:
		m_bOnEnVscrollDisabled = false;
		UpdateLinesArraySize();
		
		// Take care of breaking/completing "<!--" combination:
		int nCommentStart = FindCommentStartHelper(nCharPosition);
		if(nCommentStart != -1)
			InvalidateColorStates(nCommentStart);
		}
	else if(strDeleted.GetLength()) {// At this point the chars have already been deleted:
		UpdateLinesArraySize();
		if(strDeleted.FindOneOf(_T("<>\"")) != -1)	{
			bDeferedToOnChange ? (m_nOnChangeCharPosition = nCharPosition) : InvalidateColorStates(nCharPosition);
			}
		else {// Take care of breaking/completing "<!--" "-->" combinations:
			int nCommentStart = FindCommentStartHelper(nCharPosition);
			if(nCommentStart != -1)
				InvalidateColorStates(nCommentStart);
			else {
				int nCommentEnd = FindCommentEndHelper(nCharPosition - 1);	// For VK_DELETE and VK_BACK nCharPosition - 1 should be used
				if(nCommentEnd != -1)	// Just broke/completed comment end combination
					InvalidateColorStates(nCommentStart);
				}
			}
		}
	}

void CRichEditCtrlEx::OnSize(UINT nType, int cx, int cy) {
	TRACE(_T("OnSize \n"));
	CRichEditCtrl::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(m_nLineCount > 1)	{// m_nLineCount is initialized to 1 in PreSubclassWindow.
	 // However LoadFile->StreamIn also gets here before m_nLineCount had a chance to be updated,
	 // so this if is just a workarround for that
		ColorVisibleLines();
	}
}

/**
 * OnEnVscroll handles all cases of change in the view area of the edit control, with one exception -
 * When clicking the scroll bar mouse itself, EN_VSCROLL isn't send.
 * This handler handles this case.
 */
void CRichEditCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {	
	// TODO: Add your message handler code here and/or call default
	TRACE(_T("OnVScroll nSBCode = %d\n"), nSBCode);
	CRichEditCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

	// The SB_THUMBTRACK request code occurs as the user drags the scroll box. 
	if(nSBCode == SB_THUMBTRACK /*SB_THUMBPOSITION*/)	// Drag scroll box to specified position
	{
		SetFocus();	// This is important!!! If the user drags/releases the scroll thumb without focus - the thub jumps and flickers!
		ColorVisibleLines();
		}
	}

/**
 * OnEnVscroll is received when a keyboard event causes a change in the view area of the edit control,
 * for example, pressing HOME, END, PAGE UP, PAGE DOWN, UP ARROW, or DOWN ARROW.
 * However EN_VSCROLL isn't sent when when clicking/dragging the scroll bar mouse itself, which will be handled by OnVScroll.
 * NOTE: Wh might also get here upon ctrl+v when pasted text scrolls the contents.
 */
void CRichEditCtrlEx::OnEnVscroll() {
	TRACE(_T("OnEnVScroll\n"));
	// TODO: Add your control notification handler code here

	if(!m_bOnEnVscrollDisabled)
		ColorVisibleLines();
	}

// Activate the background coloring timer if CRichEditCtrl has the focus.
// Otherwise we'll just wait till WM_SETFOCUS
void CRichEditCtrlEx::StartColoringTimer() {

	if(m_uiBckgdTimerInterval == 0 || m_nBckgdTimerNumOfLines <= 0)
		return;	// Timer was disabled by a call to SetBckgdColorTimer
	
	m_bBckgdTimerActivated = true;
	SetTimer(TIMER_BACKGROUNDCOLORING, m_uiBckgdTimerInterval, NULL);
	}

// CURRENTLY NOT USED
void CRichEditCtrlEx::StopColoringTimer() {

	if(m_bBckgdTimerActivated)	{// Disable current background coloring timer:
		BOOL bRes = KillTimer(TIMER_BACKGROUNDCOLORING);	ASSERT(bRes);
		m_bBckgdTimerActivated = false;
		}
	}

int CRichEditCtrlEx::GetTextRange(int nFirst, int nLast, CString& refString) {
	TEXTRANGE tr;

	tr.chrg.cpMin=0;
	tr.chrg.cpMax=-1;
	return SendMessage(EM_GETTEXTRANGE,0,(uint32_t)&tr);
	}

int CRichEditCtrlEx::CharFromPos(CPoint pt) {

//	return SendMessage(EM_CHARFROMPOS,0,(uint32_t)&pt);
	return SendMessage(EM_CHARFROMPOS,0,(uint32_t)&pt);
	}


