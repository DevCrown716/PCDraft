// ShowBmpWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ShowBmpWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowBmpWnd

CShowBmpWnd::CShowBmpWnd(int iLimitHeight, int nPositionType):
   m_iLimitHeight(iLimitHeight)
{
	m_nPositionType = nPositionType + 1;
}

CShowBmpWnd::~CShowBmpWnd()
{
}

UINT UM_SHOWBMP_UPDATE = RegisterWindowMessage("UM_SHOWBMP_UPDATE");

BEGIN_MESSAGE_MAP(CShowBmpWnd, CWnd)
	//{{AFX_MSG_MAP(CShowBmpWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(UM_SHOWBMP_UPDATE, OnReceiveBmp)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CShowBmpWnd message handlers
LPARAM CShowBmpWnd::OnReceiveBmp(WPARAM wParam, LPARAM lParam)
{
	if(m_bitmapReceived.GetSafeHandle())
		VERIFY(m_bitmapReceived.DeleteObject());
   m_bitmapReceived.Attach((HANDLE) lParam);
	m_nPositionType = wParam;
	Invalidate();
	return 1;
}

void CShowBmpWnd::OnPaint() 
{
	CPaintDC theDC(this); // device context for painting
	CRect windowRect, clientRect;

	GetClientRect(&windowRect);
	CBitmap memoryBmp;

	memoryBmp.CreateCompatibleBitmap(&theDC, windowRect.Width(), windowRect.Height());

	CGDIBitmapDC dc(&theDC,&memoryBmp);

	{
		CGDIBackColor bc(&dc,::GetSysColor(COLOR_BTNFACE));
		dc.ExtTextOut(0,0,ETO_OPAQUE,windowRect,NULL,0,NULL);
	}
	{
		CGDIBackColor bc(&dc,GetSysColor(IsWindowEnabled()?COLOR_BTNTEXT:COLOR_BTNSHADOW));
      DrawNonClient(&dc,&windowRect,&clientRect,GetSysColor(IsWindowEnabled()?COLOR_BTNTEXT:COLOR_BTNSHADOW), RGB(255, 255, 255));
	}
	if(m_bitmapReceived.GetSafeHandle())
	{
		CGDIBitmapDC memoryDC(&theDC,&m_bitmapReceived);
		BITMAP bmpInfo;
      m_bitmapReceived.GetBitmap(&bmpInfo);
		int nDrawWidth, nDrawHeight, left=0, top=0;
      int cy = m_iLimitHeight;
      int srctop = (bmpInfo.bmHeight - m_iLimitHeight) / 2;

      if (cy < 0)
      {

         cy = bmpInfo.bmHeight;
         srctop = 0;

      }

		nDrawWidth = ( bmpInfo.bmWidth < clientRect.Width() ) ? bmpInfo.bmWidth : clientRect.Width();
		nDrawHeight = ( bmpInfo.bmHeight < clientRect.Height() ) ? cy : clientRect.Height();
		switch (m_nPositionType)
		{
		case LEFTTOP_POSITION:
			left = clientRect.left;
			top = clientRect.top;
			break;
		case CENTER_POSITION:
			left = clientRect.left + (clientRect.Width() - nDrawWidth)/2;
			top = clientRect.top + (clientRect.Height() - nDrawHeight)/2;
		default: break;
		}
		dc.BitBlt( left, top, nDrawWidth, nDrawHeight, &memoryDC, 0, srctop, SRCCOPY);
	}
	theDC.BitBlt( 0, 0, windowRect.Width(), windowRect.Height(), &dc, 0, 0, SRCCOPY);
}

//void CShowBmpWnd::DrawBorder(CDC *pDC, LPRECT pRect, LPRECT pOutClient, int nStyle)
//{
//	CRect r;
//	r.SetRect(pRect->left,pRect->top,pRect->right-1,pRect->top+1);
//	pDC->ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL);
//	r.SetRect(pRect->left,pRect->bottom-2,pRect->right-1,pRect->bottom-1);
//	pDC->ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL);
//	r.SetRect(pRect->left+3,pRect->bottom-1,pRect->right,pRect->bottom);
//	pDC->ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL);
//	r.SetRect(pRect->left,pRect->top,pRect->left+1,pRect->bottom-1);
//	pDC->ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL);
//	r.SetRect(pRect->right-2,pRect->top,pRect->right-1,pRect->bottom-1);
//	pDC->ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL);
//	r.SetRect(pRect->right-1,pRect->top+3,pRect->right,pRect->bottom);
//	pDC->ExtTextOut(0,0,ETO_OPAQUE,r,NULL,0,NULL); 
//	pOutClient->left = pRect->left + 1;
//	pOutClient->bottom = pRect->bottom - 2;
//	pOutClient->right = pRect->right - 2;
//	pOutClient->top = pRect->top + 1;
//}
