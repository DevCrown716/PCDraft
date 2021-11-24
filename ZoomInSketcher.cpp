// ZoomInSketcher.cpp: implementation of the CZoomInSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "pcdraftview.h"
#include "ZoomInSketcher.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CZoomInSketcher::CZoomInSketcher(int nZoomFactor)
{
	m_nZoomFactor=nZoomFactor+1;
}

void CZoomInSketcher::OnDrawSketcher(CDC *pDC)
{
	pDC->DrawFocusRect(&m_Rect);
}

void CZoomInSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC, pt);
	m_pView->GetClientSize(m_HalfSize);
	m_HalfSize.cx>>=m_nZoomFactor;
	m_HalfSize.cy>>=m_nZoomFactor;
	OnUpdatePt(pt);
}

void CZoomInSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_ptCenter=ptUpdate;
	pointd ptCenter=m_pView->m_Mapper.LPtoDP(m_ptCenter);
	m_Rect.SetRect(ptCenter.x-m_HalfSize.cx,ptCenter.y-m_HalfSize.cy,ptCenter.x+m_HalfSize.cx,ptCenter.y+m_HalfSize.cy);

	CRect rc;
	m_pView->GetActiveClientRect(rc);
	rc.OffsetRect(m_pView->GetDeviceScrollPosition());

	pointd delta=rc.BottomRight()-m_Rect.BottomRight();
	if(delta.x>0) delta.x=0;
	if(delta.y>0) delta.y=0;
	m_Rect.OffsetRect(delta);

	delta=m_Rect.TopLeft()-rc.TopLeft();
	if(delta.x>0) delta.x=0;
	if(delta.y>0) delta.y=0;
	m_Rect.OffsetRect(-delta);
}
