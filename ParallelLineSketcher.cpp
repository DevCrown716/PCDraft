// ParallelLineSketcher.cpp: implementation of the CParallelLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ParallelLineSketcher.h"

#include "gu.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDParallelLine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

LongPoint CParallelLineSketcher::AdjustPosition(LongPoint ptTo)
{
	if(GetKeyState(VK_SHIFT)<0)
		guGetHV45LPt(&ptTo,&m_ptFrom);
	if(GetKeyState(VK_OPTION)<0)
		m_pView->SnapLPtToLength(m_ptFrom,ptTo);
	return ptTo;
}

void CParallelLineSketcher::OnDrawSketcher(CDC * pDC)
{
	LongPoint pp1,pp2,pp3,pp4;
	guParallelLn(m_ptFrom,m_ptTo,m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp1,&pp2,&pp3,&pp4);
	pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp1));
	pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp2));
	pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp3));
	pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp4));
}

bool CParallelLineSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	if(m_ptFrom!=m_ptTo)
	{
		m_pView->SaveUndo(IDS_CREATION);
		CMDParallelLine* pShape=new CMDParallelLine(m_ptFrom,m_ptTo,m_Attrs);
		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);

		return true;
	}
	return false;
}
