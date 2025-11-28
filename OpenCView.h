// OpenCEditView.h : interface of the COpenCEditView class
//
/////////////////////////////////////////////////////////////////////////////

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

