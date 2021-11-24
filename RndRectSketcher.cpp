// RoundRectSketcher.cpp: implementation of the CRndRectSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RndRectSketcher.h"

#include "gu.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDRndRectangle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CRndRectSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_ptTo=ptUpdate;
	if(GetKeyState(VK_SHIFT)<0)
	{
		// rectangle sketcher draws squares
		Coord a=m_ptTo.x-m_ptFrom.x;
		Coord b=m_ptTo.y-m_ptFrom.y;
		if(Abs(a)>Abs(b)) a=Sign(a)*Abs(b);
		else b=Sign(b)*Abs(a);
		m_ptTo.x=m_ptFrom.x+a;
		m_ptTo.y=m_ptFrom.y+b;
	}

	m_lptCorner=Abs(m_ptTo-m_ptFrom);
	m_pView->SetShowSize(0,1,m_lptCorner.x);
	m_pView->SetShowSize(2,2,m_lptCorner.y);
	m_lptCorner>>=2;
	if(m_nMode==kCornerElliptical)
	{
		m_pView->SetShowSize(1,5,m_lptCorner.x);
		m_pView->SetShowSize(3,6,m_lptCorner.y);
	}
	else
	{
		if(m_lptCorner.x<m_lptCorner.y) m_lptCorner.y=m_lptCorner.x;
		else  m_lptCorner.x=m_lptCorner.y;
		m_pView->SetShowSize(1,4,m_lptCorner.x);
	}
}

void CRndRectSketcher::OnDrawSketcher(CDC * pDC)
{
	CRect rc(m_pView->m_Mapper.LPtoDP(m_ptFrom),m_pView->m_Mapper.LPtoDP(m_ptTo));
	pointd ptc=m_pView->m_Mapper.LPtoDP(m_lptCorner<<1);
	rc.NormalizeRect();
	rc.right++;
	rc.bottom++;
	pDC->RoundRect(&rc,ptc);
}

bool CRndRectSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	if(m_ptFrom!=m_ptTo)
	{
		m_pView->SaveUndo(IDS_CREATION);

		CMDRndRectangle* pShape;
		pShape=new CMDRndRectangle((CMDRndRectangle::CornerType)m_nMode);

		LongRect lrc(m_ptFrom,m_ptTo);
		lrc.Normalize();
		for(int i=0;i<4;i++) pShape->SetShapePoint(i,lrc.GetCorner(i));
		pShape->m_lptCornerDelta=m_lptCorner;

		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		pShape->SetFillModel(m_pView->GetDocument()->m_Fill);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		return true;
	}
	return false;
}
