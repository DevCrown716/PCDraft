// RefPointSketcher.cpp: implementation of the CRefPointSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RefPointSketcher.h"

#include "PCDraftView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CRefPointSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC, pt);
	m_lptRef=pt;
	ShowCursor(FALSE);
}

bool CRefPointSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	ShowCursor(TRUE);
	return true;
}

void CRefPointSketcher::OnCancelSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCancelSketch(pDC,pt);
	ShowCursor(TRUE);
}

CSketcher::ContinueMode CRefPointSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_MOUSEMOVE:
		return ContinueUpdatePt;

	case WM_LBUTTONDOWN:
		return Complete;
	}
	return CSketcher::QueryContinue();
}

void CRefPointSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_lptRef=ptUpdate;
}

#define	kBoxWidth		25
#define	kDeltaCorner	9
#define	kDeltaCircle	5

void CRefPointSketcher::OnDrawSketcher(CDC * pDC)
{
	pointd ptCenter=m_pView->m_Mapper.LPtoDP(m_lptRef);
	CRect rc=BoundRectOfCircle(ptCenter,kBoxWidth-kDeltaCircle-1);

	{
		CPen pen(PS_DOT,1,RGB(0,0,0));
		CGDIObject<CPen> op(pDC,&pen);
		pDC->Ellipse(&rc);
	}

	rc.InflateRect(kDeltaCircle,kDeltaCircle);

	pDC->MoveTo(ptCenter.x, rc.top);
	pDC->LineTo(ptCenter.x, rc.top+kDeltaCorner);
	pDC->MoveTo(ptCenter.x, rc.bottom);
	pDC->LineTo(ptCenter.x, rc.bottom-kDeltaCorner);
	pDC->MoveTo(rc.left, ptCenter.y);
	pDC->LineTo(rc.left+kDeltaCorner, ptCenter.y);
	pDC->MoveTo(rc.right,ptCenter.y);
	pDC->LineTo(rc.right-kDeltaCorner,ptCenter.y);

	pDC->MoveTo(ptCenter.x-4,ptCenter.y);
	pDC->LineTo(ptCenter.x+4,ptCenter.y);
	pDC->MoveTo(ptCenter.x,ptCenter.y-4);
	pDC->LineTo(ptCenter.x,ptCenter.y+4);
}
