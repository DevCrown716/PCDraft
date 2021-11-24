// MacButton.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "MacButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMacButton

CMacButton::CMacButton()
{
	m_bPushed=false;
	m_bClick=false;
}

CMacButton::~CMacButton()
{
}


BEGIN_MESSAGE_MAP(CMacButton, CWnd)
	//{{AFX_MSG_MAP(CMacButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMacButton message handlers

BOOL CMacButton::Create(LPCTSTR lpszCaption,DWORD dwStyle,const RECT& rect,CWnd* pParentWnd,UINT nID)
{
	return CWnd::Create(NULL,lpszCaption,dwStyle,rect,pParentWnd,nID);
}

void CMacButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_bClick=true;
	m_bPushed=true;
	SetCapture();
	Invalidate();
	UpdateWindow();
}

void CMacButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bClick)
	{
		m_bClick=false;
		ReleaseCapture();
		if(m_bPushed)
		{
			m_bPushed=false;
			Invalidate();
			UpdateWindow();
			GetParent()->SendMessage(WM_COMMAND,GetWindowLong(GetSafeHwnd(),GWL_ID),0);
		}
	}
}

void CMacButton::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bClick)
	{
		CRect rc;
		GetClientRect(&rc);
		if(rc.PtInRect(point))
		{
			if(!m_bPushed)
			{
				m_bPushed=true;
				Invalidate();
				UpdateWindow();
			}
		}
		else
		{
			if(m_bPushed)
			{
				m_bPushed=false;
				Invalidate();
				UpdateWindow();
			}
		}
	}
}

void CMacButton::OnPaint() 
{
	CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);
	CString theText;
	GetWindowText(theText);
	COLORREF rgbTextColor;
	if(IsWindowEnabled())
	{
		dc.DrawFrameControl(&rc,DFC_BUTTON,DFCS_BUTTONPUSH|(m_bPushed?DFCS_PUSHED:0));
		if(m_bPushed)
		{
			rc.left++;
			rc.top++;
		}
		rgbTextColor=::GetSysColor(COLOR_BTNTEXT);
	}
	else
	{
		dc.DrawFrameControl(&rc,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_INACTIVE);
		rgbTextColor=::GetSysColor(COLOR_GRAYTEXT);
	}
	CGDIBackMode theBackMode(&dc,TRANSPARENT);
	CGDITextColor theTextColor(&dc,rgbTextColor);
	CGDIObject<CFont> theFont(&dc,CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));
	dc.DrawText(theText,&rc,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
}

void CMacButton::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	Invalidate();
}
