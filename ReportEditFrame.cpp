// ReportEditFrame.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ReportEditFrame.h"
#include "ReportEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MINSCROLLBARWIDTH = 4*GetSystemMetrics(SM_CXVSCROLL);

static UINT UM_DELAY_SIZE = RegisterWindowMessage("UM_DELAY_SIZE");

/////////////////////////////////////////////////////////////////////////////
// CReportEditFrame

IMPLEMENT_DYNCREATE(CReportEditFrame, CMDIChildWnd)

CReportEditFrame::CReportEditFrame()
{
	m_cxReserved=100;
}

CReportEditFrame::~CReportEditFrame()
{
}


BEGIN_MESSAGE_MAP(CReportEditFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CReportEditFrame)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
	ON_COMMAND(IDC_SELINFO,OnSelInfo)
	ON_REGISTERED_MESSAGE(UM_DELAY_SIZE,OnDelaySize)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportEditFrame message handlers

int CReportEditFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CRect rc(0,0,0,0);
	VERIFY(m_wndSBHorz.Create(WS_VISIBLE|WS_CHILD|SBS_HORZ,rc,this,AFX_IDW_HSCROLL_FIRST));
	VERIFY(m_wndSBVert.Create(WS_VISIBLE|WS_CHILD|SBS_VERT,rc,this,AFX_IDW_HSCROLL_FIRST+1));
	VERIFY(m_wndSBBox.Create(WS_VISIBLE|WS_CHILD|SBS_SIZEBOX,rc,this,-1));
	CString strCaption;
	strCaption.LoadString(IDS_OBJINFO);
	VERIFY(m_wndSelInfo.Create(strCaption,WS_CHILD|WS_VISIBLE,rc,this,IDC_SELINFO));

	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_wndSBVert.ShowWindow(SW_SHOW);
	m_wndSBHorz.ShowWindow(SW_SHOW);
	m_wndSBBox.ShowWindow(SW_SHOW);
	m_wndSelInfo.ShowWindow(SW_SHOW);

	return 0;
}

void CReportEditFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	GetActiveView()->SendMessage(WM_HSCROLL,MAKELONG(nSBCode,nPos),(LPARAM)pScrollBar->m_hWnd);
}

CScrollBar* CReportEditFrame::GetScrollBar(int nBar) const
{
	return nBar==SB_HORZ ? const_cast<CScrollBar*>(&m_wndSBHorz)
		: nBar==SB_VERT ? const_cast<CScrollBar*>(&m_wndSBVert) : NULL;
}

LRESULT CReportEditFrame::OnDelaySize(WPARAM wParam, LPARAM lParam)
{
	CView* pView=GetActiveView();
	CRect rc;
	GetClientRect(&rc);
	int cx=rc.right,cy=rc.bottom;
	// First, size the view.
	// 
	int cxReserved=m_cxReserved;
	if (cxReserved>(rc.Width()-MINSCROLLBARWIDTH))
		// make room for horz scrollbar
		cxReserved=rc.Width()-MINSCROLLBARWIDTH;
	int cyHScroll=GetSystemMetrics(SM_CYHSCROLL)-1;
	int cxVScroll=GetSystemMetrics(SM_CXVSCROLL)-1;
	if(cxReserved>=0)
	{
		rc.right-=cxVScroll;
		rc.bottom-=cyHScroll;
	}
	pView->MoveWindow(rc);
	// Next, move the scroll bars: horz, vert and box
	// 
	if(cxReserved<0)
		return 0;
	rc.left=rc.right-2;
	rc.right+=cxVScroll;
	rc.bottom-=1;
	m_wndSBVert.MoveWindow(rc);
	rc.bottom+=1;

	rc.left=max(cxReserved,0);
	rc.right-=cxVScroll+1;
	rc.top=rc.bottom-2;
	rc.bottom+=cyHScroll;
	m_wndSBHorz.MoveWindow(rc);

	if(cxReserved>=0)
	{
		CRect rc2=rc;
		rc2.left=0;
		rc2.right=cxReserved;
		m_wndSelInfo.MoveWindow(rc2);
	}

	rc.left=rc.right+1;
	rc.right+=cxVScroll+1;
	m_wndSBBox.MoveWindow(rc);
	return 0;
}

void CReportEditFrame::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType,cx,cy);

	CView* pView=GetActiveView();
	if(!pView)
	{
		PostMessage(UM_DELAY_SIZE);
		return;
	}
	OnDelaySize(0,0);
}

void CReportEditFrame::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	GetActiveView()->SendMessage(WM_VSCROLL,MAKELONG(nSBCode,nPos),(LPARAM)pScrollBar->m_hWnd);
}

BOOL CReportEditFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if(GetActiveView()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CReportEditFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style |= WS_CLIPCHILDREN;
	return CMDIChildWnd::PreCreateWindow(cs);
}

void CReportEditFrame::OnSelInfo()
{
	((CReportEditView*)GetActiveView())->DoSelInfo();
}
