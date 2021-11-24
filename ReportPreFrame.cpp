// ReportPreFrame.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ReportPreFrame.h"
#include "ReportPreView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int MINSCROLLBARWIDTH = 4*GetSystemMetrics(SM_CXVSCROLL);

static	UINT UM_DELAY_SIZE = RegisterWindowMessage("UM_DELAY_SIZE");

/////////////////////////////////////////////////////////////////////////////
// CReportPreFrame

IMPLEMENT_DYNCREATE(CReportPreFrame, CMDIChildWnd)

CReportPreFrame::CReportPreFrame()
{
	m_cxReserved=200;
}

CReportPreFrame::~CReportPreFrame()
{
}


BEGIN_MESSAGE_MAP(CReportPreFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CReportPreFrame)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
	ON_COMMAND(IDC_PRINT,OnPrint)
	ON_COMMAND(IDC_PRINTSETUP,OnPrintSetup)
	ON_COMMAND(IDC_UPDATE,OnUpdate)
	ON_REGISTERED_MESSAGE(UM_DELAY_SIZE,OnDelaySize)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReportPreFrame message handlers

BOOL CReportPreFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if(GetActiveView()->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL CReportPreFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style |= WS_CLIPCHILDREN;
	return CMDIChildWnd::PreCreateWindow(cs);
}

int CReportPreFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	CRect rc(0,0,0,0);
	VERIFY(m_wndSBHorz.Create(WS_VISIBLE|WS_CHILD|SBS_HORZ,rc,this,AFX_IDW_HSCROLL_FIRST));
	VERIFY(m_wndSBVert.Create(WS_VISIBLE|WS_CHILD|SBS_VERT,rc,this,AFX_IDW_HSCROLL_FIRST+1));
	VERIFY(m_wndSBBox.Create(WS_VISIBLE|WS_CHILD|SBS_SIZEBOX,rc,this,-1));
	CString strCaption;
	strCaption.LoadString(IDS_PRINTELLPS);
	VERIFY(m_wndPrint.Create(strCaption,WS_CHILD|WS_VISIBLE,rc,this,IDC_PRINT));
	strCaption.LoadString(IDS_PRINTSETUP);
	VERIFY(m_wndPrintSetup.Create(strCaption,WS_CHILD|WS_VISIBLE,rc,this,IDC_PRINTSETUP));
	strCaption.LoadString(IDS_UPDATE);
	VERIFY(m_wndUpdate.Create(strCaption,WS_CHILD|WS_VISIBLE,rc,this,IDC_UPDATE));

	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_wndSBVert.ShowWindow(SW_SHOW);
	m_wndSBHorz.ShowWindow(SW_SHOW);
	m_wndSBBox.ShowWindow(SW_SHOW);
	m_wndPrint.ShowWindow(SW_SHOW);
	m_wndPrintSetup.ShowWindow(SW_SHOW);
	m_wndUpdate.ShowWindow(SW_SHOW);

	return 0;
}

void CReportPreFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	GetActiveView()->SendMessage(WM_HSCROLL,MAKELONG(nSBCode,nPos),(LPARAM)pScrollBar->m_hWnd);
}

LRESULT CReportPreFrame::OnDelaySize(WPARAM wParam, LPARAM lParam)
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
		rc2.right=cxReserved/4;
		m_wndPrint.MoveWindow(rc2);
		rc2.left=rc2.right;
		rc2.right+=cxReserved/2;
		m_wndPrintSetup.MoveWindow(rc2);
		rc2.left=rc2.right;
		rc2.right=cxReserved;
		m_wndUpdate.MoveWindow(rc2);
	}

	rc.left=rc.right+1;
	rc.right+=cxVScroll+1;
	m_wndSBBox.MoveWindow(rc);
	return 0;
}

void CReportPreFrame::OnSize(UINT nType, int cx, int cy) 
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

void CReportPreFrame::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	GetActiveView()->SendMessage(WM_VSCROLL,MAKELONG(nSBCode,nPos),(LPARAM)pScrollBar->m_hWnd);
}

CScrollBar* CReportPreFrame::GetScrollBar(int nBar) const
{
	return nBar==SB_HORZ ? const_cast<CScrollBar*>(&m_wndSBHorz)
		: nBar==SB_VERT ? const_cast<CScrollBar*>(&m_wndSBVert) : NULL;
}

void CReportPreFrame::OnPrint()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_FILE_PRINT);
}

void CReportPreFrame::OnPrintSetup()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND,ID_FILE_PRINT_SETUP);
}

void CReportPreFrame::OnUpdate()
{
	((CReportPreView*)GetActiveView())->DoUpdate();
}
