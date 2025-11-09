// OpenC.h : main header file for the OPENC application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include <stdint.h>

#define WM_ADDTEXT (WM_USER+1)
#define WM_CLSWINDOW (WM_USER+2)


/////////////////////////////////////////////////////////////////////////////
// COpenCApp:
// See OpenC.cpp for the implementation of this class
//

class CStringEx : public CString {
	public:
		enum Options {
			NO_OPTIONS=0,
			COMMA_DELIMIT=1,
			};
		static const int BASE64_MAXLINE;
		static const char *EOL;
		static const char decimalChar,thousandChar;
		static const char CRchar,LFchar,TABchar;
	public:
		static const char m_base64tab[];
	public:
		CString Tokenize(CString delimiter, int& first);
		static CStringEx CommaDelimitNumber(const char *);
		static CStringEx CommaDelimitNumber(CString);
		static CStringEx CommaDelimitNumber(DWORD);
		CStringEx SubStr(int begin, int len) const;					// substring from s[begin] to s[begin+len]
		BYTE Asc(int);
		int Val(int base=10);
		double Val();
		struct in_addr IPVal();
		void Repeat(int);
		void Repeat(const char *,int);
		void Repeat(char,int);
		void AddCR() { CStringEx::operator+=(LFchar); }
		void RemoveLeft(int n) { CStringEx::operator=(Mid(n)); }
		void RemoveRight(int n) { CStringEx::operator=(Mid(1,GetLength()-n)); }		// era un'idea per fare LEFT$ di tot char, ma Trimright c'è già anche se diversa...
		void Trim() { CString::TrimLeft(); CString::TrimRight(); }	
		static bool IsAlpha(char);
		bool IsAlpha(int);
		static bool IsAlnum(char);
		bool IsAlnum(int);
		static bool IsDigit(char);
		bool IsDigit(int);
		static bool IsPrint(char);
		bool IsPrint(int);
		int FindNoCase(CString substr,int start=0);
		int ReverseFindNoCase(CString substr);
		WORD GetAsciiLength();
		CStringEx Encode64();
		int Decode64();
		CStringEx FormatTime(int m=0,CTime mT=0);
		CStringEx FormatSize(DWORD);
		void Print();
		void Debug();
		CStringEx() : CString() {};		// servono tutti i costruttori "perché non ne ha di virtual, la CString" !
		// https://www.codeguru.com/cpp/cpp/string/ext/article.php/c2793/CString-Extension.htm
		// https://www.codeproject.com/Articles/2396/Simple-CString-Extension
		CStringEx(const CString& stringSrc) : CString(stringSrc) {};
		// bah, eppure non sembra... 2021...
//		CStringEx(const CStringEx& stringSrc) : CString(stringSrc) {};
		CStringEx(TCHAR ch, int nRepeat = 1) : CString(ch, nRepeat) {};
//		CStringEx(LPCTSTR lpch, int nLength) : CString(lpch, nLength) {};
//		CStringEx(const unsigned char *psz) : CString(psz) {};
		CStringEx(LPCWSTR lpsz) : CString(lpsz) {};
		CStringEx(LPCSTR lpsz) : CString(lpsz) {};
//		CStringEx(const char c) {char s[2]={'\0', '\0'}; s[0]=c; CString::operator=(s);}
		CStringEx(int i, const char* format="%d", DWORD options=NO_OPTIONS);
		CStringEx(double d, const char* format="%02lf", DWORD options=NO_OPTIONS);
		virtual ~CStringEx() {};
private:
	CString InsertSeparator(DWORD);
	};


class CWinAppEx : public CWinApp {

	DECLARE_DYNAMIC(CWinAppEx)
public:
	friend class CMainFrame;

	CWinAppEx(bool bResourceSmartUpdate = TRUE);
	virtual ~CWinAppEx();

	int ExitInstance();

	LPCTSTR SetRegistryBase(LPCTSTR lpszSectionName = NULL);
	LPCTSTR	GetRegistryBase() { return m_strRegSection; }

	// Saved data version:
	int GetDataVersionMajor() const { return m_iSavedVersionMajor; }
	int GetDataVersionMinor() const { return m_iSavedVersionMinor; }
	int GetDataVersion() const;

	bool InitMouseManager();
	bool InitContextMenuManager();
	bool InitKeyboardManager();
	bool InitShellManager();
	bool InitTooltipManager();


	bool IsResourceSmartUpdate() const { return m_bResourceSmartUpdate; }
	void EnableLoadWindowPlacement(bool bEnable = TRUE) { m_bLoadWindowPlacement = bEnable; }


	// Call one of these in CMyApp::InitInstance just after ProcessShellCommand() and before pMainFrame->ShowWindow().
	bool LoadState(CMDIFrameWnd* pFrame, LPCTSTR lpszSectionName = NULL);
	bool LoadState(CFrameWnd* pFrame, LPCTSTR lpszSectionName = NULL);

	virtual bool CleanState(LPCTSTR lpszSectionName = NULL);

	bool SaveState(CMDIFrameWnd* pFrame, LPCTSTR lpszSectionName = NULL);
	bool SaveState(CFrameWnd* pFrame, LPCTSTR lpszSectionName = NULL);

	bool IsStateExists(LPCTSTR lpszSectionName /*=NULL*/);

	virtual bool OnViewDoubleClick(CWnd* pWnd, int iViewId);
	virtual bool ShowPopupMenu(UINT uiMenuResId, const CPoint& point, CWnd* pWnd);

	CString GetRegSectionPath(LPCTSTR szSectionAdd = _T(""));

	// These functions load and store values from the "Custom" subkey
	// To use subkeys of the "Custom" subkey use GetSectionInt() etc. instead
	int GetInt(LPCTSTR lpszEntry, int nDefault = 0);
	CString GetString(LPCTSTR lpszEntry, LPCTSTR lpszDefault = _T(""));
	bool GetBinary(LPCTSTR lpszEntry, LPBYTE* ppData, UINT* pBytes);
	bool GetObject(LPCTSTR lpszEntry, CObject& obj);
	bool WriteInt(LPCTSTR lpszEntry, int nValue );
	bool WriteString(LPCTSTR lpszEntry, LPCTSTR lpszValue );
	bool WriteBinary(LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);
	bool WriteObject(LPCTSTR lpszEntry, CObject& obj);

	// These functions load and store values from a given subkey
	// of the "Custom" subkey. For simpler access you may use GetInt() etc.
	int GetSectionInt( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, int nDefault = 0);
	CString GetSectionString( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = _T(""));
	bool GetSectionBinary(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPBYTE* ppData, UINT* pBytes);
	bool GetSectionObject(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, CObject& obj);
	bool WriteSectionInt( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, int nValue );
	bool WriteSectionString( LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPCTSTR lpszValue );
	bool WriteSectionBinary(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);
	bool WriteSectionObject(LPCTSTR lpszSubSection, LPCTSTR lpszEntry, CObject& obj);

	void SetTitle();
  bool FirstInstance(int nCmdShow=SW_SHOW);

	// WinHelp override:
	virtual void OnAppContextHelp(CWnd* pWndControl, const DWORD dwHelpIDArray []);

	// Idle processing override:
	virtual bool OnWorkspaceIdle(CWnd* /*pWnd*/) { return FALSE; }

public:
	bool m_bLoadUserToolbars;
//	CProfileStore *prStore;

protected:

	// Overidables for customization
	virtual void OnClosingMainFrame();
	
	virtual void PreLoadState() {}    // called before anything is loaded
	virtual void LoadCustomState() {} // called after everything is loaded
	virtual void PreSaveState() {}    // called before anything is saved
	virtual void SaveCustomState(int bTutto=FALSE) {} // called after everything is saved

	virtual bool LoadWindowPlacement(CRect& rectNormalPosition, int& nFflags, int& nShowCmd);
	virtual bool StoreWindowPlacement(const CRect& rectNormalPosition, int nFflags, int nShowCmd);
	virtual bool ReloadWindowPlacement(CFrameWnd* pFrame);

protected:
	CString m_strRegSection;
  bool bClassRegistered;
	bool AppInited;

	bool m_bKeyboardManagerAutocreated;
	bool m_bContextMenuManagerAutocreated;
	bool m_bMouseManagerAutocreated;
	bool m_bUserToolsManagerAutoCreated;
	bool m_bTearOffManagerAutoCreated;
	bool m_bShellManagerAutocreated;
	bool m_bTooltipManagerAutocreated;
	bool m_bForceDockStateLoad; // Load dock bars state even it's not valid
	bool m_bLoadSaveFrameBarsOnly;
	bool m_bSaveState;          // Automatically save state when the main frame is closed.
	bool m_bForceImageReset;    // Force image reset every time when the frame is loaded
	bool m_bLoadWindowPlacement;

	const bool m_bResourceSmartUpdate; // Automatic toolbars/menu resource update

	int m_iSavedVersionMajor;
	int m_iSavedVersionMinor;
	};

class COpenCDoc2;

class COpenCApp : public CWinAppEx {
public:
	enum {
		debugMode=0x80000000,
		preProcOnly=1,
		noMacro=2,
		synCheckOnly=4,
		checkStack=0x100,
		checkPtr=0x200,
		charUnsigned=0x400,
		multipleStrings=0x800,
		pascalCalls=0x1000,
		inlineCalls=0x2000,
		outSource=0x10000,
		outAsm=0x20000,
		outListing=0x40000,
		ottimizzaSpeed=0x1000000,
		ottimizzaSize=0x2000000,
		ottimizzaLoop=0x4000000,
		ottimizzaConst=0x8000000
		};

public:
	COpenCApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenCApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
public:
	char *getNow(char *);
	char *getNowGMT(char *);
	int WritePrivateProfileString(const char *,const char *,const char *);
	int WritePrivateProfileString(const char *s, int k, const char *v=NULL) { char ks[32]; LoadString(m_hInstance,k,ks,32); return WritePrivateProfileString(s,ks,v);};
	int WritePrivateProfileInt(const char *, const char *, int);
	int WritePrivateProfileInt(const char *s, int k, int v)  { char ks[32]; LoadString(m_hInstance,k,ks,32); return WritePrivateProfileInt(s,ks,v);};
	int GetPrivateProfileInt(const char *s, const char *k, int def=0);
	int GetPrivateProfileInt(const char *s, int k, int def=0) { char ks[32]; LoadString(m_hInstance,k,ks,32); return GetPrivateProfileInt(s,ks,def);};
	int GetPrivateProfileString(const char *s, const char *k, char *v, int len, char *def=NULL);
	int GetPrivateProfileString(const char *s, int k, char *v, int len, char *def=NULL) { char ks[32]; LoadString(m_hInstance,k,ks,32); return GetPrivateProfileString(s,ks,v,len,def);};
	CTime GetPrivateProfileTime(char *,char *);
	CTime GetPrivateProfileTime(char *s,int k) { char ks[32]; LoadString(m_hInstance,k,ks,32); return GetPrivateProfileTime(s,ks); }
	CTimeSpan GetPrivateProfileTimeSpan(char *,char *);
	CTimeSpan GetPrivateProfileTimeSpan(char *s,int k) { char ks[32]; LoadString(m_hInstance,k,ks,32); return GetPrivateProfileTimeSpan(s,ks); }
	int WritePrivateProfileTime(char *, char *, CTime );
	int WritePrivateProfileTime(char *s, int k, CTime t) { char ks[32]; LoadString(m_hInstance,k,ks,32); return WritePrivateProfileTime(s,ks,t); }
	int WritePrivateProfileTime(char *, char *, CTimeSpan );
	int WritePrivateProfileTime(char *s, int k, CTimeSpan t) { char ks[32]; LoadString(m_hInstance,k,ks,32); return WritePrivateProfileTime(s,ks,t); }
	char *getProfileKey(char *,const char *);
	void WriteOutputWndText(char *,int n=-1);
	void ClearOutputWnd();

public:
	char *variabiliKey,*fileApertiKey;
	DWORD Opzioni;
	BYTE MemoryModel;

	CMultiDocTemplate *pDocTemplate,*pDocTemplate2;
	COpenCDoc2 *theOutput;
	CString ccName,altreDefine;


	//{{AFX_MSG(COpenCApp)
	afx_msg void OnAppAbout();
	afx_msg void OnStrumentiOpzioni();
	afx_msg void OnFileNew();
	afx_msg void OnUpdateFileApriprogetto(CCmdUI* pCmdUI);
	afx_msg void OnFileApriprogetto();
	afx_msg void OnFileNuovo();
	afx_msg void OnUpdateFileNuovo(CCmdUI* pCmdUI);
	afx_msg void OnCompilaTutto();
	afx_msg void OnUpdateCompilaTutto(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


extern COpenCApp theApp;



/////////////////////////////////////////////////////////////////////////////
