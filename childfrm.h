
#pragma once
#include <afxext.h>

class CMySplitterWnd : public CSplitterWnd			// gemini 9/26
{
  DECLARE_DYNCREATE(CMySplitterWnd)

public:
  CMySplitterWnd();
  virtual ~CMySplitterWnd();
	BOOL IsSplit() const { return m_bIsSplit; }
  void SetSplitState(BOOL bSplit) { m_bIsSplit = bSplit; }
// Override nativo di MFC per il ricalcolo del layout
  virtual void RecalcLayout();

protected:
	BOOL m_bIsSplit; // TRUE = 2 viste aperte, FALSE = 100%/0%
  int  m_nRatio;   // Proporzione riga superiore in decimi (es. 5 = 50%, 7 = 70%

protected:
  // Intercettiamo il doppio click sulla barra/maniglia dello splitter
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  DECLARE_MESSAGE_MAP()
	};

// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

class CChildFrame : public CMDIChildWnd {
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
protected:
	CMySplitterWnd m_wndSplitter;
	BOOL m_bInitSplitter;
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
