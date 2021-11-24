// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "PCDraft.h"

#include "ChildFrm.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	kMinWidth	300
#define	kMinHeight	100

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
   ON_WM_CREATE() // Interface Update - Infinisys Ltd.
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
/*	CRect rc;
	::GetClientRect(((CMDIFrameWnd*)AfxGetMainWnd())->m_hWndMDIClient,&rc);
	cs.y=0;
	cs.cy=rc.bottom;
*/
	cs.style|=WS_MAXIMIZE|WS_VISIBLE;
   DestroyMenu(cs.hMenu); // Interface Update - Infinisys Ltd.
   cs.hMenu = NULL; // Interface Update - Infinisys Ltd.
	if(!CMDIChildWnd::PreCreateWindow(cs))
   {
      return FALSE; // Interface Update - Infinisys Ltd.
   }
   cs.dwExStyle &= ~WS_EX_CLIENTEDGE; // Interface Update - Infinisys Ltd.
   return TRUE; // Interface Update - Infinisys Ltd.
}

void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	CView* pView=GetActiveView();
	if(pView && pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
	{
      auto title = ((CPCDraftDoc*)pView->GetDocument())->CreateTitleW();
      auto acp = get_cp(title, CP_ACP);
		SetWindowText(acp);
	}
	else
		CMDIChildWnd::OnUpdateFrameTitle(bAddToTitle);
   if(GetTopLevelFrame() != NULL)
   {
      GetTopLevelFrame()->OnUpdateFrameTitle(bAddToTitle);
   }
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

void CChildFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CMDIChildWnd::OnGetMinMaxInfo(lpMMI);
	CRect rcRect(0,0,kMinWidth+4,kMinHeight+4);
	::AdjustWindowRectEx(rcRect,GetStyle(),TRUE,GetExStyle());
	lpMMI->ptMinTrackSize.x=rcRect.Width();
	lpMMI->ptMinTrackSize.y=rcRect.Height();
}

// Interface Update - Infinisys Ltd.
int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
      return -1;
   SetMenu(NULL);
   // TODO:  Add your specialized creation code here
   ::DestroyMenu(m_hMenuShared);
   m_hMenuShared = NULL;
   return 0;
}
