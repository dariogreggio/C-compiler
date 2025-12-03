// OpenCEditView.h : interface of the COpenCEditView class
//
/////////////////////////////////////////////////////////////////////////////

class CRichEditCtrlEx;

class COpenCView : public CRichEditView {
protected: // create from serialization only
	COpenCView();
	DECLARE_DYNCREATE(COpenCView)

// Attributes
public:
	UINT_PTR m_uTimerID;
	BOOL m_bDelayUpdateItems;

	BOOL CreateView(int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext);
	COpenCDoc* GetDocument();
	BOOL IsModified() { return GetRichEditCtrl().GetModify(); }
	static DWORD CALLBACK MyStreamInCallback(DWORD , LPBYTE , LONG , LONG *);
	static DWORD CALLBACK MyStreamOutCallback(DWORD , LPBYTE , LONG , LONG *);
	long StreamIn(EDITSTREAM);
	long StreamOut(EDITSTREAM);

// Operations
public:
	BOOL GetWindowPos(RECT *);
	uint32_t CharFromPos(POINT);
	BOOL SetTabStops(const int& cxEachStop);
	uint32_t GetLineCount();
//	CRichEditCtrl& GetRichEditCtrl() { return (CRichEditCtrlEx*)&GetRichEditCtrl(); };

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenCEditView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COpenCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(COpenCView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditTrovaselezione();
	afx_msg void OnUpdateEditTrovaselezione(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in OpenCEditView.cpp
inline COpenCDoc* COpenCView::GetDocument()
   { return (COpenCDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// COpenCViewOut view


enum FastHtmlColorState {
	epsUnknown		= 0x00,
	epsInTag		= 0x01,
	epsInDblQuotes	= 0x02,
	epsInComment	= 0x03,
	epsInNormalText	= 0x04,
	epsError		= 0x10
};
#define LINE_COLORED	0x80	// This bit indicates that a line is already colored

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx window

class CRichEditCtrlEx : public CRichEditCtrl {
// Construction/Destruction
public:
	// Default constructor
	CRichEditCtrlEx();
	// Default destructor
	virtual ~CRichEditCtrlEx();

public:
// Character format functions
	// Sets the character format to be used for Tags
	void SetTagCharFormat(int nFontHeight = 8, 
		COLORREF clrFontColour = RGB(128, 0, 0), 
		CString strFontFace = _T("Courier New"),
		bool bParse = true);
	// Sets  the character format to be used for Tags
	void SetTagCharFormat(CHARFORMAT& cfTags, bool bParse = true);
	// Sets the character format to be used for Quoted text
	void SetQuoteCharFormat(int nFontHeight = 8, 
		COLORREF clrFontColour = RGB(0, 128, 128), 
		CString strFontFace = _T("Courier New"),bool bParse = true);
	// Sets  the character format to be used for Quoted text
	void SetQuoteCharFormat(CHARFORMAT& cfQuoted, bool bParse = true);
	// Sets the character format to be used for Comments
	void SetCommentCharFormat(int nFontHeight = 8, 
		COLORREF clrFontColour = RGB(0, 128, 0), 
		CString strFontFace = _T("Courier New"),bool bParse = true);
	// Sets  the character format to be used for Comments
	void SetCommentCharFormat(CHARFORMAT& cfComments, bool bParse = true);
	// Sets the character format to be used for Normal Text
	void SetTextCharFormat(int nFontHeight = 8, 
		COLORREF clrFontColour = RGB(0, 0, 0), 
		CString strFontFace = _T("Courier New"),bool bParse = true);
	// Sets  the character format to be used for Normal Text
	void SetTextCharFormat(CHARFORMAT& cfText, bool bParse = true);

// Parsing functions
	// Parses all lines in the control, colouring each line accordingly.
	void ParseAllLines();
	
// Miscellaneous functions
	// Loads the contents of the specified file into the control.
	// Replaces the existing contents and parses all lines.
	void LoadFile(CString& strPath);
	
	// Enables/disables the background coloring timer. If enabled, event
	// is raised every uiInterval millis and nNumOfLines uncolored lines are colored.
	void SetBckgdColorTimer(UINT uiInterval = 1000, int nNumOfLines = 10);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRichEditCtrlEx)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CRichEditCtrlEx)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnChange();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CHARFORMAT	m_cfTags;
	CHARFORMAT	m_cfText;
	CHARFORMAT	m_cfQuoted;
	CHARFORMAT	m_cfComment;

	// To handle multiline comments correctly, I must track the color state of the end of every line  
	int		m_nLineCount;		// Number of lines in RichEditCtr
	BYTE*	m_pLinesEndState;	// Array of size m_nLineCount for holding the color state of each line's end char   

	bool	m_bOnEnVscrollDisabled;
	int		m_nOnChangeCharPosition;	// OnKeyDown signals OnChange to InvalidateColorStates

	UINT	m_uiBckgdTimerInterval;		// How many millis between each iteration
	int		m_nBckgdTimerNumOfLines;	// How many lines to color in each iteration
	bool	m_bBckgdTimerActivated;

private:
// Helper functions
	int GetLastVisibleLine();
	FastHtmlColorState ParseLines(LPCTSTR pLines, int nCharPosition, bool bColor, int nCurrentLine = -1);
	void ColorVisibleLines(int nCharPosition = -1);
	void InvalidateColorStates(int nLineIndex);
	void UpdateLinesArraySize();

	int GetTextRange(int nFirst, int nLast, CString& refString);
	int CharFromPos(CPoint pt);

	int ColorRangeHelper(int nColorStart, int nColorEnd, CHARFORMAT charFormat, int nColorFromChar = -1);
	int GetLineHelper(int nLineIndex, CString& strLine, int nLineLength = -1);
	void TrimRightCrLfHelper(CString& strText, int nLength = -1);
	int FindCommentStartHelper(int nCharPosition);
	int FindCommentEndHelper(int nCharPosition);
	void SetFirstLineCharColor(int nLineIndex);

	void StartColoringTimer();
	void StopColoringTimer();	// CURRENTLY NOT USED
	BOOL IsWindowCompletelyObscured();

public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnVscroll();
};


