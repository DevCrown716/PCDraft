// SlideShowFrame.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "SlideShowFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlideShowFrame

IMPLEMENT_DYNCREATE(CSlideShowFrame, CFrameWnd)

CSlideShowFrame::CSlideShowFrame()
{
}

CSlideShowFrame::~CSlideShowFrame()
{
}


BEGIN_MESSAGE_MAP(CSlideShowFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CSlideShowFrame)
	ON_WM_GETMINMAXINFO()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlideShowFrame message handlers

BOOL CSlideShowFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	return CFrameWnd::PreCreateWindow(cs);
}

void CSlideShowFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CRect rc(0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	CalcWindowRect(&rc);
	lpMMI->ptMaxSize.x=lpMMI->ptMinTrackSize.x=lpMMI->ptMaxTrackSize.x=rc.Width();
	lpMMI->ptMaxSize.y=lpMMI->ptMinTrackSize.y=lpMMI->ptMaxTrackSize.y=rc.Height();
	lpMMI->ptMaxPosition.x=rc.left;
	lpMMI->ptMaxPosition.y=rc.top;
}


void CSlideShowFrame::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	CFrameWnd::OnWindowPosChanging(lpwndpos);
	CRect rc(0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
	CalcWindowRect(&rc);
	lpwndpos->x=rc.left;
	lpwndpos->y=rc.top;
	lpwndpos->cx=rc.Width();
	lpwndpos->cy=rc.Height();
}

void CSlideShowFrame::OnClose() 
{
	CWnd::OnClose();
}
