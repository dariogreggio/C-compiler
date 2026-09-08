// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "OpenC.h"

#include "MainFrm.h"
#include "OpenCDoc.h"
#include "ChildFrm.h"
#include "OpenCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame() {
	// TODO: add member initialization code here
	m_bInitSplitter=FALSE;
	}

CChildFrame::~CChildFrame() {
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs,
	CCreateContext* pContext) {
	int i;
	CRect cr;

	GetClientRect(&cr);

/*	i=m_wndSplitter.Create(this,
		0, 1,                 // TODO: adjust the number of rows, columns
		CSize( 100, 40 ),      // TODO: adjust the minimum pane size
		pContext);*/
	i=m_wndSplitter.CreateStatic(this, 2, 1);

	if(!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(COpenCView),
		CSize(0,0 /*cr.Width(), cr.Height()*/ /*/2*/), pContext)) 	{
		MessageBox( "Error setting up splitter view 1", "ERROR", MB_OK | MB_ICONERROR );
		return FALSE;
		}
	if(!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(COpenCView),
		CSize(0 /*cr.Width()*/, 0 /*cr.Height()/2*/), pContext)) 	{
		MessageBox( "Error setting up splitter view 2", "ERROR", MB_OK | MB_ICONERROR );
		return FALSE;
		}

// 4. Configurazione delle dimensioni iniziali: 
  // Riga 0 = Altezza massima (cr.Height()), dimensione minima 50px
  // Riga 1 = Altezza 0, dimensione minima 0px (completamente nascosta)
//  m_wndSplitter.SetRowInfo(0, 32767 /*cr.Height()*/, 10 /*50*/);		// meglio lasciar fare a onsize!
//  m_wndSplitter.SetRowInfo(1, 0, 0);
// Ricalcola il layout affinché la riga 1 rimanga del tutto chiusa
//  m_wndSplitter.RecalcLayout();

	m_wndSplitter.SetSplitState(FALSE); // Parte chiuso
	m_bInitSplitter=TRUE;

	return i;
	}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs

	cs.style &= ~WS_VSCROLL;
	return CMDIChildWnd::PreCreateWindow(cs);
	}


/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers


void CChildFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) {
	
//	lpMMI->ptMaxSize.y=500;		// (non funzia, volevo evitare che andasse sopra la output...
	
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
	}


void CChildFrame::OnSize(UINT nType, int cx, int cy) {

    CMDIChildWnd::OnSize(nType, cx, cy);

// Passa semplicemente il ridimensionamento allo splitter, che userà il suo RecalcLayout
    if (m_bInitSplitter && nType != SIZE_MINIMIZED)
    {
        m_wndSplitter.MoveWindow(0, 0, cx, cy);
    
    }
	}




// ---------------------------------------------------

IMPLEMENT_DYNCREATE(CMySplitterWnd, CSplitterWnd)

BEGIN_MESSAGE_MAP(CMySplitterWnd, CSplitterWnd)
    ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

CMySplitterWnd::CMySplitterWnd() {
    m_bIsSplit = FALSE; // Di default la seconda vista è CHIUSA (100%/0%)
	}

CMySplitterWnd::~CMySplitterWnd()
{
}


void CMySplitterWnd::RecalcLayout() {
  CRect rc;
  GetClientRect(&rc);

  // Se la finestra ha una dimensione valida
  if (rc.Height() > 0)    {
      int nCurH0, nMin0, nCurH1, nMin1;
      GetRowInfo(0, nCurH0, nMin0);
      GetRowInfo(1, nCurH1, nMin1);

      if (!m_bIsSplit)      {
          // --- STATO CHIUSO ---
          // Tutto il peso va alla prima riga
          m_pDynamicViewClass = NULL; // Disabilita comportamenti dinamici anomali
          SetRowInfo(0, rc.Height(), 10);
          SetRowInfo(1, 0, 0);
      }
      else      {
          // --- STATO APERTO ---
          // Se l'utente ha trascinato la riga sotto a 0, aggiorniamo lo stato a CHIUSO
          if (nCurH1 <= 5 && nCurH0 > 0)          {
              m_bIsSplit = FALSE;
              SetRowInfo(0, rc.Height(), 10);
              SetRowInfo(1, 0, 0);
          }
          else          {
              // Se era aperto al 50/50 o proporzionato, mantieni la proporzione sull'altezza totale
              int nTotal = nCurH0 + nCurH1;
              if (nTotal > 0)              {
                  int nNewH0 = (nCurH0 * rc.Height()) / nTotal;
                  int nNewH1 = rc.Height() - nNewH0;

                  SetRowInfo(0, nNewH0, 10);
                  SetRowInfo(1, nNewH1, 10);
              }
              else              {
                  // Default 50/50
                  SetRowInfo(0, rc.Height() / 2, 10);
                  SetRowInfo(1, rc.Height() / 2, 10);
              }
          }
      }
		}

  // Chiama il metodo base di MFC per riposizionare effettivamente i controlli figli
  CSplitterWnd::RecalcLayout();
	}

void CMySplitterWnd::OnLButtonDblClk(UINT nFlags, CPoint point) {
    CRect rc;
    GetClientRect(&rc);

    // Inverti lo stato al doppio click
    m_bIsSplit = !m_bIsSplit;

    if (m_bIsSplit)    {
        // Apre a 50/50
        int nHalf = rc.Height() / 2;
        SetRowInfo(0, nHalf, 10);
        SetRowInfo(1, nHalf, 10);
    }
    else    {
        // Chiude a 100%/0%
        SetRowInfo(0, rc.Height(), 10);
        SetRowInfo(1, 0, 0);
    }

    RecalcLayout();
	}

