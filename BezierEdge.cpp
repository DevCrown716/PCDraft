// BezierEdge.cpp: implementation of the CBezierEdge class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "BezierEdge.h"

#include "phmath.h"
#include "DraftContext.h"
#include "MDBezierShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CBezierEdge,CEdge,1)

//////////////////////////////////////////////////////////////////////

void CBezierEdge::Serialize(CArchive& ar)
{
	CEdge::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_lptFromCtl;
		ar >> m_lptToCtl;
	}
	else
	{
		ar << m_lptFromCtl;
		ar << m_lptToCtl;
	}
}

void CBezierEdge::Draw(CDraftContext* pDC) const
{
	pDC->BezierDash(m_lptFrom,m_lptFromCtl,m_lptToCtl,m_lptTo);
//	pDC->Bezier(m_lptFrom,m_lptFromCtl,m_lptToCtl,m_lptTo);
}

void CBezierEdge::DrawOutlinePath(CDraftContext* pDC) const
{
   pointd p1;
   pointd p2;
   pointd p3;
   pointd p4;
   p1 = pDC->GetMapper()->LPtoDPF(m_lptFrom);
   p2 = pDC->GetMapper()->LPtoDPF(m_lptFromCtl);
   p3 = pDC->GetMapper()->LPtoDPF(m_lptToCtl);
   p4 = pDC->GetMapper()->LPtoDPF(m_lptTo);
   pDC->path()->AddBezier(__pointf(p1), __pointf(p2), __pointf(p3), __pointf(p4));

}

bool CBezierEdge::PtAtEdge(const pointd& pt, const CMapper* pMapper) const
{
	CDrawPrim dp;
   pointd p1;
   pointd p2;
   pointd p3;
   pointd p4;
   pMapper->LPtoDP(p1, m_lptFrom);
   pMapper->LPtoDP(p2, m_lptFromCtl);
   pMapper->LPtoDP(p3, m_lptToCtl); 
   pMapper->LPtoDP(p4, m_lptTo);
	if(dp.checkBezier(p1, p2, p3, p4,pt,1)) return true;
	return false;
}

CMDShape* CBezierEdge::CreateShape() const
{
	CMDBezierShape* pShape=NULL;
	pShape=new CMDBezierShape(4);
	pShape->SetShapePoint(0,m_lptFrom);
	pShape->SetShapePoint(1,m_lptFromCtl);
	pShape->SetShapePoint(2,m_lptToCtl);
	pShape->SetShapePoint(3,m_lptTo);
	pShape->CalculateExtents();
	return pShape;
}

CEdge* CBezierEdge::CreateCopy() const
{
	return new CBezierEdge(m_lptFrom,m_lptFromCtl,m_lptToCtl,m_lptTo);
}

void CBezierEdge::Reverse()
{
	CEdge::Reverse();
	LongPoint lpt=m_lptFromCtl;
	m_lptFromCtl=m_lptToCtl;
	m_lptToCtl=lpt;
}

LongRect CBezierEdge::GetExtents() const
{
	LongRect lrcExt=CEdge::GetExtents();
	lrcExt|=m_lptFromCtl;
	lrcExt|=m_lptToCtl;
	return lrcExt;
}

void CBezierEdge::Offset(const LongPoint &lpt)
{
	CEdge::Offset(lpt);
	m_lptFromCtl+=lpt;
	m_lptToCtl+=lpt;
}

bool CBezierEdge::IsEqual(CEdge *pEdge)
{
	CBezierEdge* pBezEdge=(CBezierEdge*)pEdge;
	return CEdge::IsEqual(pEdge) && m_lptFromCtl==pBezEdge->m_lptFromCtl && m_lptToCtl==pBezEdge->m_lptToCtl;
}

bool CBezierEdge::Concatenate(CEdge *pEdge)
{
	return false;
}

const LongPoint& CBezierEdge::GetPoint(int i) const
{
	switch(i)
	{
	case 2:
		return m_lptFromCtl;
	case 3:
		return m_lptToCtl;
	default:
		return CEdge::GetPoint(i);
	}
}

void CBezierEdge::SetPoint(int i,const LongPoint& lpt)
{
	switch(i)
	{
	case 2:
		m_lptFromCtl=lpt;
		break;
	case 3:
		m_lptToCtl=lpt;
		break;
	default:
		CEdge::SetPoint(i,lpt);
		break;
	}
}
/*
void CBezierEdge::Scale(const LongRect &lrcFrom, const LongRect &lrcTo)
{
	CEdge::Scale(lrcFrom,lrcTo);

	LongRatio ratio;
	if(Width(lrcFrom)>Height(lrcFrom))
	{
		ratio.fN=Width(lrcTo).GetFixed();
		ratio.fD=Width(lrcFrom).GetFixed();
	}
	else
	{
		ratio.fN=Height(lrcTo).GetFixed();
		ratio.fD=Height(lrcFrom).GetFixed();
	}

	m_lptFromCtl=lrcTo.GetCorner(0)+(m_lptFromCtl-lrcFrom.GetCorner(0))*ratio;
	m_lptToCtl=lrcTo.GetCorner(0)+(m_lptToCtl-lrcFrom.GetCorner(0))*ratio;
}
*/
void CBezierEdge::Scale(const LongRatio& ratio, double factor)
{
	CEdge::Scale(ratio,factor);
	m_lptFromCtl*=ratio;
	m_lptToCtl*=ratio;
	if(factor)
	{
		m_lptFromCtl*=factor;
		m_lptToCtl*=factor;
	}
}

void CBezierEdge::Rotate(Angle angle, const LongPoint &lptAbout)
{
	CEdge::Rotate(angle,lptAbout);
	m_lptFromCtl=mtRotPoint(m_lptFromCtl,angle,lptAbout);
	m_lptToCtl=mtRotPoint(m_lptToCtl,angle,lptAbout);
}

void CBezierEdge::Mirror(const LongPoint& lptRef1,const LongPoint& lptRef2)
{
	CEdge::Mirror(lptRef1,lptRef2);
	LongPoint perpPt;
	guNearPtLnPt(m_lptFromCtl,lptRef1,lptRef2,&perpPt);
	m_lptFromCtl+=((perpPt-m_lptFromCtl)<<1);
	guNearPtLnPt(m_lptToCtl,lptRef1,lptRef2,&perpPt);
	m_lptToCtl+=((perpPt-m_lptToCtl)<<1);
}

void CBezierEdge::Flip(bool bVertical,const LongPoint& lpt)
{
	CEdge::Flip(bVertical,lpt);
	if(bVertical)
	{
		m_lptFromCtl.y=lpt.y-m_lptFromCtl.y;
		m_lptToCtl.y=lpt.y-m_lptToCtl.y;
	}
	else
	{
		m_lptFromCtl.x=lpt.x-m_lptFromCtl.x;
		m_lptToCtl.x=lpt.x-m_lptToCtl.x;
	}
}

double CBezierEdge::SubAreaCalc(const LongRatio &ratio)
{
	double area = 0.0 ;
	area+=lineArea(ratio, m_lptFrom, m_lptFromCtl);
	area+=lineArea(ratio, m_lptFromCtl, m_lptToCtl);
	area+=lineArea(ratio, m_lptToCtl, m_lptTo);

/*	bool runInReverse = IsShapeFlags ( kRunInReverseMask ) ;
	if ( runInReverse )
		area = - area ; // compensate for going around in the wrong direction by negating value */
	return area * 0.5 ;
}
