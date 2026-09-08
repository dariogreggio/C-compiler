// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////


class CMainFrame : public CMDIFrameWnd {
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	CFont myFont;

	CDialogBar  m_wndOutputBar;   // Barra messaggi (Bottom)
  CEdit       m_wndOutputEdit;

  CDialogBar  m_wndProjectBar;  // Barra albero progetti (Left)
  CTreeCtrl   m_wndProjectTree; // Il controllo albero vero e proprio

	int numErrors,numWarnings;

// Operations
public:
	RECT *getOutputWndRect(RECT *);
	CTreeCtrl& GetProjectTree() { return m_wndProjectTree; }

	void AddOutputText(LPCTSTR lpszText);
	void ClearOutputText();
	int AddText(const char *s,int m);
	int Cls();
	void GoToRichEditLine(int lineNum);

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
	CStatusBar  m_wndStatusBar;
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
	afx_msg LRESULT OnAddText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnClsWindow(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
