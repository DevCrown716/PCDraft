// ParallelPolySketcher.cpp: implementation of the CParallelPolySketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ParallelPolySketcher.h"

#include "gu.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDParallelPolygon.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CParallelPolySketcher::OnDrawSketcher(CDC * pDC)
{
	LongPoint pp11,pp12,pp13,pp14;
	LongPoint pp21,pp22,pp23,pp24;
	guParallelLn(m_pt[0],m_pt[1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);
	if(m_Attrs.m_nEndCap&1)
	{
		pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp12));
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp14));
	}
	for(int i=1;i<m_nCurPoint;i++)
	{
		pp11=pp12;
		pp12=pp22;
		pp13=pp14;
		pp14=pp24;

		guParallelLn(m_pt[i],m_pt[i+1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);

		pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp11));
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp12));
		pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp13));
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp14));
		if(m_Attrs.m_nJoint)
		{
			pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp12));
			pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp14));
		}
	}
	pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp12));
	pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp22));
	pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp14));
	pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp24));
	if(m_Attrs.m_nEndCap&2)
	{
		pDC->MoveTo(m_pView->m_Mapper.LPtoDP(pp22));
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(pp24));
	}

	{
		pointd pt=m_pView->m_Mapper.LPtoDP(m_pt[m_nCurPoint-1]);
		pDC->PatBlt(pt.x-2,pt.y-2,5,5,DSTINVERT);
	}
}

bool CParallelPolySketcher::OnCompleteSketch(CDC *pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	if(m_nCurPoint>1)
	{
		m_pView->SaveUndo(IDS_CREATION);

		CMDParallelPolygon* pShape=new CMDParallelPolygon(m_nCurPoint,m_bClosed,m_Attrs);

		for(int i=0;i<m_nCurPoint;i++)
			pShape->SetShapePoint(i,m_pt[i]);

		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		if(m_bClosed) pShape->SetFillModel(m_pView->GetDocument()->m_Fill);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);

		return true;
	}
	return false;
}
