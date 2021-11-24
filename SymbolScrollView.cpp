// SymbolScrollView.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "SymbolScrollView.h"
#include "mdshape.h"
#include "draftcontext.h"
#include "mapper.h"
#include "SymbolLibraryDoc.h"
#include "SymbolLibraryView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSymbolScrollView

IMPLEMENT_DYNAMIC(CSymbolScrollView, CScrollView)

CSymbolScrollView::CSymbolScrollView()
{
   m_pShape=NULL;
	m_bHideHandles=true;
}

CSymbolScrollView::~CSymbolScrollView()
{
}


BEGIN_MESSAGE_MAP(CSymbolScrollView, CScrollView)
	//{{AFX_MSG_MAP(CSymbolScrollView)
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_MOUSEACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSymbolScrollView drawing

void CSymbolScrollView::SetShape(CMDShape* pShape)
{
	m_bHideHandles=true;
	m_pShape=pShape;
	CSize sizeTotal;
	if(pShape)
	{
		CMapper theMapper;
		LongRect lrc=pShape->CalculateBasicExtents();
		pShape->ExtendForPen(lrc);
		CRect rc=theMapper.LPtoDP(lrc);
		CPoint pt=theMapper.LPtoDP(lrc.GetCorner(0));
		m_pShape->DoShapeOffset(theMapper.DPtoLP(pointd(5.f,5.f)-pt));
		sizeTotal.cx=rc.Width()+10;
		sizeTotal.cy=rc.Height()+10;
	}
	else
	{
		sizeTotal.cx=sizeTotal.cy=1;
	}
	SetScrollSizes(MM_TEXT,sizeTotal);
	Invalidate();
}

void CSymbolScrollView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	CSize sizeTotal;
	sizeTotal.cx=sizeTotal.cy=1;
	SetScrollSizes(MM_TEXT, sizeTotal);
}

void CSymbolScrollView::HideHandles(bool bHideHandles)
{
	m_bHideHandles=bHideHandles;
	Invalidate();
}

void CSymbolScrollView::OnDraw(CDC* pDC)
{
	if(m_pShape)
	{
		CRect rc;
		GetClientRect(&rc);
		CRgn theClipRgn;
		theClipRgn.CreateRectRgnIndirect(&rc);
		pDC->SelectClipRgn(&theClipRgn);

		CMapper theMapper;
		CDraftContext theContext(pDC,&theMapper, NULL, (CVersionDoc *) GetDocument());
		m_pShape->DrawShape(&theContext);
		if(!m_bHideHandles)
		{
			m_pShape->DrawHandles(&theContext);
			// draw datum
			CGDIROP rop(pDC,R2_NOT);
			CPoint pt=theMapper.LPtoDP(m_pShape->GetShapeDatum());
			pDC->MoveTo(pt+CPoint(-4,-4));
			pDC->LineTo(pt+CPoint(5,5));
			pDC->MoveTo(pt+CPoint(4,-4));
			pDC->LineTo(pt+CPoint(-5,5));
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSymbolScrollView diagnostics

#ifdef _DEBUG
void CSymbolScrollView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CSymbolScrollView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSymbolScrollView message handlers

void CSymbolScrollView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(m_pShape && !m_bHideHandles)
	{
		point+=GetDeviceScrollPosition();
		CMapper theMapper;
		LongRect ext=m_pShape->GetExtents();
		CRect rc=theMapper.LPtoDP(ext);
		rc.right++;
		rc.bottom++;
		if(rc.PtInRect(point))
		{
			LongPoint lpt=theMapper.DPtoLP(point);
			if(lpt.x<ext.left) lpt.x=ext.left;
			if(lpt.x>ext.right) lpt.x=ext.right;
			if(lpt.y<ext.top) lpt.y=ext.top;
			if(lpt.y>ext.bottom) lpt.y=ext.bottom;

			Coord topLeft=theMapper.DPtoLP(2), bottomRight=theMapper.DPtoLP(3);
			LongRect lrcProx(lpt.x-topLeft,lpt.y-topLeft,lpt.x+bottomRight,lpt.y+bottomRight);

			int nDatum=m_pShape->Snap(&lpt,lrcProx,0);
			if(nDatum<0)
			{
				m_pShape->SetShapeDatum(lpt);
				Invalidate();
			}
			else
			{
				CClientDC cdc(this);
				CPoint ptOrg=GetDeviceScrollPosition();
				cdc.SetViewportOrg(-ptOrg);

				CGDIROP rop(&cdc,R2_NOT);
				CPoint pt=theMapper.LPtoDP(m_pShape->GetShapeDatum());
				// erase old datum
				cdc.MoveTo(pt+CPoint(-4,-4));
				cdc.LineTo(pt+CPoint(5,5));
				cdc.MoveTo(pt+CPoint(4,-4));
				cdc.LineTo(pt+CPoint(-5,5));

				m_pShape->SetShapeDatum(nDatum);
				pt=theMapper.LPtoDP(m_pShape->GetShapeDatum());
				int nCount=5;
				while(nCount--)
				{
					cdc.MoveTo(pt+CPoint(-4,-4));
					cdc.LineTo(pt+CPoint(5,5));
					cdc.MoveTo(pt+CPoint(4,-4));
					cdc.LineTo(pt+CPoint(-5,5));
					Sleep(100);
				}
			}
			((CSymbolLibraryView*)GetParent())->GetDocument()->SetModifiedFlag();
		}
	}
}

void CSymbolScrollView::CenterShapeDatum()
{
	m_pShape->SetShapeDatum(kCenterDatum);
	((CSymbolLibraryView*)GetParent())->GetDocument()->SetModifiedFlag();
	Invalidate();
}

BOOL CSymbolScrollView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(nHitTest==HTCLIENT && !m_bHideHandles)
	{
		SetCursor(AfxGetApp()->LoadCursor(IDC_SMALL_CROSS));
		return TRUE;
	}
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}


int CSymbolScrollView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	return CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
