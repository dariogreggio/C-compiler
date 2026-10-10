#include "stdafx.h"
#include "OpenC.h"
#include "OpenCdlg.h"
#include <afxdlgs.h>		// per CFolderPicker

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
// CMyFindReplaceDialog

BEGIN_MESSAGE_MAP(CMyFindReplaceDialog, CFindReplaceDialog)
	//{{AFX_MSG_MAP(CMyFindReplaceDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMyFindReplaceDialog::CMyFindReplaceDialog(BOOL bFindDialogOnly,COpenCDoc* pParent) 
	: CFindReplaceDialog() {

  // 1. Diciamo alla struttura di Windows di usare il NOSTRO template di risorse
  m_fr.Flags |= FR_ENABLETEMPLATE;
  m_fr.lpTemplateName = MAKEINTRESOURCE(IDD_MY_FIND_DIALOG);
  m_fr.hInstance = AfxGetInstanceHandle();

  m_fr.lpTemplateName = bFindDialogOnly ? MAKEINTRESOURCE(IDD_MY_REPLACE_DIALOG) : MAKEINTRESOURCE(IDD_MY_FIND_DIALOG);
	}

void CMyFindReplaceDialog::DoDataExchange(CDataExchange* pDX) {
	CFindReplaceDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyFindReplaceDialog)
// no!	DDX_Text(pDX, 1152, m_strSearch);
	//}}AFX_DATA_MAP
	}

BOOL CMyFindReplaceDialog::OnInitDialog() {

  CFindReplaceDialog::OnInitDialog();
  
	/*
// Sottoclasse la ComboBox con ID 1152
  m_cmbFindHistory.SubclassDlgItem(1152, this);

  // Ripopola la ComboBox con le ricerche precedenti
  POSITION pos = s_findHistory.GetHeadPosition();
  while (pos)
      m_cmbFindHistory.AddString(s_findHistory.GetNext(pos));

  // Se c'è almeno un elemento, seleziona il più recente
  if (m_cmbFindHistory.GetCount() > 0)
      m_cmbFindHistory.SetCurSel(0);
  */

		// Qui puoi personalizzare la grafica al volo, o impostare lo stato dei TUOI controlli extra
  // es: CheckDlgButton(IDC_CHK_ALL_TABS, BST_CHECKED);

  return TRUE;
	}

/*
void CCustomFindDlg::OnFindNext() {	// o nel gestore del bottone "Trova"
  CString strFind;
  m_cmbFindHistory.GetWindowText(strFind);

  strFind.Trim();
  if(!strFind.IsEmpty())    {
    // Rimuovi duplicati esistenti
    POSITION pos = s_findHistory.Find(strFind);
    if (pos)
        s_findHistory.RemoveAt(pos);

    // Aggiungi in cima alla cronologia
    s_findHistory.AddHead(strFind);

    // Limita la cronologia ad esempio agli ultimi 10-15 elementi
    while(s_findHistory.GetCount() > 15)
      s_findHistory.RemoveTail();
		}

  // Prosegui con la ricerca standard
  CFindReplaceDialog::OnFindNext();
	}*/

/////////////////////////////////////////////////////////////////////////////
// CMyFindInFilesDialog

BEGIN_MESSAGE_MAP(CMyFindInFilesDialog, CDialog)
	//{{AFX_MSG_MAP(CMyFindInFilesDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMyFindInFilesDialog::CMyFindInFilesDialog(CWnd *pParent) 
	: CDialog(CMyFindInFilesDialog::IDD, pParent) {

	m_strSearch = _T("");
	}

void CMyFindInFilesDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyFindInFilesDialog)
	DDX_Text(pDX, 1152, m_strSearch);
	//}}AFX_DATA_MAP
	}

BOOL CMyFindInFilesDialog::OnInitDialog() {

  CDialog::OnInitDialog();

	CenterWindow();

	SetWindowText("Cerca in tutti i file");

	m_strSearch=theApp.m_strLastSearch;
  
  // Qui puoi personalizzare la grafica al volo, o impostare lo stato dei TUOI controlli extra
  // es: CheckDlgButton(IDC_CHK_ALL_TABS, BST_CHECKED);

  return TRUE;
	}


/////////////////////////////////////////////////////////////////////////////
CGotoLineDlg::CGotoLineDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CGotoLineDlg::IDD, pParent) {

	//{{AFX_DATA_INIT(CGotoLineDlg)
	m_nLineNumber = FALSE;
	//}}AFX_DATA_INIT
  m_nLineNumber = 1;
	}

void CGotoLineDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);

  // Collega l'Edit box alla variabile intera con validazione automatica (DDX/DDV)
	//{{AFX_DATA_MAP(CGotoLineDlg)
  DDX_Text(pDX, IDC_EDIT_LINE, m_nLineNumber);
  DDV_MinMaxInt(pDX, m_nLineNumber, 1, 1000000); // Evita valori <= 0
	//}}AFX_DATA_MAP
	}

BEGIN_MESSAGE_MAP(CGotoLineDlg, CDialog)
	//{{AFX_MSG_MAP(CGotoLineDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CGotoLineDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  
  // Seleziona il testo nell'Edit Box così l'utente può sovrascriverlo subito
  CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_LINE);
  if(pEdit) {
    pEdit->SetSel(0, -1);
    pEdit->SetFocus();
		}
  return FALSE; // Abbiamo impostato noi il focus manualmente
	}


/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropPage1 property page

IMPLEMENT_DYNCREATE(COpzioniCompilPropPage1, CPropertyPage)

COpzioniCompilPropPage1::COpzioniCompilPropPage1() : CPropertyPage(COpzioniCompilPropPage1::IDD)
{
	//{{AFX_DATA_INIT(COpzioniCompilPropPage1)
	m_RicaricaProgettoPartenza = FALSE;
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
	DDX_Control(pDX, IDC_COMBO5, m_CartellaLibrerie);
	DDX_Control(pDX, IDC_COMBO4, m_CartellaInclude);
	DDX_Control(pDX, IDC_COMBO1, m_CCcombo);
	DDX_Check(pDX, IDC_CHECK2, m_RicaricaProgettoPartenza);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpzioniCompilPropPage1, CPropertyPage)
	//{{AFX_MSG_MAP(COpzioniCompilPropPage1)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
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
	
	_tcscpy(myBuf,"cc*.dll");
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
	m_RicaricaProgettoPartenza=theApp.AutoRicaricaProgetto;
	m_CartellaInclude.SelectString(0,theApp.CartellaInclude);
	m_CartellaLibrerie.SelectString(0,theApp.CartellaLibrerie);

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
	m_InserisciCommenti = FALSE;
	m_WarningErrori = FALSE;
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
	DDX_Check(pDX, IDC_CHECK19, m_InserisciCommenti);
	DDX_Check(pDX, IDC_CHECK18, m_WarningErrori);
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
	m_InserisciCommenti=theApp.Opzioni & COpenCApp::preProcCommenti ? 1 : 0;
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
	m_AbsRel=theApp.AbsRel;
	m_Warning=theApp.Warning;

	isInitialized=TRUE;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}


/////////////////////////////////////////////////////////////////////////////
// COpzioniCompilPropPage3 property page

IMPLEMENT_DYNCREATE(COpzioniCompilPropPage3, CPropertyPage)

COpzioniCompilPropPage3::COpzioniCompilPropPage3() : CPropertyPage(COpzioniCompilPropPage3::IDD)
{
	//{{AFX_DATA_INIT(COpzioniCompilPropPage3)
	m_TabSize = 0;
	m_TestoColorato = FALSE;
	m_SalvaCompila = FALSE;
	m_SalvaChiedi = FALSE;
	m_RicaricaFileAuto = FALSE;
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
	DDX_Text(pDX, IDC_EDIT1, m_TabSize);
	DDX_Check(pDX, IDC_CHECK1, m_TestoColorato);
	DDX_Check(pDX, IDC_CHECK2, m_SalvaCompila);
	DDX_Check(pDX, IDC_CHECK3, m_SalvaChiedi);
	DDX_Check(pDX, IDC_CHECK4, m_RicaricaFileAuto);
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

	m_TestoColorato = theApp.TestoColorato;
	m_TabSize = 2;		// finire :)

//	m_SalvaCompila = ;
//	m_SalvaChiedi = ;
	m_RicaricaFileAuto = theApp.AutoRicaricaFiles;
	m_TabSize=theApp.lTabSize;

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



void COpzioniCompilPropPage1::OnButton1() {
// non lo trova mai...	CFolderPickerDialog myDlg(		);
	BROWSEINFO bi;
	char lpBuffer[256];
	LPITEMIDLIST lpdi;

	ZeroMemory(&bi,sizeof(bi));
	bi.hwndOwner=m_hWnd;
	bi.pidlRoot=NULL; //DESKTOP
	bi.pszDisplayName=lpBuffer;
	bi.lpszTitle="Scegliere la cartella:";
	bi.ulFlags=0;
	bi.iImage=NULL;
	if(lpdi=SHBrowseForFolder(&bi)) {
		if(SHGetPathFromIDList(lpdi, lpBuffer)) {
//			m_CartellaLibrerie=lpBuffer;
 			UpdateData(FALSE);
			}
		else
			AfxMessageBox("Selezionare una posizione valida!",MB_ICONEXCLAMATION);
		}

	
	}

void COpzioniCompilPropPage1::OnButton2() {
// non lo trova mai...	CFolderPickerDialog myDlg(		);
	BROWSEINFO bi;
	char lpBuffer[256];
	LPITEMIDLIST lpdi;

	ZeroMemory(&bi,sizeof(bi));
	bi.hwndOwner=m_hWnd;
	bi.pidlRoot=NULL; //DESKTOP
	bi.pszDisplayName=lpBuffer;
	bi.lpszTitle="Scegliere la cartella:";
	bi.ulFlags=0;
	bi.iImage=NULL;
	if(lpdi=SHBrowseForFolder(&bi)) {
		if(SHGetPathFromIDList(lpdi, lpBuffer)) {
//			m_CartellaLibrerie=lpBuffer;
 			UpdateData(FALSE);
			}
		else
			AfxMessageBox("Selezionare una posizione valida!",MB_ICONEXCLAMATION);
		}
	
	}
