#include "stdafx.h"
#include "OpenC.h"

#include "OpenCDoc.h"
#include "OpenCView2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputView
/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(COutputView, CEditView)

BEGIN_MESSAGE_MAP(COutputView, CEditView)
	//{{AFX_MSG_MAP(COutputView)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

COutputView::COutputView() {
	}

COutputView::~COutputView() {
	}


/////////////////////////////////////////////////////////////////////////////
// COpenCEditView diagnostics

#ifdef _DEBUG
void COutputView::AssertValid() const
{
	CEditView::AssertValid();
}

void COutputView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

COpenCDoc2* COutputView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenCDoc2)));
	return (COpenCDoc2*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// COutputView message handlers


int COutputView::OnCreate(LPCREATESTRUCT lpCreateStruct) {

	if (CEditView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	GetEditCtrl().SetReadOnly();
	SetFont(&GetDocument()->myFont,TRUE);
	
	return 0;
	}


