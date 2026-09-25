// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////


#pragma once

// Messaggio personalizzato per notificare il MainFrame // Gemini 9/26
#define WM_GOTO_OUTPUT_LINE (WM_USER + 101)

class COutputEdit : public CEdit {
  DECLARE_DYNAMIC(COutputEdit)

public:
  COutputEdit();
  virtual ~COutputEdit();
	// Seleziona la riga specificata nell'Edit dell'output
  void SelectLine(int nLineIndex);
  // Cerca il prossimo errore partendo dalla riga corrente e lo elabora
  void ProcessNextError(BOOL bForward	);

protected:
  void ProcessCurrentLine();

  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

  DECLARE_MESSAGE_MAP()
};


class COutputBar : public CDialogBar {
  DECLARE_DYNAMIC(COutputBar)

public:
  COutputBar();
  virtual ~COutputBar();

  // Controlli interni
  COutputEdit m_wndOutputEdit,m_wndFindInFilesDlg;
	CTabCtrl    m_wndOutputTab;

  virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult);
  DECLARE_MESSAGE_MAP()
};

class CMainFrame : public CMDIFrameWnd {
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	CFont myFont;
	CImageList il;
	HTREEITEM projectTreeRoot;

	CStatusBar  m_wndStatusBar;

	COutputBar  m_wndOutputBar;   // Barra messaggi (Bottom)

  CDialogBar  m_wndProjectBar;  // Barra albero progetti (Left)
  CTreeCtrl   m_wndProjectTree; // Il controllo albero vero e proprio

	int numErrors,numWarnings;

// Operations
public:
	void ActivateViewByTitle(const CString& strTargetTitle);

	RECT *getOutputWndRect(RECT *);
	CTreeCtrl& GetProjectTree() { return m_wndProjectTree; }
	RECT *getToolbarRect(RECT *rc) { m_wndToolBar.GetClientRect(rc); return rc;}

	void AddOutputText(LPCTSTR lpszText);
	void ClearOutputText();
	int AddText(const char *s,int m);
	int Cls();
	void GoToRichEditLine(int,const char *,bool);
	LRESULT OnGotoOutputLine(WPARAM wParam, LPARAM lParam);
	void OnNextError();
	void OnPrevError();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CToolBar    m_wndToolBar;

public:

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateFileSalvatutto(CCmdUI* pCmdUI);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnWindowCascade();
	afx_msg void OnWindowTileHorz();
	afx_msg void OnVisualizzaFinestradioutput();
	afx_msg void OnUpdateVisualizzaFinestradioutput(CCmdUI* pCmdUI);
	afx_msg void OnVisualizzaFinestraprogetto();
	afx_msg void OnUpdateVisualizzaFinestraprogetto(CCmdUI* pCmdUI);
	afx_msg void OnFinestraChiuditutte();
	afx_msg void OnUpdateFinestraChiuditutte(CCmdUI* pCmdUI);
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnTreeOpen();
	afx_msg void OnTreeImpostazioni();
	afx_msg void OnTreeEscludi();
	afx_msg void OnTreePropriet();
	afx_msg void OnUpdateTreeEscludi(CCmdUI* pCmdUI);
	afx_msg void OnTreeAdd();
	afx_msg void OnTreeElimina();
	afx_msg void OnUpdateTreeElimina(CCmdUI* pCmdUI);
	afx_msg void OnFileSalvatutto();
	//}}AFX_MSG
	afx_msg LRESULT OnAddText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClsWindow(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTreeDoubleClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeRightClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTreeItemExpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnDeleteitemTree(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
