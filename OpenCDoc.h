// OpenCDoc.h : interface of the COpenCDoc class
//
/////////////////////////////////////////////////////////////////////////////

//#include <afxrich.h>
//troppo incasinato... per ora faccio cosi': (v. anche View)
#define CRichEditDoc CDocument

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


class COpenCDoc : public CExRichDocument {
public:
protected: // create from serialization only
	COpenCDoc();
	DECLARE_DYNCREATE(COpenCDoc)

// Attributes
public:
	char myPrfSection[128];

protected:

// Overrides
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
