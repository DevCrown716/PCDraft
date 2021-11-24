// SymbolFrame.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "SymbolFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	kMinWidth	268
#define	kMinHeight	353

/////////////////////////////////////////////////////////////////////////////
// CSymbolFrame

IMPLEMENT_DYNCREATE(CSymbolFrame, CMDIChildWnd)

CSymbolFrame::CSymbolFrame()
{
}

CSymbolFrame::~CSymbolFrame()
{
}


BOOL CSymbolFrame::PreCreateWindow(CREATESTRUCT& cs)
{
   DestroyMenu(cs.hMenu); // Interface Update - Infinisys Ltd.
   cs.hMenu = NULL; // Interface Update - Infinisys Ltd.
   if(!CMDIChildWnd::PreCreateWindow(cs))
   {
      return FALSE; // Interface Update - Infinisys Ltd.
   }
   cs.dwExStyle &= ~WS_EX_CLIENTEDGE; // Interface Update - Infinisys Ltd.
   return TRUE; // Interface Update - Infinisys Ltd.
}


BEGIN_MESSAGE_MAP(CSymbolFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CSymbolFrame)
	ON_WM_GETMINMAXINFO()
	ON_WM_MDIACTIVATE()
   ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSymbolFrame message handlers


// Interface Update - Infinisys Ltd.
int CSymbolFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
      return -1;
   SetMenu(NULL);
   // TODO:  Add your specialized creation code here
   ::DestroyMenu(m_hMenuShared);
   m_hMenuShared = NULL;
   return 0;
}

void CSymbolFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
	CRect rcRect(0,0,kMinWidth+4,kMinHeight+4);
	::AdjustWindowRectEx(rcRect,GetStyle(),TRUE,GetExStyle());
	lpMMI->ptMinTrackSize.x=rcRect.Width();
	lpMMI->ptMinTrackSize.y=rcRect.Height();
}

void CSymbolFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	
	if(bActivate)
		GetActiveView()->SetFocus();
	
}
