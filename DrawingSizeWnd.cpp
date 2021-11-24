// DrawingSizeWnd.cpp : implementation file
//

#include "stdafx.h"
#include "PCDraft.h"
#include "DrawingSizeWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDrawingSizeWnd

CDrawingSizeWnd::CDrawingSizeWnd()
{
	m_PageSize.cx=0;
	m_PageSize.cy=0;
	m_NumPage.x=0;
	m_NumPage.y=0;
	m_MinNumPage.x=0;
	m_MinNumPage.y=0;
	m_bTrackSize=false;
}

CDrawingSizeWnd::~CDrawingSizeWnd()
{
}

BEGIN_MESSAGE_MAP(CDrawingSizeWnd, CWnd)
	//{{AFX_MSG_MAP(CDrawingSizeWnd)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDrawingSizeWnd message handlers

BOOL CDrawingSizeWnd::Create(CRect& rc,CWnd *pParent,UINT nID)
{
	return CWnd::Create(AfxRegisterWndClass(0,0,(HBRUSH)GetStockObject(WHITE_BRUSH)),"",WS_CHILD|WS_VISIBLE|WS_BORDER,rc,pParent,nID,NULL);
}

void CDrawingSizeWnd::OnPaint() 
{
	CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);

	// 453.39cm Drawing/Size - Infinisys Ltd
   CBrush brush;
   brush.CreateStockObject(LTGRAY_BRUSH);
   dc.FillRect(rc,&brush);
   int cx= m_DevPageSize.cx;
   int cy= m_DevPageSize.cy;
   int w = cx *m_MaxNumPage.x;
   int h = cy *m_MaxNumPage.y;
   dc.FillSolidRect(0,0,w,h,RGB(255,255,255));
   for(int x=cx, i = 0;i < m_MaxNumPage.x;x+=cx, i++)
	{
		dc.MoveTo(x,0);
		dc.LineTo(x,h);
	}
   for(int y=cy,i = 0;i < m_MaxNumPage.y;y+=cy,i++)
	{
		dc.MoveTo(0,y);
		dc.LineTo(w,y);
	}
	rc.SetRect(0,0,cx*m_NumPage.x,cy*m_NumPage.y);
	dc.InvertRect(&rc);
}

void CDrawingSizeWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	if(m_bTrackSize)
	{
		CRect rc;
		GetClientRect(&rc);
		rc.right+=max(m_DevPageSize.cx,2);
		rc.bottom+=max(m_DevPageSize.cy,2);
		if(rc.PtInRect(point))
			SetNewDrawingSize(point);
	}
}

void CDrawingSizeWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	SetNewDrawingSize(point);
	m_bTrackSize=true;
}


void CDrawingSizeWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bTrackSize)
	{
		ReleaseCapture();
		m_bTrackSize=false;
	}
}

void CDrawingSizeWnd::SetNewDrawingSize(CPoint pt)
{
	int x=pt.x/(m_DevPageSize.cx)+1;
	int y=pt.y/(m_DevPageSize.cy) +1;
	if(x<m_MinNumPage.x) x=m_MinNumPage.x;
	if(y<m_MinNumPage.y) y=m_MinNumPage.y;
   if(x> m_MaxNumPage.x) x=m_MaxNumPage.x;
   if(y > m_MaxNumPage.y) y=m_MaxNumPage.y;
   if(x != m_NumPage.x || y != m_NumPage.y)
	{
		m_NumPage.x=x;
		m_NumPage.y=y;
		Invalidate();
		GetParent()->SendMessage(DSN_SIZECHANGED,(WPARAM)GetDlgCtrlID());
	}
}

void CDrawingSizeWnd::OnSize(UINT nType, int cx, int cy) 
{
	m_DevPageSize.cx=::MulDiv(cx-10,m_PageSize.cx,kMaxDocH);
	m_DevPageSize.cy=::MulDiv(cy-10,m_PageSize.cy,kMaxDocV);
   m_MaxNumPage.x =(kMaxDocH / m_PageSize.cx) + ((kMaxDocH % m_PageSize.cx == 0) ? 0 : 1);
   m_MaxNumPage.y = (kMaxDocV / m_PageSize.cy) + ((kMaxDocV % m_PageSize.cy == 0) ? 0 : 1);
	CWnd::OnSize(nType, cx, cy);
}

CSize CDrawingSizeWnd::GetSize()
{
	CSize theSize;
	theSize.cx=m_PageSize.cx*m_NumPage.x;
	theSize.cy=m_PageSize.cy*m_NumPage.y;
	if(theSize.cx>kMaxDocH)
		theSize.cx=kMaxDocH;
	if(theSize.cy>kMaxDocH)
		theSize.cy=kMaxDocV;
	return theSize;
}
