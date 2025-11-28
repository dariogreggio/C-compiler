// OpenCDoc.cpp : implementation of the COpenCDoc class
//

#include "stdafx.h"
#include "OpenC.h"

#include "OpenCDoc.h"
#include "openCview.h"
#include "openCview2.h"
//#include "cc\cc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenCDoc

IMPLEMENT_DYNCREATE(COpenCDoc, CRichEditDoc)

BEGIN_MESSAGE_MAP(COpenCDoc, CRichEditDoc)
	//{{AFX_MSG_MAP(COpenCDoc)
	ON_COMMAND(ID_COMPILA_FILE, OnCompilaFile)
	ON_UPDATE_COMMAND_UI(ID_COMPILA_FILE, OnUpdateCompilaFile)
	ON_COMMAND(ID_EDIT_REPEAT, OnEditRepeat)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPEAT, OnUpdateEditRepeat)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenCDoc construction/destruction

COpenCDoc::COpenCDoc() {

	prfSection=myPrfSection;
	}

COpenCDoc::~COpenCDoc() {
	
	}

BOOL COpenCDoc::OnNewDocument() {

	if(!CExRichDocument::OnNewDocument())
		return FALSE;

	return TRUE;
	}

BOOL COpenCDoc::OnOpenDocument(LPCTSTR lpszPathName) {
	char myBuf[64],*p;
	RECT rc;
	COpenCView *w=(COpenCView *)getView();

	if(!CExRichDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	p=strrchr(lpszPathName,'\\');
	if(p) {
		p++;
		strcpy(myPrfSection,p);
		}
	GetPrivateProfileString(IDS_COORDINATECHILD,myBuf,32);
	if(*myBuf) {
		sscanf(myBuf,"%d,%d,%d,%d",&rc.left,&rc.top,&rc.right,&rc.bottom);	// sono coord. client rispetto alla MDIFRAME madre della mia ChildFrame
		if(!IsRectEmpty(&rc))
//			rc.left-=4;		// (per motivi ignoti (credo sia colpa della toolbar)...
//			rc.right+=4;
//			rc.top-=19+4;		// 
//			rc.bottom+=4;
//			rc.left=10; rc.right=400;
//			rc.top=10; rc.bottom=200;
//			w->GetParent()->GetParent()->SetWindowPos(NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,SWP_NOZORDER);
			// DUE GetParent perche' c'e' Splitter!!
			w->GetParent()->GetParent()->SetWindowPos(NULL,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,SWP_NOZORDER);
		}

	return TRUE;
	}

BOOL COpenCDoc::OnSaveDocument(LPCTSTR lpszPathName) {

	return CDocument::OnSaveDocument(lpszPathName);
	}


void COpenCDoc::OnCloseDocument() {
	CString S,S1;
	RECT rc,rc2;
	char myBuf[64];
	COpenCView *w=(COpenCView *)getView();
	
	strcpy(myPrfSection,(LPCTSTR)GetTitle());

	w->GetWindowPos(&rc);
	S.LoadString(IDS_OPZIONI);
	S1.LoadString(IDS_COORDINATECHILD);
	wsprintf(myBuf,"%d,%d,%d,%d",rc.left,rc.top,rc.right,rc.bottom);
	theApp. /*prStore->*/ WritePrivateProfileString(myPrfSection,S1,myBuf);

//	SaveState(m_nDocType);

	CExRichDocument::OnCloseDocument();
	}

CRichEditCntrItem* COpenCDoc::CreateClientItem(REOBJECT* preo) const {
	// cast away constness of this
	return new COpenCCntrItem(preo, (COpenCDoc*)this);
	}

void COpenCDoc::OnDeactivateUI(BOOL bUndoable) {
	COpenCView *w=(COpenCView *)getView();

	if(w->m_bDelayUpdateItems)
		UpdateAllItems(NULL);
/*	SaveState(m_nDocType);			boh, v. wordpad nel caso
	CRichEditDoc::OnDeactivateUI(bUndoable);
	COIPF* pFrame = (COIPF*)m_pInPlaceFrame;
	if (pFrame)	{
		if (pFrame->GetMainFrame())
			ForceDelayed(pFrame->GetMainFrame());
		if (pFrame->GetDocFrame())
			ForceDelayed(pFrame->GetDocFrame());
		}*/
	}

/////////////////////////////////////////////////////////////////////////////
// COpenCDoc serialization

void COpenCDoc::Serialize(CArchive& ar) {
	EDITSTREAM es;

	if(ar.IsStoring())	{
		es.dwCookie = (DWORD)ar.GetFile();
//		es.pfnCallback = COpenCView::MyStreamOutCallback;
		((COpenCView*)m_viewList.GetHead())->StreamOut(es);

//		((CRichEditView*)m_viewList.GetHead())->Serialize(ar);  //da MultiPad...

		}
	else {
//		CFile cFile(ar.stream,CFile::read);

		es.dwCookie = (DWORD)ar.GetFile();
//		es.pfnCallback = COpenCView::MyStreamInCallback;
		((COpenCView*)m_viewList.GetHead())->StreamIn(es);
//		((COpenCView*)m_viewList.GetTail())->StreamIn(es);

//		((CRichEditView*)m_viewList.GetHead())->Serialize(ar);  //da MultiPad...
		m_nDocLines=((COpenCView*)m_viewList.GetHead())->GetLineCount();		// FINIRE
		}
	}

/////////////////////////////////////////////////////////////////////////////
// COpenCDoc diagnostics

#ifdef _DEBUG
void COpenCDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void COpenCDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenCDoc commands

void COpenCDoc::OnCompilaFile() {
	char *args[32];
	char myBuf[256],myBuf2[256],n[256],*p;
	int i,j;
	CString ts,parms;
	HINSTANCE hInst;
	HANDLE hFile;
	WIN32_FIND_DATA wfd;
	typedef DWORD (__stdcall *ccFunc)(CWnd *,int,char **);		// "stdcall" serve proprio!!
	ccFunc f;

	if(((COpenCView*)m_viewList.GetHead())->IsModified() /*IsModified()*/)
		OnSaveDocument(GetPathName());	
	ts=GetPathName();
	args[0]="openc.exe";		// per compatibilità...
	args[1]=(char *)(LPCTSTR)ts;
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

	if(theApp.MemoryModel==0)		// in MSVC... in MSDEV son poi spariti :)
		parms+="-AS ";		// SMALL questo non c'è, credo vada di default
	else
		parms+="-AL ";		// LARGE

	if(theApp.Warning>0) {
		CStringEx S;
		S.Format("-W%u ",theApp.Warning);
		parms+=S;
		}

	strcpy(myBuf,(LPCTSTR)parms);			// non funziona!! lei si aspetta un puntatore per ogni switch...
	p=strtok(myBuf," ");
	for(i=2; i<32 && p!=NULL; i++) {
		args[i]=p;
		p=strtok(NULL," ");
		}

	hInst=LoadLibrary((LPCTSTR)theApp.ccName);
	if(hInst) {
		f=(ccFunc)GetProcAddress(hInst,"Compila");
		if(f) {
			CWnd *v=theApp.theOutput->getView()->GetParent();

			(*f)(v,i,(char **)args);
			goto fine;
			}
		FreeLibrary(hInst);
		}
	
	AfxMessageBox("Impossibile caricare il compilatore",MB_ICONEXCLAMATION);

fine:
		;
	//myCC->CompilaC(3,(char **)args);
	
	}

void COpenCDoc::OnUpdateCompilaFile(CCmdUI* pCmdUI) {
	
	pCmdUI->Enable(!GetPathName().IsEmpty() && !theApp.ccName.IsEmpty());
	}

/////////////////////////////////////////////////////////////////////////////
// CWordPadCntrItem implementation

IMPLEMENT_SERIAL(COpenCCntrItem, CRichEditCntrItem, 0)

COpenCCntrItem::COpenCCntrItem(REOBJECT *preo, COpenCDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer) {

	}

/////////////////////////////////////////////////////////////////////////////
// CWordPadCntrItem diagnostics

#ifdef _DEBUG
void COpenCCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void COpenCCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif



/////////////////////////////////////////////////////////////////////////////
// COpenC2Doc

IMPLEMENT_DYNCREATE(COpenCDoc2, CDocument)

BEGIN_MESSAGE_MAP(COpenCDoc2, CDocument)
	//{{AFX_MSG_MAP(COpenCDoc2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenCDoc2 construction/destruction

COpenCDoc2::COpenCDoc2() {

	myFont.CreateFont(16,8,0,0,FW_THIN,0,0,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH | FF_MODERN,"arial");
	numErrors=numWarnings=0;
	}

COpenCDoc2::~COpenCDoc2()
{
}

BOOL COpenCDoc2::OnNewDocument() {

	if(!CExDocument::OnNewDocument())
		return FALSE;

	SetTitle("Output");

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// COpenCDoc2 serialization

void COpenCDoc2::Serialize(CArchive& ar) {
	}

/////////////////////////////////////////////////////////////////////////////
// COpenCDoc2 diagnostics

#ifdef _DEBUG
void COpenCDoc2::AssertValid() const
{
	CDocument::AssertValid();
}

void COpenCDoc2::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COpenCDoc2 commands

int COpenCDoc2::AddText(char *s,int m) {
	int i=0;
	char myBuf[64],*p;
	COutputView *v=(COutputView *)getView();

	if(v) {
		p=(char *)GlobalAlloc(GPTR,strlen(s)+2);
		strcpy(p,s);
		v->PostMessage(WM_ADDTEXT,0,(LPARAM)p);
		i=1;
		}
	switch(m) {
		case 1:
			numErrors++;
			break;
		case 2:
			numWarnings++;
			break;
		}

	SetModifiedFlag(FALSE);
	return i;
	}

int COpenCDoc2::Cls() {
	int i=0;
	COutputView *v=(COutputView *)getView();

	if(v) {
		v->PostMessage(WM_CLSWINDOW,0,0);
		i=1;
		}
	numErrors=numWarnings=0;
	return i;
	}


void COpenCDoc::OnEditRepeat() {
	
	}

void COpenCDoc::OnUpdateEditRepeat(CCmdUI* pCmdUI) {
	
	}




