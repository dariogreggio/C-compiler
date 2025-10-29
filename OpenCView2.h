/////////////////////////////////////////////////////////////////////////////
// COutputView window

class COutputView : public CEditView {
protected: // create from serialization only
	COutputView();
	DECLARE_DYNCREATE(COutputView)

// Attributes
public:

// Operations
public:
	COpenCDoc2 *GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COutputView)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COutputView();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// Generated message map functions
protected:
	//{{AFX_MSG(COutputView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in OutputView.cpp
inline COpenCDoc2* COutputView::GetDocument() 
	{ return (COpenCDoc2 *)m_pDocument; }
#endif

