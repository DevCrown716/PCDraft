// OriginSketcher.cpp: implementation of the COriginSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "OriginSketcher.h"
#include "pcdraftview.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COriginSketcher::COriginSketcher()
{

}

COriginSketcher::~COriginSketcher()
{

}

void COriginSketcher::OnDrawSketcher(CDC * pDC)
{
	CRect rc;
	m_pView->GetActiveClientRect(rc);
	rc.OffsetRect(m_pView->GetDeviceScrollPosition());

	pointd pt=m_pView->m_Mapper.LPtoDPF(m_ptOrigin);

	CPen pen(PS_DOT,0,COLORREF(0));
	CGDIObject<CPen> oldPen(pDC,&pen);

	pDC->MoveTo(rc.left,pt.y);
	pDC->LineTo(rc.right,pt.y);
	
	pDC->MoveTo(pt.x,rc.top);
	pDC->LineTo(pt.x,rc.bottom);
}

void COriginSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC, pt);
	OnUpdatePt(pt);
}

void COriginSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_ptOrigin=ptUpdate;
}
