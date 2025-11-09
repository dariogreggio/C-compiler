// Versione Windows: 23/9/1996
// 7/9/2001:	v2.0
// OpenC.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OpenC.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "OpenCDoc.h"
#include "OpenCView.h"
#include "OpenCView2.h"
#include "OpenCdlg.h"
#include <mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenCApp

BEGIN_MESSAGE_MAP(COpenCApp, CWinAppEx)
	//{{AFX_MSG_MAP(COpenCApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_STRUMENTI_OPZIONI, OnStrumentiOpzioni)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_UPDATE_COMMAND_UI(ID_FILE_APRIPROGETTO, OnUpdateFileApriprogetto)
	ON_COMMAND(ID_FILE_APRIPROGETTO, OnFileApriprogetto)
	ON_COMMAND(ID_FILE_NUOVO, OnFileNuovo)
	ON_UPDATE_COMMAND_UI(ID_FILE_NUOVO, OnUpdateFileNuovo)
	ON_COMMAND(ID_COMPILA_TUTTO, OnCompilaTutto)
	ON_UPDATE_COMMAND_UI(ID_COMPILA_TUTTO, OnUpdateCompilaTutto)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenCApp construction

COpenCApp::COpenCApp() {

	variabiliKey="variabili";
	fileApertiKey="fileAperti";
	}

/////////////////////////////////////////////////////////////////////////////
// The one and only COpenCApp object

COpenCApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COpenCApp initialization

BOOL COpenCApp::InitInstance() {
	RECT rc;
	int i;
	CSplashDlg *splashDlg=NULL;
	char myBuf[128];

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	EnableLoadWindowPlacement(TRUE);		// qua lo voglio
	{
	CRect rc;		// ... mettere RC in winApp??
	int nFlags,nCmd;
	rc.bottom=min(GetSystemMetrics(SM_CYSCREEN)-48,600);
	rc.right=min(GetSystemMetrics(SM_CXSCREEN)-24,800);
	rc.left=100;
	rc.top=100;
	if(theApp.m_bLoadWindowPlacement)
		theApp.LoadWindowPlacement(rc,nFlags,nCmd);
	}

	SetRegistryBase(_T("ADPM Synthesis"));


	LoadStdProfileSettings(8);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	pDocTemplate = new CMultiDocTemplate(
		IDR_CTYPE,
		RUNTIME_CLASS(COpenCDoc),
		RUNTIME_CLASS(CChildFrame), // base MDI child frame
		RUNTIME_CLASS(COpenCView));
	AddDocTemplate(pDocTemplate);
	pDocTemplate2 = new CMultiDocTemplate(
		IDR_CTYPE2,
		RUNTIME_CLASS(COpenCDoc2),
		RUNTIME_CLASS(CChildFrame2), // per output MDI child frame
		RUNTIME_CLASS(COutputView));
	AddDocTemplate(pDocTemplate2);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;


#ifndef _DEBUG
	if(cmdInfo.m_bShowSplash)
		splashDlg=new CSplashDlg;
	if(splashDlg) {
		splashDlg->Create(IDD_SPLASH);
//		splashDlg->SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOREDRAW | SWP_NOMOVE | SWP_NOSIZE);
		}
#endif

  i= GetPrivateProfileString(variabiliKey,IDS_NOMECC,myBuf,128);
	ccName=myBuf;
  i= GetPrivateProfileString(variabiliKey,IDS_ALTREDEFINE,myBuf,128);
	altreDefine=myBuf;
	Opzioni=GetPrivateProfileInt(variabiliKey,IDS_OPZIONI);
	MemoryModel=GetPrivateProfileInt(variabiliKey,IDS_MEMORYMODEL);

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	if(splashDlg)
		splashDlg->DestroyWindow();
	delete splashDlg;

	return TRUE;
	}



int COpenCApp::ExitInstance() {

	WritePrivateProfileInt(variabiliKey,IDS_OPZIONI,Opzioni);
	WritePrivateProfileInt(variabiliKey,IDS_MEMORYMODEL,MemoryModel);
	WritePrivateProfileString(variabiliKey,IDS_ALTREDEFINE,(LPCTSTR)altreDefine);
	WritePrivateProfileString(variabiliKey,IDS_NOMECC,(LPCTSTR)ccName);
	return CWinApp::ExitInstance();
	}


char *COpenCApp::getProfileKey(char *d,const char *s) {
	strcpy(d,m_pszRegistryKey);
	strcat(d,"\\");
	strcat(d,m_pszAppName);
	strcat(d,"\\");
	strcat(d,"Settings");
	if(s) {
		strcat(d,"\\");
		strcat(d,s);
		}
	return d;
	}

int COpenCApp::WritePrivateProfileString(const char *s,const char *v,const char *n) {
	char myBuf[256];
	HKEY pk,pksub;
	int i,retVal=-1;

	getProfileKey(myBuf,s);
	if(!RegCreateKeyEx(HKEY_CURRENT_USER,"Software",0L,"",REG_OPTION_NON_VOLATILE,
		KEY_WRITE,NULL,&pk,NULL)) {
		if(v) {
			if(!RegCreateKeyEx(pk,myBuf,0L,"",REG_OPTION_NON_VOLATILE,
				KEY_WRITE,NULL,&pksub,NULL)) {
					retVal=RegSetValueEx(pksub,v,0,REG_SZ,(const BYTE *)n,strlen(n));
				RegCloseKey(pksub);
				}
			}
		else {
			RegDeleteKey(pk,myBuf);
			}
		RegCloseKey(pk);
		}

	return retVal;
  }

int COpenCApp::WritePrivateProfileInt(const char *s,const char *v,int n) {
	char myBuf[256];
	HKEY pk,pksub;
	int i,retVal=-1;

	getProfileKey(myBuf,s);
	if(!RegCreateKeyEx(HKEY_CURRENT_USER,"Software",0L,"",REG_OPTION_NON_VOLATILE,
		KEY_WRITE,NULL,&pk,NULL)) {
		if(!RegCreateKeyEx(pk,myBuf,0L,"",REG_OPTION_NON_VOLATILE,
			KEY_WRITE,NULL,&pksub,NULL)) {
			retVal=RegSetValueEx(pksub,v,0,REG_DWORD,(const BYTE *)&n,4);
			RegCloseKey(pksub);
			}
		RegCloseKey(pk);
		}

	return retVal;
  }

int COpenCApp::GetPrivateProfileString(const char *s, const char *k, char *v, int len, char *def) {
	char myBuf[256];
	HKEY pk,pksub;
	int i,retVal=-1;
	DWORD vType,vLen=len;

	getProfileKey(myBuf,s);
	if(def)
		strcpy(v,def);
	else
		*v=0;
	if(!RegOpenKeyEx(HKEY_CURRENT_USER,"Software",0L,KEY_READ,&pk)) {
		if(!RegOpenKeyEx(pk,myBuf,0L,KEY_READ,&pksub)) {
			retVal=RegQueryValueEx(pksub,k,0,NULL,(BYTE *)v,&vLen);
			RegCloseKey(pksub);
			}
		RegCloseKey(pk);
		}
	return retVal;
	}

int COpenCApp::GetPrivateProfileInt(const char *s, const char *k, int def) {
	char myBuf[256];
	HKEY pk,pksub;
	int i,v,retVal=def;
	DWORD vType,vLen=4;

	getProfileKey(myBuf,s);
	v=def;
	if(!RegOpenKeyEx(HKEY_CURRENT_USER,"Software",0L,KEY_READ,&pk)) {
		if(!RegOpenKeyEx(pk,myBuf,0L,KEY_READ,&pksub)) {
			i=RegQueryValueEx(pksub,k,0,NULL,(BYTE *)&v,&vLen);
			if(!i) {
				retVal=v;
				}
			RegCloseKey(pksub);
			}
		RegCloseKey(pk);
		}
	return retVal;
	}

CTime COpenCApp::GetPrivateProfileTime(char *s,char *v) {
	char myBuf[64];
	int i,h,d,m,y;

	GetPrivateProfileString(s,v,myBuf,16,"" /*"01/01/1997 00:00"*/);
	d=*myBuf ? atoi(myBuf) : 1;
	m=*myBuf ? atoi(myBuf+3) : 1;
	y=*myBuf ? atoi(myBuf+6) : 1997;
	h=*myBuf ? atoi(myBuf+11) : 0;
	i=*myBuf ? atoi(myBuf+14) : 0;
	{
		CTime t(y,m,d,h,i,0);
		return t;
		}
	}

CTimeSpan COpenCApp::GetPrivateProfileTimeSpan(char *s,char *v) {
	char myBuf[64];
	int i,h,m;

	GetPrivateProfileString(s,v,myBuf,8,"" /*"00:00"*/);
	h=*myBuf ? atoi(myBuf) : 1;
	m=*myBuf ? atoi(myBuf+3) : 1;
	{
		CTimeSpan ts(0,h,m,0);
		return ts;
		}
	}

int COpenCApp::WritePrivateProfileTime(char *s,char *v,CTime t) {
	CString c;

	c=t > 0 ? t.Format("%d/%m/%Y %H:%M") : "";
	return WritePrivateProfileString(s,v,(LPCTSTR)c);
	}

int COpenCApp::WritePrivateProfileTime(char *s,char *v,CTimeSpan t) {
	CString c;

	c=t.Format("%H:%M");
	return WritePrivateProfileString(s,v,(LPCTSTR)c);
	}


char *COpenCApp::getNow(char *s) {
	int i;

	_strdate(s);
	i=s[0];
	s[0]=s[3];
	s[3]=i;
	i=s[1];
	s[1]=s[4];
	s[4]=i;
	_strtime(s+9);
	s[8]=' ';

	return s;
	}

char *COpenCApp::getNowGMT(char *myBuf) {
	time_t aclock;
	struct tm *newtime;
	int i;

	time(&aclock);                 // Get time in seconds 
	newtime = localtime(&aclock);  // Convert time to struct tm form 
	strcpy(myBuf,asctime(newtime));
	i=-(_timezone/3600);
	if(!i)
		strcpy(myBuf+24," GMT\xd\xa");
	else
		wsprintf(myBuf+24," %c%02d00\xd\xa",i>=0 ? '+' : '-',abs(i));

	return myBuf;
	}

void COpenCApp::WriteOutputWndText(char *s,int n) {
	
	theOutput->AddText(s,n);
	}

void COpenCApp::ClearOutputWnd() {
	
	theOutput->Cls();
	}


/////////////////////////////////////////////////////////////////////////////
// COpenCApp commands

// App command to run the dialog
void COpenCApp::OnAppAbout() {
	CAboutDlg aboutDlg;

	aboutDlg.DoModal();
	}

void COpenCApp::OnFileNew() {
	char myBuf[256],myBuf1[64];
	int i;
	
	theOutput=(COpenCDoc2 *)pDocTemplate2->OpenDocumentFile(NULL);
	i=0;
	do {
		wsprintf(myBuf1,"File%u",i);
		theApp.GetPrivateProfileString(fileApertiKey,myBuf1,myBuf,256);
		if(*myBuf)
			pDocTemplate->OpenDocumentFile(myBuf);
		i++;
		} while(*myBuf);

	}

void COpenCApp::OnStrumentiOpzioni() {
	int i;
	COpzioniCompilPropSheet mySheet("Opzioni",m_pMainWnd);
	COpzioniCompilPropPage1 myPage0;
	COpzioniCompilPropPage2 myPage1;
	COpzioniCompilPropPage3 myPage2;
	
	mySheet.AddPage(&myPage0);
	mySheet.AddPage(&myPage1);
	mySheet.AddPage(&myPage2);
	if(mySheet.DoModal() == IDOK) {
		if(myPage0.isInitialized) {
			ccName=myPage0.theCC.Left(myPage0.theCC.Find(':'));
			}
		if(myPage1.isInitialized) {
			Opzioni=0;
			Opzioni |= myPage1.m_Debug ? debugMode : 0;
			Opzioni |= myPage1.m_SoloPre ? preProcOnly : 0;
			Opzioni |= myPage1.m_NoMacro ? noMacro : 0;
			Opzioni |= myPage1.m_SynCheckOnly ? synCheckOnly : 0;
			Opzioni |= myPage1.m_CheckStack ? checkStack : 0;
			Opzioni |= myPage1.m_CheckPtr ? checkPtr : 0;
			Opzioni |= myPage1.m_CharUnsigned ? charUnsigned : 0;
			Opzioni |= myPage1.m_MultipleStrings ? multipleStrings : 0;
			Opzioni |= myPage1.m_PascalCalls ? pascalCalls : 0;
			Opzioni |= myPage1.m_InlineCalls ? inlineCalls : 0;
			Opzioni |= myPage1.m_OutSource ? outSource : 0;
			Opzioni |= myPage1.m_OutAsm ? outAsm : 0;
			Opzioni |= myPage1.m_OutListing ? outListing : 0;
			Opzioni |= myPage1.m_OttimizzaLoop ? ottimizzaLoop : 0;
			Opzioni |= myPage1.m_OttimizzaVelocita ? ottimizzaSpeed : 0;
			Opzioni |= myPage1.m_OttimizzaDimensione ? ottimizzaSize : 0;
			Opzioni |= myPage1.m_OttimizzaCostanti ? ottimizzaConst : 0;
			altreDefine=myPage1.m_AltreDefine;
			MemoryModel=myPage1.m_MemoryModel;
			}
		if(myPage2.isInitialized) {
			}
		}
	}




void COpenCApp::OnUpdateFileApriprogetto(CCmdUI* pCmdUI) {
	
	}

void COpenCApp::OnFileApriprogetto() 
{
	// TODO: Add your command handler code here
	
}

void COpenCApp::OnFileNuovo() {
	
	pDocTemplate->OpenDocumentFile(NULL);
	}

void COpenCApp::OnUpdateFileNuovo(CCmdUI* pCmdUI) {
	
	}



void COpenCApp::OnCompilaTutto() {
	
	pDocTemplate->SaveAllModified();
	}

void COpenCApp::OnUpdateCompilaTutto(CCmdUI* pCmdUI) {

	pCmdUI->Enable(!ccName.IsEmpty());
	}



bool CWinAppEx::ReloadWindowPlacement(class CFrameWnd *w) {
	CRect r;
	int n,n2;
	return LoadWindowPlacement(r,n,n2);  // boh...
	}
bool CWinAppEx::StoreWindowPlacement(class CRect const &rc,int n,int n2) {
	CString S,S1;
	char myBuf[64];

	if(!n2) {
		S.LoadString(IDS_OPZIONI);
		S1.LoadString(IDS_COORDINATE);
		wsprintf(myBuf,"%d,%d,%d,%d",rc.left,rc.top,rc.right,rc.bottom);
		theApp. /*prStore->*/ WriteProfileString(S,S1,myBuf);
		}

	return 1;
	}
bool CWinAppEx::LoadWindowPlacement(class CRect &rc,int &n,int &n2) {
	CString S,S1,S2;

	S.LoadString(IDS_OPZIONI);
	S1.LoadString(IDS_COORDINATE);
	S2=theApp.GetProfileString(S,S1,"100,100");
	return sscanf((LPCTSTR)S2,"%d,%d,%d,%d",&rc.left,&rc.top,&rc.right,&rc.bottom);
	}
void CWinAppEx::OnClosingMainFrame() {
	CRect r;
	if(m_bLoadWindowPlacement) {
		if(m_pMainWnd) {
			m_pMainWnd->GetWindowRect(&r);
			StoreWindowPlacement(r,0,m_pMainWnd->IsIconic() || m_pMainWnd->IsZoomed());
			}
		}
	}
void CWinAppEx::OnAppContextHelp(class CWnd *w,unsigned long const * const n) {
	}
bool CWinAppEx::ShowPopupMenu(unsigned int n,class CPoint const &pt,class CWnd *w) {
	CMenu myMenu;
	// n FINIRE se serve :)
	myMenu.GetSubMenu(2)->GetSubMenu(2)->GetSubMenu(0)->TrackPopupMenu(TPM_LEFTBUTTON | TPM_LEFTALIGN, pt.x, pt.y, m_pMainWnd);
	return 1;
	}
bool CWinAppEx::OnViewDoubleClick(class CWnd *w,int n) {
	return 1;
	}
bool CWinAppEx::CleanState(char const *s) {
	return 1;
	}
struct CRuntimeClass *CWinAppEx::GetRuntimeClass() const {
	return CWinApp::GetRuntimeClass();
	}
CWinAppEx::CWinAppEx(bool n) : m_bResourceSmartUpdate(0),CWinApp() {
	}
int CWinAppEx::ExitInstance() {
	CRect r;
	if(m_bSaveState)
		SaveCustomState();
	return CWinApp::ExitInstance();
	}
CWinAppEx::~CWinAppEx() {
	}
bool CWinAppEx::WriteInt(LPCTSTR lpszEntry, int nValue) {
	return 0;
	}
bool CWinAppEx::WriteString(LPCTSTR lpszEntry, LPCTSTR lpszValue) {
	CString S,S1;
	S.LoadString(IDS_OPZIONI);
	return theApp. /*prStore->*/ WriteProfileString(S,lpszEntry,lpszValue);
	}
bool CWinAppEx::WriteSectionInt(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, int nValue) {
	return 0;
	}
bool CWinAppEx::WriteSectionString(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPCTSTR lpszValue) {
	return theApp. /*prStore->*/ WriteProfileString(lpszSubSection,lpszEntry,lpszValue);
	}
int CWinAppEx::GetInt(LPCTSTR lpszEntry, int nDefault) {
	return 0;
	}
CString CWinAppEx::GetString(LPCTSTR lpszEntry, LPCTSTR lpszDefault) {
	CString S;
	return S;
	}
int CWinAppEx::GetSectionInt(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, int nDefault) {
	return 0;
	}
CString CWinAppEx::GetSectionString(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault) {
	CString S;
	return S;
	}
LPCTSTR CWinAppEx::SetRegistryBase(LPCTSTR lpszSectionName) {
	SetRegistryKey(lpszSectionName);
	return m_pszRegistryKey;
	}
CString CWinAppEx::GetRegSectionPath(LPCTSTR szSectionAdd) {
	CString S;
	S.LoadString(IDS_OPZIONI);
	return S;
	}


// ----------------------------------------------------------------------------------------------------------------------------
CString CStringEx::Tokenize(CString delimiter, int& first) {
  CString token;
  int end = Find(delimiter, first);

  if(end != -1) {
    int count = end-first;
    token = Mid(first,count);
    first = end+delimiter.GetLength();
    return token;
	  }
  else {
    int count = GetLength() - first;
    if(count <= 0)
      return "";

    token = Mid(first,count);
    first = GetLength();
    return token;
		}
	}
CStringEx CStringEx::SubStr(int begin, int len) const {
	return CString::Mid(begin, len);
	}
int CStringEx::FindNoCase(CString substr,int start) {
	CString s1=*this,s2;
	s1.MakeUpper();
	s2=substr;
	s2.MakeUpper();
	return s1.Find(s2,start);
	}
int CStringEx::ReverseFindNoCase(CString substr) {
	CString s1=*this,s2;
	int start=s1.GetLength()-s2.GetLength();
	s1.MakeUpper();
	s2=substr;
	s2.MakeUpper();
	while(start>=0) {
		if(s1.Find(s2,start)>=0)
			return start;
		start--;
		}
	return -1;
	}
CStringEx::CStringEx(int i, const char *format, DWORD options) {

	Format(format,i);
	if(options & COMMA_DELIMIT)
		CString::operator=(CommaDelimitNumber(*this));
	}
CStringEx::CStringEx(double d, const char *format, DWORD options) {

	Format(format,d);
	if(options & COMMA_DELIMIT)
		CString::operator=(CommaDelimitNumber(*this));
	}
CStringEx CStringEx::CommaDelimitNumber(const char *s) {
	CStringEx s2=s;												// convert to CStringEx
	return CommaDelimitNumber(s2);
	}
CStringEx CStringEx::CommaDelimitNumber(CString s2) {
	CStringEx dp;
	CStringEx q2;											// working string
	CStringEx posNegChar=s2.Left(1);				// get the first char
	bool posNeg=!posNegChar.IsDigit(0);			// if not digit, then assume + or -

	if(posNeg) 											// if so, strip off
		s2=s2.Mid(1);
	if(s2.Find(decimalChar)>=0) {
		dp=s2.Mid(s2.Find(decimalChar)+1);							// remember everything to the right of the decimal point
		s2=s2.Left(s2.Find(decimalChar));				// get everything to the left of the first decimal point
		}
	while(s2.GetLength() > 3) {									// if more than three digits...
		CStringEx s3(thousandChar);
		s3+=s2.Right(3);		// insert a comma before the last three digits (100's)
		q2=s3+q2;											// append this to our working string
		s2=s2.Left(s2.GetLength()-3);							// get everything except the last three digits
		}
	q2=s2+q2;												// prepend remainder to the working string
	if(!dp.IsEmpty()) {									// if we have decimal point...
		q2+=decimalChar;							// append it and the digits
		q2+=dp;							// append it and the digits
		}
	if(posNeg)											// if we stripped off a +/- ...
		q2=posNegChar+q2;			// add it back in

	return q2;											// this is our final comma delimited string
	}

CStringEx CStringEx::CommaDelimitNumber(DWORD n) {
	CStringEx q2;

	q2.Format("%u",n);
	q2=CommaDelimitNumber(q2);
	return q2;
	}

BYTE CStringEx::Asc(int pos) {

	return GetAt(pos);
	}

int CStringEx::Val(int base) {

	switch(base) {
		case 10:
		default:
			return atoi((LPCTSTR)this);
			break;
		case 16:		// fare...
			break;
		}
	}

double CStringEx::Val() {

	return strtod((LPCTSTR)this,NULL);
	}

void CStringEx::Repeat(int n) {
	CString s2=*this;

	Empty();
	while(n--)
		CString::operator+=(s2);
	}

void CStringEx::Repeat(const char *s,int n) {

	Empty();
	while(n--)
		CString::operator+=(s);
	}

void CStringEx::Repeat(char c,int n) {

	Empty();
	while(n--)
		CString::operator+=(c);
	}

bool CStringEx::IsAlpha(char ch) {

	return (ch>='A' && ch<='Z') || (ch>='a' && ch<='z');
	}

bool CStringEx::IsAlpha(int pos) {

	return IsAlpha(GetAt(pos));
	}

bool CStringEx::IsAlnum(char ch) {

	return IsAlpha(ch) || IsDigit(ch);
	}

bool CStringEx::IsAlnum(int pos) {

	return IsAlnum(GetAt(pos));
	}

bool CStringEx::IsDigit(char ch) {

	return (ch>='0' && ch<='9');
	}

bool CStringEx::IsDigit(int pos) {

	return IsDigit(GetAt(pos));
	}

bool CStringEx::IsPrint(char ch) {

	return (ch>=' ' && ch<'\x7f');			//127 escluso
	}

bool CStringEx::IsPrint(int pos) {

	return IsPrint(GetAt(pos));
	}

void CStringEx::Print() {

	AfxMessageBox(*this);
	}

void CStringEx::Debug() {

#ifdef _DEBUG
	Print();
#endif
	}

WORD CStringEx::GetAsciiLength() {			// utile per saltare ESC ecc in stampa citofono LCD ecc
	WORD i,j;

	for(i=0,j=0; i<GetLength(); i++)
		if(IsPrint(i))
			j++;
	return j;
	}

// _T("%h %l %u %t \"%r\" %>s %b")		v. Apache...

CStringEx CStringEx::FormatTime(int m,CTime mT) {

	if(!(*((DWORD *)&mT)))
		mT=CTime::GetCurrentTime();

	switch(m) {
		case 0:
			CString::operator=(mT.Format("%d/%m/%Y %H:%M:%S"));
			break;
		case 1:
			Format(_T("%02u/%s/%02u:%02u:%02u:%02u %02d00"),
				mT.GetDay(),
"???",//				CTimeEx::Num2Month3(mT.GetMonth()), METTERE :D
				mT.GetCurrentTime().GetYear(),
				mT.GetCurrentTime().GetHour(),
				mT.GetMinute(),
				mT.GetSecond(),
				-(_timezone/3600)+(_daylight ? 1 : 0)
				);
			break;
		case 2:
			CString::operator=(mT.Format(_T("%a, %d %b %Y %H:%M:%S %Z")));
			break;
		}

	return *this;
	}


const char CStringEx::m_base64tab[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                      "abcdefghijklmnopqrstuvwxyz0123456789+/";
const int CStringEx::BASE64_MAXLINE=76;
const char *CStringEx::EOL="\r\n";
const char CStringEx::decimalChar=',',CStringEx::thousandChar='.';		// GetLocale ??
const char CStringEx::CRchar='\r',CStringEx::LFchar='\n',CStringEx::TABchar='\t';
CStringEx CStringEx::Encode64() {
	CStringEx S2;

  //Set up the parameters prior to the main encoding loop
  int nInPos  = 0;
  int nLineLen = 0;

  // Get three characters at a time from the input buffer and encode them
  for(int i=0; i<GetLength()/3; ++i) {

    //Get the next 2 characters
    int c1 = Asc(nInPos++) & 0xFF;
    int c2 = Asc(nInPos++) & 0xFF;
    int c3 = Asc(nInPos++) & 0xFF;

    //Encode into the 4 6 bit characters
    S2 += m_base64tab[(c1 & 0xFC) >> 2];
    S2 += m_base64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
    S2 += m_base64tab[((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6)];
    S2 += m_base64tab[c3 & 0x3F];
    nLineLen += 4;

    //Handle the case where we have gone over the max line boundary
    if(nLineLen >= BASE64_MAXLINE-3) {
      const char *cp = EOL;
      S2 += *cp++;
      if(*cp) {
        S2 += *cp;
				}
      nLineLen = 0;
			}
		}

  // Encode the remaining one or two characters in the input buffer
  const char *cp;
  switch(GetLength() % 3) {
    case 0:
      cp = EOL;
      S2 += *cp++;
      if(*cp) {
        S2 += *cp;
				}
      break;
    case 1:
    {
      int c1 = Asc(nInPos) & 0xFF;
      S2 += m_base64tab[(c1 & 0xFC) >> 2];
      S2 += m_base64tab[((c1 & 0x03) << 4)];
      S2 += '=';
      S2 += '=';
      cp = EOL;
      S2 += *cp++;
      if(*cp) {
        S2 += *cp;
				}
      break;
    }
    case 2:
    {
      int c1 = Asc(nInPos++) & 0xFF;
      int c2 = Asc(nInPos) & 0xFF;
      S2 += m_base64tab[(c1 & 0xFC) >> 2];
      S2 += m_base64tab[((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4)];
      S2 += m_base64tab[((c2 & 0x0F) << 2)];
      S2 += '=';
      cp = EOL;
      S2 += *cp++;
      if(*cp) {
        S2 += *cp;
				}
      break;
    }
    default: 
      ASSERT(FALSE); 
      break;
	  }

  CString::operator=(S2);
  return *this;
	}

int CStringEx::Decode64() {
	CStringEx sInput;
	int m_nBitsRemaining;
	ULONG m_lBitStorage;

  m_nBitsRemaining = 0;

	sInput=*this;
  Empty();  
	if(sInput.GetLength() == 0)
		return 0;

	//Build Decode Table
  int nDecode[256];
	for(int i=0; i<256; i++) 
		nDecode[i] = -2; // Illegal digit
	for(i=0; i<64; i++) {
		nDecode[m_base64tab[i]] = i;
		nDecode[m_base64tab[i] | 0x80] = i; // Ignore 8th bit
		nDecode['='] = -1; 
		nDecode['=' | 0x80] = -1; // Ignore MIME padding char
		}

	// Decode the Input
  i=0;
  TCHAR* szOutput = GetBuffer(sInput.GetLength());
	for(int p=0; p<sInput.GetLength(); p++) {
		int c = sInput[p];
		int nDigit = nDecode[c & 0x7F];
		if(nDigit < -1) {
      ReleaseBuffer();  
			return 0;
			}
		else if(nDigit >= 0) {
			// i (index into output) is incremented by write_bits()
//			WriteBits(nDigit & 0x3F, 6, szOutput, i);
			UINT nScratch;

			m_lBitStorage = (m_lBitStorage << 6) | (nDigit & 0x3F);
			m_nBitsRemaining += 6;
			while(m_nBitsRemaining > 7) {
				nScratch = m_lBitStorage >> (m_nBitsRemaining - 8);
				szOutput[i++] = (TCHAR) (nScratch & 0xFF);
				m_nBitsRemaining -= 8;
				}
			}
		}	
  szOutput[i] = _T('\0');
  ReleaseBuffer();

	return i;
	}

CString CStringEx::InsertSeparator(DWORD dwNumber) {

  Format("%u", dwNumber);
  
  for(int i=GetLength()-3; i > 0; i -= 3) {
    Insert(i, ",");
    }

  return *this;
  }

CStringEx CStringEx::FormatSize(DWORD dwFileSize) {
  static const DWORD dwKB = 1024;          // Kilobyte
  static const DWORD dwMB = 1024 * dwKB;   // Megabyte
  static const DWORD dwGB = 1024 * dwMB;   // Gigabyte

  DWORD dwNumber, dwRemainder;

  if(dwFileSize < dwKB) {
//    InsertSeparator(dwFileSize) + " B";		// non funziona (usare  *this o Format) e poi non mi piace!
    InsertSeparator(dwFileSize);
		} 
  else {
    if(dwFileSize < dwMB) {
      dwNumber = dwFileSize / dwKB;
      dwRemainder = (dwFileSize * 100 / dwKB) % 100;

      Format("%s.%02d KB", (LPCSTR)InsertSeparator(dwNumber), dwRemainder);
			}
    else {
      if(dwFileSize < dwGB) {
        dwNumber = dwFileSize / dwMB;
        dwRemainder = (dwFileSize * 100 / dwMB) % 100;
        Format("%s.%02d MB", InsertSeparator(dwNumber), dwRemainder);
				}
      else {
        if(dwFileSize >= dwGB) {
          dwNumber = dwFileSize / dwGB;
          dwRemainder = (dwFileSize * 100 / dwGB) % 100;
          Format("%s.%02d GB", InsertSeparator(dwNumber), dwRemainder);
					}
				}
			}
		}

  // Display decimal points only if needed
  // another alternative to this approach is to check before calling str.Format, and 
  // have separate cases depending on whether dwRemainder == 0 or not.
  Replace(".00", "");

	return *this;
	}



