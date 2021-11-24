// DistributeSketcher.cpp: implementation of the CDistributeSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DistributeSketcher.h"

#include "gu.h"
#include "PCDraftView.h"
#include "MDDimStr.h"
#include "MDAssocDim.h"
#include "DraftContext.h"


//////////////////////////////////////////////////////////////////////

CDistributeSketcher::CDistributeSketcher(CMDShapeList* pShapeList,bool bByDatum) : m_pShapeList(pShapeList), m_bByDatum(bByDatum)
{
	m_nCount=m_pShapeList->GetCount()-1;
	POSITION pos=m_pShapeList->GetHeadPosition();
	while(pos)
		if(m_pShapeList->GetNext(pos)->IsKindOf(RUNTIME_CLASS(CMDDimStr))) m_nCount--;
}


void CDistributeSketcher::OnDrawSketcher(CDC * pDC)
{

	CLineSketcher::OnDrawSketcher(pDC);

	double distH = ( m_ptTo.x.GetRawDouble () - m_ptFrom.x.GetRawDouble () ) / m_nCount;
	double distV = ( m_ptTo.y.GetRawDouble () - m_ptFrom.y.GetRawDouble () ) / m_nCount;
	POSITION pos=m_pShapeList->GetHeadPosition();
	int i=0;
	while(pos)
	{
		CMDShape* pShape=m_pShapeList->GetNext(pos);
		if(pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr)) || (pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) && m_pView->IsLinkedShape(pShape,-1))) continue;
		LongRect ext=pShape->GetExtents();
		LongPoint objPt=m_bByDatum?pShape->GetShapeDatum():Middle(ext);
		ext.Offset(m_ptFrom.x-objPt.x+WrFixedFromRawDoubleTruncated (i * distH),m_ptFrom.y-objPt.y+WrFixedFromRawDoubleTruncated(i*distV));
		CRect rc=m_pView->m_Mapper.LPtoDP(ext);
      pDC->DrawFocusRect(&rc);
		i++;

	}

}


bool CDistributeSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	double distH = ( m_ptTo.x.GetRawDouble () - m_ptFrom.x.GetRawDouble () ) / m_nCount;
	double distV = ( m_ptTo.y.GetRawDouble () - m_ptFrom.y.GetRawDouble () ) / m_nCount;
	POSITION pos=m_pShapeList->GetHeadPosition();
	int i=0;
	while(pos)
	{
		CMDShape* pShape=m_pShapeList->GetNext(pos);
		if(pShape->IsKindOf(RUNTIME_CLASS(CMDDimStr)) || (pShape->IsKindOf(RUNTIME_CLASS(CMDAssocDim)) && m_pView->IsLinkedShape(pShape,-1))) continue;
		LongRect ext=pShape->GetExtents();
		LongPoint objPt=m_bByDatum?pShape->GetShapeDatum():Middle(ext);
		m_pView->InvalidateShape(pShape,true);
		LongPoint delta(m_ptFrom.x-objPt.x+WrFixedFromRawDoubleTruncated (i * distH),m_ptFrom.y-objPt.y+WrFixedFromRawDoubleTruncated(i*distV));
		pShape->DoShapeOffset(delta);
		CMDShape* pAttachedShape=pShape->GetAttachedShape();
		if(pAttachedShape) pAttachedShape->DoShapeOffset(delta);
		m_pView->InvalidateShape(pShape,true);
		i++;
	}
	return true;
}
