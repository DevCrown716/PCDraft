// RubberBandSketcher.cpp: implementation of the CRubberBandSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RubberBandSketcher.h"

#include "gu.h"
#include "PCDraftView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CRubberBandSketcher::OnDrawSketcher(CDC *pDC)
{
	m_rc.SetRect(m_pView->m_Mapper.LPtoDP(m_ptFrom),m_pView->m_Mapper.LPtoDP(m_ptTo));
	m_rc.NormalizeRect();

	if(!m_rc.IsRectEmpty())
	{
		pDC->DrawFocusRect(&m_rc);
	}
}

bool CRubberBandSketcher::OnCompleteSketch(CDC *pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	return true;
}
