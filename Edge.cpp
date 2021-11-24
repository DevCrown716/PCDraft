// Edge.cpp: implementation of the CEdge class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "Edge.h"

#include "gu.h"
#include "phmath.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "MDLineShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CEdge,CObject,1)

//////////////////////////////////////////////////////////////////////

void CEdge::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_nType;
		ar >> m_lptFrom;
		ar >> m_lptTo;
	}
	else
	{
		ar << m_nType;
		ar << m_lptFrom;
		ar << m_lptTo;
	}
}

void CEdge::Draw(CDraftContext* pDC) const
{
	pDC->Line(m_lptFrom,m_lptTo);
}

void CEdge::DrawOutlinePath(CDraftContext* pDC) const
{
   pointd p1;
   pointd p2;
   p1 = pDC->GetMapper()->LPtoDPF(m_lptFrom);
   p2 = pDC->GetMapper()->LPtoDPF(m_lptTo);
   pDC->path()->AddLine(__pointf(p1), __pointf(p2));
   pDC->MoveTo(m_lptTo);
}

bool CEdge::PtAtEdge(const pointd& pt,const CMapper* pMapper) const
{
   pointd p1;
   pointd p2;
   
   pMapper->LPtoDP(p1, m_lptFrom);
   pMapper->LPtoDP(p2, m_lptTo);
	return guPtAtLine(pt,p1,p2,1,0);
}

bool CEdge::PtAtEdge(const LongPoint& lpt) const
{
//	return guPtAtLine(pointd(lpt.x.GetFixed(),lpt.y.GetFixed()),pointd(m_lptFrom.x.GetFixed(),m_lptFrom.y.GetFixed()),pointd(m_lptTo.x.GetFixed(),m_lptTo.y.GetFixed()),1,0);
	LongPoint lipt;
	if(guNearPtLnPt(lpt,m_lptFrom,m_lptTo,&lipt))
	{
		if(guAlmostEqualPt(lpt,lipt))
		{
			Angle angle=mtAngle(m_lptFrom,m_lptTo);
			LongPoint lptTo=mtRotPoint(m_lptTo,-angle,m_lptFrom);
			LongPoint lptRot=mtRotPoint(lpt,-angle,m_lptFrom);
			if(lptRot.x>=m_lptFrom.x && lptRot.x<=lptTo.x) return true;
		}
	}
	return false;
}

void CEdge::PerpPt(const LongPoint &lptTo, LongPoint *lptFrom) const
{
	guNearPtLnPt(lptTo,m_lptFrom,m_lptTo,lptFrom);
}

CMDShape* CEdge::CreateShape() const
{
	CMDLineShape* pShape=new CMDLineShape(m_lptFrom,m_lptTo);
	return pShape;
}

void CEdge::Reverse()
{
	LongPoint lpt=m_lptFrom;
	m_lptFrom=m_lptTo;
	m_lptTo=lpt;
}

CEdge* CEdge::CreateCopy() const
{
	return new CEdge(m_lptFrom,m_lptTo);
}

LongRect CEdge::GetExtents() const
{
	return guPt2Rect(m_lptFrom,m_lptTo);
}

void CEdge::Offset(const LongPoint &lpt)
{
	m_lptFrom+=lpt;
	m_lptTo+=lpt;
}

bool CEdge::IsEqual(CEdge *pEdge)
{
	return m_nType==pEdge->m_nType && m_lptFrom==pEdge->m_lptFrom && m_lptTo==pEdge->m_lptTo;
}

short CEdge::gLastEdgeSign=0;

int CEdge::Intersect(CEdge* pEdge,CEdgeList* pList,CEdgeList* pOtherList,bool bRevert)
{
	int found=0;
	switch(pEdge->m_nType)
	{
	case kLineEdge:
		{
            // SMK.SSA can be error last guAlmostEqualPt may be  guAlmostEqualPt(m_lptTo,pEdge->m_lptTo) ?
			if(guAlmostEqualPt(m_lptFrom,pEdge->m_lptFrom) || guAlmostEqualPt(m_lptFrom,pEdge->m_lptTo) ||
				guAlmostEqualPt(m_lptTo,pEdge->m_lptFrom) || guAlmostEqualPt(m_lptFrom,pEdge->m_lptTo))
				break;

			LongRect rect1=guPt2Rect(m_lptFrom,m_lptTo);
			LongRect rect2=guPt2Rect(pEdge->m_lptFrom,pEdge->m_lptTo);
			rect1.Inset( -FixedMinDelta, -FixedMinDelta);
			rect2.Inset( -FixedMinDelta, -FixedMinDelta);
			LongRect sectRect;
			if(!guSectRect(rect1, rect2, &sectRect)) break;

			LongPoint lipt;
			short nSign=0;
			if(guLongLnLnPt(m_lptFrom,m_lptTo,pEdge->m_lptFrom,pEdge->m_lptTo, &lipt) && sectRect.Contains(lipt))
			{
				Angle ang=mtAngle(pEdge->m_lptFrom,pEdge->m_lptTo);
				if(guAlmostEqualPt(m_lptFrom,lipt))
				{
					nSign=RoundToShort(Sign(mtRotPoint(m_lptTo,ang,pEdge->m_lptFrom).y-pEdge->m_lptFrom.y));
				}
				else if(guAlmostEqualPt(m_lptTo,lipt))
				{
					nSign=RoundToShort(Sign(mtRotPoint(m_lptFrom,ang,pEdge->m_lptFrom).y-pEdge->m_lptFrom.y));
				}
				else
				{
					if(pList)
					{
						pList->AddTail(new CEdge(lipt,m_lptTo));
						m_lptTo=lipt;
					}
					found++;
				}
				if(!guAlmostEqualPt(pEdge->m_lptFrom,lipt) && !guAlmostEqualPt(pEdge->m_lptTo,lipt))
				{
					if(pOtherList)
					{
						pOtherList->AddTail(new CEdge(lipt,pEdge->m_lptTo));
						pEdge->m_lptTo=lipt;
					}
					found++;
				}
				if(found<2)
				{
					if(gLastEdgeSign)
					{
						if(gLastEdgeSign==nSign) found=-1;
						else found=1;
						gLastEdgeSign=0;
					}
					else gLastEdgeSign=nSign;
				}
			}
/*			else
			{
				if(!pList && !pOtherList)
				{
					LongPoint lipt;
					if(guNearPtLnPt(m_lptFrom,pEdge->m_lptFrom,pEdge->m_lptTo,&lipt))
					{
						if(guAlmostEqualPt(m_lptFrom,lipt))	return 2;
					}
				}
			}
*/
		}
		break;
	case kArcEdge:
		return pEdge->Intersect(this,pOtherList,pList,true);
	default:
		break;
	}
	return found;
}

bool CEdge::Concatenate(CEdge *pEdge)
{
	if(pEdge->m_nType==kLineEdge)
	{
		// for horizontal or vertical only
		if(m_lptTo==pEdge->m_lptFrom &&
			((m_lptFrom.x==m_lptTo.x && pEdge->m_lptFrom.x==pEdge->m_lptTo.x) ||
			(m_lptFrom.y==m_lptTo.y && pEdge->m_lptFrom.y==pEdge->m_lptTo.y)))
		{
			m_lptTo=pEdge->m_lptTo;
			return true;
		}
	}
	return false;
}

const LongPoint& CEdge::GetPoint(int i) const
{
	switch(i)
	{
	case 0:
		return m_lptFrom;
	case 1:
		return m_lptTo;
	default:
		ASSERT(0);
	}
	return LongPoint::kZeroPt;
}

void CEdge::SetPoint(int i,const LongPoint& lpt)
{
	switch(i)
	{
	case 0:
		m_lptFrom=lpt;
		break;
	case 1:
		m_lptTo=lpt;
		break;
	default:
		ASSERT(0);
	}
}

void CEdge::Scale(const LongRect &lrcFrom, const LongRect &lrcTo)
{
/*	LongRatio ratio;
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

	m_lptFrom=lrcTo.GetCorner(0)+(m_lptFrom-lrcFrom.GetCorner(0))*ratio;
	m_lptTo=lrcTo.GetCorner(0)+(m_lptTo-lrcFrom.GetCorner(0))*ratio;
*/
	LongRatio xRatio,yRatio;
	xRatio.fN=(lrcTo.right-lrcTo.left).GetFixed();
	xRatio.fD=(lrcFrom.right-lrcFrom.left).GetFixed();
	yRatio.fN=(lrcTo.bottom-lrcTo.top).GetFixed();
	yRatio.fD=(lrcFrom.bottom-lrcFrom.top).GetFixed();

	int nPoints=GetNumPoints();
	for(int i=0;i<nPoints;i++)
	{
		LongPoint lptOffset=GetPoint(i)-lrcFrom.GetCorner(0);
		lptOffset.x*=xRatio;
		lptOffset.y*=yRatio;
		SetPoint(i,lrcTo.GetCorner(0)+lptOffset);
	}
}

void CEdge::Scale(const LongRatio& ratio, double factor)
{
	m_lptFrom*=ratio;
	m_lptTo*=ratio;
	if(factor)
	{
		m_lptFrom*=factor;
		m_lptTo*=factor;
	}
}

void CEdge::Rotate(Angle angle, const LongPoint &lptAbout)
{
	m_lptFrom=mtRotPoint(m_lptFrom,angle,lptAbout);
	m_lptTo=mtRotPoint(m_lptTo,angle,lptAbout);
}

void CEdge::Mirror(const LongPoint& lptRef1,const LongPoint& lptRef2)
{
	LongPoint perpPt;
	guNearPtLnPt(m_lptFrom,lptRef1,lptRef2,&perpPt);
	m_lptFrom+=((perpPt-m_lptFrom)<<1);
	guNearPtLnPt(m_lptTo,lptRef1,lptRef2,&perpPt);
	m_lptTo+=((perpPt-m_lptTo)<<1);
}

void CEdge::Flip(bool bVertical,const LongPoint& lpt)
{
	if(bVertical)
	{
		m_lptFrom.y=lpt.y-m_lptFrom.y;
		m_lptTo.y=lpt.y-m_lptTo.y;
	}
	else
	{
		m_lptFrom.x=lpt.x-m_lptFrom.x;
		m_lptTo.x=lpt.x-m_lptTo.x;
	}
}

bool CEdge::Snap(LongPoint *lptSnap, const LongRect &lrcProx, short nType) const
{
	if(lrcProx.Contains(m_lptFrom))
	{
		*lptSnap=m_lptFrom;
		return true;
	}
	if(lrcProx.Contains(m_lptTo))
	{
		*lptSnap=m_lptTo;
		return true;
	}
	return false;
}

double CEdge::SubAreaCalc(const LongRatio &ratio)
{
	double area = lineArea(ratio, m_lptFrom, m_lptTo);

/*	bool runInReverse = IsShapeFlags ( kRunInReverseMask ) ;
	if ( runInReverse )
		area = - area ; // compensate for going around in the wrong direction by negating value */
	return area; 
} 

double CEdge::lineArea(const LongRatio &ratio, const LongPoint &from, const LongPoint &to)
{
	return CMDShape::ScaleDist(ratio, to.x-from.x)* CMDShape::ScaleDist(ratio,to.y+from.y)/2 ;
}
