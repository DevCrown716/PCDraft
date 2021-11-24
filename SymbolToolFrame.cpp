// SymbolToolFrame.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "SymbolToolFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSymbolToolFrame

IMPLEMENT_DYNCREATE(CSymbolToolFrame, CMiniFrameWnd)

CSymbolToolFrame::CSymbolToolFrame()
{
}

CSymbolToolFrame::~CSymbolToolFrame()
{
}


BEGIN_MESSAGE_MAP(CSymbolToolFrame, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CSymbolToolFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSymbolToolFrame message handlers

int CSymbolToolFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	pSysMenu->DeleteMenu(SC_SIZE, MF_BYCOMMAND);
	pSysMenu->DeleteMenu(SC_MINIMIZE, MF_BYCOMMAND);
	pSysMenu->DeleteMenu(SC_MAXIMIZE, MF_BYCOMMAND);
	pSysMenu->DeleteMenu(SC_RESTORE, MF_BYCOMMAND);
	
	return 0;
}
