// EndmarksWnd.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "EndmarksWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT UM_SETCAPS = RegisterWindowMessage("UM_SETCAPS");

/////////////////////////////////////////////////////////////////////////////
// CEndmarksWnd

CEndmarksWnd::CEndmarksWnd()
{
	m_bitmap.LoadBitmap(IDB_ENDMARKS);
	m_nCap1=m_nCap2=0;
	m_bSubClassed=false;
}

CEndmarksWnd::~CEndmarksWnd()
{
}


BEGIN_MESSAGE_MAP(CEndmarksWnd, CWnd)
	//{{AFX_MSG_MAP(CEndmarksWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(UM_SETCAPS,OnSetCaps)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEndmarksWnd message handlers

BOOL CEndmarksWnd::Create(CRect& rc,CWnd *pParent,UINT nID)
{
	BOOL bResult=CWnd::Create(AfxRegisterWndClass(0),"",WS_CHILD|WS_VISIBLE|SS_NOTIFY,rc,pParent,nID,NULL);
	if(bResult) SetWindowPos(NULL,0,0,53,14,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
	return bResult;
}

BOOL CEndmarksWnd::Subclass(CWnd *pParent,UINT nID)
{
	BOOL bResult=SubclassDlgItem(nID,pParent);
	if(bResult)
	{
		SetWindowPos(NULL,0,0,53,14,SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
		m_bSubClassed=true;
	}
	return bResult;
}

void CEndmarksWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

   CRect r,rc;
	GetClientRect(&r);
	if(IsWindowEnabled())
	{
      DrawNonClient(&dc,&r,&rc,GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 255)); // Opacity/Transparency - Infinisys Ltd
      {
			CGDITextColor tc(&dc,GetSysColor(COLOR_BTNTEXT));
			CGDIBackColor bc(&dc,GetSysColor(COLOR_WINDOW));

			CGDIBitmapDC memDC(&dc,&m_bitmap);
			dc.BitBlt(rc.left,rc.top+1,25,11,&memDC,2,m_nCap1*13+1,SRCCOPY);
			dc.BitBlt(rc.left+25,rc.top+1,25,11,&memDC,27,m_nCap2*13+1,SRCCOPY);
		}
		{
			// Opacity/Transparency - Infinisys Ltd
			//CRect r;
			//r.SetRect(rc.left,rc.top,rc.right-1,rc.top+1);
			//dc.ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL);
			//r.SetRect(rc.left,rc.bottom-2,rc.right-1,rc.bottom-1);
			//dc.ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL);
			//r.SetRect(rc.left+3,rc.bottom-1,rc.right,rc.bottom);
			//dc.ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL);
			//r.SetRect(rc.left,rc.top,rc.left+1,rc.bottom-1);
			//dc.ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL);
			//r.SetRect(rc.right-2,rc.top,rc.right-1,rc.bottom-1);
			//dc.ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL);
			//r.SetRect(rc.right-1,rc.top+3,rc.right,rc.bottom);
			//dc.ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL);
		}
	}
	else
	{
		CGDIBackColor bc(&dc,GetSysColor(COLOR_BTNFACE));
		dc.ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
	}
	
}

LRESULT CEndmarksWnd::OnSetCaps(WPARAM wParam,LPARAM lParam)
{
	m_nCap1=BYTE(LOWORD(wParam));
	m_nCap2=BYTE(HIWORD(wParam));
	Invalidate();
	return 0;
}

void CEndmarksWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_bSubClassed) CWnd::OnLButtonDown(nFlags, point);
	else GetParent()->SendMessage(WM_COMMAND,MAKEWPARAM(GetWindowLong(GetSafeHwnd(),GWL_ID),0),(LPARAM)GetSafeHwnd());
}
