#include "stdafx.h"
#include "OpenC.h"
#include "OpenCdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg dialog


CSplashDlg::CSplashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSplashDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSplashDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSplashDlg, CDialog)
	//{{AFX_MSG_MAP(CSplashDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropPage1 property page

IMPLEMENT_DYNCREATE(COpzioniCompilPropPage1, CPropertyPage)

COpzioniCompilPropPage1::COpzioniCompilPropPage1() : CPropertyPage(COpzioniCompilPropPage1::IDD)
{
	//{{AFX_DATA_INIT(COpzioniCompilPropPage1)
	//}}AFX_DATA_INIT
	isInitialized=FALSE;
}

COpzioniCompilPropPage1::~COpzioniCompilPropPage1()
{
}

void COpzioniCompilPropPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpzioniCompilPropPage1)
	DDX_Control(pDX, IDC_COMBO1, m_CCcombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpzioniCompilPropPage1, CPropertyPage)
	//{{AFX_MSG_MAP(COpzioniCompilPropPage1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropPage1 message handlers
BOOL COpzioniCompilPropPage1::OnInitDialog() {
	char myBuf[256],myBuf2[256],myBuf3[256];
	HINSTANCE hInst;
	HANDLE hFile;
	WIN32_FIND_DATA wfd;
	typedef DWORD (__stdcall *VerFunc)(char *,char *);		// "stdcall" serve proprio!!
	VerFunc f;

	CPropertyPage::OnInitDialog();
	
	strcpy(myBuf,"cc*.dll");
	if((hFile=FindFirstFile(myBuf,&wfd)) != INVALID_HANDLE_VALUE) {
		do {
			hInst=LoadLibrary(wfd.cFileName);
			if(hInst) {
				f=(VerFunc)GetProcAddress(hInst,"GetVersione");
				if(f) {
					(*f)(myBuf2,myBuf3);
					strcpy(myBuf,wfd.cFileName);
					strcat(myBuf,": ");
					strcat(myBuf,myBuf2);
					strcat(myBuf," v");
					strcat(myBuf,myBuf3);
					m_CCcombo.AddString(myBuf);
					}
				FreeLibrary(hInst);
				}
			} while(FindNextFile(hFile,&wfd));
		FindClose(hFile);
		}
	m_CCcombo.SelectString(0,theApp.ccName);

	isInitialized=TRUE;
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}


void COpzioniCompilPropPage1::OnOK() {
	int i;

	i=m_CCcombo.GetCurSel();
	if(i>=0)
		m_CCcombo.GetLBText(i,theCC);
	
	CPropertyPage::OnOK();
	}


/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropPage2 property page

IMPLEMENT_DYNCREATE(COpzioniCompilPropPage2, CPropertyPage)

COpzioniCompilPropPage2::COpzioniCompilPropPage2() : CPropertyPage(COpzioniCompilPropPage2::IDD)
{
	//{{AFX_DATA_INIT(COpzioniCompilPropPage2)
	m_SoloPre = FALSE;
	m_NoMacro = FALSE;
	m_SynCheckOnly = FALSE;
	m_CheckStack = FALSE;
	m_CheckPtr = FALSE;
	m_CharUnsigned = FALSE;
	m_MultipleStrings = FALSE;
	m_PascalCalls = FALSE;
	m_InlineCalls = FALSE;
	m_OutSource = FALSE;
	m_OutAsm = FALSE;
	m_OutListing = FALSE;
	m_OttimizzaLoop = FALSE;
	m_Debug = FALSE;
	m_AltreDefine = _T("");
	m_MemoryModel = -1;
	m_OttimizzaCostanti = FALSE;
	m_OttimizzaDimensione = FALSE;
	m_OttimizzaVelocita = FALSE;
	m_AbsRel = -1;
	m_Warning = -1;
	//}}AFX_DATA_INIT
	isInitialized=FALSE;
}

COpzioniCompilPropPage2::~COpzioniCompilPropPage2()
{
}

void COpzioniCompilPropPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpzioniCompilPropPage2)
	DDX_Check(pDX, IDC_CHECK5, m_SoloPre);
	DDX_Check(pDX, IDC_CHECK9, m_NoMacro);
	DDX_Check(pDX, IDC_CHECK12, m_SynCheckOnly);
	DDX_Check(pDX, IDC_CHECK1, m_CheckStack);
	DDX_Check(pDX, IDC_CHECK2, m_CheckPtr);
	DDX_Check(pDX, IDC_CHECK4, m_CharUnsigned);
	DDX_Check(pDX, IDC_CHECK13, m_MultipleStrings);
	DDX_Check(pDX, IDC_CHECK3, m_PascalCalls);
	DDX_Check(pDX, IDC_CHECK10, m_InlineCalls);
	DDX_Check(pDX, IDC_CHECK8, m_OutSource);
	DDX_Check(pDX, IDC_CHECK7, m_OutAsm);
	DDX_Check(pDX, IDC_CHECK6, m_OutListing);
	DDX_Check(pDX, IDC_CHECK11, m_OttimizzaLoop);
	DDX_Check(pDX, IDC_CHECK14, m_Debug);
	DDX_Text(pDX, IDC_EDIT1, m_AltreDefine);
	DDV_MaxChars(pDX, m_AltreDefine, 200);
	DDX_CBIndex(pDX, IDC_COMBO2, m_MemoryModel);
	DDX_Check(pDX, IDC_CHECK17, m_OttimizzaCostanti);
	DDX_Check(pDX, IDC_CHECK16, m_OttimizzaDimensione);
	DDX_Check(pDX, IDC_CHECK15, m_OttimizzaVelocita);
	DDX_Radio(pDX, IDC_RADIO1, m_AbsRel);
	DDX_CBIndex(pDX, IDC_COMBO3, m_Warning);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpzioniCompilPropPage2, CPropertyPage)
	//{{AFX_MSG_MAP(COpzioniCompilPropPage2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropPage2 message handlers

BOOL COpzioniCompilPropPage2::OnInitDialog() {

	CPropertyPage::OnInitDialog();
	
	m_Debug=theApp.Opzioni & COpenCApp::debugMode ? 1 :0;
	m_SoloPre=theApp.Opzioni &  COpenCApp::preProcOnly ? 1 : 0;
	m_NoMacro=theApp.Opzioni & COpenCApp::noMacro ? 1 : 0;
	m_SynCheckOnly=theApp.Opzioni & COpenCApp::synCheckOnly ? 1 : 0;
	m_CheckStack=theApp.Opzioni & COpenCApp::checkStack ? 1 : 0;
	m_CheckPtr=theApp.Opzioni & COpenCApp::checkPtr ? 1 : 0;
	m_CharUnsigned=theApp.Opzioni & COpenCApp::charUnsigned ? 1 : 0;
	m_MultipleStrings=theApp.Opzioni & COpenCApp::multipleStrings ? 1 : 0;
	m_PascalCalls=theApp.Opzioni & COpenCApp::pascalCalls ? 1 : 0;
	m_InlineCalls=theApp.Opzioni & COpenCApp::inlineCalls ? 1 : 0;
	m_OutSource=theApp.Opzioni & COpenCApp::outSource ? 1 : 0;
	m_OutAsm=theApp.Opzioni & COpenCApp::outAsm ? 1 : 0;
	m_OutListing=theApp.Opzioni & COpenCApp::outListing ? 1 : 0;
	m_OttimizzaLoop=theApp.Opzioni & COpenCApp::ottimizzaLoop ? 1 : 0;
	m_OttimizzaVelocita=theApp.Opzioni & COpenCApp::ottimizzaSpeed ? 1 : 0;
	m_OttimizzaDimensione=theApp.Opzioni & COpenCApp::ottimizzaSize ? 1 : 0;
	m_OttimizzaCostanti=theApp.Opzioni & COpenCApp::ottimizzaConst ? 1 : 0;
	m_AltreDefine=theApp.altreDefine;

	m_MemoryModel=theApp.MemoryModel;
	m_Warning=theApp.Warning;

	isInitialized=TRUE;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}

/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropSheet

IMPLEMENT_DYNAMIC(COpzioniCompilPropSheet, CPropertySheet)

COpzioniCompilPropSheet::COpzioniCompilPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

COpzioniCompilPropSheet::COpzioniCompilPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

COpzioniCompilPropSheet::~COpzioniCompilPropSheet()
{
}


BEGIN_MESSAGE_MAP(COpzioniCompilPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(COpzioniCompilPropSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropSheet message handlers


/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropPage3 property page

IMPLEMENT_DYNCREATE(COpzioniCompilPropPage3, CPropertyPage)

COpzioniCompilPropPage3::COpzioniCompilPropPage3() : CPropertyPage(COpzioniCompilPropPage3::IDD)
{
	//{{AFX_DATA_INIT(COpzioniCompilPropPage3)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	isInitialized=FALSE;
}

COpzioniCompilPropPage3::~COpzioniCompilPropPage3()
{
}

void COpzioniCompilPropPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpzioniCompilPropPage3)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpzioniCompilPropPage3, CPropertyPage)
	//{{AFX_MSG_MAP(COpzioniCompilPropPage3)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropPage3 message handlers

BOOL COpzioniCompilPropPage3::OnInitDialog() {

	CPropertyPage::OnInitDialog();

	isInitialized=TRUE;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}


