// LineWidthWnd.cpp : implementation file
//

#include "stdafx.h"
#include "LineWidthWnd.h"

#include "GDIHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLineWidthWnd

CLineWnd::CLineWnd() // Gradient/Opacity - Infinisys Ltd
{
}

CLineWnd::~CLineWnd() // Gradient/Opacity - Infinisys Ltd
{
}


CLineWidthWnd::CLineWidthWnd()
{
	m_nWidthRegim = 1;
	m_bFlag = false;
}

CLineWidthWnd::~CLineWidthWnd()
{
}

UINT UM_LINEWIDTH_UPDATE = RegisterWindowMessage("UM_LINEWIDTH_UPDATE");

BEGIN_MESSAGE_MAP(CLineWidthWnd, CWnd)
	//{{AFX_MSG_MAP(CLineWidthWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(UM_LINEWIDTH_UPDATE, OnNewLine)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CSepWnd,CWnd) // Gradient/Opacity - Infinisys Ltd
   //{{AFX_MSG_MAP(CLineWidthWnd)
   ON_WM_PAINT() // Gradient/Opacity - Infinisys Ltd
   //}}AFX_MSG_MAP
END_MESSAGE_MAP() // Gradient/Opacity - Infinisys Ltd

/////////////////////////////////////////////////////////////////////////////
// CLineWidthWnd message handlers

void CLineWidthWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	int top, Width;	
	CRect windowRect, clientRect;
	CString outStr;
	COLORREF borderClr, textClr;
	if (IsWindowEnabled())
	{
		borderClr = RGB( 0, 0, 0);
		textClr = ::GetSysColor(COLOR_BTNTEXT);
	}else
	{
		borderClr = ::GetSysColor(COLOR_BTNSHADOW);
		textClr = ::GetSysColor(COLOR_GRAYTEXT);
	}

	GetClientRect(&windowRect);
   dc.SetBkColor(borderClr);
	DrawNonClient( &dc, &windowRect, &clientRect, borderClr, RGB(255, 255, 255)); // Gradient/Opacity - Infinisys Ltd
	
	switch (m_nWidthRegim)
	{
		case 0x81: Width = 1; outStr = "H"; break;
		case 0xA1: Width = 1; outStr = "0.5"; break;
		case 0xC1: Width = 1; outStr = "0.75"; break;
		default: Width = m_nWidthRegim;
				 outStr.Format("%d", m_nWidthRegim);
	}
	top = clientRect.top + (clientRect.Height() - Width)/ 2;
	dc.FillSolidRect( clientRect.left, top, clientRect.Width(), Width, borderClr);
	dc.SetBkColor(RGB(255, 255, 255));
	CFont smallFont, *pOldFont;
	smallFont.CreatePointFont(80, "Arial", &dc);
	pOldFont = dc.SelectObject(&smallFont);
	if (m_bFlag) outStr = "N" + outStr;
	dc.SetTextColor(textClr);
	dc.DrawText(outStr, &clientRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	dc.SelectObject(pOldFont);
}

LPARAM CLineWidthWnd::OnNewLine(WPARAM wParam, LPARAM lParam)
{
 	if ((m_bFlag==(BOOL)wParam) && (m_nWidthRegim==lParam)) return 1;
	m_bFlag = wParam;
	m_nWidthRegim = lParam;
	Invalidate();
	return 1;
}
CSepWnd::CSepWnd() // Gradient/Opacity - Infinisys Ltd
{
}
CSepWnd::~CSepWnd() // Gradient/Opacity - Infinisys Ltd
{
}
void CSepWnd::OnPaint() // Gradient/Opacity - Infinisys Ltd
{

   CPaintDC dc(this); // device context for painting

   CRect rectClient;

   GetClientRect(rectClient);
   dc.FillSolidRect(rectClient.left+2,rectClient.top + rectClient.Height() / 2, rectClient.Width()-2, 1, GetSysColor(COLOR_3DSHADOW));

}

void CLineWnd::DrawNonClient(CDC * pdc,CRect * pRect,CRect * pOutClient,COLORREF crBorder, COLORREF crBk) // Gradient/Opacity - Infinisys Ltd
{

   CRect rect(pRect);

   rect.bottom--;
   rect.right--;
   rect.OffsetRect(1,1);
   COLORREF crBorder1 = RGB(192,192,192);
   pdc->FillSolidRect(rect.right,rect.top,-1,rect.Height(),crBorder1);
   pdc->FillSolidRect(rect.left, rect.bottom,rect.Width(),-1,crBorder1);

   rect.OffsetRect(-1,-1);
   pdc->Draw3dRect(rect,crBorder,crBorder);

   

	pOutClient->left = pRect->left + 1;
	pOutClient->bottom = pRect->bottom - 2;
	pOutClient->right = pRect->right - 2;
	pOutClient->top = pRect->top + 1;

   pdc->FillSolidRect(pOutClient,RGB(255,255,255));

   pOutClient->DeflateRect(1,1);

}
