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
//#include "OpenCView2.h"
#include "OpenCdlg.h"
#include <mmsystem.h>
#include <afxadv.h>		// per RecentFileList/progetti

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
	ON_COMMAND(ID_FILE_APRIPROGETTO, OnFileApriprogetto)
	ON_UPDATE_COMMAND_UI(ID_FILE_APRIPROGETTO, OnUpdateFileApriprogetto)
	ON_COMMAND(ID_COMPILA_TUTTO, OnCompilaProgetto)
	ON_UPDATE_COMMAND_UI(ID_COMPILA_TUTTO, OnUpdateCompilaProgetto)
	ON_COMMAND(ID_FILE_SALVAPROGETTO, OnFileSalvaprogetto)
	ON_UPDATE_COMMAND_UI(ID_FILE_SALVAPROGETTO, OnUpdateFileSalvaprogetto)
	ON_COMMAND(ID_FILE_CHIUDIPROGETTO, OnFileChiudiprogetto)
	ON_UPDATE_COMMAND_UI(ID_FILE_CHIUDIPROGETTO, OnUpdateFileChiudiprogetto)
	ON_COMMAND(ID_FILE_SALVAPROGETTOCONNOME, OnFileSalvaprogettoconnome)
	ON_UPDATE_COMMAND_UI(ID_FILE_SALVAPROGETTOCONNOME, OnUpdateFileSalvaprogettoconnome)
	ON_COMMAND(ID_FILE_NUOVOPROGETTO, OnFileNuovoprogetto)
	ON_COMMAND(ID_COMPILA_COMPILATUTTO, OnCompilaCompilatutto)
	ON_UPDATE_COMMAND_UI(ID_COMPILA_COMPILATUTTO, OnUpdateCompilaCompilatutto)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND_EX_RANGE(ID_PROJECT_MRU_1, ID_PROJECT_MRU_LAST, OnOpenRecentProject)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE1 + 7, OnUpdateRecentFileMenu)
  ON_UPDATE_COMMAND_UI_RANGE(ID_PROJECT_MRU_1, ID_PROJECT_MRU_LAST, OnUpdateRecentProjectMenu)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenCApp construction

COpenCApp::COpenCApp() {

	CoInitialize(NULL); //this must be called FIRST!


	//m_hinstRE41 is a HINSTANCE type member var of COpenCApp
	m_hinstRE41=LoadLibrary(TEXT("msftedit.dll"));			// per usare RichEdit più recenti!
	//this DLL must be loaded.
// ovvero	AfxInitRichEdit2();
	
	variabiliKey="variabili";
	fileApertiKey="fileAperti";
	}

COpenCApp::~COpenCApp() {
	if(m_hinstRE41)
		FreeLibrary(m_hinstRE41);
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
	CCommandLineInfoEx cmdInfo;
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


	m_pRecentProjectList = new CRecentProjectList(ID_PROJECT_MRU_1, 
        _T("Recent Project List"), 
        _T("Progetto&%d"), 
        4);

  // Carica i progetti recenti salvati nelle precedenti sessioni
 // m_pRecentProjectList->ReadList();
// 3. Prova a leggere il registro dentro il try/catch sicuro (al primo giro non c'è!
	try {
		m_pRecentProjectList->ReadList();
		}
	catch (...) {
			// Ignora eventuali errori di prima lettura
		}

//	m_pRecentProjectList->Add(_T("C:\\straporcodio.mak"));
	// 4. Se la lista è vuota (es. prima volta che la lanci), 
	// scriviamo subito la struttura base nel registro per crearla!
	if(m_pRecentProjectList->GetSize() == 0)	{
		m_pRecentProjectList->WriteList();
		}


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

// Assegniamo il nostro DocManager custom ad MFC
    m_pDocManager = new CMyDocManager();
		
		pDocTemplate = new CMultiDocTemplate(
		IDR_CTYPE,
		RUNTIME_CLASS(COpenCDoc),
		RUNTIME_CLASS(CChildFrame), // base MDI child frame
		RUNTIME_CLASS(COpenCView));
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if(!pMainFrame->LoadFrame(IDR_MAINFRAME))
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
	AbsRel=GetPrivateProfileInt(variabiliKey,IDS_ABSREL);

	Warning=GetPrivateProfileInt(variabiliKey,IDS_WARNING);
	TestoColorato=GetPrivateProfileInt(variabiliKey,IDS_TESTOCOLORATO);
	AutoRicaricaProgetto=GetPrivateProfileInt(variabiliKey,IDS_AUTORICARICAPROGETTO);
  i= GetPrivateProfileString(variabiliKey,IDS_CARTELLAINCLUDE,myBuf,128);
	CartellaInclude=myBuf;
  i= GetPrivateProfileString(variabiliKey,IDS_CARTELLALIBRERIE,myBuf,128);
	CartellaLibrerie=myBuf;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Se m_nShellCommand è FileOpen, Windows o l'utente ha passato un percorso di file
	if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen && !cmdInfo.m_strFileName.IsEmpty()) 	{
		// Estraiamo l'estensione per capire il tipo di file
		int nDotPos = cmdInfo.m_strFileName.ReverseFind('.');
		CString strExt = (nDotPos != -1) ? cmdInfo.m_strFileName.Mid(nDotPos) : _T("");
		strExt.MakeUpper();

		if(strExt == _T(".MAK")) {
			// È un progetto: carichi l'intero ambiente .MAK
			LoadProject(cmdInfo.m_strFileName);
			}
		else if (strExt == _T(".C") || strExt == _T(".H") || strExt == _T(".CPP") || strExt == _T(".ASM") || strExt == _T(".INC")) {
			// È un sorgente singolo: apri solo il documento
			/*ActivateViewByTitle?*/OpenDocumentFile(cmdInfo.m_strFileName);
			}

			// Evitiamo che MFC tenti di riaprire di nuovo il file o crei un documento vuoto
			cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
		}
	else if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew) {
		// RIGA DI COMANDO VUOTA: Auto-reload dell'ultimo progetto!
		if(AutoRicaricaProgetto) {
			if(m_pRecentProjectList) {
				OnOpenRecentProject(ID_PROJECT_MRU_1);
	//			OnFileNuovoprogetto();
		//		LoadProject((*m_pRecentProjectList)[0]);
				}
			}

		else
			RestoreStandaloneSession();

		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
		}

	// Passiamo a ProcessShellCommand solo se c'è altro da gestire
	if(cmdInfo.m_nShellCommand != CCommandLineInfo::FileNothing)	{
		// Dispatch commands specified on the command line
		if(!ProcessShellCommand(cmdInfo))
			return FALSE;
		}

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	if(splashDlg)
		splashDlg->DestroyWindow();
	delete splashDlg;

	return TRUE;
	}

int COpenCApp::ExitInstance() {

	StopFileMonitoring();

// Salva e libera la memoria all'uscita
  if(m_pRecentProjectList) {
    m_pRecentProjectList->WriteList();
    delete m_pRecentProjectList;
    m_pRecentProjectList = NULL;
    }
	WritePrivateProfileInt(variabiliKey,IDS_OPZIONI,Opzioni);
	WritePrivateProfileInt(variabiliKey,IDS_MEMORYMODEL,MemoryModel);
	WritePrivateProfileInt(variabiliKey,IDS_ABSREL,AbsRel);
	WritePrivateProfileInt(variabiliKey,IDS_WARNING,Warning);
	WritePrivateProfileInt(variabiliKey,IDS_TESTOCOLORATO,TestoColorato);
	WritePrivateProfileInt(variabiliKey,IDS_AUTORICARICAPROGETTO,AutoRicaricaProgetto);
	WritePrivateProfileString(variabiliKey,IDS_ALTREDEFINE,(LPCTSTR)altreDefine);
	WritePrivateProfileString(variabiliKey,IDS_NOMECC,(LPCTSTR)ccName);
	WritePrivateProfileString(variabiliKey,IDS_CARTELLAINCLUDE,(LPCTSTR)CartellaInclude);
	WritePrivateProfileString(variabiliKey,IDS_CARTELLALIBRERIE,(LPCTSTR)CartellaLibrerie);
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
	
	((CMainFrame*)m_pMainWnd)->AddText(s,n);
	}

void COpenCApp::ClearOutputWnd() {
	
	((CMainFrame*)m_pMainWnd)->Cls();
	}


/////////////////////////////////////////////////////////////////////////////
// COpenCApp commands

// App command to run the dialog
void COpenCApp::OnAppAbout() {
	CAboutDlg aboutDlg;

	aboutDlg.DoModal();
	}

void COpenCApp::OnFileNew() {
	
	pDocTemplate->OpenDocumentFile(NULL);
	}

void COpenCApp::RestoreStandaloneSession() {
	char myBuf[256],myBuf1[64];
	int i;
	
	i=0;
	do {
		wsprintf(myBuf1,"File%u",i);
		theApp.GetPrivateProfileString(fileApertiKey,myBuf1,myBuf,256);
		if(*myBuf)
			((CMainFrame*)m_pMainWnd)->ActivateViewByTitle(myBuf);
//			pDocTemplate->OpenDocumentFile(myBuf);
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
//			CartellaInclude=myPage0.;
//			CartellaLibrerie=myPage0.;
			AutoRicaricaProgetto=myPage0.m_RicaricaProgettoPartenza;
			}
		if(myPage1.isInitialized) {
			Opzioni=0;
			Opzioni |= myPage1.m_Debug ? debugMode : 0;
			Opzioni |= myPage1.m_SoloPre ? preProcOnly : 0;
			Opzioni |= myPage1.m_NoMacro ? noMacro : 0;
			Opzioni |= myPage1.m_SynCheckOnly ? synCheckOnly : 0;
			Opzioni |= myPage1.m_InserisciCommenti ? preProcCommenti : 0;
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
			AbsRel=myPage1.m_AbsRel;
			Warning=myPage1.m_Warning;
			}
		if(myPage2.isInitialized) {
			TestoColorato=myPage2.m_TestoColorato;
			}
		}
	}



int COpenCApp::LoadProject(const char *nomeprj,COpenCDoc *pDoc) {
	CStdioFile file;
	int i;

	/* per file MAK
PROGETTO.EXE: FILE1.OBJ FILE2.OBJ
	link.exe FILE1.OBJ FILE2.OBJ /OUT:PROGETTO.EXE

FILE1.OBJ: FILE1.C
	cl.exe /c FILE1.C

FILE2.OBJ: FILE2.C
	cl.exe /c FILE2.C*/

	CStringEx strLine;
	RECT rc;
	bool bInOpenCSession = false;
	enum Section { SEC_NONE, SEC_BOOKMARKS, SEC_BREAKPOINTS } currentSec = SEC_NONE;

	if(file.Open(nomeprj,CFile::modeRead)) {

		while(file.ReadString(strLine)) {
			strLine.Trim();

			// Se è un commento dell'IDE
			if(strLine.Left(1) == _T("#"))    {
				if(strLine.FindNoCase(_T("[BOOKMARKS]")) != -1)        {
					currentSec = SEC_BOOKMARKS;
					continue;
					}
				else if(strLine.FindNoCase(_T("[BREAKPOINTS]")) != -1)        {
					currentSec = SEC_BREAKPOINTS;
					continue;
					}
				else if(strLine.Find(_T("FILE: ")) != -1)        {
					CStringEx strData = strLine.Mid(1); // Rimuovi '#'
					SetRectEmpty(&rc);
					strData.Trim();
					int nEqual = strData.Find(_T(':'));
					if(nEqual != -1)            {
						CStringEx strFile = strData.Mid(nEqual + 2),S;
						bool mode;
						S=strFile;
						strFile.Trim();

						if(strFile[0]=='\"') {
							strFile=strFile.Mid(1,strFile.Find('\"',1)-1);
							}
						if(S.Find(',')>=0) {
							S=S.Mid(S.Find(',')+1);
							S.Trim();
							mode=S[0] == '1';
							S=S.Mid(S.Find(',')+1);		// ev. coordinate finestra, poi... e anche flag di aperta (ovvero ambe le cose :) se 0 chiusa se no aperta
							S.Trim();
							rc.left=atoi(S);
//							if(atoi(S)) {		// DOPO perché richiama questa!
//								OpenDocumentFile(strFile);
//								}


							}

						if(!pDoc) {// solo se apertura progetto
							strFile.MakeUpper();
							AddFileToProject(strFile,mode,&rc);
							}

						// reparse andrebbe fatto solo alla fine, se all'apertura, v.sotto
						
						}
					}

				// Se siamo in una sezione dati, pulisci il '#' iniziale e leggi il valore
				if(currentSec == SEC_BOOKMARKS) {
					if(pDoc) {
						CStringEx strData = strLine.Mid(1),S; // Rimuovi '#'
						strData.Trim();

						int nEqual = strData.Find(_T('='));
						if(nEqual != -1)            {
							CStringEx strFile = strData.Left(nEqual);
							CStringEx strLines = strData.Mid(nEqual + 1);
							strFile.Trim();
							strLines.Trim();

							S=pDoc->GetTitle();
//							S.SplitPath(pDoc->GetPathName(),5);
							if(!strFile.CompareNoCase(S)) {
								int i=0,j;
								while(S=strLines.Tokenize(',',i)) {
									if(S.IsEmpty())
										break;
									j=atoi(S);
									pDoc->SetBookmark(j);
									}
								}

							}
						}
					}
				else if(currentSec == SEC_BREAKPOINTS) {
					if(pDoc) {
						CStringEx strData = strLine.Mid(1),S; // Rimuovi '#'
						strData.Trim();

						int nEqual = strData.Find(_T('='));
						if(nEqual != -1)            {
							CStringEx strFile = strData.Left(nEqual);
							CStringEx strLines = strData.Mid(nEqual + 1);
							strFile.Trim();
							strLines.Trim();

							S=pDoc->GetTitle();
//							S.SplitPath(pDoc->GetPathName(),5);
							if(!strFile.CompareNoCase(S)) {
								int i=0,j;
								while(S=strLines.Tokenize(',',i)) {
									if(S.IsEmpty())
										break;
									j=atoi(S);
									pDoc->SetBreakpoint(j);
									}
								}

							}
						}
					}
				}
			else    {
				currentSec = SEC_NONE; // Se si esce dai commenti, azzera la sezione

				if(strLine.Left(3) == _T("CPU"))    {
					ccName=strLine.Mid(6,20);
					}
				else if(strLine.Left(6) == _T("CFLAGS"))    {
					flagsProgetto=strLine.Mid(9,100);
					if(flagsProgetto.Find('$')) {		// patch per ora, elimino le variabili $
						flagsProgetto=flagsProgetto.Left(flagsProgetto.Find('$')-1);
						}
					}
				else if(strLine.Left(7) == _T("OPTIMIZ"))    {

					}
				else if(strLine.Left(8) == _T("INCLUDES"))    {

					}
				else if(strLine.Left(4) == _T("ROOT"))    {
					pathProgetto=strLine.Mid(7,100);
					}

				}
			}
		file.Close();

		if(!pDoc) {
			ReparseProgetto();
			for(i=0; i<fileProgetto.GetSize(); i++) {
				if(fileProgetto[i].rc.left) {		// per ora lo usiamo come flag!
					((CMainFrame*)m_pMainWnd)->ActivateViewByTitle/*OpenDocumentFile*/(fileProgetto[i].nomefile);
					}
				}
			}
		return 1;
		}

	return 0;
	}

CString COpenCApp::ParseOpzioni() {
	CString parms;

	if(!theApp.altreDefine.IsEmpty()) {
		parms+="-D";
		parms+=theApp.altreDefine;
		parms+=" ";
		}
	if(theApp.Opzioni & COpenCApp::debugMode)
		parms+="-d ";
	if(theApp.Opzioni & COpenCApp::synCheckOnly)
		parms+="-E ";		// non e' proprio cosi'... questo dovrebbe applicarsi anche al codice C e non al solo preprocessore...
	else {
		if(theApp.Opzioni & COpenCApp::preProcOnly)
			parms+="-P ";
		if(theApp.Opzioni & COpenCApp::preProcCommenti)
			parms+="-C ";
		}
	if(theApp.Opzioni & COpenCApp::outSource)
		parms+="-Fc ";
	if(theApp.Opzioni & COpenCApp::outAsm)
		parms+="-Fa ";
	if(theApp.Opzioni & COpenCApp::outListing)
		parms+="-Fl ";
	if(theApp.Opzioni & COpenCApp::checkStack)
		parms+="-Ge ";
	else
		parms+="-Gs ";
	if(theApp.Opzioni & COpenCApp::pascalCalls)
		parms+="-Gc ";
	else
		parms+="-Gd ";
	if(theApp.Opzioni & COpenCApp::multipleStrings)
		parms+="-Gf ";
// parms+="Ox" // tipo CPU...
	if(theApp.Opzioni & COpenCApp::charUnsigned)
		parms+="-J ";
// parms+="NT" ND // data segment, text segment

	// v. anche O1 O2 ecc per ottimizzazioni
	if(theApp.Opzioni & COpenCApp::ottimizzaSpeed)
		parms+="-Ot ";
	if(theApp.Opzioni & COpenCApp::ottimizzaSize)
		parms+="-Os ";
	if(theApp.Opzioni & COpenCApp::ottimizzaLoop)
		parms+="-Ol ";
	if(theApp.Opzioni & COpenCApp::ottimizzaConst)
		parms+="-O1 ";
	// altre ottimizzazioni...
	if(theApp.Opzioni & COpenCApp::noMacro)
		parms+="-u ";
//	if(theApp.Opzioni & COpenCApp::preProcOnly)
//		parms+="-w ";		// no warning...
//	if(theApp.Opzioni & COpenCApp::preProcOnly)
//		parms+="-WX ";		// warning as errors
//	if(theApp.Opzioni & COpenCApp::preProcOnly)
//		parms+="-Wn ";		// livello..
	if(theApp.Opzioni & COpenCApp::checkPtr)
		parms+="-Zr ";

	switch(theApp.MemoryModel) {		// in MSVC... in MSDEV son poi spariti :)
		case 0:
			parms+="-AT ";		// Tiny/Compact
			break;
		case 1:
			parms+="-AS ";		// SMALL questo non c'è, credo vada di default
			break;
		case 2:
			parms+="-AM ";		// Medium
			break;
		case 3:
			parms+="-AL ";		// LARGE
			break;
		}

	if(theApp.AbsRel) {
		parms+="-mr ";		// relativo
		}

	if(theApp.Warning>0) {
		CStringEx S;
		S.Format("-W%u ",theApp.Warning);
		parms+=S;
		}

	return parms;
	}

void COpenCApp::OnFileApriprogetto() {
	CStringEx S,S1;
	CFileDialog myDlg(TRUE,NULL,nomeProgetto,OFN_FILEMUSTEXIST | OFN_SHOWHELP | OFN_HIDEREADONLY,
		"File progetto (*.mak)|*.mak|Tutti i file (*.*)|*.*||"
		);

	if(myDlg.DoModal() == IDOK) {

		OnFileNuovoprogetto();

		nomeProgetto=myDlg.GetPathName();
		S1.SplitPath(nomeProgetto,5);

		if(LoadProject(myDlg.GetPathName())) {

			updateWindowTitle(S1);

			S="Progetto "+S+" aperto correttamente.";
			((CMainFrame*)m_pMainWnd)->m_wndStatusBar.SetWindowText(S);

	// Aggiunge il percorso in testa alla lista dei progetti recenti
			COpenCApp* pApp = (COpenCApp*)AfxGetApp();
			if(pApp && pApp->m_pRecentProjectList) {
				pApp->m_pRecentProjectList->Add(myDlg.GetPathName());
				}
				
//			CloseAllDocuments(FALSE); // v. sotto
			}
		else {
			nomeProgetto.Empty();
			AfxMessageBox("File non trovato!");
			}
		}
	}

/* per notificare a ev. file aperti i nuovi bookmark ecc... mah (per ora faccio AfxGetApp()->CloseAllDocuments(FALSE);
// Dopo aver caricato i bookmark del file 'strFilePath' dal .MAK:
CDocManager* pDocMgr = AfxGetApp()->m_pDocManager;
if (pDocMgr){
    POSITION posTemplate = pDocMgr->GetFirstDocTemplatePosition();
    while (posTemplate != NULL)
    {
        CDocTemplate* pTemplate = pDocMgr->GetNextDocTemplate(posTemplate);
        POSITION posDoc = pTemplate->GetFirstDocPosition();
        while (posDoc != NULL)
        {
            CDocument* pDoc = pTemplate->GetNextDoc(posDoc);
            
            // Verifichiamo se questo documento aperto corrisponde al file del progetto
            if (pDoc->GetPathName().CompareNoCase(strFilePath) == 0)
            {
                // Opzione A: Se hai un metodo custom sulla tua vista/documento
                POSITION posView = pDoc->GetFirstViewPosition();
                while (posView != NULL)
                {
                    CView* pView = pDoc->GetNextView(posView);
                    if (pView && pView->IsKindOf(RUNTIME_CLASS(CMySourceView)))
                    {
                        ((CMySourceView*)pView)->ApplyBookmarks(listOfBookmarks);
                    }
                }
                
                // Opzione B: Usa la notifica standard MFC per aggiornare le viste
                // pDoc->UpdateAllViews(NULL, HINT_RELOAD_BOOKMARKS);
            }
        }
    }
}*/

void COpenCApp::OnUpdateFileApriprogetto(CCmdUI* pCmdUI) {

	
	}


int COpenCApp::SaveProject(const char *nomeprj) {
	CStdioFile file;
	char myBuf[256];
	int i;
	const char *separator="# ====================================================================\n";
	CStringEx S,S2,thePath;

	SaveAllModified();

	S.SplitPath(nomeprj,1);
	thePath.SplitPath(nomeprj,2);
	thePath=S+thePath;

	if(file.Open(nomeprj,CFile::modeCreate | CFile::modeWrite)) {
		file.WriteString(separator);
		file.WriteString("# MAKEFILE generato da OpenC\n");
		file.WriteString(separator);
		file.WriteString("# --- PROJECT SETTINGS ---\n");
		sprintf(myBuf,"PROJECT = %s\n",(LPCTSTR)S.SplitPath(nomeprj,3));
		file.WriteString(myBuf);
		sprintf(myBuf,"TARGET = $(PROJECT).exe\n");		// beh verificare personalizzare!
		file.WriteString(myBuf);
		file.WriteString(separator);

		file.WriteString("# ROOT DIRECTORY\n");
		file.WriteString("ROOT = .\n");		// o thePath, ma così è rilocabile
		file.WriteString("\n");
		for(i=0; i<fileProgetto.GetSize(); i++) {
			COpenCDoc *pDoc=GetDocByTitle(fileProgetto[i].nomefile);
			RECT rc={0,0,0,0};
			if(pDoc) {
				COpenCView *w=(COpenCView *)pDoc->getView();
				if(w) 		// 
					w->GetWindowPos(&rc);
				}

			if(fileProgetto[i].nomefile.FindNoCase(thePath) >= 0) {
				S.SplitPath(fileProgetto[i].nomefile,5);
				wsprintf(myBuf,"# FILE: \"%s\",%u,%u,%u,%u,%u\n",(LPCTSTR)S,fileProgetto[i].flag,rc.left,rc.top,rc.right,rc.bottom);
				}// left è sempre >0 (causa Tree) per cui è ok come flag! top può essere 0
			else
				wsprintf(myBuf,"# FILE: \"%s\",%u,%u,%u,%u,%u\n",(LPCTSTR)fileProgetto[i].nomefile,fileProgetto[i].flag,rc.left,rc.top,rc.right,rc.bottom);
			// ev. aggiungere coord finestra
			file.WriteString(myBuf);
			}

		file.WriteString("\n");
		file.WriteString(separator);
		file.WriteString("# OPZIONI DI COMPILAZIONE (Gestite da OpenC IDE)\n");
		sprintf(myBuf,"CPU = %s\n",(LPCTSTR)ccName);
		file.WriteString(myBuf);
		sprintf(myBuf,"OPTIMIZ = %s\n",(LPCTSTR)ccName);
		file.WriteString(myBuf);
		sprintf(myBuf,"INCLUDES = -I.\\include\n");
		file.WriteString(myBuf);
		file.WriteString(separator);

		file.WriteString("\n");
//		sprintf(myBuf,"CFLAGS = %s\n","-Fc; -Fa; -Fl; -Gs; -Gd; -O1; -AM;  $(CPU) $(OPTIMIZ) $(INCLUDES) -Wall");
		sprintf(myBuf,"CFLAGS = %s %s\n",(LPCTSTR)ParseOpzioni(),"$(CPU) $(OPTIMIZ) $(INCLUDES)");
		file.WriteString(myBuf);
/*!IF "$(DEBUG)" == "1"
CFLAGS = -c -Zi -Od -D_DEBUG $(INCLUDES)
!ELSE
CFLAGS = -c -O2 -DNDEBUG $(INCLUDES)
!ENDIF*/

		file.WriteString("\n");
		file.WriteString(separator);
		file.WriteString("# TARGET E REGOLE\n");
		file.WriteString(separator);

/*		PROGETTO.EXE: FILE1.OBJ FILE2.OBJ
	link.exe FILE1.OBJ FILE2.OBJ /OUT:PROGETTO.EXE

FILE1.OBJ: FILE1.C
	cl.exe /c FILE1.C

FILE2.OBJ: FILE2.C
	cl.exe /c FILE2.C*/

		file.WriteString("OBJS = ");
		for(i=0; i<fileProgetto.GetSize(); i++) {
			if(fileProgetto[i].nomefile.ReverseFindNoCase(".C")>=0) {
				if(fileProgetto[i].nomefile.FindNoCase(thePath) >= 0) {
					S2.SplitPath(fileProgetto[i].nomefile,5);
					}
				else
					S2=fileProgetto[i].nomefile;
				S=S2.Left(S2.ReverseFindNoCase('.'));
				S+=".obj";
				if(S.Find(' ')>=0) {
					file.WriteString(" \"");
					file.WriteString((LPCTSTR)S);
					file.WriteString("\" ");
					}
				else {
					file.WriteString(" ");
					file.WriteString((LPCTSTR)S);
					}
				}
			}
		file.WriteString("\n\n");


		sprintf(myBuf,"all: $(OBJS)\n");
		file.WriteString(myBuf);


		// questo non dovrebbe servire, la regola è semplicemente .c.obj e poi ci penserà l'esecutore del build (dice
		for(i=0; i<fileProgetto.GetSize(); i++) {
			if(fileProgetto[i].nomefile.ReverseFindNoCase(".C")>=0) {
				if(fileProgetto[i].nomefile.FindNoCase(thePath) >= 0) {
					S2.SplitPath(fileProgetto[i].nomefile,5);
					}
				else
					S2=fileProgetto[i].nomefile;
				S=S2;
				S=S.Mid(S.ReverseFindNoCase('.'));
				S+=".obj";
				sprintf(myBuf,"%s: %s\n",(LPCTSTR)S,(LPCTSTR)S2);		// .c.obj
				file.WriteString(myBuf);
				sprintf(myBuf,"\t cc %s %s\n","$(CFLAGS)",(LPCTSTR)S2);		// "$<"
				file.WriteString(myBuf);
				}
			}


		file.WriteString("\n");
		file.WriteString(separator);
		file.WriteString("# [OpenC-Session] - DO NOT EDIT MANUALLY\n");
		file.WriteString(separator);

		file.WriteString("# [Bookmarks]\n");
//		SaveProjectSection(const char *nomeprj,COpenCDoc *pDoc);
		for(i=0; i<fileProgetto.GetSize(); i++) {
			file.WriteString("# ");
			S.SplitPath(fileProgetto[i].nomefile,5);
			file.WriteString(S);
			file.WriteString("=");
			file.WriteString("\n");
			}

		file.WriteString("\n# [Breakpoints]\n");
		for(i=0; i<fileProgetto.GetSize(); i++) {
			file.WriteString("# ");
			S.SplitPath(fileProgetto[i].nomefile,5);
			file.WriteString(S);
			file.WriteString("=");
			file.WriteString("\n");
			}
		//# IFS.C=15,30

		file.Close();
		return 1;
		}

	return 0;
	}

COpenCDoc *COpenCApp::GetDocByTitle(LPCTSTR lpszTitle) {

  if(!lpszTitle)
      return NULL;

  // 1. Accediamo al template manager dell'applicazione
  POSITION posTemplate = AfxGetApp()->GetFirstDocTemplatePosition();
  while (posTemplate) {
    CDocTemplate* pTemplate = AfxGetApp()->GetNextDocTemplate(posTemplate);
    if(pTemplate) {
      // 2. Scorriamo tutti i documenti aperti gestiti da questo template
      POSITION posDoc = pTemplate->GetFirstDocPosition();
      while(posDoc) {
        COpenCDoc *pDoc = (COpenCDoc*)pTemplate->GetNextDoc(posDoc);
        if(pDoc) {
          // 3. Confrontiamo il titolo (o se preferisci il percorso completo pDoc->GetPathName())
          if(pDoc->GetTitle().CompareNoCase(lpszTitle) == 0) {
            return pDoc; // Trovato!
						}
          }
        }
      }
    }

  return NULL; // Non trovato (il file non è attualmente aperto nell'IDE)
	}

int COpenCApp::SaveProjectSection(const char *nomeprj,COpenCDoc *pDoc) {
	CStdioFile file1,file2;
	char myBuf[256],myBuf2[64];
	int i;
	enum Section { SEC_NONE, SEC_BOOKMARKS, SEC_BREAKPOINTS } currentSec = SEC_NONE;
	CStringEx S,S2,thePath;
	CStringEx strLine;
	CStringEx strBookmarks,strBreakpoints;

	S.SplitPath(nomeprj,1);
	thePath.SplitPath(nomeprj,2);
	thePath=S+thePath;
	S2.SplitPath(nomeprj,3);
	S2 += ".TMP";

	if(file1.Open(nomeprj,CFile::modeRead)) {
		if(file2.Open(S2,CFile::modeCreate | CFile::modeWrite)) {
			for(;;) {
				if(!file1.ReadString(strLine))
					break;

				if(strLine.Left(1) == _T("#"))    {
					if(strLine.FindNoCase(_T("[BOOKMARKS]")) != -1)        {
						currentSec = SEC_BOOKMARKS;
						file2.WriteString(strLine);
						file2.WriteString("\n");
						continue;
						}
					else if(strLine.FindNoCase(_T("[BREAKPOINTS]")) != -1)        {
						currentSec = SEC_BREAKPOINTS;
						file2.WriteString(strLine);
						file2.WriteString("\n");
						continue;
						}
					

					// Se siamo in una sezione dati, pulisci il '#' iniziale e leggi il valore
					if(currentSec == SEC_BOOKMARKS) {
						if(pDoc) {
							CStringEx strData = strLine.Mid(1); // Rimuovi '#'
							strData.Trim();

							int nEqual = strData.Find(_T('='));
							if(nEqual != -1)            {
								CStringEx strFile = strData.Left(nEqual);
								CStringEx strLines = strData.Mid(nEqual + 1);
								strFile.Trim();
								strLines.Trim();

								S=pDoc->GetTitle();
								if(!strFile.CompareNoCase(S)) {
/*								_tcscpy(myBuf,pDoc->GetTitle());
								_tcscat(myBuf,"=");
								for(i=0; i<pDoc->m_bookmarks.GetSize(); i++) {
									wsprintf(myBuf2,"%u,",pDoc->m_bookmarks[i]);
									_tcscat(myBuf,myBuf2);
									}
								if(myBuf[_tcslen(myBuf)-1] == ',')
									myBuf[_tcslen(myBuf)-1]=0;
*/
								
									strBookmarks.Format(_T("# %s="), pDoc->GetTitle());

									for(int i=0; i < pDoc->m_bookmarks.GetSize(); i++) {
										CString strNum;
										strNum.Format(_T("%u,"), pDoc->m_bookmarks[i]);
										strBookmarks += strNum; // Nessuna reallocazione, va diretto nella memoria già pronta!
										}

									strBookmarks.TrimRight(_T(','));
									strLine=strBookmarks;
									}
								}
							}
						}
					else if(currentSec == SEC_BREAKPOINTS) {
						if(pDoc) {
							CStringEx strData = strLine.Mid(1),S; // Rimuovi '#'
							strData.Trim();

							int nEqual = strData.Find(_T('='));
							if(nEqual != -1)            {
								CStringEx strFile = strData.Left(nEqual);
								CStringEx strLines = strData.Mid(nEqual + 1);
								strFile.Trim();
								strLines.Trim();

								S=pDoc->GetTitle();
								if(!strFile.CompareNoCase(S)) {
/*								S=pDoc->GetTitle();
								_tcscpy(myBuf,pDoc->GetTitle());
								_tcscat(myBuf,"=");
								for(i=0; i<pDoc->m_breakpoints.GetSize(); i++) {
									wsprintf(myBuf2,"%u,",pDoc->m_breakpoints[i]);
									_tcscat(myBuf,myBuf2);
									}
								if(myBuf[_tcslen(myBuf)-1] == ',')
									myBuf[_tcslen(myBuf)-1]=0;*/
									strBreakpoints.Format(_T("# %s="), pDoc->GetTitle());

									for(int i=0; i < pDoc->m_breakpoints.GetSize(); i++) {
										CString strNum;
										strNum.Format(_T("%u,"), pDoc->m_breakpoints[i]);
										strBreakpoints += strNum; // Nessuna reallocazione, va diretto nella memoria già pronta!
										}

									strBreakpoints.TrimRight(_T(','));
									strLine=strBreakpoints;
									}

								}
							}
						}
					}
				else {
//					if(strLine.Left(1) == _T("#"))
						currentSec = SEC_NONE; // Se si esce dai commenti, azzera la sezione (

					}

				file2.WriteString(strLine);
				file2.WriteString("\n");
				/*
				file.WriteString("# [Bookmarks]\n");
				if(pDoc) {
					}
				//# ARROWS.C=12,45,102


				file.WriteString("\n");
				file.WriteString("# [Breakpoints]\n");
				if(pDoc) {
					}
				//# IFS.C=15,30
				*/
				}

			file2.Close();
			file1.Close();
			// Sostituisce "test.tmp" a "test.mak" sovrascrivendolo se esiste già
			if(::MoveFileEx(S2, nomeprj, MOVEFILE_REPLACE_EXISTING)) {
				// Rinomina/Sostituzione avvenuta con successo
				}
			else {
				DWORD dwErr = ::GetLastError(); // Gestione errore (es. file bloccato)
				}
			return 1;
			}
		}

	return 0;
	}

void COpenCApp::OnFileSalvaprogetto() {

	if(!nomeProgetto.IsEmpty())
		SaveProject(nomeProgetto);
	else
		OnFileSalvaprogettoconnome();
	
	}

void COpenCApp::OnUpdateFileSalvaprogetto(CCmdUI* pCmdUI) {

	pCmdUI->Enable(!nomeProgetto.IsEmpty() || progettoModified);
	}

void COpenCApp::OnUpdateRecentFileMenu(CCmdUI* pCmdUI) {		// serve per andare insubmenu causa bug  https://www.codeguru.com/cplusplus/mru-list-in-a-submenu-the-mfc-bug-and-how-to-correct-it/

	if(pCmdUI->m_pSubMenu) { // updating a submenu?
		// update your submenu here, if you need to
		return;
		}
	CWinApp::OnUpdateRecentFileMenu(pCmdUI);
	}


void CRecentProjectList::UpdateMenu(CCmdUI* pCmdUI){
  ASSERT(pCmdUI != NULL);

  // 1. Individuiamo il sottomenu target
  CMenu* pTargetMenu = pCmdUI->m_pSubMenu;
  if(pTargetMenu == NULL && pCmdUI->m_pMenu != NULL)    {
    pTargetMenu = pCmdUI->m_pMenu->GetSubMenu(pCmdUI->m_nIndex);
	  }

  if(!pTargetMenu)    {
    CRecentFileList::UpdateMenu(pCmdUI);
    return;
		}

  // 2. Determiniamo l'ID base (es. ID_PROJECT_MRU_1 / 32820)
  UINT nBaseID = (m_nStart != 0) ? m_nStart : pCmdUI->m_nID;

  // 3. Pulizia totale delle vecchie voci nel sottomenu
  for (int i = 0; i < m_nSize; i++) {
    pTargetMenu->DeleteMenu(nBaseID + i, MF_BYCOMMAND);
    }
  pTargetMenu->DeleteMenu(pCmdUI->m_nID, MF_BYCOMMAND); // Rimuove "Nessun progetto"

  // 4. Inserimento dinamico sicuro (senza dipendere da GetDisplayName)
  int nInserted = 0;

  for(int iMRU = 0; iMRU < m_nSize; iMRU++) {
    // Accesso diretto a m_arrNames (evita i bachi di GetDisplayName in Release)
    if (iMRU >= m_nSize) 
        break;

    CString strPath = m_arrNames[iMRU];

    if (!strPath.IsEmpty()) {
      CString strItem;
      strItem.Format(_T("&%d %s"), nInserted + 1, (LPCTSTR)strPath);

      // Posizione = nInserted (0, 1, 2...)
      // ID Comando = nBaseID + iMRU (32820, 32821...)
      pTargetMenu->InsertMenu(nInserted, MF_STRING | MF_ENABLED | MF_BYPOSITION, 
                              nBaseID + iMRU, strItem);
      
      nInserted++;
      }
		}

  // Se non abbiamo inserito nulla, disabilitiamo il menu
  if(nInserted == 0)
    pCmdUI->Enable(FALSE);

  pCmdUI->m_bEnableChanged = TRUE;
	}


void COpenCApp::OnUpdateRecentProjectMenu(CCmdUI* pCmdUI) {

	TRACE(_T("OnUpdateRecentProjectMenu chiamato, pCmdUI->m_nID = %d, m_pMenu = %p, m_pSubMenu = %p\n"), 
    pCmdUI->m_nID, pCmdUI->m_pMenu,pCmdUI->m_pSubMenu);

  if(m_pRecentProjectList)
    // Chiamata alla nuova UpdateMenu sovrascritta
    m_pRecentProjectList->UpdateMenu(pCmdUI);
  else
    pCmdUI->Enable(FALSE);
	}


// Gestisce il click dell'utente su uno dei progetti recenti nel menu
BOOL COpenCApp::OnOpenRecentProject(UINT nID) {
	CStringEx S,S1;

  if(!m_pRecentProjectList)
    return FALSE;

  int nIndex = nID - ID_PROJECT_MRU_1;
  CString strProjectPath = (*m_pRecentProjectList)[nIndex];

  if(!strProjectPath.IsEmpty()) {
		CloseAllDocuments(FALSE); // v. sotto

		nomeProgetto=strProjectPath;
		S.SplitPath(nomeProgetto,5);

    // Invoca la tua funzione personalizzata per caricare il Makefile / Progetto
    CMainFrame* pMainFrame = (CMainFrame*)m_pMainWnd;
    if(!LoadProject(strProjectPath)) {
      // Se il file non esiste più, rimuovilo dall'MRU!
      m_pRecentProjectList->Remove(nIndex);
			nomeProgetto.Empty();
			AfxMessageBox("Il progetto non esiste o è stato rimosso",MB_ICONEXCLAMATION);
      }
		else {
			updateWindowTitle(S);

			S="Progetto "+strProjectPath+" aperto correttamente.";
			pMainFrame->m_wndStatusBar.SetWindowText(S);

      }
    }
  return TRUE;
	}


BOOL COpenCApp::CompilaFile(CStringEx ts,CStringEx ots) {
	char *args[32];
	char myBuf[256],myBuf2[256],n[256],*p;
	int i,j;
	CStringEx parms;
	HINSTANCE hInst;
	HANDLE hFile;
	WIN32_FIND_DATA wfd;
	typedef DWORD (__stdcall *ccFunc)(CWnd *,int,char **);		// "stdcall" serve proprio!!
	ccFunc f;

	args[0]="cc.exe";		// per compatibilità...
	args[1]=(char *)(LPCTSTR)ts;

	parms=theApp.ParseOpzioni();

	if(!nomeProgetto.IsEmpty())
		parms=flagsProgetto;

	strcpy(myBuf,(LPCTSTR)parms);			// non funziona!! lei si aspetta un puntatore per ogni switch...
	p=strtok(myBuf," ");
	for(i=2; i<32 && p!=NULL; i++) {
		args[i]=p;
		p=strtok(NULL," ");
		}

	hInst=LoadLibrary((LPCTSTR)theApp.ccName);
	if(hInst) {
		f=(ccFunc)GetProcAddress(hInst,"Compila");
		// ev. passare ots
		if(f) {
			CWnd *v=theApp.m_pMainWnd;

			(*f)(v,i,(char **)args);
			goto fine;
			}
		FreeLibrary(hInst);
		}
	
	AfxMessageBox("Impossibile caricare il compilatore",MB_ICONEXCLAMATION);
	return 0;

fine:
	return 1;
	}


FILETIME COpenCApp::CercaInclude(const CString& strFilePath) {
	CStringList visitedFiles;

	return GetMaxIncludeTimestamp(strFilePath,visitedFiles);
	}

// gestione ricerca file include con controllo al rientro (Esempio di firma ricorsiva
FILETIME COpenCApp::GetMaxIncludeTimestamp(const CString& strFilePath, CStringList& visitedFiles) {
	CStdioFile file;
	CStringEx strLine;

  // 1. Se il file è già stato analizzato in questa catena, usciamo subito (evita loop infiniti)
  if(visitedFiles.Find(strFilePath))
    return GetFileLastWriteTime(strFilePath); // Ritorna la data di questo file senza ri-esplorarlo

  visitedFiles.AddTail(strFilePath);

  FILETIME ftMax = GetFileLastWriteTime(strFilePath);

  // 2. Apri il file, leggi riga per riga, cerca #include
  // 3. Per ogni include trovato e risolto nel percorso su disco:
	if(file.Open(strFilePath,CFile::modeRead)) {

		while(file.ReadString(strLine)) {
			strLine.Trim();

			}
		file.Close();
		}
			//  FILETIME ftChild = GetMaxIncludeTimestamp(strIncPath, visitedFiles);
//  if(CompareFileTime(&ftChild, &ftMax) > 0) 
//		ftMax = ftChild;

  return ftMax;
	}

// Struttura ausiliaria per ottenere il timestamp di un file
FILETIME COpenCApp::GetFileLastWriteTime(LPCTSTR lpszPath) {
  FILETIME ftLastWrite = { 0, 0 };
  HANDLE hFile = ::CreateFile(lpszPath, GENERIC_READ, FILE_SHARE_READ, 
                              NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if(hFile != INVALID_HANDLE_VALUE) {
    ::GetFileTime(hFile, NULL, NULL, &ftLastWrite);
    ::CloseHandle(hFile);
    }
  return ftLastWrite;
	}

// Confronta due FILETIME (-1 se ft1 < ft2, 1 se ft1 > ft2, 0 se uguali)
LONG COpenCApp::CompareFileTimes(const FILETIME& ft1, const FILETIME& ft2) {
  return ::CompareFileTime(&ft1, &ft2);
	}

BOOL COpenCApp::BuildAll(LPCTSTR lpszOutputDir, BOOL bForceRebuild /* = FALSE */) {
  int nTotalFiles = fileProgetto.GetSize();
  int nCompiledCount = 0;
  BOOL bSuccess = TRUE;
	CString S;

	m_pMainWnd->PostMessage(WM_CLSWINDOW,0,(LPARAM)NULL);
  for(int i=0; i < nTotalFiles; i++) {
		if(fileProgetto.GetAt(i).flag) {
			CStringEx strSourcePath = fileProgetto.GetAt(i).nomefile;
    
			// Costruiamo il percorso del file .OBJ
			CStringEx strFileNameOnly;
			strFileNameOnly.SplitPath(strSourcePath,4);
			if(!strFileNameOnly.CompareNoCase(".C")) {
				strFileNameOnly.SplitPath(strSourcePath,3);
				int nDotPos = strFileNameOnly.ReverseFind(_T('.'));
				if(nDotPos != -1)
					strFileNameOnly = strFileNameOnly.Left(nDotPos);

				CString strObjPath;
	//			strObjPath.Format(_T("%s\\%s.OBJ"), lpszOutputDir, strFileNameOnly);
				strObjPath.Format(_T("%s\\%s.ASM"), lpszOutputDir, (LPCTSTR)strFileNameOnly);

				BOOL bNeedsRecompile = FALSE;

				// SE bForceRebuild è TRUE, forziamo la compilazione a prescindere!
				if(bForceRebuild)
					bNeedsRecompile = TRUE;
				else    {
					// Altrimenti eseguiamo il solito controllo sui timestamp
					FILETIME ftSource = GetFileLastWriteTime(strSourcePath);
					FILETIME ftObj    = GetFileLastWriteTime(strObjPath);

					if(ftObj.dwLowDateTime == 0 && ftObj.dwHighDateTime == 0)
						bNeedsRecompile = TRUE; // .OBJ inesistente
					else if(CompareFileTimes(ftSource, ftObj) > 0)
						bNeedsRecompile = TRUE; // .C modificato
					else if(CompareFileTimes(CercaInclude(strSourcePath),ftSource) > 0)
						bNeedsRecompile = TRUE; // .H modificato
					}

				// Esecuzione Compilazione
				if(bNeedsRecompile) {
					BOOL bResult = CompilaFile(strSourcePath, strObjPath);
					if(!bResult) {
						bSuccess = FALSE;
						break; // Si interrompe al primo errore di compilazione
						}
					nCompiledCount++;
					}
				else {
					char *p=(LPSTR)GlobalAlloc(GPTR,256);
					_tcscpy(p,(LPCTSTR)strSourcePath);
					m_pMainWnd->PostMessage(WM_ADDTEXT,0,(LPARAM)p);
					p=(LPSTR)GlobalAlloc(GPTR,256);
					_tcscpy(p,"Salto... (non necessario)");
					m_pMainWnd->PostMessage(WM_ADDTEXT,0,(LPARAM)p);
					}
				} // .C
			else if(!strFileNameOnly.CompareNoCase(".ASM")) {
				strFileNameOnly.SplitPath(strSourcePath,3);
				int nDotPos = strFileNameOnly.ReverseFind(_T('.'));
				if(nDotPos != -1)
					strFileNameOnly = strFileNameOnly.Left(nDotPos);

				CString strObjPath;
				strObjPath.Format(_T("%s\\%s.OBJ"), lpszOutputDir, (LPCTSTR)strFileNameOnly);

				if(ccName.FindNoCase("GD24032")>=0)
					S="as24";
				else if(ccName.FindNoCase("68000")>=0)
					S="as68";
				else if(ccName.FindNoCase("8086")>=0)
					S="as86";
				else
					S="as";
				//chiamare ASxx a seconda
				CString flags=" /l /E /Fe /x /s";		// v. as, ELF, silent, listing, map/error, 
				strObjPath.Empty();		// qua non serve :) di default
				S=_T("as24.exe ")+flags+" "+strSourcePath +" "+ strObjPath;
				ExecuteAndCaptureOutput((LPCTSTR)S, ((CMainFrame*)m_pMainWnd)->m_wndOutputBar.m_wndOutputEdit);
//				S=flags+" "+strSourcePath;
	//			ShellExecute(m_pMainWnd->m_hWnd,NULL,"as24.exe",S,NULL,SW_SHOW);

				}
			}
		}

  return bSuccess;
	}

BOOL COpenCApp::ExecuteAndCaptureOutput(LPCTSTR lpszCommandLine, CEdit& wndEditOutput) {
  HANDLE hReadPipe   = NULL;
  HANDLE hWritePipe  = NULL;
  HANDLE hStdInRead  = NULL;
  HANDLE hStdInWrite = NULL;

  SECURITY_ATTRIBUTES sa;
  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.bInheritHandle = TRUE;
  sa.lpSecurityDescriptor = NULL;

  // Pipe per STDOUT / STDERR
  if(!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
    return FALSE;

  SetHandleInformation(hReadPipe, HANDLE_FLAG_INHERIT, 0);

    // Pipe vuota per STDIN (invia EOF se l'eseguibile attende input da tastiera)
  if(CreatePipe(&hStdInRead, &hStdInWrite, &sa, 0)) {
    SetHandleInformation(hStdInWrite, HANDLE_FLAG_INHERIT, 0);
    }

  STARTUPINFO si;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
  si.wShowWindow = SW_HIDE;
  si.hStdOutput = hWritePipe;
  si.hStdError  = hWritePipe;
  si.hStdInput  = hStdInRead; // Imposta lo STDIN vuoto

  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(pi));

  TCHAR szCmd[MAX_PATH * 2];
  _tcscpy(szCmd, lpszCommandLine);

  BOOL bSuccess = CreateProcess(
    NULL, 
    szCmd, 
    NULL, 
    NULL, 
    TRUE, 
    0/*CREATE_NO_WINDOW*/, 
    NULL, 
    NULL, 
    &si, 
    &pi
    );

  // Chiudiamo subito il lato lettura/scrittura di STDIN nel padre
  if(hStdInRead)  
		CloseHandle(hStdInRead);
  if(hStdInWrite) 
		CloseHandle(hStdInWrite);

  if(!bSuccess) {
    CloseHandle(hWritePipe);
    CloseHandle(hReadPipe);
    return FALSE;
		}

  // Chiudiamo l'handle di scrittura del padre: resta attivo solo quello del figlio
  CloseHandle(hWritePipe);

  char szBuffer[512];
  DWORD dwBytesRead = 0;

  // Ora la ReadFile legge tutto fino alla fine senza più bloccarsi sulle macro o sul getch()!
  while(ReadFile(hReadPipe, szBuffer, sizeof(szBuffer) - 1, &dwBytesRead, NULL) && dwBytesRead > 0) {
    szBuffer[dwBytesRead] = '\0';

    int nLen = wndEditOutput.GetWindowTextLength();
    wndEditOutput.SetSel(nLen, nLen);

    #ifdef _UNICODE
        CA2W wBuffer(szBuffer);
        wndEditOutput.ReplaceSel(wBuffer);
    #else
        wndEditOutput.ReplaceSel(szBuffer);
    #endif

    wndEditOutput.UpdateWindow();
    }

  WaitForSingleObject(pi.hProcess, INFINITE);

  DWORD dwExitCode = 0;
  GetExitCodeProcess(pi.hProcess, &dwExitCode);

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  CloseHandle(hReadPipe);

  return (dwExitCode == 0);
	}
	

void COpenCApp::OnCompilaProgetto() {
	
	pDocTemplate->SaveAllModified();

	BuildAll(theApp.pathProgetto,FALSE);

	}

void COpenCApp::OnUpdateCompilaProgetto(CCmdUI* pCmdUI) {

	pCmdUI->Enable(!nomeProgetto.IsEmpty() && !ccName.IsEmpty());
	}

void COpenCApp::OnCompilaCompilatutto() {
	pDocTemplate->SaveAllModified();
	
	// Se il percorso del file nel Makefile non è già assoluto (es. non inizia con "C:\" o "\\")
/*	if(::PathIsRelative(strFileName)) {
			// Combiniamo la cartella del file .mak con il percorso relativo del file
			TCHAR szFullPath[MAX_PATH];
			::PathCombine(szFullPath, strProjectPath, strFileName);
			strFinalPath = szFullPath;
		}
	else
			strFinalPath = strFileName;*/
	BuildAll(theApp.pathProgetto,TRUE);
	}

void COpenCApp::OnUpdateCompilaCompilatutto(CCmdUI* pCmdUI) {
	pCmdUI->Enable(!nomeProgetto.IsEmpty() && !ccName.IsEmpty());
	
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

CStringEx CStringEx::SplitPath(LPCTSTR path,BYTE mode) {
	char myBuf[256],myBuf2[64];

	switch(mode) {
		case 1:
			_splitpath(path,myBuf,NULL,NULL,NULL);
			*this=myBuf;
			break;
		case 2:
			_splitpath(path,NULL,myBuf,NULL,NULL);
			*this=myBuf;
			break;
		case 3:
			_splitpath(path,NULL,NULL,myBuf,NULL);
			*this=myBuf;
			break;
		case 4:
			_splitpath(path,NULL,NULL,NULL,myBuf);
			*this=myBuf;
			break;
		case 5:
			_splitpath(path,NULL,NULL,myBuf,myBuf2);
			_tcscat(myBuf,myBuf2);
			*this=myBuf;
			break;
		}

	return *this;
	}

CStringEx CStringEx::GetASCII() {
  char  szASCII[1024];

  ::WideCharToMultiByte(CP_ACP, 0,(WCHAR*)(LPCTSTR)*this, 1024, szASCII, -1, NULL,NULL);		// hmm non ha molto senso, bisognerebbe sapere che la CString è unicode
	return szASCII;
	}

WCHAR *CStringEx::GetUnicode(WCHAR *szUnicode) {

  ::MultiByteToWideChar(CP_ACP, 0, *this, -1, szUnicode, 1024);
	return szUnicode;
	}


/////////////////////////////////////////////////////////////////////////////////////////
void COpenCApp::OnFileChiudiprogetto() {
	CStringEx S;
	int i;

	if(progettoModified) {
		i=AfxMessageBox("Il progetto è stato modificato: salvarlo?",MB_YESNOCANCEL | MB_DEFBUTTON1 | MB_ICONQUESTION);
		if(i == IDOK)
			OnFileSalvaprogetto();
		else if(i == IDCANCEL)
			return;
		}

	((CMainFrame*)m_pMainWnd)->PostMessage(WM_COMMAND,ID_FINESTRA_CHIUDITUTTE,0);

	updateWindowTitle("");

	fileProgetto.RemoveAll();	
	nomeProgetto.Empty();
	progettoModified=FALSE;
	((CMainFrame*)theApp.m_pMainWnd)->m_wndProjectTree.DeleteAllItems();
	((CMainFrame*)theApp.m_pMainWnd)->projectTreeRoot=((CMainFrame*)theApp.m_pMainWnd)->m_wndProjectTree.InsertItem("Progetto",4,4);
	}

void COpenCApp::OnUpdateFileChiudiprogetto(CCmdUI* pCmdUI) {
	
	pCmdUI->Enable(!nomeProgetto.IsEmpty());
	}

void COpenCApp::OnFileSalvaprogettoconnome() {
	CStringEx S,S1;
	CFileDialog myDlg(FALSE,".mak",nomeProgetto,OFN_OVERWRITEPROMPT | OFN_SHOWHELP,
		"File progetto (*.mak)|*.mak|Tutti i file (*.*)|*.*||"
		);
	
	if(myDlg.DoModal() == IDOK) {
		S=myDlg.GetPathName();
		S.SplitPath(S,5);
		nomeProgetto=S;

		SaveProject(myDlg.GetPathName());
		m_pRecentProjectList->Add(myDlg.GetPathName());		// pare logico

		updateWindowTitle(S);

		progettoModified=FALSE;
		}
	}

void COpenCApp::OnUpdateFileSalvaprogettoconnome(CCmdUI* pCmdUI) {

	pCmdUI->Enable(!nomeProgetto.IsEmpty() || progettoModified);
	}

void COpenCApp::updateWindowTitle(CStringEx S) {
	CStringEx S1,S2;
	int i;

	S.SplitPath(S,5);

	S1="Progetto";
	if(!S.IsEmpty()) {
		S2.SplitPath(S,3);
		S2.MakeUpper();
		S1 += ": "+S2;
		}
	((CMainFrame*)theApp.m_pMainWnd)->m_wndProjectTree.SetItemText(((CMainFrame*)theApp.m_pMainWnd)->projectTreeRoot,S1);

	m_pMainWnd->GetWindowText(S1);
	i=S1.ReverseFindNoCase("[");
	if(i>0) 
		S1=S1.Mid(0,i-1);
//	m_pMainWnd->SetWindowText(S1);
//	m_pMainWnd->GetWindowText(S1);
	if(!S.IsEmpty()) {
		S1+=" ["+S;
		S1+="]";
		}
	m_pMainWnd->SetWindowText(S1);
	}

void COpenCApp::OnFileNuovoprogetto() {
	int i;

	if(progettoModified) {
		i=AfxMessageBox("Il progetto è stato modificato: salvarlo?",MB_YESNOCANCEL | MB_DEFBUTTON1 | MB_ICONQUESTION);
		if(i == IDOK)
			OnFileSalvaprogetto();
		else if(i == IDCANCEL)
			return;
		}

//	((CMainFrame*)m_pMainWnd)->PostMessage(WM_COMMAND,ID_FINESTRA_CHIUDITUTTE,0);
	CloseAllDocuments(FALSE); // 

	fileProgetto.RemoveAll();	
	nomeProgetto.Empty();
	pathProgetto.Empty();
	flagsProgetto.Empty();
	progettoModified=FALSE;

	ReparseProgetto();
	updateWindowTitle("");
	}

BOOL COpenCApp::AddFileToProject(const char *s,bool mode,RECT *rc) {
	int i;
	struct PROGETTO_ENTRY pe;

	for(i=0; i<fileProgetto.GetSize(); i++) {
		if(!fileProgetto[i].nomefile.CompareNoCase(s)) {
			AfxMessageBox("Il file è già presente nel progetto!",MB_ICONEXCLAMATION);
			return FALSE;
			}
		}

	pe.flag=mode;
	pe.nomefile=s;
	if(rc)
		pe.rc=*rc;
	fileProgetto.Add(pe);
	ReparseProgetto();		// hmmm andrebbe fatto solo alla fine, se all'apertura, v.sopra
	return TRUE;
	}

void COpenCApp::ReparseProgetto() {
	CMainFrame *f=((CMainFrame*)theApp.m_pMainWnd);
	int i;
	HTREEITEM tp1,tp2,tp3,tp;
	CString S;

	f->m_wndProjectTree.DeleteAllItems();
	f->projectTreeRoot=f->m_wndProjectTree.InsertItem("Progetto",4,4);
	f->m_wndProjectTree.SetItemData(f->projectTreeRoot,0);//marker
//	m_wndProjectTree.SetItemImage(tp1,0,1);
	tp1=f->m_wndProjectTree.InsertItem("Source",0,1,f->projectTreeRoot);
	f->m_wndProjectTree.SetItemData(tp1,1);//marker
	tp2=f->m_wndProjectTree.InsertItem("Header",0,1,f->projectTreeRoot);
	f->m_wndProjectTree.SetItemData(tp1,2);//marker
	tp3=f->m_wndProjectTree.InsertItem("Altro",0,1,f->projectTreeRoot);
	f->m_wndProjectTree.SetItemData(tp1,3);//marker
	for(i=0; i<fileProgetto.GetSize(); i++) {
		CStringEx S;
		S.SplitPath(fileProgetto[i].nomefile,5);
		if(fileProgetto[i].nomefile.ReverseFindNoCase(".C")>=0) {
			tp=f->m_wndProjectTree.InsertItem(S,2,2,tp1);
			f->m_wndProjectTree.SetItemImage(tp,fileProgetto[i].flag ? 2 : 3,fileProgetto[i].flag ? 2 : 3);
			f->m_wndProjectTree.SetItemData(tp,(DWORD)&fileProgetto[i]);
			}
		else if(fileProgetto[i].nomefile.ReverseFindNoCase(".H")>=0 || fileProgetto[i].nomefile.ReverseFindNoCase(".INC")>=0) {
			tp=f->m_wndProjectTree.InsertItem(S,2,2,tp2);
			f->m_wndProjectTree.SetItemImage(tp,fileProgetto[i].flag ? 2 : 3,fileProgetto[i].flag ? 2 : 3);
			f->m_wndProjectTree.SetItemData(tp,(DWORD)&fileProgetto[i]);
			}
		else {
			tp=f->m_wndProjectTree.InsertItem(S,2,2,tp3);
			f->m_wndProjectTree.SetItemImage(tp,fileProgetto[i].flag ? 2 : 3,fileProgetto[i].flag ? 2 : 3);
			f->m_wndProjectTree.SetItemData(tp,(DWORD)&fileProgetto[i]);
			}
		}

	f->m_wndProjectTree.Expand(f->projectTreeRoot,TVE_EXPAND);
	f->m_wndProjectTree.Expand(tp1,TVE_EXPAND);
	f->m_wndProjectTree.Expand(tp2,TVE_EXPAND);
	f->m_wndProjectTree.Expand(tp3,TVE_EXPAND);

	}


// --- Monitoraggio File Globale ---------------------------------------------------------------------------------
void COpenCApp::StartFileMonitoring() {

  if(m_pMonThread)
    return; // Già avviato

  // Evento manuale per fermare il thread alla chiusura dell'IDE
  m_hMonStopEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

  // Avvia il thread worker globale a priorità bassa
  m_pMonThread = AfxBeginThread(GlobalFileMonTask, this, THREAD_PRIORITY_IDLE);
	}

void COpenCApp::StopFileMonitoring() {

  if(m_hMonStopEvent)
    ::SetEvent(m_hMonStopEvent);

  if(m_pMonThread != NULL && m_pMonThread->m_hThread) {
    // Attende la chiusura pulita del thread (max 1,5 secondi)
    ::WaitForSingleObject(m_pMonThread->m_hThread, 2000);
    m_pMonThread = NULL;
		}

  if(m_hMonStopEvent) {
    ::CloseHandle(m_hMonStopEvent);
    m_hMonStopEvent = NULL;
    }
	}

// --- Funzioni Thread-Safe per registrare e rimuovere file ---

void COpenCApp::RegisterMonitoredFile(LPCTSTR lpszPath, HWND hWndView, FILETIME ftLastWrite) {
  CSingleLock lock(&m_csMonitoredFiles, TRUE);

  // Evita duplicati per lo stesso HWND
  for(int i=0; i < m_arrMonitoredFiles.GetSize(); i++) {
    if(m_arrMonitoredFiles[i].hWndView == hWndView) {
      m_arrMonitoredFiles[i].strPath = lpszPath;
      m_arrMonitoredFiles[i].ftLastWrite = ftLastWrite;
      return;
      }
		}

  SMonitoredFile item;
  item.strPath = lpszPath;
  item.hWndView = hWndView;
  item.ftLastWrite = ftLastWrite;
  m_arrMonitoredFiles.Add(item);

  // Se è il primo file registrato, avvia il thread se non era attivo
  if(!m_pMonThread) {
    StartFileMonitoring();
    }
	}

void COpenCApp::UnregisterMonitoredFile(HWND hWndView) {
  CSingleLock lock(&m_csMonitoredFiles, TRUE);

  for(int i=0; i < m_arrMonitoredFiles.GetSize(); i++) {
    if(m_arrMonitoredFiles[i].hWndView == hWndView) {
      m_arrMonitoredFiles.RemoveAt(i);
      break;
      }
    }
	}

// Da chiamare quando SALVI il file dall'editor, per evitare falsi allarmi
void COpenCApp::UpdateMonitoredFileTimestamp(LPCTSTR lpszPath, FILETIME ftNewTime) {
  CSingleLock lock(&m_csMonitoredFiles, TRUE);

  for(int i=0; i < m_arrMonitoredFiles.GetSize(); i++) {
    if(m_arrMonitoredFiles[i].strPath.CompareNoCase(lpszPath) == 0)
      m_arrMonitoredFiles[i].ftLastWrite = ftNewTime;
    }
	}

// --- Il Thread di Polling ---

UINT AFX_CDECL COpenCApp::GlobalFileMonTask(LPVOID pParam) {
  COpenCApp* pApp = (COpenCApp*)pParam;

  while(TRUE)    {
    // Attende 1500 ms; se nel frattempo m_hMonStopEvent viene segnalato, esce dal loop
    DWORD dwWait = ::WaitForSingleObject(pApp->m_hMonStopEvent, 1500);
    if(dwWait == WAIT_OBJECT_0) {
      break; // Chiusura applicazione richiesta!
		  }

		// Scansione thread-safe di tutti i file aperti
		CSingleLock lock(&pApp->m_csMonitoredFiles, TRUE);

		for(int i=0; i < pApp->m_arrMonitoredFiles.GetSize(); i++) {
			SMonitoredFile& file = pApp->m_arrMonitoredFiles[i];

			if(file.hWndView && ::IsWindow(file.hWndView)) {
				WIN32_FILE_ATTRIBUTE_DATA wfd;
				if(::GetFileAttributesEx(file.strPath, GetFileExInfoStandard, &wfd)) {
					// Se la data su disco è più recente del nostro timestamp
					if(::CompareFileTime(&wfd.ftLastWriteTime, &file.ftLastWrite) > 0) {
						// Aggiorna subito il timestamp per evitare notifiche doppie
						file.ftLastWrite = wfd.ftLastWriteTime;

						// Invia il messaggio in asincrono alla Vista interessata
						::PostMessage(file.hWndView, WM_MY_FILE_CHANGED, 0, 0);
						}
					}
				}
			}
		lock.Unlock();
		}

  return 0;
	}


/////////////////////////////////////////////////////////////////////////////
CCommandLineInfoEx::CCommandLineInfoEx() {

	m_bShowSplash=TRUE;
	m_debugLevel=0;
	}

void CCommandLineInfoEx::ParseParam(LPCTSTR lpszParam, BOOL bFlag, BOOL bLast) {

  if(bFlag) {
    // Gestione degli switch /flag o -flag
    if(!_tcsicmp(lpszParam, _T("nobanner")))
      m_bShowSplash = FALSE;
    else if(!_tcsnicmp(lpszParam, _T("debuglevel="), 11))
      m_debugLevel = _ttoi(lpszParam + 11);
    else if(!_tcsicmp(lpszParam, _T("debug")))
      m_debugLevel = 1;
    else if(!_tcsicmp(lpszParam, _T("autobuild")))
      m_autoBuild = 1;
    else
      CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
			}
    else {
      // È stato passato un nome di file direttamente (senza / o -)
      // Salvi il percorso nella classe base o in una variabile tua
      CCommandLineInfo::ParseParam(lpszParam, bFlag, bLast);
    }
	}

