// ResizePalette.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ResizePalette.h"
#include "CustomMiniDockFrameWnd.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CResizePalette

CResizePalette::CResizePalette()
{
	m_bShowPos=true;
}

CResizePalette::~CResizePalette()
{
}

BEGIN_MESSAGE_MAP(CResizePalette, CDialogBar)
	//{{AFX_MSG_MAP(CResizePalette)
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(UM_MAXIMIZE_CLICK,OnMaximizeClick)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CResizePalette message handlers

BOOL CResizePalette::Create(CWnd *pParentWnd)
{
	BOOL bResult=CDialogBar::Create(pParentWnd,IDD_RESIZE_PALETTE,CBRS_NOALIGN|CBRS_SIZE_FIXED|CBRS_FLOATING,IDR_RESIZE_PALETTE);
	if(bResult)
	{
		CToolTipCtrl* tips=((CMainFrame*)pParentWnd)->GetToolTip();

		tips->AddTool(GetDlgItem(IDC_ABSOLUTE));
		tips->AddTool(GetDlgItem(IDC_RELATIVE));
		tips->AddTool(GetDlgItem(ID_TOOL_RESIZE_SELECTPT));
		tips->AddTool(GetDlgItem(ID_TOOL_RESIZE_APPLY));

		ModifyStyle(WS_VISIBLE,0,0);
		SetBorders(1,1,1,1);
		EnableDocking(0);
		InvertResizeOpt();
	}
	return bResult;
}

BOOL CResizePalette::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		CWnd* pApply=GetDlgItem(ID_TOOL_RESIZE_APPLY);
		if(pApply->IsWindowEnabled())
		{
			CWnd* pWnd=pApply->SetFocus();
			HWND hWnd=pWnd->Detach();
			SendMessage(WM_COMMAND,ID_TOOL_RESIZE_APPLY,0);

			// let MFC do its idle processing
			LONG lIdle = 0;
			while(AfxGetApp()->OnIdle(lIdle++));  

			pWnd=CWnd::FromHandle(hWnd);
			pWnd->SetFocus();
			return TRUE;
		}
	}

	return CDialogBar::PreTranslateMessage(pMsg);
}

void CResizePalette::OffsetWindow(UINT nID,int nDelta)
{
	CRect rc;
	CWnd* pWnd=GetDlgItem(nID);
	pWnd->GetWindowRect(&rc);
	ScreenToClient(&rc);
	pWnd->SetWindowPos(NULL,rc.left,rc.top+nDelta,0,0,SWP_NOSIZE|SWP_NOZORDER);
}

void CResizePalette::InvertResizeOpt()
{
	static UINT gCtrls[] = { IDC_STATIC_POSITION,IDC_STATIC_X,IDC_STATIC_Y,ID_TOOL_RESIZE_POSITION_X,ID_TOOL_RESIZE_POSITION_Y,0 };
	int nDelta;
	if(m_bShowPos)
	{
		nDelta=-50;
		m_bShowPos=false;
	}
	else
	{
		nDelta=50;
		m_bShowPos=true;
	}
	OffsetWindow(ID_TOOL_RESIZE_SELECTPT,nDelta);
	OffsetWindow(ID_TOOL_RESIZE_APPLY,nDelta);
	m_sizeDefault.cy+=nDelta;
	for(int i=0;gCtrls[i];i++)
		GetDlgItem(gCtrls[i])->ShowWindow(m_bShowPos?SW_SHOW:SW_HIDE);
}

LRESULT CResizePalette::OnMaximizeClick(WPARAM wParam,LPARAM lParam)
{
	((CCustomMiniDockFrameWnd*)GetParentFrame())->InvertMaximized();
	InvertResizeOpt();
	GetParentFrame()->RecalcLayout();
	return 0;
}
