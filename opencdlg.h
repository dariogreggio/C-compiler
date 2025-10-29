#if !defined(AFX_OPZIONICOMPIL_H__90A69200_A0C3_11D5_94AA_00A0C9AFFE49__INCLUDED_)
#define AFX_OPZIONICOMPIL_H__90A69200_A0C3_11D5_94AA_00A0C9AFFE49__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAboutDlg : public CDialog {
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilDlg dialog

class COpzioniCompilDlg : public CDialog {
// Construction
public:
	COpzioniCompilDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpzioniCompilDlg)
	enum { IDD = IDD_OPZIONI_COMPIL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpzioniCompilDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpzioniCompilDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg dialog

class CSplashDlg : public CDialog
{
// Construction
public:
	CSplashDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSplashDlg)
	enum { IDD = IDD_SPLASH };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSplashDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropPage1 dialog

class COpzioniCompilPropPage1 : public CPropertyPage
{
	DECLARE_DYNCREATE(COpzioniCompilPropPage1)

// Construction
public:
	COpzioniCompilPropPage1();
	~COpzioniCompilPropPage1();

public:
	CString theCC;

// Dialog Data
	//{{AFX_DATA(COpzioniCompilPropPage1)
	enum { IDD = IDD_OPZIONI_COMPIL_PAGE1 };
	CComboBox	m_CCcombo;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COpzioniCompilPropPage1)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COpzioniCompilPropPage1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropPage2 dialog

class COpzioniCompilPropPage2 : public CPropertyPage
{
	DECLARE_DYNCREATE(COpzioniCompilPropPage2)

// Construction
public:
	COpzioniCompilPropPage2();
	~COpzioniCompilPropPage2();

// Dialog Data
	//{{AFX_DATA(COpzioniCompilPropPage2)
	enum { IDD = IDD_OPZIONI_COMPIL_PAGE2 };
	BOOL	m_SoloPre;
	BOOL	m_NoMacro;
	BOOL	m_SynCheckOnly;
	BOOL	m_CheckStack;
	BOOL	m_CheckPtr;
	BOOL	m_CharUnsigned;
	BOOL	m_MultipleStrings;
	BOOL	m_PascalCalls;
	BOOL	m_InlineCalls;
	BOOL	m_OutSource;
	BOOL	m_OutAsm;
	BOOL	m_OutListing;
	BOOL	m_OttimizzaLoop;
	BOOL	m_Debug;
	CString	m_AltreDefine;
	int		m_MemoryModel;
	BOOL	m_AbsRel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COpzioniCompilPropPage2)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COpzioniCompilPropPage2)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropSheet

class COpzioniCompilPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(COpzioniCompilPropSheet)

// Construction
public:
	COpzioniCompilPropSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	COpzioniCompilPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpzioniCompilPropSheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COpzioniCompilPropSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(COpzioniCompilPropSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropPage3 dialog

class COpzioniCompilPropPage3 : public CPropertyPage
{
	DECLARE_DYNCREATE(COpzioniCompilPropPage3)

// Construction
public:
	COpzioniCompilPropPage3();
	~COpzioniCompilPropPage3();

// Dialog Data
	//{{AFX_DATA(COpzioniCompilPropPage3)
	enum { IDD = IDD_OPZIONI_COMPIL_PAGE3 };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(COpzioniCompilPropPage3)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(COpzioniCompilPropPage3)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPZIONICOMPIL_H__90A69200_A0C3_11D5_94AA_00A0C9AFFE49__INCLUDED_)

