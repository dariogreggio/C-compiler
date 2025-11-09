// OpenCDoc.h : interface of the COpenCDoc class
//
/////////////////////////////////////////////////////////////////////////////

//#include <afxrich.h>
//troppo incasinato... per ora faccio cosi': (v. anche View)
//#define CRichEditDoc CDocument

// non va più IsModified!!

class COpenCDoc;
class COpenCView;

class CExRichDocument : public CRichEditDoc {
public:
	char *prfSection;
	DWORD Opzioni;
// Operations
public:
	CView *getView() { POSITION pos=GetFirstViewPosition(); return GetNextView(pos); }
	void getWindow(RECT *);
	void move(int, int, int x2=0, int y2=0);
	int GetPrivateProfileInt(int k) { return theApp.GetPrivateProfileInt(prfSection,k);};
	int WritePrivateProfileInt(int k, int v)  { return theApp.WritePrivateProfileInt(prfSection,k,v);};
	int GetPrivateProfileString(int k,char *s,int l,char *def=NULL) { return theApp.GetPrivateProfileString(prfSection,k,s,l,def ? def : "");};
	int WritePrivateProfileString(int k, const char *s)  { return theApp.WritePrivateProfileString(prfSection,k,s);};
	CTime GetPrivateProfileTime(int k) { return theApp.GetPrivateProfileTime(prfSection,k);};
	CTimeSpan GetPrivateProfileTimeSpan(int k) { return theApp.GetPrivateProfileTimeSpan(prfSection,k);};
	int WritePrivateProfileTime(int k, CTime t) { return theApp.WritePrivateProfileTime(prfSection,k,t);};
	int WritePrivateProfileTime(int k, CTimeSpan t) { return theApp.WritePrivateProfileTime(prfSection,k,t);};
	};


class COpenCCntrItem : public CRichEditCntrItem {
	DECLARE_SERIAL(COpenCCntrItem)

// Constructors
public:
	COpenCCntrItem(REOBJECT* preo = NULL, COpenCDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	COpenCDoc* GetDocument()
		{ return (COpenCDoc*)COleClientItem::GetDocument(); }
	COpenCView* GetActiveView()
		{ return (COpenCView*)COleClientItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenCCntrItem)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

class COpenCDoc : public CExRichDocument {
public:
protected: // create from serialization only
	COpenCDoc();
	DECLARE_DYNCREATE(COpenCDoc)

// Attributes
public:
	char myPrfSection[128];

protected:
	uint32_t m_nDocLines;

// Overrides
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;
	virtual void OnDeactivateUI(BOOL bUndoable);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenCDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COpenCDoc();
  UINT GetDocumentLength() { return m_nDocLines; }		// cmq non viene usata... v. skypic
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(COpenCDoc)
	afx_msg void OnCompilaFile();
	afx_msg void OnUpdateCompilaFile(CCmdUI* pCmdUI);
	afx_msg void OnEditRepeat();
	afx_msg void OnUpdateEditRepeat(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	};

/////////////////////////////////////////////////////////////////////////////

class CExDocument : public CDocument {
public:
	char *prfSection;
	DWORD Opzioni;
// Operations
public:
	CView *getView() { POSITION pos=GetFirstViewPosition(); return GetNextView(pos); }
	void getWindow(RECT *);
	void move(int, int, int x2=0, int y2=0);
	int GetPrivateProfileInt(int k) { return theApp.GetPrivateProfileInt(prfSection,k);};
	int WritePrivateProfileInt(int k, int v)  { return theApp.WritePrivateProfileInt(prfSection,k,v);};
	int GetPrivateProfileString(int k,char *s,int l,char *def=NULL) { return theApp.GetPrivateProfileString(prfSection,k,s,l,def ? def : "");};
	int WritePrivateProfileString(int k, const char *s)  { return theApp.WritePrivateProfileString(prfSection,k,s);};
	CTime GetPrivateProfileTime(int k) { return theApp.GetPrivateProfileTime(prfSection,k);};
	CTimeSpan GetPrivateProfileTimeSpan(int k) { return theApp.GetPrivateProfileTimeSpan(prfSection,k);};
	int WritePrivateProfileTime(int k, CTime t) { return theApp.WritePrivateProfileTime(prfSection,k,t);};
	int WritePrivateProfileTime(int k, CTimeSpan t) { return theApp.WritePrivateProfileTime(prfSection,k,t);};
	};


class COpenCDoc2 : public CExDocument {
protected: // create from serialization only
	COpenCDoc2();
	DECLARE_DYNCREATE(COpenCDoc2)

// Attributes
public:
	CFont myFont;
	DWORD numErrors,numWarnings;
protected:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenCDoc2)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	int AddText(char *,int n=-1);
	int Cls();
	virtual ~COpenCDoc2();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(COpenCDoc2)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	};

/////////////////////////////////////////////////////////////////////////////
