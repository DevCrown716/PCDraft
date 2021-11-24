// RefLineSketcher.cpp: implementation of the CRefLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RefLineSketcher.h"

#include "gu.h"
#include "PCDraftView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CRefLineSketcher::OnDrawSketcher(CDC *pDC)
{
	if(m_ptFrom!=m_ptTo)
	{
		pointd pt1,pt2;
		pt1=m_pView->m_Mapper.LPtoDP(m_ptFrom);
		pt2=m_pView->m_Mapper.LPtoDP(m_ptTo);
		CRect rc;
		pDC->GetClipBox(&rc);
		guExtendLineToRect(&pt1,&pt2,rc);
		pDC->MoveTo(pt1);
		pDC->LineTo(pt2);
	}
}

bool CRefLineSketcher::OnCompleteSketch(CDC *pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	return m_ptFrom!=m_ptTo;
}
