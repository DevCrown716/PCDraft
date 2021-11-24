// ArcEdge.cpp: implementation of the CArcEdge class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "ArcEdge.h"

#include "gu.h"
#include "phmath.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "MDArcShape.h"
#include "MDLineShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define		mSq(x)			((double)(x)*(double)(x))
const Coord kAlmostSameCriteria = 400.0 / 65536.0;

IMPLEMENT_SERIAL(CArcEdge,CEdge,1)

//////////////////////////////////////////////////////////////////////

void CArcEdge::Serialize(CArchive& ar)
{
	CEdge::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_lptCenter;
		ar >> m_Radii;
		ar >> m_nDirection;
		ar >> m_RotAngle;
	}
	else
	{
		ar << m_lptCenter;
		ar << m_Radii;
		ar << m_nDirection;
		ar << m_RotAngle;
	}
}

void CArcEdge::Draw(CDraftContext* pDC) const
{
	pDC->Arc(m_lptCenter,m_Radii,m_lptFrom,m_lptTo,m_nDirection,m_RotAngle);
}

void CArcEdge::DrawOutlinePath(CDraftContext* pDC) const
{

   pDC->Arc(m_lptCenter, m_Radii, m_lptFrom, m_lptTo, m_nDirection, m_RotAngle);
}

bool CArcEdge::PtAtEdge(const pointd& pt, const CMapper* pMapper) const
{
	pointd radii(pMapper->LPtoDP(m_Radii.x),pMapper->LPtoDP(m_Radii.y));
	if(!guPtAtEllip(pt, pMapper->LPtoDPF(m_lptCenter),(short)radii.x,(short)radii.y,m_RotAngle,1,kCenterLine,false)) return false;
	return CheckAngles(pMapper->DPtoLP(pt));
}

void CArcEdge::PerpPt(const LongPoint &lptTo, LongPoint *lptFrom) const
{
	Angle angle;
	angle=mtATan2(lptTo-m_lptCenter);
	LongPoint ptBase=m_lptCenter;
	ptBase.x+=Pythag(m_lptFrom-m_lptCenter);
	*lptFrom=mtRotPoint(ptBase,angle,m_lptCenter);
}

LongPoint CArcEdge::MidPoint() const
{
	bool bCircular=m_Radii.x==m_Radii.y;
	Angle startAngle,endAngle;

	if(bCircular)
	{
		startAngle=mtAngle(m_lptCenter,m_lptFrom);
		endAngle=mtAngle(m_lptCenter,m_lptTo);
	}
	else
	{
		startAngle=guLPtToLAngle(m_Radii.x,m_Radii.y,mtRotPoint(m_lptFrom,-m_RotAngle,m_lptCenter),m_lptCenter);
		endAngle=guLPtToLAngle(m_Radii.x,m_Radii.y,mtRotPoint(m_lptTo,-m_RotAngle,m_lptCenter),m_lptCenter);
	}
	if(m_nDirection==AD_CLOCKWISE)
	{
		if(startAngle<=endAngle) startAngle+=360;
	}
	else
	{
		if(startAngle>=endAngle) endAngle+=360;
	}
	Angle midAngle=mtFixAngle((startAngle+endAngle)>>1);
	return ArcPoint(midAngle);
}

CMDShape* CArcEdge::CreateShape() const
{
	CMDArcShape* pShape=new CMDArcShape();
	if(m_Radii.x==m_Radii.y) pShape->m_nMode=CMDArcShape::kRadius;
	else
	{
		pShape->m_nMode=CMDArcShape::kElliptic;
		pShape->SetShapePoint(3,m_lptCenter+mtRotPoint(m_Radii,m_RotAngle));
		pShape->SetRotation(m_RotAngle);
	}
	pShape->SetShapePoint(0,m_lptCenter);
	pShape->SetShapePoint(1,m_lptFrom);
	pShape->SetShapePoint(2,m_lptTo);
	pShape->m_nArcDirection=m_nDirection;
	return pShape;
}

CEdge* CArcEdge::CreateCopy() const
{
	return new CArcEdge(m_lptCenter,m_Radii,m_lptFrom,m_lptTo,m_nDirection,m_RotAngle);
}

void CArcEdge::Reverse()
{
	CEdge::Reverse();
	m_nDirection = (m_nDirection==AD_CLOCKWISE)?AD_COUNTERCLOCKWISE:AD_CLOCKWISE;
}

void CArcEdge::Offset(const LongPoint &lpt)
{
	CEdge::Offset(lpt);
	m_lptCenter+=lpt;
}

bool CArcEdge::IsEqual(CEdge *pEdge)
{
	CArcEdge* pArcEdge=(CArcEdge*)pEdge;
	return CEdge::IsEqual(pEdge) && m_lptCenter==pArcEdge->m_lptCenter && m_Radii==pArcEdge->m_Radii;
}

int CArcEdge::Intersect(CEdge* pEdge,CEdgeList* pList,CEdgeList* pOtherList,bool bRevert)
{
	LongPoint		arcCenter;
	LongPoint		linePt1, linePt2, arcPt1, arcPt2;
	Angle			arcStartAng, arcEndAng, iAngle;
	double			m, b, r, k, z1, z2, z3, s1, s2, dy, dx;
	double			x1, y1, cx, cy;
	bool			isVert, isHorz;


	LongPoint		iPt1, iPt2;
	LongPoint		center1, center2;
	LongPoint		startPt1, endPt1, startPt2, endPt2;
	Coord			radius1, radius2 ;
	Angle			startAngle1, endAngle1, iAngle1;
	Angle			startAngle2, endAngle2, iAngle2;
	double			cx1, cx2, cy1, cy2;
	double			oldcx2, oldcy2, oldx1, oldx2, oldy1, oldy2 ;
	double			trans, theta1, theta2 ;
	double			x2, y2, r1, r2, d1 ;
	double			m1, m2, m3, a, c ;
	double			aSq, bSq, A, B, C, D, E1, F, newY1, newX1,
					rotSin, rotSinSq, rotCos, rotCosSq;

	int count=0;
	CEdge* pSelfCopy=CreateCopy();
	CEdge* pEdgeCopy=pEdge->CreateCopy();

	if (m_Radii.x == m_Radii.y) {
		switch(pEdge->m_nType)
		{
		case kLineEdge:
			
			linePt1 = pEdge->m_lptFrom;
			linePt2 = pEdge->m_lptTo;
			arcPt1 = m_lptTo;
			arcPt2 = m_lptFrom;
			
			if((guAlmostEqualPt ( linePt1 , arcPt1 ) && guAlmostEqualPt ( linePt2 , arcPt2 )) ||
				(guAlmostEqualPt ( linePt2 , arcPt1 ) && guAlmostEqualPt ( linePt1 , arcPt2 )))
				break;
			
			
			cx = m_lptCenter.x.GetRawDouble () ;
			cy = m_lptCenter.y.GetRawDouble () ;
			r = m_Radii.x.GetRawDouble () ;
			
			isHorz = false ;
			isVert = false ;
			if (linePt2.y == linePt1.y)
				isHorz = true ;
			else 
				if (linePt2.x == linePt1.x)
					isVert = true ;
				
				if (isVert)
				{
					iPt1.x = iPt2.x = linePt2.x ;
					k = iPt1.x.GetRawDouble () ;
					z1 = 1.0 ;
					z2 = -2.0 * cy ;
					z3 = mSq(k) - (2.0 * k * cx) + mSq(cx) + mSq(cy) - mSq(r) ;
				}
				else 
				{
					if (isHorz)
					{
						iPt1.y = iPt2.y = linePt2.y ;
						k = iPt1.y.GetRawDouble () ;
						z1 = 1.0 ;
						z2 = -2.0 * cx ;
						z3 = mSq(k) - (2.0 * k * cy) + mSq(cx) + mSq(cy) - mSq(r) ;
					}
					else
					{
						dy = ( linePt2.y - linePt1.y ).GetRawDouble () ;
						dx = ( linePt2.x - linePt1.x ).GetRawDouble () ;
						y1 = linePt1.y.GetRawDouble () ;
						x1 = linePt1.x.GetRawDouble () ;
						m = dy/dx ;
						b = y1 - (m * x1) ;
						
						z1 = mSq(m) + 1.0 ;
						z2 = (2.0 * m * b) - (2.0 * cx) - (2.0 * cy * m) ;
						z3 = mSq(cx) + mSq(cy) + mSq(b) - (2.0 * cy * b) - mSq(r) ;
					}
				}
				
				if (!SolveBinomial(&z1, &z2, &z3, &s1, &s2))
					break;
				
				if (isVert)
				{
					iPt1.y.SetFromRawDoubleTruncated ( s1 ) ;
					iPt2.y.SetFromRawDoubleTruncated ( s2 ) ;
				}
				else 
				{
					if (isHorz)
					{
						iPt1.x.SetFromRawDoubleTruncated ( s1 ) ;
						iPt2.x.SetFromRawDoubleTruncated ( s2 ) ;
					}
					else
					{
						iPt1.x.SetFromRawDoubleTruncated ( s1 ) ;
						iPt2.x.SetFromRawDoubleTruncated ( s2 ) ;
						
						iPt1.y.SetFromRawDoubleTruncated ( ( m * s1 ) + b ) ;
						iPt2.y.SetFromRawDoubleTruncated ( ( m * s2 ) + b ) ;
					}
				}
				
				startAngle1 = mtAngle(m_lptCenter,m_lptFrom);
				if(bRevert?(Pythag(iPt1-linePt1)>Pythag(iPt2-linePt1)):((m_nDirection==AD_CLOCKWISE) == (mtFixAngle(mtAngle(m_lptCenter,iPt1)-startAngle1) < mtFixAngle(mtAngle(m_lptCenter,iPt2)-startAngle1))))
				{
					LongPoint temp=iPt1;
					iPt1=iPt2;
					iPt2=temp;
				}
				
				if(pSelfCopy->PtAtEdge(iPt1) && pEdgeCopy->PtAtEdge(iPt1))
				{
					if(!guAlmostEqualPt(linePt1,iPt1) && !guAlmostEqualPt(linePt2,iPt1))
					{
						if(pOtherList)
						{
							pOtherList->AddTail(new CEdge(iPt1,pEdge->m_lptTo));
							pEdge->m_lptTo=iPt1;
						}
						count++;
					}
					
					if(!guAlmostEqualPt(arcPt1,iPt1) && !guAlmostEqualPt(arcPt2,iPt1))
					{
						if(pList)
						{
							pList->AddTail(new CArcEdge(m_lptCenter,m_Radii,iPt1,m_lptTo,m_nDirection));
							m_lptTo=iPt1;
						}
						count++;
					}
					else if(arcPt1==arcPt2 && !pList) count++;
				}

				if(!guAlmostEqualPt(iPt1,iPt2) &&pSelfCopy->PtAtEdge(iPt2) && pEdgeCopy->PtAtEdge(iPt2))
				{
					bool bZeroCount=(count==0);
					if(!guAlmostEqualPt(linePt1,iPt2) && !guAlmostEqualPt(linePt2,iPt2))
					{
						if(bZeroCount && pOtherList)
						{
							pOtherList->AddTail(new CEdge(iPt2,pEdge->m_lptTo));
							pEdge->m_lptTo=iPt2;
						}
						count++;
					}
					
					if(!guAlmostEqualPt(arcPt1,iPt2) && !guAlmostEqualPt(arcPt2,iPt2))
					{
						if(bZeroCount && pList)
						{
							pList->AddTail(new CArcEdge(m_lptCenter,m_Radii,iPt2,m_lptTo,m_nDirection));
							m_lptTo=iPt2;
						}
						count++;
					}
					else if(arcPt1==arcPt2 && !pList) count++;
				}
				break;
				
	case kArcEdge:
		if(((CArcEdge*)pEdge)->m_Radii.x != ((CArcEdge*)pEdge)->m_Radii.y) goto splitIntersect;
		center1 = m_lptCenter;
		radius1 = m_Radii.x;
		startAngle1 = mtAngle(m_lptCenter,m_lptFrom);
		endAngle1	= mtAngle(m_lptCenter,m_lptTo);
		startPt1 = m_lptFrom;
		endPt1 = m_lptTo;
		
		center2 = ((CArcEdge*)pEdge)->m_lptCenter;
		radius2 = ((CArcEdge*)pEdge)->m_Radii.x;
		startAngle2 = mtAngle(((CArcEdge*)pEdge)->m_lptCenter,pEdge->m_lptFrom);
		endAngle2 = mtAngle(((CArcEdge*)pEdge)->m_lptCenter,pEdge->m_lptTo);
		startPt2 = pEdge->m_lptFrom;
		endPt2 = pEdge->m_lptTo;
		
		
		if (guAlmostEqualPt ( center1 , center2 ))
			break;
		
		r1 = radius1.GetRawDouble () ;
		r2 = radius2.GetRawDouble () ;
		
		cx1 = center1.x.GetRawDouble () ;
		cy1 = center1.y.GetRawDouble () ;
		
		cx2 = center2.x.GetRawDouble () ;
		cy2 = center2.y.GetRawDouble () ;
		
		d1 = RawDoublePythag ( center2 - center1 ) ;
		
		if (d1 >= (r1 + r2))
			break;
		
		trans = 0.0 ;
		
		if ((cx1 != cx2) && (cy1 != cy2)) {
			oldcx2 = cx2 ;
			oldcy2 = cy2 ;
			
			trans = atan((cy2 - cy1)/(cx2 - cx1)) ;
			
			cy2 = cy1 ;
			
			if (cx2 > cx1)
				cx2 = cx1 + d1 ;
			else
				cx2 = cx1 - d1 ;
		}
		
		
		m1 = 2.0 * (cx2 - cx1) ;
		m2 = 2.0 * (cy2 - cy1) ;
		m3 = mSq(cx1) - mSq(cx2) + mSq(cy1) - mSq(cy2) + mSq(r2) - mSq(r1) ;
		
		if (cx1 == cx2) {
			y1 = y2 = -m3/m2 ;
			a = 1.0 ;
			b = -2.0 * cx1 ;
			c = mSq(cx1) - mSq(r1) + mSq(y1) - (2.0 * y1 * cy1) + mSq(cy1) ;
			
			if (!SolveBinomial(&a, &b, &c, &x1, &x2))
				break;
		}
		else {
			x1 = x2 = -m3/m1 ;
			a = 1.0 ;
			b = -2.0 * cy1 ;
			c = mSq(cy1) - mSq(r1) + mSq(x1) - (2.0 * x1 * cx1) + mSq(cx1) ;
			
			if (!SolveBinomial(&a, &b, &c, &y1, &y2))
				break;
		}
		
		if (trans != 0.0) {
			oldx1 = x1 ;
			oldx2 = x2 ;
			
			oldy1 = y1 ;
			oldy2 = y2 ;
			
			theta1 = atan((y1-cy1)/(x1-cx1)) ;
			theta2 = atan((y2-cy1)/(x2-cx1)) ;
			
			x1 = cx1 + ((oldx1 - cx1) * cos(trans)) - ((oldy1 - cy1) * (sin(trans))) ;
			x2 = cx1 + ((oldx2 - cx1) * cos(trans)) - ((oldy2 - cy1) * (sin(trans))) ;
			
			y1 = cy1 + ((oldx1 - cx1) * sin(trans)) + ((oldy1 - cy1) * (cos(trans))) ;
			y2 = cy1 + ((oldx2 - cx1) * sin(trans)) + ((oldy2 - cy1) * (cos(trans))) ;
		}
		
		iPt1.x.SetFromRawDoubleTruncated ( x1 ) ;
		iPt1.y.SetFromRawDoubleTruncated ( y1 ) ;
		iPt2.x.SetFromRawDoubleTruncated ( x2 ) ;
		iPt2.y.SetFromRawDoubleTruncated ( y2 ) ;
		
		if((m_nDirection==AD_CLOCKWISE) == (mtAngle(m_lptCenter,iPt1) < mtAngle(m_lptCenter,iPt2)))
		{
			LongPoint temp=iPt1;
			iPt1=iPt2;
			iPt2=temp;
		}
		
		if(pSelfCopy->PtAtEdge(iPt1) && pEdgeCopy->PtAtEdge(iPt1))
		{
			if (!guAlmostEqualPt ( startPt1 , iPt1 ) && !guAlmostEqualPt ( endPt1 , iPt1 ))
			{
				if (pList)
				{
					pList->AddTail(new CArcEdge(m_lptCenter, m_Radii, iPt1, m_lptTo, m_nDirection));
					m_lptTo = iPt1;
				}
				count++;
			}
			
			if (!guAlmostEqualPt ( startPt2 , iPt1 ) && !guAlmostEqualPt ( endPt2 , iPt1 ))
			{
				if (pOtherList)
				{
					pOtherList->AddTail(new CArcEdge(((CArcEdge*)pEdge)->m_lptCenter, ((CArcEdge*)pEdge)->m_Radii, iPt1, pEdge->m_lptTo, ((CArcEdge*)pEdge)->m_nDirection));
					pEdge->m_lptTo = iPt1;				
				}
				count++;
			}
			
		}
		
		if(pSelfCopy->PtAtEdge(iPt2) && pEdgeCopy->PtAtEdge(iPt2))
		{
			bool bZeroCount=(count==0);
			if (!guAlmostEqualPt ( startPt1 , iPt2 ) && !guAlmostEqualPt ( endPt1 , iPt2 ))
			{
				if (bZeroCount && pList)
				{
					pList->AddTail(new CArcEdge(m_lptCenter, m_Radii, iPt2, m_lptTo, m_nDirection));
					m_lptTo = iPt2;
				}
				count++;
			}
			
			if (!guAlmostEqualPt ( startPt2 , iPt2 ) && !guAlmostEqualPt ( endPt2 , iPt2 ))
			{
				if (bZeroCount && pOtherList)
				{
					pOtherList->AddTail(new CArcEdge(((CArcEdge*)pEdge)->m_lptCenter, ((CArcEdge*)pEdge)->m_Radii, iPt2, pEdge->m_lptTo, ((CArcEdge*)pEdge)->m_nDirection));
					pEdge->m_lptTo = iPt2;
				}
				count++;
			}
		}	
		break;
		default:
		break;
		}
	}
	else
	{
//-------------------------------------------
		switch(pEdge->m_nType)
		{
		case kLineEdge:
			
			linePt1 = pEdge->m_lptFrom;
			linePt2 = pEdge->m_lptTo;
			arcPt1 = m_lptTo;
			arcPt2 = m_lptFrom;
			
			if((guAlmostEqualPt ( linePt1 , arcPt1 ) && guAlmostEqualPt ( linePt2 , arcPt2 )) ||
				(guAlmostEqualPt ( linePt2 , arcPt1 ) && guAlmostEqualPt ( linePt1 , arcPt2 )))
				break;
			
			
			startAngle1 = mtAngle(m_lptCenter,m_lptFrom);
			endAngle1   = mtAngle(m_lptCenter,m_lptTo);
			
			cx = m_lptCenter.x.GetRawDouble () ;
			cy = m_lptCenter.y.GetRawDouble () ;
			
			dy = ( linePt2.y - linePt1.y ).GetRawDouble () ;
			dx = ( linePt2.x - linePt1.x ).GetRawDouble () ;
			
			isHorz = isVert = false ;
			
			aSq = m_Radii.x.GetRawDouble () * m_Radii.x.GetRawDouble () ;
			bSq = m_Radii.y.GetRawDouble () * m_Radii.y.GetRawDouble () ;
			
			rotSin = mtSin(m_RotAngle);
			rotSinSq = rotSin*rotSin;
			
			rotCos = mtCos(m_RotAngle);
			rotCosSq = rotCos*rotCos;
			
			C 	= aSq * rotSinSq + bSq * rotCosSq ;
			D 	= (2 * rotCos * rotSin) * ( bSq - aSq) ;
			E1 	= aSq * rotCosSq + bSq * rotSinSq ;
			F 	= aSq * bSq ;
			
			if (linePt2.y == linePt1.y)
				isHorz = true ;
			else 
				if (linePt2.x == linePt1.x)
					isVert = true ;
				
			if (isVert)
			{
				k = ( linePt2.x - m_lptCenter.x ).GetRawDouble () ;	/* includes transformation for the change of origin*/
				z1 = E1 ;
				z2 = D*k;
				z3 = k*k*C - F;
			}
			else
			{
				if (isHorz)
				{
					k = ( linePt2.y - m_lptCenter.y ).GetRawDouble () ;	/* includes transformation for the change of origin*/
					z1 = C;
					z2 = D*k;
					z3 = k*k*E1 - F;
				}
				else
				{
					y1 = linePt2.y.GetRawDouble () ;
					x1 = linePt2.x.GetRawDouble () ;
					
					A = dy/dx ;
					/* make the center of the ellipse as the origin */
					newY1 = y1 - cy;
					newX1 = x1 - cx;
					B = newY1 - (A * newX1) ;
					z1 = ( C + A*D + E1*A*A );
					z2 = ( D*B + 2*A*B*E1 );
					z3 = ( E1*B*B - F );
				}
			}
				
			if (!SolveBinomial(&z1, &z2, &z3, &s1, &s2))
				break;
			
			if (isVert)
			{
				y1 = s1 + cy;
				y2 = s2 + cy;
				x1 = x2 = k + cx ;
			}
			else 
			{
				if (isHorz)
				{
					x1 = s1 + cx ;
					x2 = s2 + cx;
					y1 = y2 = k + cy ;
				}
				else
				{
					x1 = s1 + cx ;
					x2 = s2 + cx;
					y1 = (A*s1) + B + cy ;
					y2 = (A*s2) + B + cy ;
				}
			}
					
			iPt1.x.SetFromRawDoubleTruncated ( x1 );
			iPt1.y.SetFromRawDoubleTruncated ( y1 );
			iPt2.x.SetFromRawDoubleTruncated ( x2 );
			iPt2.y.SetFromRawDoubleTruncated ( y2 ) ;
			if(bRevert?(Pythag(iPt1-linePt1)>Pythag(iPt2-linePt1)):((m_nDirection==AD_CLOCKWISE) == (mtFixAngle(mtAngle(m_lptCenter,iPt1)-startAngle1) < mtFixAngle(mtAngle(m_lptCenter,iPt2)-startAngle1))))
			{
				LongPoint temp=iPt1;
				iPt1=iPt2;
				iPt2=temp;
			}
			
			if(pSelfCopy->PtAtEdge(iPt1) && pEdgeCopy->PtAtEdge(iPt1))
			{
				if(!guAlmostEqualPt(linePt1,iPt1) && !guAlmostEqualPt(linePt2,iPt1))
				{
					if(pOtherList)
					{
						pOtherList->AddTail(new CEdge(iPt1,pEdge->m_lptTo));
						pEdge->m_lptTo=iPt1;
					}
					count++;
				}
				
				if(!guAlmostEqualPt(arcPt1,iPt1) && !guAlmostEqualPt(arcPt2,iPt1))
				{
					if (pList)
					{
						pList->AddTail(new CArcEdge(m_lptCenter,m_Radii,iPt1,m_lptTo,m_nDirection,m_RotAngle));
						m_lptTo=iPt1;
					}
					count++;
				}
				else if(arcPt1==arcPt2 && !pList) count++;
			}

			if(pSelfCopy->PtAtEdge(iPt2) && pEdgeCopy->PtAtEdge(iPt2))
			{
				bool bZeroCount=(count==0);
				if(!guAlmostEqualPt(linePt1,iPt2) && !guAlmostEqualPt(linePt2,iPt2))
				{
					if(bZeroCount && pOtherList)
					{
						pOtherList->AddTail(new CEdge(iPt2,pEdge->m_lptTo));
						pEdge->m_lptTo=iPt2;
					}
					count++;
				}
				
				if(!guAlmostEqualPt(arcPt1,iPt2) && !guAlmostEqualPt(arcPt2,iPt2))
				{
					if(bZeroCount && pList)
					{
						pList->AddTail(new CArcEdge(m_lptCenter,m_Radii,iPt2,m_lptTo,m_nDirection,m_RotAngle));
						m_lptTo=iPt2;
					}
					count++;
				}
				else if(arcPt1==arcPt2 && !pList) count++;
			}
			break;
						
		case kArcEdge:
			{
splitIntersect:
				if(guAlmostEqualPt(m_lptCenter,((CArcEdge*)pEdge)->m_lptCenter))
				{
					LongPoint radii=((CArcEdge*)pEdge)->m_Radii;
					if(m_Radii.x==radii.x || m_Radii.y==radii.x || m_Radii.x==radii.y || m_Radii.y==radii.y) return 0;
				}

				Angle angle;
				Angle startAngle=guLPtToLAngle(m_Radii.x,m_Radii.y,mtRotPoint(m_lptFrom,-m_RotAngle,m_lptCenter),m_lptCenter);
				Angle endAngle=guLPtToLAngle(m_Radii.x,m_Radii.y,mtRotPoint(m_lptTo,-m_RotAngle,m_lptCenter),m_lptCenter);

				if(m_nDirection==AD_CLOCKWISE)
				{
					if(startAngle<=endAngle) startAngle+=360;
				}
				else
				{
					if(startAngle>=endAngle) endAngle+=360;
				}

				Angle incr;
				incr.SetFromRawDoubleTruncated( WrFixed(500).GetRawDouble()/(m_Radii.x.GetRawDouble() + m_Radii.y.GetRawDouble() + 65536.0));
				if(incr < FixedOnePointFive) incr=FixedOnePointFive;
				if(incr > Abs(endAngle-startAngle)) incr=Abs(endAngle-startAngle)/5;

				LongPoint lptFrom,lptTo;
				lptTo=m_lptFrom;
				if(m_nDirection==AD_CLOCKWISE)
				{
					for(angle=startAngle-incr;angle>endAngle;angle-=incr)
					{
						lptFrom=lptTo;
						lptTo=ArcPoint(angle);
						CEdge* pLineEdge=new CEdge(lptFrom,lptTo);
						if(count) count+=pLineEdge->Intersect(pEdge,NULL,NULL);
						else
						{
							count=pLineEdge->Intersect(pEdge,NULL,pOtherList);
							if(count && pList)
							{
								pList->AddTail(new CArcEdge(m_lptCenter,m_Radii,pEdge->m_lptTo,m_lptTo,m_nDirection,m_RotAngle));
								m_lptTo=pEdge->m_lptTo;
							}
						}
						delete pLineEdge;
					}
				}
				else
				{
					for(angle=startAngle+incr;angle<endAngle;angle+=incr)
					{
						lptFrom=lptTo;
						lptTo=ArcPoint(angle);
						CEdge* pLineEdge=new CEdge(lptFrom,lptTo);
						if(count) count+=pLineEdge->Intersect(pEdge,NULL,NULL);
						else
						{
							count=pLineEdge->Intersect(pEdge,NULL,pOtherList);
							if(count && pList)
							{
								pList->AddTail(new CArcEdge(m_lptCenter,m_Radii,pEdge->m_lptTo,m_lptTo,m_nDirection,m_RotAngle));
								m_lptTo=pEdge->m_lptTo;
							}
						}
						delete pLineEdge;
					}
				}

			}
			break;
		default:
			break;
		}
	}
//-------------------------------------------

	delete pSelfCopy;
	delete pEdgeCopy;

	return count;
}

bool CArcEdge::CheckAngles(const LongPoint& lpt) const
{
	if(m_lptFrom==m_lptTo) return true;	// whole ellipse
	Angle angle = mtFixAngle(mtAngle(m_lptCenter, lpt));
	Angle startAngle = mtFixAngle(mtAngle(m_lptCenter, m_lptFrom));
	Angle endAngle = mtFixAngle(mtAngle(m_lptCenter, m_lptTo));
	if(m_nDirection==AD_CLOCKWISE) return AngleBetween(angle,endAngle,startAngle);
	else return AngleBetween(angle,startAngle,endAngle);
}

bool CArcEdge::PtAtEdge(const LongPoint& lpt) const
{
	if (m_Radii.x == m_Radii.y)
	{
		if(Abs(Pythag(m_lptCenter-lpt)-m_Radii.x)>2*kAlmostSameCriteria) return false;
	}
	else
	{
		LongPoint lpt1=mtRotPoint(lpt,-m_RotAngle,m_lptCenter)-m_lptCenter;
		double xd=lpt1.x.GetRawDouble()/m_Radii.x.GetRawDouble();
		double yd=lpt1.y.GetRawDouble()/m_Radii.y.GetRawDouble();
		if(Abs(WrFixed(xd*xd+yd*yd-1))>2*kAlmostSameCriteria) return false;
	}
	return CheckAngles(lpt);
}

bool CArcEdge::SolveBinomial (double *d, double *e, double *f, double *x1, double *x2)
{
	double		q, a1, b1 ;
	double		a,b,c;

	a = *d;	b = *e; c = *f;
	q = (b*b) - (4.0 * a * c) ;
	if (q < 0)
		return false;

	if (q)
		q = sqrt(q) ;

	a1 = -b/(2.0 * a) ;
	b1 = q/(2.0 * a) ;
	*x1 = a1 + b1 ;
	*x2 = a1 - b1 ;

	return true;
} 

LongRect CArcEdge::GetExtents() const
{
	if(m_Radii.x==m_Radii.y)
	{
		LongPoint lpt;
		LongRect lrcExt=CEdge::GetExtents();
		Angle startAngle = mtFixAngle(mtAngle(m_lptCenter, m_lptFrom));
		Angle endAngle = mtFixAngle(mtAngle(m_lptCenter, m_lptTo));
		if(m_nDirection==AD_CLOCKWISE)
		{
			if(AngleBetween(0,endAngle,startAngle))
			{
				lpt=m_lptCenter;
				lpt.x+=m_Radii.x;
				lrcExt|=mtRotPoint(lpt,m_RotAngle);
			}
			if(AngleBetween(90,endAngle,startAngle))
			{
				lpt=m_lptCenter;
				lpt.y-=m_Radii.y;
				lrcExt|=mtRotPoint(lpt,m_RotAngle);
			}
			if(AngleBetween(180,endAngle,startAngle))
			{
				lpt=m_lptCenter;
				lpt.x-=m_Radii.x;
				lrcExt|=mtRotPoint(lpt,m_RotAngle);
			}
			if(AngleBetween(270,endAngle,startAngle))
			{
				lpt=m_lptCenter;
				lpt.y+=m_Radii.y;
				lrcExt|=mtRotPoint(lpt,m_RotAngle);
			}
		}
		else
		{
			if(AngleBetween(0,startAngle,endAngle))
			{
				lpt=m_lptCenter;
				lpt.x+=m_Radii.x;
				lrcExt|=mtRotPoint(lpt,m_RotAngle);
			}
			if(AngleBetween(90,startAngle,endAngle))
			{
				lpt=m_lptCenter;
				lpt.y-=m_Radii.y;
				lrcExt|=mtRotPoint(lpt,m_RotAngle);
			}
			if(AngleBetween(180,startAngle,endAngle))
			{
				lpt=m_lptCenter;
				lpt.x-=m_Radii.x;
				lrcExt|=mtRotPoint(lpt,m_RotAngle);
			}
			if(AngleBetween(270,startAngle,endAngle))
			{
				lpt=m_lptCenter;
				lpt.y+=m_Radii.y;
				lrcExt|=mtRotPoint(lpt,m_RotAngle);
			}
		}
		return lrcExt;
	}
	else
	{
		Angle angle;
		Angle startAngle=guLPtToLAngle(m_Radii.x,m_Radii.y,mtRotPoint(m_lptFrom,-m_RotAngle,m_lptCenter),m_lptCenter);
		Angle endAngle=guLPtToLAngle(m_Radii.x,m_Radii.y,mtRotPoint(m_lptTo,-m_RotAngle,m_lptCenter),m_lptCenter);

		if(m_nDirection==AD_CLOCKWISE)
		{
			if(startAngle<=endAngle) startAngle+=360;
		}
		else
		{
			if(startAngle>=endAngle) endAngle+=360;
		}

		Angle incr;
		incr.SetFromRawDoubleTruncated( WrFixed(500).GetRawDouble()/(m_Radii.x.GetRawDouble() + m_Radii.y.GetRawDouble() + 65536.0));
		if(incr < FixedOnePointFive) incr=FixedOnePointFive;
		if(incr > Abs(endAngle-startAngle)) incr=Abs(endAngle-startAngle)/5;

		LongBoundRect lrcExt;
		lrcExt|=m_lptFrom;
		lrcExt|=m_lptTo;
		if(m_nDirection==AD_CLOCKWISE)
		{
			for(angle=startAngle-incr;angle>endAngle;angle-=incr) lrcExt|=ArcPoint(angle);
		}
		else
		{
			for(angle=startAngle+incr;angle<endAngle;angle+=incr) lrcExt|=ArcPoint(angle);
		}
		return lrcExt;
	}
}

bool CArcEdge::AngleBetween(Angle angle, Angle startAngle, Angle endAngle)
{
	angle=mtFixAngle(angle-startAngle);
	endAngle=mtFixAngle(endAngle-startAngle);
	return (angle<=endAngle);
}

bool CArcEdge::Concatenate(CEdge *pEdge)
{
	if(pEdge->m_nType==kArcEdge)
	{
		CArcEdge* pArc=(CArcEdge*)pEdge;
		if(m_lptCenter==pArc->m_lptCenter && m_nDirection==pArc->m_nDirection && m_lptTo==pArc->m_lptFrom)
		{
			m_lptTo=pArc->m_lptTo;
			return true;
		}
	}
	return false;
}

const LongPoint& CArcEdge::GetPoint(int i) const
{
	switch(i)
	{
	case 2:
		return m_lptCenter;
	default:
		return CEdge::GetPoint(i);
	}
}

void CArcEdge::SetPoint(int i,const LongPoint& lpt)
{
	switch(i)
	{
	case 2:
		m_lptCenter=lpt;
		break;
	default:
		CEdge::SetPoint(i,lpt);
		break;
	}
}

void CArcEdge::Scale(const LongRect &lrcFrom, const LongRect &lrcTo)
{
	CEdge::Scale(lrcFrom,lrcTo);

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

	m_lptCenter=lrcTo.GetCorner(0)+(m_lptCenter-lrcFrom.GetCorner(0))*ratio;
	m_Radii*=ratio;
*/
	LongRatio xRatio,yRatio;
	xRatio.fN=(lrcTo.right-lrcTo.left).GetFixed();
	xRatio.fD=(lrcFrom.right-lrcFrom.left).GetFixed();
	yRatio.fN=(lrcTo.bottom-lrcTo.top).GetFixed();
	yRatio.fD=(lrcFrom.bottom-lrcFrom.top).GetFixed();
	m_Radii.x*=xRatio;
	m_Radii.y*=yRatio;
	m_Radii=Abs(m_Radii);
}

void CArcEdge::Scale(const LongRatio& ratio, double factor)
{
	CEdge::Scale(ratio,factor);
	m_lptCenter*=ratio;
	m_Radii=Abs(m_Radii*ratio);
	if(factor)
	{
		m_lptCenter*=factor;
		m_Radii*=factor;
	}
}

LongPoint CArcEdge::ArcPoint(Angle angle) const
{
	return m_lptCenter+mtRotPoint(LongPoint(mtFixMulDbl(m_Radii.x, mtCos(angle)),mtFixMulDbl(m_Radii.y, mtSin(angle))),m_RotAngle);
}

void CArcEdge::Rotate(Angle angle, const LongPoint &lptAbout)
{
	CEdge::Rotate(angle,lptAbout);
	m_lptCenter=mtRotPoint(m_lptCenter,angle,lptAbout);
	if(m_Radii.x!=m_Radii.y) m_RotAngle+=angle;
}

void CArcEdge::Mirror(const LongPoint& lptRef1,const LongPoint& lptRef2)
{
	CEdge::Mirror(lptRef1,lptRef2);
	LongPoint perpPt;
	guNearPtLnPt(m_lptCenter,lptRef1,lptRef2,&perpPt);
	m_lptCenter+=((perpPt-m_lptCenter)<<1);
	m_nDirection=m_nDirection==AD_CLOCKWISE?AD_COUNTERCLOCKWISE:AD_CLOCKWISE;
	if(m_Radii.x!=m_Radii.y)
	{
		Angle refAngle=mtAngle(lptRef1,lptRef2);
		m_RotAngle=mtFixAngle(2*refAngle-m_RotAngle);
	}
}

void CArcEdge::Flip(bool bVertical,const LongPoint& lpt)
{
	CEdge::Flip(bVertical,lpt);
	if(bVertical) m_lptCenter.y=lpt.y-m_lptCenter.y;
	else m_lptCenter.x=lpt.x-m_lptCenter.x;
	m_nDirection=m_nDirection==AD_CLOCKWISE?AD_COUNTERCLOCKWISE:AD_CLOCKWISE;
	if(m_Radii.x!=m_Radii.y)
	{
		m_RotAngle=mtFixAngle((bVertical?180:0)-m_RotAngle);
	}
}

bool CArcEdge::Snap(LongPoint *lptSnap, const LongRect &lrcProx, short nType) const
{
	if(lrcProx.Contains(m_lptCenter))
	{
		*lptSnap=m_lptCenter;
		return true;
	}
	return CEdge::Snap(lptSnap,lrcProx,nType);
}

void CArcEdge::Flatten(CMDShapeList *pList)
{
	Angle angle;
	Angle startAngle=guLPtToLAngle(m_Radii.x,m_Radii.y,mtRotPoint(m_lptFrom,-m_RotAngle,m_lptCenter),m_lptCenter);
	Angle endAngle=guLPtToLAngle(m_Radii.x,m_Radii.y,mtRotPoint(m_lptTo,-m_RotAngle,m_lptCenter),m_lptCenter);

	if(m_nDirection==AD_CLOCKWISE)
	{
		if(startAngle<=endAngle) startAngle+=360;
	}
	else
	{
		if(startAngle>=endAngle) endAngle+=360;
	}

	Angle incr;
	incr.SetFromRawDoubleTruncated( WrFixed(500).GetRawDouble()/(m_Radii.x.GetRawDouble() + m_Radii.y.GetRawDouble() + 65536.0));
	if(incr < FixedOnePointFive) incr=FixedOnePointFive;
	if(incr > Abs(endAngle-startAngle)) incr=Abs(endAngle-startAngle)/5;

	LongPoint lpt1,lpt2=m_lptFrom;
	if(m_nDirection==AD_CLOCKWISE)
	{
		for(angle=startAngle-incr;angle>endAngle;angle-=incr)
		{
			lpt1=lpt2;
			lpt2=ArcPoint(angle);
			pList->AddTail(new CMDLineShape(lpt1,lpt2));
		}
	}
	else
	{
		for(angle=startAngle+incr;angle<endAngle;angle+=incr)
		{
			lpt1=lpt2;
			lpt2=ArcPoint(angle);
			pList->AddTail(new CMDLineShape(lpt1,lpt2));
		}
	}
}

double CArcEdge::SubAreaCalc(const LongRatio &ratio)
{
	DoublePoint centrePt,startPt,endPt,MidPt;
	CMDShape::ScaleLPoint( ratio, m_lptCenter , centrePt);
	CMDShape::ScaleLPoint( ratio, m_lptFrom,  startPt);
	CMDShape::ScaleLPoint( ratio, m_lptTo,  endPt);
	CMDShape::ScaleLPoint( ratio, MidPoint(), MidPt );

	double area = ( MidPt.x - startPt.x ) * ( startPt.y + MidPt.y ) ;
	area += ( endPt.x - MidPt.x ) * ( MidPt.y + endPt.y ) ;
	area += ( startPt.x - endPt.x ) * ( endPt.y + startPt.y ) ;

	// We now have the area of the triangle between the start, mid and end point
	// but we only need to know its sign to apply to the arc's area
	double sign = ( area >= 0 ) ? 1.0 : -1.0 ;

	CMDArcShape * pShape = (CMDArcShape *)CreateShape();
	area = pShape->GetRawArea(ratio) * sign * 2.0;
	delete pShape;
	// We have the area of the 'cheese' of the arc, but need to add the
	// triangle between the start, centre and end points.

	area+=(centrePt.x-startPt.x)*(startPt.y+centrePt.y);
	area+=(endPt.x-centrePt.x)*(centrePt.y+endPt.y);

	return area*0.5;
}
