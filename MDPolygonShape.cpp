// MDPolygonShape.cpp: implementation of the CMDPolygonShape class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDPolygonShape.h"
#include "MDLineShape.h"
#include "MDBezierShape.h"
#include "LinkVertex.h"

#include "phmath.h"
#include "gu.h"
#include "BezierEdge.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "dxfutil.h"
#include "PCDraftView.h"
#include "MainFrm.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDPolygonShape,CMDFillShape,1)

//////////////////////////////////////////////////////////////////////

CMDPolygonShape::CMDPolygonShape(PolygonType nType,int nNumPoints,bool bClosed) : m_nType(nType),m_nCount(nNumPoints),m_bClosed(bClosed)
{
	m_pt.SetSize(m_nCount);
}

void CMDPolygonShape::Serialize(CArchive& ar)
{
	int i;

	CMDFillShape::Serialize(ar);
	
	if(ar.IsLoading())
	{
		ar >> i;
		m_nType=PolygonType(i);
        BYTE getData;
        ar >> getData; 
        m_bClosed = getData == TRUE;
		ar >> m_nCount;
		m_pt.SetSize(m_nCount);
		for(i=0;i<m_nCount;i++)
			ar >> m_pt[i];
	}
	else
	{
		ar << m_nType;
		ar << (BYTE) (m_bClosed ? 1:0 );
		ar << m_nCount;
		for(i=0;i<m_nCount;i++)
			ar << m_pt[i];
	}
}

LongRect CMDPolygonShape::CalculateBasicExtents() const
{
	LongRect lbr=CMDFillShape::CalculateBasicExtents();
	switch(m_nType)
	{
	case kFreeSpline:
		{
			LongPoint lpt1,lpt2,lpt3;
			if(m_bClosed)
			{
				lpt1=CuVertex(GetShapePoint(m_nCount-1),GetShapePoint(0),GetShapePoint(1));
			}
			else lpt1=GetShapePoint(0);
			lpt2=CuVertex(GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
			if(m_nCount<4) lpt3=GetShapePoint(2);
			else lpt3=CuVertex(GetShapePoint(1),GetShapePoint(2),GetShapePoint(3));

			LongPoint lpt12,lpt32;
			guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
			if(!m_bClosed)
			{
				lbr|=lpt1;
				lbr|=lpt12;
			}
			for(int i=3;i<(m_nCount+(m_bClosed?3:0));i++)
			{
				LongPoint lpt0=lpt2;
				lpt1=lpt32;
				lpt2=lpt3;
				if(i==m_nCount-1 && !m_bClosed) lpt3=GetShapePoint(i);
				else lpt3=CuVertex(GetShapePoint((i-1)%m_nCount),GetShapePoint(i%m_nCount),GetShapePoint((i+1)%m_nCount));
				guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
				lbr|=lpt0;
				lbr|=lpt1;
				lbr|=lpt12;
			}
			lbr|=lpt2;
			if(!m_bClosed)
			{
				lbr|=lpt32;
				lbr|=lpt3;
			}
		}
		break;
	case kPolySpline:
		{
			LongPoint lptCtrl,lpt1;
			if(m_bClosed)
			{
				lpt1=BezVertex(GetShapePoint(0),GetShapePoint(1));
				lbr|=BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0));
				lbr|=BezCtrlPt(GetShapePoint(0),GetShapePoint(m_nCount-1));
				lbr|=BezCtrlPt(GetShapePoint(0),GetShapePoint(1));
				lptCtrl=BezCtrlPt(GetShapePoint(1),GetShapePoint(0));
			}
			else
			{
				lpt1=GetShapePoint(0);
				lptCtrl=BezCtrlPt(GetShapePoint(0),GetShapePoint(1));
			}
			for(int i=1;i<(m_nCount-(m_bClosed?1:2));i++)
			{
				LongPoint lpt0=lpt1;
				lpt1=BezVertex(GetShapePoint(i),GetShapePoint(i+1));
				lbr|=lpt0;
				lbr|=lptCtrl;
				lbr|=BezCtrlPt(GetShapePoint(i),GetShapePoint(i+1));
				lptCtrl=BezCtrlPt(GetShapePoint(i+1),GetShapePoint(i));
			}
			lbr|=lpt1;
			lbr|=lptCtrl;
			if(m_bClosed)
			{
				lbr|=BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(0));
				lbr|=BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0));
			}
			else
			{
				lbr|=BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(m_nCount-2));
				lbr|=GetShapePoint(m_nCount-1);
			}
		}
		break;
	}

	return lbr;
}

void CMDPolygonShape::ExtendForPen(LongRect& lrcExtent) const
{
	short nSavedAlign=m_PenModel.m_nAlignment;
	const_cast<CMDPolygonShape*>(this)->m_PenModel.m_nAlignment=kCenterLine;
	CMDFillShape::ExtendForPen(lrcExtent);
	const_cast<CMDPolygonShape*>(this)->m_PenModel.m_nAlignment=nSavedAlign;
}

LongPoint CMDPolygonShape::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<m_nCount);
	return m_pt[nIndex];
}

void CMDPolygonShape::SetShapePoint(int nIndex, const LongPoint &lpt)
{
	ASSERT(nIndex>=0 && nIndex<m_nCount);
	m_pt[nIndex]=lpt;
}

int CMDPolygonShape::GetNumShapeHandles() const
{
	if(m_nType!=kPolygon && m_nType!=kParallel) return m_nCount;
	return (2*m_nCount-(m_bClosed?0:1));
}

LongPoint CMDPolygonShape::GetShapeHandle(int nIndex) const
{
	if(nIndex<m_nCount) return CMDFillShape::GetShapeHandle(nIndex);
	ASSERT(nIndex<GetNumShapeHandles());
	return Average(m_pt[nIndex%m_nCount], m_pt[(nIndex+1)%m_nCount]);
}

int	CMDPolygonShape::GetNumShapeEdges() const
{
	return (m_nType==kPolySpline)?(m_nCount-(m_bClosed?0:2)):(m_nCount-(m_bClosed?0:1));
}

CEdge* CMDPolygonShape::GetShapeEdge(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapeEdges());

	switch(m_nType)
	{
	case kFreehand:
	case kPolygon:
		return new CEdge(GetShapePoint(nIndex),GetShapePoint((nIndex+1)%m_nCount));
	case kFreeSpline:
		{
			LongPoint lpt1,lpt2,lpt3;
			LongPoint lpt12,lpt32;
			if(m_bClosed)
			{
				lpt1=CuVertex(GetShapePoint(nIndex?nIndex-1:m_nCount-1),GetShapePoint(nIndex),GetShapePoint((nIndex+1)%m_nCount));
				lpt2=CuVertex(GetShapePoint(nIndex),GetShapePoint((nIndex+1)%m_nCount),GetShapePoint((nIndex+2)%m_nCount));
//				if(!nIndex && m_nCount<4) lpt3=GetShapePoint(nIndex+2);
//				else
					lpt3=CuVertex(GetShapePoint((nIndex+1)%m_nCount),GetShapePoint((nIndex+2)%m_nCount),GetShapePoint((nIndex+3)%m_nCount));
				guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);

				LongPoint lpt0=lpt2;
				lpt1=lpt32;
				lpt2=lpt3;
				lpt3=CuVertex(GetShapePoint((nIndex+2)%m_nCount),GetShapePoint((nIndex+3)%m_nCount),GetShapePoint((nIndex+4)%m_nCount));
				guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
				return new CBezierEdge(lpt0,lpt1,lpt12,lpt2);
			}
			else
			{
				if(!nIndex)
				{
					lpt1=GetShapePoint(0);
					lpt2=CuVertex(GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
					if(m_nCount<4) lpt3=GetShapePoint(nIndex+2);
					else lpt3=CuVertex(GetShapePoint(nIndex+1),GetShapePoint(nIndex+2),GetShapePoint(nIndex+3));
					guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
					return new CBezierEdge(lpt1,lpt1,lpt12,lpt2);
				}
				if(nIndex==m_nCount-3)
				{
					return new CBezierEdge(lpt2,lpt32,lpt3,lpt3);
				}

				return new CBezierEdge(BezVertex(GetShapePoint(nIndex),GetShapePoint(nIndex+1)),BezCtrlPt(GetShapePoint(nIndex+1),GetShapePoint(nIndex)),BezCtrlPt(GetShapePoint(nIndex+1),GetShapePoint(nIndex+2)),BezVertex(GetShapePoint(nIndex+1),GetShapePoint(nIndex+2)));
			}

			if(m_bClosed)
				lpt1=CuVertex(GetShapePoint(m_nCount-1),GetShapePoint(nIndex),GetShapePoint(nIndex+1));
			else lpt1=GetShapePoint(nIndex);
			lpt2=CuVertex(GetShapePoint(nIndex),GetShapePoint(nIndex+1),GetShapePoint(nIndex+2));
			if(m_nCount<4) lpt3=GetShapePoint(nIndex+2);
			else lpt3=CuVertex(GetShapePoint(nIndex+1),GetShapePoint(nIndex+2),GetShapePoint(nIndex+3));

			guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
			if(!m_bClosed)
			{
				return new CBezierEdge(lpt1,lpt1,lpt12,lpt2);
			}
			for(int i=3;i<(m_nCount+(m_bClosed?3:0));i++)
			{
				LongPoint lpt0=lpt2;
				lpt1=lpt32;
				lpt2=lpt3;
				if(i==m_nCount-1 && !m_bClosed) lpt3=GetShapePoint(i);
				else lpt3=CuVertex(GetShapePoint((i-1)%m_nCount),GetShapePoint(i%m_nCount),GetShapePoint((i+1)%m_nCount));
				guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);

				return new CBezierEdge(lpt0,lpt1,lpt12,lpt2);
			}
			if(!m_bClosed)
			{
				return new CBezierEdge(lpt2,lpt32,lpt3,lpt3);
			}
		}
		break;
	case kPolySpline:
		{
			if(m_bClosed)
			{
				if(!nIndex)
					return new CBezierEdge(BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(nIndex)),BezCtrlPt(GetShapePoint(nIndex),GetShapePoint(m_nCount-1)),BezCtrlPt(GetShapePoint(nIndex),GetShapePoint(nIndex+1)),BezVertex(GetShapePoint(nIndex),GetShapePoint(nIndex+1)));
				return new CBezierEdge(BezVertex(GetShapePoint(nIndex-1),GetShapePoint(nIndex)),BezCtrlPt(GetShapePoint(nIndex),GetShapePoint(nIndex-1)),BezCtrlPt(GetShapePoint(nIndex),GetShapePoint((nIndex+1)%m_nCount)),BezVertex(GetShapePoint(nIndex),GetShapePoint((nIndex+1)%m_nCount)));
			}
			else
			{
				if(!nIndex)
					return new CBezierEdge(GetShapePoint(0),BezCtrlPt(GetShapePoint(nIndex),GetShapePoint(nIndex+1)),BezCtrlPt(GetShapePoint(nIndex+1),GetShapePoint(nIndex+2)),BezVertex(GetShapePoint(1),GetShapePoint(2)));
				if(nIndex==m_nCount-3)
					return new CBezierEdge(BezVertex(GetShapePoint(nIndex),GetShapePoint(nIndex+1)),BezCtrlPt(GetShapePoint(nIndex+1),GetShapePoint(nIndex)),BezCtrlPt(GetShapePoint(nIndex+2),GetShapePoint(nIndex+1)),GetShapePoint(nIndex+2));
				return new CBezierEdge(BezVertex(GetShapePoint(nIndex),GetShapePoint(nIndex+1)),BezCtrlPt(GetShapePoint(nIndex+1),GetShapePoint(nIndex)),BezCtrlPt(GetShapePoint(nIndex+1),GetShapePoint(nIndex+2)),BezVertex(GetShapePoint(nIndex+1),GetShapePoint(nIndex+2)));
			}
		}
		break;
	}
	return NULL;
}

void CMDPolygonShape::DrawShape(CDraftContext* pDC) const
{

   CMDFillShape::DrawFill(pDC);

   pDC->path_destroy(true);

   if (m_PenModel.m_nPenType != kPenInvis)
   {

      pDC->SetPen(m_PenModel);

      DrawShapeOutline(pDC);

   }

}

void CMDPolygonShape::DrawHandles(CDraftContext* pDC) const
{
	int i;
	LongPoint lpt;
	for(i=0;i<m_nCount;i++)
	{
		lpt=GetShapePoint(i);
		pDC->DrawHandle(lpt,m_bLocked);
		if(m_nType==kPolySpline/* || m_nType==kFreeSpline*/)
		{
			if(i) pDC->LineTo(lpt);
			else pDC->MoveTo(lpt);
		}
/*		if(m_nType==kFreeSpline && i && i<(m_nCount-1))
		{
			pDC->DrawHandle(CuVertex(GetShapePoint(i-1),GetShapePoint(i),GetShapePoint(i+1)));
		}*/
	}
	if(m_nType==kPolySpline && m_bClosed) pDC->LineTo(GetShapePoint(0));
	if(m_nType==kPolygon || m_nType==kParallel)
	{
		for(i=0;i<m_nCount-(m_bClosed?0:1);i++)
		{
			lpt=Average(GetShapePoint(i), GetShapePoint((i+1)%m_nCount));
			pDC->DrawHandle(lpt,m_bLocked);
		}
	}
}

void CMDPolygonShape::DrawShapeOutline(CDraftContext* pDC) const
{
	switch(m_nType)
	{
	case kFreehand:
	case kPolygon:
	case kParallel:
		{
			pDC->PolyLine(m_pt,m_nCount,m_bClosed,-1,-1);
		}
		break;
	case kFreeSpline:
		{
			LongPoint lpt1,lpt2,lpt3;
			if(m_bClosed)
			{
				lpt1=CuVertex(GetShapePoint(m_nCount-1),GetShapePoint(0),GetShapePoint(1));
			}
			else lpt1=GetShapePoint(0);
			lpt2=CuVertex(GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
			if(m_nCount<4) lpt3=GetShapePoint(2);
			else lpt3=CuVertex(GetShapePoint(1),GetShapePoint(2),GetShapePoint(3));

			LongPoint lpt12,lpt32;
			guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
			if(m_bClosed) pDC->MoveTo(lpt2);
			else pDC->BezierDash(lpt1,lpt1,lpt12,lpt2,-1);
			for(int i=3;i<(m_nCount+(m_bClosed?3:0));i++)
			{
				LongPoint lpt0=lpt2;
				lpt1=lpt32;
				lpt2=lpt3;
				if(i==m_nCount-1 && !m_bClosed) lpt3=GetShapePoint(i);
				else lpt3=CuVertex(GetShapePoint((i-1)%m_nCount),GetShapePoint(i%m_nCount),GetShapePoint((i+1)%m_nCount));
				guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
				pDC->BezierDash(lpt0,lpt1,lpt12,lpt2);

			}
			if(!m_bClosed)
			{
				pDC->BezierDash(lpt2,lpt32,lpt3,lpt3,0,-1);
			}
		}
		break;
	case kPolySpline:
		{
			LongPoint lptCtrl,lpt1;
			if(m_bClosed)
			{
				lpt1=BezVertex(GetShapePoint(0),GetShapePoint(1));
				pDC->BezierDash(BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0)),
								BezCtrlPt(GetShapePoint(0),GetShapePoint(m_nCount-1)),
								BezCtrlPt(GetShapePoint(0),GetShapePoint(1)),
								lpt1);
				lptCtrl=BezCtrlPt(GetShapePoint(1),GetShapePoint(0));
			}
			else
			{
				lpt1=GetShapePoint(0);
				pDC->MoveTo(lpt1);
				lptCtrl=BezCtrlPt(GetShapePoint(0),GetShapePoint(1));
			}
			for(int i=1;i<(m_nCount-(m_bClosed?1:2));i++)
			{
				LongPoint lpt0=lpt1;
				lpt1=BezVertex(GetShapePoint(i),GetShapePoint(i+1));
				pDC->BezierDash(lpt0,lptCtrl,BezCtrlPt(GetShapePoint(i),GetShapePoint(i+1)),lpt1,(i>1)?0:-1);
				lptCtrl=BezCtrlPt(GetShapePoint(i+1),GetShapePoint(i));
			}
			if(m_bClosed)
			{
				pDC->BezierDash(lpt1,lptCtrl,BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(0)),BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0)));
			}
			else
			{
				pDC->BezierDash(lpt1,lptCtrl,BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(m_nCount-2)),GetShapePoint(m_nCount-1),0,-1);
			}
		}
		break;
	}
}

bool CMDPolygonShape::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
	switch(m_nType)
	{
	case kFreehand:
	case kPolygon:
		{
			for(int i=0;i<(m_nCount-1);i++)
			{
				if(guPtAtLine(pt,pMapper->LPtoDPF(GetShapePoint(i)),pMapper->LPtoDPF(GetShapePoint(i+1)),nWidth,kCenterLine))
				{
					return true;
				}
			}
			if(m_bClosed && guPtAtLine(pt,pMapper->LPtoDPF(GetShapePoint(0)),pMapper->LPtoDPF(GetShapePoint(m_nCount-1)),nWidth,kCenterLine)) return true;
		}
		break;

	case kFreeSpline:
		{
			CDrawPrim dp;
			pointd pt1,pt2,pt3;

			LongPoint lpt1,lpt2,lpt3;
			if(m_bClosed)
			{
				lpt1=CuVertex(GetShapePoint(m_nCount-1),GetShapePoint(0),GetShapePoint(1));
			}
			else lpt1=GetShapePoint(0);
			lpt2=CuVertex(GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
			if(m_nCount<4) lpt3=GetShapePoint(2);
			else lpt3=CuVertex(GetShapePoint(1),GetShapePoint(2),GetShapePoint(3));

			LongPoint lpt12,lpt32;
			guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
			if(m_bClosed);
			else
			{
				pt1=pMapper->LPtoDPF(lpt1);
				pt2=pMapper->LPtoDPF(lpt12);
				pt3=pMapper->LPtoDPF(lpt2);
				if(dp.checkBezier(pt1,pt1,pt2,pt3,pt,nWidth)) return true;
			}
			for(int i=3;i<(m_nCount+(m_bClosed?3:0));i++)
			{
				lpt1=lpt32;
				lpt2=lpt3;
				if(i==m_nCount-1 && !m_bClosed) lpt3=GetShapePoint(i);
				else lpt3=CuVertex(GetShapePoint((i-1)%m_nCount),GetShapePoint(i%m_nCount),GetShapePoint((i+1)%m_nCount));
				guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);

				pointd pt0=pt2;
				pt1=pMapper->LPtoDPF(lpt1);
				pt2=pMapper->LPtoDPF(lpt12);
				pt3=pMapper->LPtoDPF(lpt2);
				if(dp.checkBezier(pt0,pt1,pt2,pt3,pt,nWidth)) return true;
			}
			if(!m_bClosed)
			{
				pt1=pMapper->LPtoDPF(lpt3);
				pt2=pMapper->LPtoDPF(lpt32);
				if(dp.checkBezier(pt3,pt2,pt1,pt1,pt,nWidth)) return true;
			}
		}
		break;
	case kPolySpline:
		{
			CDrawPrim dp;
			pointd pt1,ptCtrl;

			LongPoint lptCtrl,lpt1;
			if(m_bClosed)
			{
				pt1=pMapper->LPtoDPF(BezVertex(GetShapePoint(0),GetShapePoint(1)));
				if(dp.checkBezier(pMapper->LPtoDPF(BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0))),
								pMapper->LPtoDPF(BezCtrlPt(GetShapePoint(0),GetShapePoint(m_nCount-1))),
								pMapper->LPtoDPF(BezCtrlPt(GetShapePoint(0),GetShapePoint(1))),
								pt1,pt,nWidth)) return true;
				lptCtrl=pMapper->LPtoDP(BezCtrlPt(GetShapePoint(1),GetShapePoint(0)));
			}
			else
			{
				pt1=pMapper->LPtoDPF(GetShapePoint(0));
				ptCtrl=pMapper->LPtoDPF(BezCtrlPt(GetShapePoint(0),GetShapePoint(1)));
			}
			for(int i=1;i<(m_nCount-(m_bClosed?1:2));i++)
			{
				pointd pt0=pt1;
				pt1=pMapper->LPtoDPF(BezVertex(GetShapePoint(i),GetShapePoint(i+1)));
				if(dp.checkBezier(pt0,ptCtrl,pMapper->LPtoDPF(BezCtrlPt(GetShapePoint(i),GetShapePoint(i+1))),pt1,pt,nWidth)) return true;
				ptCtrl=pMapper->LPtoDPF(BezCtrlPt(GetShapePoint(i+1),GetShapePoint(i)));
			}
			if(m_bClosed)
			{
				if(dp.checkBezier(pt1,ptCtrl,pMapper->LPtoDPF(BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(0))),pMapper->LPtoDPF(BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0))),pt,nWidth)) return true;
			}
			else
			{
				if(dp.checkBezier(pt1,ptCtrl,pMapper->LPtoDPF(BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(m_nCount-2))),pMapper->LPtoDPF(GetShapePoint(m_nCount-1)),pt,nWidth)) return true;
			}
		}
		break;
	}
	return false;
}

bool CMDPolygonShape::CanSmooth() const
{
	return (m_nType<kPolySpline && m_nCount>2);
}

void CMDPolygonShape::Smooth()
{
	switch(m_nType)
	{
	case kFreehand:
		m_nType=kFreeSpline;
		break;
	case kPolygon:
		m_nType=kPolySpline;
		break;
	case kFreeSpline:
		{
			LongPoint lpt0=GetShapePoint(0);
			LongPoint lpt1=lpt0;
			LongPoint lpt2=GetShapePoint(1);
			if(m_bClosed) SetShapePoint(0,CuVertex(GetShapePoint(m_nCount-1),lpt1,lpt2));
            int i = 1;
			for(;i<(m_nCount-1);i++)
			{
				lpt2=GetShapePoint(i);
				SetShapePoint(i,CuVertex(lpt1,lpt2,GetShapePoint(i+1)));
				lpt1=lpt2;
			}
			if(m_bClosed) SetShapePoint(i,CuVertex(lpt2,GetShapePoint(i),lpt0));
		}
		break;
	default:
		ASSERT(FALSE);
	}
}

bool CMDPolygonShape::CanUnsmooth() const
{
	return m_nType==kFreeSpline || m_nType==kPolySpline;
}

void CMDPolygonShape::Unsmooth()
{
	switch(m_nType)
	{
	case kFreeSpline:
		m_nType=kFreehand;
		break;
	case kPolySpline:
		m_nType=kPolygon;
		break;
	default:
		ASSERT(FALSE);
	}
}

void CMDPolygonShape::Unglue(CMDShapeList* pList) const
{
	switch(m_nType)
	{
	case kFreehand:
	case kPolygon:
		{
			for(int i=1;i<m_nCount;i++)
			{
				CMDShape* pShape=new CMDLineShape(GetShapePoint(i-1),GetShapePoint(i));
				pShape->SetPenModel(m_PenModel);
				pShape->CalculateExtents();
				pShape->SetShapeDatum(kDefaultDatum);
				pList->AddTail(pShape);
			}
			if(m_bClosed)
			{
				CMDShape* pShape=new CMDLineShape(GetShapePoint(m_nCount-1),GetShapePoint(0));
				pShape->SetPenModel(m_PenModel);
				pShape->CalculateExtents();
				pShape->SetShapeDatum(kDefaultDatum);
				pList->AddTail(pShape);
			}
		}
		break;
	case kFreeSpline:
		{
			LongPoint lpt1,lpt2,lpt3;
			if(m_bClosed)
			{
				lpt1=CuVertex(GetShapePoint(m_nCount-1),GetShapePoint(0),GetShapePoint(1));
			}
			else lpt1=GetShapePoint(0);
			lpt2=CuVertex(GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
			if(m_nCount<4) lpt3=GetShapePoint(2);
			else lpt3=CuVertex(GetShapePoint(1),GetShapePoint(2),GetShapePoint(3));

			LongPoint lpt12,lpt32;
			guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
			CMDBezierShape* pShape;
			if(!m_bClosed)
			{
				pShape=new CMDBezierShape(4);
				pShape->SetShapePoint(0,lpt1);
				pShape->SetShapePoint(1,lpt1);
				pShape->SetShapePoint(2,lpt12);
				pShape->SetShapePoint(3,lpt2);
				pShape->SetPenModel(m_PenModel);
				pShape->CalculateExtents();
				pShape->SetShapeDatum(kDefaultDatum);
				pList->AddTail(pShape);
			}
			for(int i=3;i<(m_nCount+(m_bClosed?3:0));i++)
			{
				LongPoint lpt0=lpt2;
				lpt1=lpt32;
				lpt2=lpt3;
				if(i==m_nCount-1 && !m_bClosed) lpt3=GetShapePoint(i);
				else lpt3=CuVertex(GetShapePoint((i-1)%m_nCount),GetShapePoint(i%m_nCount),GetShapePoint((i+1)%m_nCount));
				guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);

				pShape=new CMDBezierShape(4);
				pShape->SetShapePoint(0,lpt0);
				pShape->SetShapePoint(1,lpt1);
				pShape->SetShapePoint(2,lpt12);
				pShape->SetShapePoint(3,lpt2);
				pShape->SetPenModel(m_PenModel);
				pShape->CalculateExtents();
				pShape->SetShapeDatum(kDefaultDatum);
				pList->AddTail(pShape);
			}
			if(!m_bClosed)
			{
				pShape=new CMDBezierShape(4);
				pShape->SetShapePoint(0,lpt2);
				pShape->SetShapePoint(1,lpt32);
				pShape->SetShapePoint(2,lpt3);
				pShape->SetShapePoint(3,lpt3);
				pShape->SetPenModel(m_PenModel);
				pShape->CalculateExtents();
				pShape->SetShapeDatum(kDefaultDatum);
				pList->AddTail(pShape);
			}
		}
		break;
	case kPolySpline:
		{
			LongPoint lptCtrl,lpt1;
			CMDBezierShape* pShape;
			if(m_bClosed)
			{
				lpt1=BezVertex(GetShapePoint(0),GetShapePoint(1));

				pShape=new CMDBezierShape(4);
				pShape->SetShapePoint(0,BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0)));
				pShape->SetShapePoint(1,BezCtrlPt(GetShapePoint(0),GetShapePoint(m_nCount-1)));
				pShape->SetShapePoint(2,BezCtrlPt(GetShapePoint(0),GetShapePoint(1)));
				pShape->SetShapePoint(3,lpt1);
				pShape->SetPenModel(m_PenModel);
				pShape->CalculateExtents();
				pShape->SetShapeDatum(kDefaultDatum);
				pList->AddTail(pShape);

				lptCtrl=BezCtrlPt(GetShapePoint(1),GetShapePoint(0));
			}
			else
			{
				lpt1=GetShapePoint(0);
				lptCtrl=BezCtrlPt(GetShapePoint(0),GetShapePoint(1));
			}
			for(int i=1;i<(m_nCount-(m_bClosed?1:2));i++)
			{
				LongPoint lpt0=lpt1;
				lpt1=BezVertex(GetShapePoint(i),GetShapePoint(i+1));

				pShape=new CMDBezierShape(4);
				pShape->SetShapePoint(0,lpt0);
				pShape->SetShapePoint(1,lptCtrl);
				pShape->SetShapePoint(2,BezCtrlPt(GetShapePoint(i),GetShapePoint(i+1)));
				pShape->SetShapePoint(3,lpt1);
				pShape->SetPenModel(m_PenModel);
				pShape->CalculateExtents();
				pShape->SetShapeDatum(kDefaultDatum);
				pList->AddTail(pShape);

				lptCtrl=BezCtrlPt(GetShapePoint(i+1),GetShapePoint(i));
			}
			if(m_bClosed)
			{
				pShape=new CMDBezierShape(4);
				pShape->SetShapePoint(0,lpt1);
				pShape->SetShapePoint(1,lptCtrl);
				pShape->SetShapePoint(2,BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(0)));
				pShape->SetShapePoint(3,BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0)));
				pShape->SetPenModel(m_PenModel);
				pShape->CalculateExtents();
				pShape->SetShapeDatum(kDefaultDatum);
				pList->AddTail(pShape);
			}
			else
			{
				pShape=new CMDBezierShape(4);
				pShape->SetShapePoint(0,lpt1);
				pShape->SetShapePoint(1,lptCtrl);
				pShape->SetShapePoint(2,BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(m_nCount-2)));
				pShape->SetShapePoint(3,GetShapePoint(m_nCount-1));
				pShape->SetPenModel(m_PenModel);
				pShape->CalculateExtents();
				pShape->SetShapeDatum(kDefaultDatum);
				pList->AddTail(pShape);
			}
		}
		break;
/*	default:
		{
			for (int i=0; i<GetNumShapeEdges(); i++)
			{
				CEdge* pEdge=GetShapeEdge(i);
				CMDShape* pShape=pEdge->CreateShape();
				delete pEdge;
				pShape->SetPenModel(m_PenModel);
				pShape->CalculateExtents();
				pShape->SetShapeDatum(kDefaultDatum);
				pList->AddTail(pShape);
			}
		}
		break;
*/	}
}

int  CMDPolygonShape::AddPt(const pointd& pt, CMapper* pMapper)
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
    int i = 1;
	for(;i<m_nCount;i++)
	{
		if(guPtAtLine(pt,pMapper->LPtoDPF(GetShapePoint(i-1)),pMapper->LPtoDPF(GetShapePoint(i)),1,m_PenModel.m_nAlignment))
		{
			LongPoint lpt;
			if(GetAsyncKeyState(VK_SHIFT)<0) lpt=Average(GetShapePoint(i-1),GetShapePoint(i));
			else lpt=pMapper->DPtoLP(pt);
			m_pt.InsertAt(i,lpt);
			m_nCount++;
			return i;
		}
	}
	if(m_bClosed && guPtAtLine(pt,pMapper->LPtoDPF(GetShapePoint(i-1)),pMapper->LPtoDPF(GetShapePoint(0)),1,m_PenModel.m_nAlignment))
	{
		LongPoint lpt;
		if(GetAsyncKeyState(VK_SHIFT)<0) lpt=Average(GetShapePoint(i-1),GetShapePoint(0));
		else lpt=pMapper->DPtoLP(pt);
		m_pt.InsertAt(i,lpt);
		m_nCount++;
		return i;
	}
	return -1;
}

void CMDPolygonShape::RemovePt(int nHandle, CMDShape** ppNewShape)
{
	if(nHandle>=0 && nHandle<m_nCount)
	{
		m_pt.RemoveAt(nHandle);
		m_nCount--;
		if(m_nCount==2)
		{
			m_bClosed=false;
			m_FillModel.m_nFillType=kNoFill;

			CMDLineShape* pLine=new CMDLineShape(GetShapePoint(0),GetShapePoint(1));
			pLine->SetPenModel(GetPenModel());
			pLine->CalculateExtents();
			pLine->SetShapeDatum(kDefaultDatum);
			*ppNewShape=pLine;
		}
	}
}

CTrackContext* CMDPolygonShape::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CPolyTrackContext* p=new CPolyTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
    int i = 0;
	for(;i<m_pt.GetSize();i++)
		p->pt.SetAtGrow(i,GetShapePoint(i));
	if(nWhichHandle==kRotateHandle)
	{
		CRotateContext* pCtx=(CRotateContext*)lParam;
		LongPoint lptRot;
		switch(pCtx->m_nAbout)
		{
		case kAboutCenter:
			lptRot=Middle(GetExtents());
			break;
		case kAboutDatum:
			lptRot=GetShapeDatum();
			break;
		case kAboutRefPt:
			lptRot=pCtx->m_ptOrigin;
			break;
		default:
			ASSERT(0);
		}
		for(i=0;i<p->pt.GetSize();i++)
			p->pt[i]=mtRotPoint(p->pt[i], pCtx->m_Angle, lptRot);
	}
	return p;
}

void CMDPolygonShape::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CPolyTrackContext* p=(CPolyTrackContext*)pTrackContext;
	int nHandle=p->nWhichHandle;
	if(nHandle==kMoveHandle)
	{
		for(int i=0;i<p->pt.GetSize();i++) p->pt[i]+=pt;
	}
	else
	{
		if(nHandle<m_nCount)
		{
			p->pt[nHandle]=pt;
			if(p->bShift)
			{
				LongPoint diff=Abs(p->pt[nHandle]-m_pt[nHandle]);
				if(diff.x<diff.y) p->pt[nHandle].x=m_pt[nHandle].x;
				else p->pt[nHandle].y=m_pt[nHandle].y;
			}
		}
		else
		{
			int i=nHandle%m_nCount;
			LongPoint pt2=Average(p->pt[i],p->pt[(i+1)%m_nCount]);
			if(p->bShift)
			{
				LongPoint pt1=Average(m_pt[i],m_pt[(i+1)%m_nCount]);
				LongPoint diff=Abs(pt-pt1);
				if(diff.x<diff.y) pt.x=pt1.x;
				else pt.y=pt1.y;
			}
			LongPoint delta=pt-pt2;
			p->pt[i]+=delta;
			p->pt[(i+1)%m_nCount]+=delta;
		}
	}
}

void CMDPolygonShape::TrackShowSize(CTrackContext* pTrackContext)
{
	CPolyTrackContext* p=(CPolyTrackContext*)pTrackContext;
	int nHandle=p->nWhichHandle;
	if(nHandle<m_nCount)
	{
		int shift=0;
		if(nHandle>0)
		{
			p->pView->SetShowSize(0,5,Pythag(p->pt[nHandle]-p->pt[nHandle-1]));
			p->pView->SetShowSize(2,-1,mtAngle(p->pt[nHandle-1],p->pt[nHandle]));
			shift=1;
		}
		else if(m_bClosed)
		{
			p->pView->SetShowSize(0,5,Pythag(p->pt[nHandle]-p->pt[m_nCount-1]));
			p->pView->SetShowSize(2,-1,mtAngle(p->pt[m_nCount-1],p->pt[nHandle]));
			shift=1;
		}

		if(nHandle<m_nCount-1)
		{
			p->pView->SetShowSize(0+shift,5+shift,Pythag(p->pt[nHandle]-p->pt[nHandle+1]));
			p->pView->SetShowSize(2+shift,-1-shift,mtAngle(p->pt[nHandle+1],p->pt[nHandle]));
		}
		else if(m_bClosed)
		{
			p->pView->SetShowSize(0+shift,5+shift,Pythag(p->pt[nHandle]-p->pt[0]));
			p->pView->SetShowSize(2+shift,-1-shift,mtAngle(p->pt[0],p->pt[nHandle]));
		}
	}
	else
	{
		nHandle-=m_nCount;
		int shift=0;
		if(nHandle>0)
		{
			p->pView->SetShowSize(0,5,Pythag(p->pt[nHandle]-p->pt[nHandle-1]));
			p->pView->SetShowSize(2,-1,mtAngle(p->pt[nHandle-1],p->pt[nHandle]));
			shift=1;
		}
		else if(m_bClosed)
		{
			p->pView->SetShowSize(0,5,Pythag(p->pt[nHandle]-p->pt[m_nCount-1]));
			p->pView->SetShowSize(2,-1,mtAngle(p->pt[m_nCount-1],p->pt[nHandle]));
			shift=1;
		}

		if(nHandle<m_nCount-2)
		{
			p->pView->SetShowSize(0+shift,5+shift,Pythag(p->pt[nHandle+1]-p->pt[nHandle+2]));
			p->pView->SetShowSize(2+shift,-1-shift,mtAngle(p->pt[nHandle+2],p->pt[nHandle+1]));
		}
		else if(m_bClosed)
		{
			if(nHandle<m_nCount-1)
			{
				p->pView->SetShowSize(0+shift,5+shift,Pythag(p->pt[nHandle+1]-p->pt[0]));
				p->pView->SetShowSize(2+shift,-1-shift,mtAngle(p->pt[0],p->pt[nHandle+1]));
			}
			else
			{
				p->pView->SetShowSize(0+shift,5+shift,Pythag(p->pt[1]-p->pt[0]));
				p->pView->SetShowSize(2+shift,-1-shift,mtAngle(p->pt[0],p->pt[1]));
			}
		}
	}
}

void CMDPolygonShape::TrackAnnot(CDraftContext *pDC, CPolyTrackContext *p)
{
	if(((CMainFrame*)AfxGetMainWnd())->IsShowSizeVisible())
	{
		int nHandle=p->nWhichHandle;
		LongPoint lpt[2];
		int shift=0;
		if(nHandle<m_nCount)
		{
			if(nHandle>0)
			{
				lpt[shift]=Average(p->pt[nHandle],p->pt[nHandle-1]);
				lpt[shift]=Average(p->pt[nHandle],lpt[shift]);
				shift=1;
			}
			else if(m_bClosed)
			{
				lpt[shift]=Average(p->pt[nHandle],p->pt[m_nCount-1]);
				lpt[shift]=Average(p->pt[nHandle],lpt[shift]);
				shift=1;
			}

			if(nHandle<m_nCount-1)
			{
				lpt[shift]=Average(p->pt[nHandle],p->pt[nHandle+1]);
				lpt[shift]=Average(p->pt[nHandle],lpt[shift]);
			}
			else if(m_bClosed)
			{
				lpt[shift]=Average(p->pt[nHandle],p->pt[0]);
				lpt[shift]=Average(p->pt[nHandle],lpt[shift]);
			}
		}
		else
		{
			nHandle-=m_nCount;
			if(nHandle>0)
			{
				lpt[shift]=Average(p->pt[nHandle],p->pt[nHandle-1]);
				lpt[shift]=Average(p->pt[nHandle],lpt[shift]);
				shift=1;
			}
			else if(m_bClosed)
			{
				lpt[shift]=Average(p->pt[nHandle],p->pt[m_nCount-1]);
				lpt[shift]=Average(p->pt[nHandle],lpt[shift]);
				shift=1;
			}

			if(nHandle<m_nCount-2)
			{
				lpt[shift]=Average(p->pt[nHandle+1],p->pt[nHandle+2]);
				lpt[shift]=Average(p->pt[nHandle+1],lpt[shift]);
			}
			else if(m_bClosed)
			{
				if(nHandle<m_nCount-1)
				{
					lpt[shift]=Average(p->pt[nHandle+1],p->pt[0]);
					lpt[shift]=Average(p->pt[nHandle+1],lpt[shift]);
				}
				else
				{
					lpt[shift]=Average(p->pt[0],p->pt[1]);
					lpt[shift]=Average(p->pt[0],lpt[shift]);
				}
			}
		}
		pDC->DrawAnnot(lpt[0],IDB_ANNOT_A);
		if(shift) pDC->DrawAnnot(lpt[1],IDB_ANNOT_B);
	}
}

void CMDPolygonShape::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CPolyTrackContext* p=(CPolyTrackContext*)pTrackContext;

	switch(m_nType)
	{
	case kFreehand:
	case kPolygon:
	case kParallel:
		{
			pDC->MoveTo(p->pt[0]);
			for(int i=1;i<m_nCount;i++)
			{
				pDC->LineTo(p->pt[i]);
			}
			if(m_bClosed) pDC->LineTo(p->pt[0]);
			if(p->nWhichHandle>=0) TrackAnnot(pDC,p);
		}
		break;
	case kFreeSpline:
		{
			LongPoint lpt1,lpt2,lpt3;
			if(m_bClosed)
			{
				lpt1=CuVertex(p->pt[m_nCount-1],p->pt[0],p->pt[1]);
			}
			else lpt1=p->pt[0];
			lpt2=CuVertex(p->pt[0],p->pt[1],p->pt[2]);
			if(m_nCount<4) lpt3=p->pt[2];
			else lpt3=CuVertex(p->pt[1],p->pt[2],p->pt[3]);

			LongPoint lpt12,lpt32;
			guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
			if(m_bClosed) pDC->MoveTo(lpt2);
			else pDC->Bezier(lpt1,lpt1,lpt12,lpt2);
			for(int i=3;i<(m_nCount+(m_bClosed?3:0));i++)
			{
				lpt1=lpt32;
				lpt2=lpt3;
				if(i==m_nCount-1 && !m_bClosed) lpt3=p->pt[i];
				else lpt3=CuVertex(p->pt[(i-1)%m_nCount],p->pt[i%m_nCount],p->pt[(i+1)%m_nCount]);
				guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
				pDC->BezierTo(lpt1,lpt12,lpt2);

			}
			if(!m_bClosed)
			{
				pDC->BezierTo(lpt32,lpt3,lpt3);
			}
		}
		break;
	case kPolySpline:
		{
			LongPoint lptCtrl;
			if(m_bClosed)
			{
				pDC->Bezier(BezVertex(p->pt[m_nCount-1],p->pt[0]),
							BezCtrlPt(p->pt[0],p->pt[m_nCount-1]),
							BezCtrlPt(p->pt[0],p->pt[1]),
							BezVertex(p->pt[0],p->pt[1]));
				lptCtrl=BezCtrlPt(p->pt[1],p->pt[0]);
			}
			else
			{
				pDC->MoveTo(p->pt[0]);
				lptCtrl=BezCtrlPt(p->pt[0],p->pt[1]);
			}
			for(int i=1;i<(m_nCount-(m_bClosed?1:2));i++)
			{
				pDC->BezierTo(lptCtrl,BezCtrlPt(p->pt[i],p->pt[i+1]),BezVertex(p->pt[i],p->pt[i+1]));
				lptCtrl=BezCtrlPt(p->pt[i+1],p->pt[i]);
			}
			if(m_bClosed)
			{
				pDC->BezierTo(lptCtrl,BezCtrlPt(p->pt[m_nCount-1],p->pt[0]),BezVertex(p->pt[m_nCount-1],p->pt[0]));
			}
			else
			{
				pDC->BezierTo(lptCtrl,BezCtrlPt(p->pt[m_nCount-1],p->pt[m_nCount-2]),p->pt[m_nCount-1]);
			}
		}
		break;
	}
}

void CMDPolygonShape::UpdateVertexes(CLinkVertexList* pVertexList,int nCauseVertex)
{
	CUIntArray vertexes;
	pVertexList->GetVertexes(vertexes,this,nCauseVertex);
	LongPoint pt=GetShapePoint(nCauseVertex);
	for(int i=0;i<vertexes.GetSize();i++)
	{
		int nVertex=vertexes.GetAt(i);
		if(nVertex!=nCauseVertex)
			SetShapePoint(nVertex,pt);
	}
}

void CMDPolygonShape::TrackComplete(CTrackContext* pTrackContext)
{
	CPolyTrackContext* p=(CPolyTrackContext*)pTrackContext;
	for(int i=0;i<p->pt.GetSize();i++)
		SetShapePoint(i,p->pt[i]);
	CLinkVertexList* pVertexList=&p->pView->GetActiveLinkVertexList();
	if(p->nWhichHandle<m_nCount)
	{
		UpdateVertexes(pVertexList,p->nWhichHandle);
	}
	else
	{
		UpdateVertexes(pVertexList,p->nWhichHandle%m_nCount);
		UpdateVertexes(pVertexList,(p->nWhichHandle+1)%m_nCount);
	}
	CalculateExtents();
}

double CMDPolygonShape::GetRawArea(const LongRatio& ratio) const
{
	double area=0;
	if(m_nType<kFreeSpline || m_nType>kCubic)
	{
		LongPoint lptPrev,lptCur=GetShapePoint(0);
		for(int i=1;i<m_nCount;i++)
		{
			lptPrev=lptCur;
			lptCur=GetShapePoint(i);
			area+=ScaleDist(ratio,lptCur.y+lptPrev.y)*ScaleDist(ratio,lptCur.x-lptPrev.x)/2;
		}
		lptPrev=GetShapePoint(0);
		area+=ScaleDist(ratio,lptPrev.y+lptCur.y)*ScaleDist(ratio,lptPrev.x-lptCur.x)/2;
//		area/=2;
	}	
	else
	{
		double adjustment=0.0;
		switch(m_nType)
		{
		case kFreeSpline:
			{
				LongPoint lpt1,lpt2,lpt3;
				if(m_bClosed)
				{
					lpt1=CuVertex(GetShapePoint(m_nCount-1),GetShapePoint(0),GetShapePoint(1));
				}
				else lpt1=GetShapePoint(0);
				lpt2=CuVertex(GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
				if(m_nCount<4) lpt3=GetShapePoint(2);
				else lpt3=CuVertex(GetShapePoint(1),GetShapePoint(2),GetShapePoint(3));

				LongPoint lpt12,lpt32;
				guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
				if(!m_bClosed)
				{
					area+=ScaleDist(ratio,lpt2.y+lpt1.y)*ScaleDist(ratio,lpt2.x-lpt1.x)/2;
					adjustment+=BezEdge(ratio,lpt1,lpt1,lpt12,lpt2);
				}
				for(int i=3;i<(m_nCount+(m_bClosed?3:0));i++)
				{
					LongPoint lpt0=lpt2;
					lpt1=lpt32;
					lpt2=lpt3;
					if(i==m_nCount-1 && !m_bClosed) lpt3=GetShapePoint(i);
					else lpt3=CuVertex(GetShapePoint((i-1)%m_nCount),GetShapePoint(i%m_nCount),GetShapePoint((i+1)%m_nCount));
					guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
					area+=ScaleDist(ratio,lpt2.y+lpt0.y)*ScaleDist(ratio,lpt2.x-lpt0.x)/2;
					adjustment+=BezEdge(ratio,lpt0,lpt1,lpt12,lpt2);
				}
				if(!m_bClosed)
				{
					area+=ScaleDist(ratio,lpt3.y+lpt2.y)*ScaleDist(ratio,lpt3.x-lpt2.x)/2;
					adjustment+=BezEdge(ratio,lpt2,lpt32,lpt3,lpt3);
					lpt1=GetShapePoint(0);
					area+=ScaleDist(ratio,lpt1.y+lpt3.y)*ScaleDist(ratio,lpt1.x-lpt3.x)/2;
				}
			}
			break;
		case kPolySpline:
			{
				LongPoint lptCtrl,lpt0,lpt1;
				if(m_bClosed)
				{
					lpt0=BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0));
					lpt1=BezVertex(GetShapePoint(0),GetShapePoint(1));
					area+=ScaleDist(ratio,lpt1.y+lpt0.y)*ScaleDist(ratio,lpt1.x-lpt0.x)/2;
					adjustment+=BezEdge(ratio,lpt0,BezCtrlPt(GetShapePoint(0),GetShapePoint(m_nCount-1)),BezCtrlPt(GetShapePoint(0),GetShapePoint(1)),lpt1);
					lptCtrl=BezCtrlPt(GetShapePoint(1),GetShapePoint(0));
				}
				else
				{
					lpt1=GetShapePoint(0);
					lptCtrl=BezCtrlPt(GetShapePoint(0),GetShapePoint(1));
				}
				for(int i=1;i<(m_nCount-(m_bClosed?1:2));i++)
				{
					lpt0=lpt1;
					lpt1=BezVertex(GetShapePoint(i),GetShapePoint(i+1));
					area+=ScaleDist(ratio,lpt1.y+lpt0.y)*ScaleDist(ratio,lpt1.x-lpt0.x)/2;
					adjustment+=BezEdge(ratio,lpt0,lptCtrl,BezCtrlPt(GetShapePoint(i),GetShapePoint(i+1)),lpt1);
					lptCtrl=BezCtrlPt(GetShapePoint(i+1),GetShapePoint(i));
				}
				if(m_bClosed)
				{
					lpt0=BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0));
					area+=ScaleDist(ratio,lpt0.y+lpt1.y)*ScaleDist(ratio,lpt0.x-lpt1.x)/2;
					adjustment+=BezEdge(ratio,lpt1,lptCtrl,BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(0)),lpt0);
				}
				else
				{
					lpt0=GetShapePoint(m_nCount-1);
					area+=ScaleDist(ratio,lpt0.y+lpt1.y)*ScaleDist(ratio,lpt0.x-lpt1.x)/2;
					adjustment+=BezEdge(ratio,lpt1,lptCtrl,BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(m_nCount-2)),lpt0);
					lpt1=GetShapePoint(0);
					area+=ScaleDist(ratio,lpt1.y+lpt0.y)*ScaleDist(ratio,lpt1.x-lpt0.x)/2;
				}
			}
			break;
		}
		area+=adjustment;
	}
	return fabs(area);
}

double CMDPolygonShape::ReportCalcPerimeter(const LongRatio& ratio) const
{
	if(m_nType<kFreeSpline || m_nType>kCubic)
	{
		double perimeter=0;
		LongPoint lptPrev,lptCur=GetShapePoint(0);
		for(int i=1;i<m_nCount;i++)
		{
			lptPrev=lptCur;
			lptCur=GetShapePoint(i);
			perimeter+=ScaleDist(ratio,Pythag(lptCur-lptPrev));
		}
		if(IsClosed())
		{
			lptPrev=GetShapePoint(0);
			perimeter+=ScaleDist(ratio,Pythag(lptCur-lptPrev));
		}
		return perimeter;
	}	
	else return CMDFillShape::ReportCalcPerimeter(ratio);
}

const ResizeInfo* CMDPolygonShape::GetResizeInfo() const
{
static ResizeInfo rectResize =
{
	IDS_EDIT_COORD, 2, {IDS_X,kUnitlessValue, IDS_Y,kUnitlessValue}
};
	return &rectResize;
}

void CMDPolygonShape::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	CMDFillShape::GetResizeValue(pValues,nHandle);
	const ResizeInfo* pInfo=GetResizeInfo();
//	if(pInfo->nTitle==IDS_EDIT_COORD && pValues[2].m_Long!=-1)
	if(nHandle!=-1)
	{
		LongPoint lpt;
//		int n=pValues[2].m_Long;
		if(nHandle<m_nCount) lpt=m_pt[nHandle];
		else lpt=Average(m_pt[nHandle%m_nCount], m_pt[(nHandle+1)%m_nCount]);
		pValues[0].m_Unitless=lpt.x;
		pValues[1].m_Unitless=lpt.y;
	}
	pValues[2].m_Long=nHandle;
}

void CMDPolygonShape::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	const ResizeInfo* pInfo=GetResizeInfo();
	if(nHandle!=-1) //pInfo->nTitle==IDS_EDIT_COORD && pValues[2].m_Long!=-1)
	{
		LongPoint lpt;
		int n=pValues[2].m_Long;
		lpt.x=pValues[0].m_Unitless;
		lpt.y=pValues[1].m_Unitless;
		if(n<m_nCount) SetShapePoint(n,lpt);
		else
		{
			n%=m_nCount;
			LongPoint lpt2=Average(m_pt[n],m_pt[(n+1)%m_nCount]);
			LongPoint delta=lpt-lpt2;
			m_pt[n]+=delta;
			m_pt[(n+1)%m_nCount]+=delta;
		}
	}
	CMDFillShape::SetResizeValue(pValues,nHandle);
}

void CMDPolygonShape::SetNumShapePoints(int numPoints)
{
	m_nCount=numPoints;
	m_pt.SetSize(m_nCount);
}

void CMDPolygonShape::SetPolygonType(PolygonType type)
{
	m_nType=type;
}

void CMDPolygonShape::SetClosed(bool closed)
{
	m_bClosed=closed;
}

void CMDPolygonShape::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
	if(m_nType>kPolygon) // splines
	{
		short				vIdx1;
		LongPoint			vVertexPt;
		short				vDashIdx;
		short				vPtCount;
		CLongPointArray		vPtHdl;

		LongPoint			bPt[4];
		short				i, j, cc[124], *ccPtr, vSeg, offsetH, offsetV;
		long				x, y;
		long				vPtPtr,	vPtAry;
		long				vDbPtPtr;
		short				vTotPtCount, vNum;
		PenModel			aPenModel, capsPenModel;
		short				cap1, cap2;


		vSeg = GetNumShapeEdges();
		vPtCount = 3 * vSeg + 1;

  		if ( !vSeg )
  			return;

		aPenModel = capsPenModel = GetPenModel();
		cap1 = aPenModel.EndCap1();
		cap2 = aPenModel.EndCap2();
		vDashIdx = aPenModel.Dashing();

		vDbPtPtr = 0;


		vPtHdl.SetSize(32*vSeg+2);
		vPtPtr = vPtAry = 0;
  		
  		ccPtr = cc;
  		for ( i = 1; i < 32; i++)  {
			j = 32 - i;
			*ccPtr++ = j * j * j;
			*ccPtr++ = 3 * i * j * j;
			*ccPtr++ = 3 * i * i * j;
			*ccPtr++ = i * i * i;
  		}

  		for ( i = 0; i < vSeg; i++ )  {
			CBezierEdge* vEdgePtr = (CBezierEdge*)GetShapeEdge(i);
	  		vPtHdl[vPtPtr] = vEdgePtr->m_lptFrom;
			bPt [ 0 ] = vEdgePtr->m_lptFrom;
			bPt [ 1 ] = vEdgePtr->m_lptFromCtl;
			(bPt + 1)->x -= (bPt)->x ;
			(bPt + 1)->y -= (bPt)->y ;
			bPt [ 2 ] = vEdgePtr->m_lptToCtl;
			(bPt + 2)->x -= (bPt)->x ;
			(bPt + 2)->y -= (bPt)->y ;
			bPt [ 3 ] = vEdgePtr->m_lptTo;
			(bPt + 3)->x -= (bPt)->x ;
			(bPt + 3)->y -= (bPt)->y ;
			delete vEdgePtr;
			offsetH = (short)mtFixToReal(bPt->x);
			offsetV = (short)mtFixToReal(bPt->y);
			for ( ccPtr = cc; ccPtr < cc + 121; ccPtr += 4)  {
				x = (long)(*(ccPtr + 1) * mtFixToReal((bPt+1)->x) +
				*(ccPtr + 2) * mtFixToReal((bPt+2)->x) + *(ccPtr + 3) * mtFixToReal((bPt+3)->x));
				y = (long)(*(ccPtr + 1) * mtFixToReal((bPt+1)->y) +
				*(ccPtr + 2) * mtFixToReal((bPt+2)->y) + *(ccPtr + 3) * mtFixToReal((bPt+3)->y));
				vPtPtr++;
				vPtHdl[vPtPtr].x = offsetH + (short)(x >> 15);
				vPtHdl[vPtPtr].y = offsetV + (short)(y >> 15);
			} /* for ccPtr */

			(bPt+3)->x += bPt->x ;
			(bPt+3)->y += bPt->y ;
			vPtPtr++;
  		} /* for i */
		
		vPtHdl[vPtPtr] = *(bPt+3);

  		/* Fill the curve. */
  		vSeg <<= 5;
 		++vSeg;
  		
		vPtPtr=0;
  		vTotPtCount = vSeg;
  		while ( vTotPtCount > 0 ) {
			if ( vTotPtCount > kDxfOutMaxPolyPts )
				vNum = kDxfOutMaxPolyPts;
			else
				vNum = vTotPtCount;

			aPenModel = GetPenModel();
			DxfWritePolyHead (pFile, pLayerStr, &aPenModel,
							  pDocExtPtr, vUnitFactor, 0, ptrPalette);

			/* Write the vertices of the POINT */
			vVertexPt = vPtHdl[vPtPtr];
			vVertexPt.x = vVertexPt.x - pDocExtPtr->left;
			vVertexPt.y = pDocExtPtr->bottom - vVertexPt.y;
			DxfWritePtCord (pFile, pLayerStr, &vVertexPt,
							&vUnitFactor, vDashIdx);

			if ( vNum > 2 ) {
				for (vIdx1 = 0; vIdx1 < vNum; vIdx1++, vPtPtr++) {
					/* Write the vertices */
					vVertexPt = vPtHdl[vPtPtr];
					vVertexPt.x = vVertexPt.x - pDocExtPtr->left;
					vVertexPt.y = pDocExtPtr->bottom - vVertexPt.y;
					DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
										 &vUnitFactor, 0L, 0, vDashIdx);
				} /* for vIdx1 */
			} /* if vNum */
			else {
				vVertexPt = vPtHdl[vPtPtr];
				vVertexPt.x = vVertexPt.x - pDocExtPtr->left;
				vVertexPt.y = pDocExtPtr->bottom - vVertexPt.y;

				DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
								&vUnitFactor, 0L, 0, vDashIdx);

				/* Send the last point again because columbo accpects a polyline
				   to have atleast three points.
				*/
				DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
							 &vUnitFactor, 0L, 0, vDashIdx);

				vPtPtr++;
				vVertexPt = vPtHdl[vPtPtr];
				vVertexPt.x = vVertexPt.x - pDocExtPtr->left;
				vVertexPt.y = pDocExtPtr->bottom - vVertexPt.y;

				DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
								 &vUnitFactor, 0L, 0, vDashIdx);
			}

			DxfWriteSeqend (pFile, pLayerStr);

			vPtPtr--;
			vTotPtCount -= kDxfOutMaxPolyPts;

			if ( vTotPtCount ) vTotPtCount++;

  		}

		if ( ( cap1 || cap2 ) && !IsClosed())
		{	LongPoint	lPt1, lPt2, lPt3, lPt4;

			if (cap1) {
				lPt1 = vPtHdl[0];
				lPt2 = vPtHdl[1];
				lPt3 = vPtHdl[2];

				if ( lPt1 != lPt2) // !!!
				{	
					DxfDrawEndcap (pFile, pLayerStr, &lPt1, &lPt2, cap1, kEnd1,
					  			   &capsPenModel, false, kLOrdEndmarkOutset,
					  			   pDocExtPtr, vUnitFactor, nZoomFactor, ptrPalette);
				}
				else
				{	DxfDrawEndcap (pFile, pLayerStr, &lPt1, &lPt3, cap1, kEnd1,
								   &capsPenModel, false, kLOrdEndmarkOutset,
								   pDocExtPtr, vUnitFactor, nZoomFactor, ptrPalette);
				}
			}

			if (cap2) {
				
				lPt2 = vPtHdl[vPtHdl.GetSize()-3];
				lPt3 = vPtHdl[vPtHdl.GetSize()-2];
				lPt4 = vPtHdl[vPtHdl.GetSize()-1];

				if ( lPt3 != lPt4 )	{	
					DxfDrawEndcap (pFile, pLayerStr, &lPt3, &lPt4, cap2, kEnd2,
					  			   &capsPenModel, false, kLOrdEndmarkOutset,
			  					   pDocExtPtr, vUnitFactor, nZoomFactor, ptrPalette);
				}
				else {	
					DxfDrawEndcap (pFile, pLayerStr, &lPt2, &lPt4, cap2, kEnd2,
			  					   &capsPenModel, false, kLOrdEndmarkOutset,
			  					   pDocExtPtr, vUnitFactor, nZoomFactor, ptrPalette);
				}
			}
		}
	}
	else
	{
		CString			strBuffer, strDashName=""; //????
		LongPoint		vVertexPt;
		short			vIdx;
		short			vDashIdx;


		vDashIdx = GetPenModel().Dashing();

		DxfWritePolyHead (pFile, pLayerStr, &GetPenModel(),
						  pDocExtPtr, vUnitFactor, m_bClosed, ptrPalette);

  		/* Write the vertices of the polygon */
		for (vIdx = 0; vIdx < m_nCount; vIdx++)
  		{
  			vVertexPt.x = GetShapePoint(vIdx).x - pDocExtPtr->left;
  			vVertexPt.y = pDocExtPtr->bottom - GetShapePoint(vIdx).y;
			DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
			 	   		   &vUnitFactor, 0L, 0, vDashIdx);
		}

  		/* Write the Seqend entity */
  		DxfWriteSeqend (pFile, pLayerStr);

  		/* Custom Line Style */
  		if (vDashIdx) {
			findStrFromTab (kLTYPE, strDashName, kTablesTab);
			strBuffer.Format("%3d\r\n%s%02d\r\n", kLineTypeName, strDashName, vDashIdx);
			pFile->Write(strBuffer, strBuffer.GetLength());
  		}
	}
}

int CMDPolygonShape::SaveMacDraft(PhIOContext& ctx)
{
	short phID;
	if(m_nType<kFreeSpline)
	{
		phID=m_nType==kFreehand?PhIDFreehand:PhIDPolygon;
		int size=sizeof(PhDSPolygon)+sizeof(PhDBPoint)*(m_nCount-1);
		PhDSPolygon* ps=(PhDSPolygon*)malloc(size);
		ps->fID=phID;
		ctx.pPhShape=ps;
		CMDFillShape::SaveMacDraft(ctx);

		ps->fClosed=m_bClosed;
		ps->fDummy=0;
		ps->fCount=m_nCount;

		uptr p((char*)ps);
		p.swapDSShape();
		p.swaps(2); // fm
		p.skip(2); p.swaps(1);

		for(int i=0;i<m_nCount;i++)
		{
			ps->fPoint[i].fFlags=kPhRealVertex;
			ps->fPoint[i].fLongPt=m_pt[i];
			ps->fPoint[i].fUse=0;
			p.swapDBPoint(1);
		}

		ushort count=size;
		uptr((char*)&count).swaps(1);
		ctx.f->Write(&count,sizeof(short));
		ctx.f->Write(ps,size);
		SaveMacObjInfo(ctx);
		free(ps);
		return 1;
	}
	else
	{
		switch(m_nType)
		{
		case kFreeSpline:
			phID=PhIDFreeSpline;
			break;
		case kPolySpline:
			phID=PhIDPolySpline;
			break;
		default:
			return 0;
		}
		int nCount=m_nCount;
		if(!m_bClosed)
		{
			nCount--;
			if(m_nType==kPolySpline) nCount--;
		}

		int size=sizeof(PhDSPolygon)+sizeof(PhDBPoint)*(3*nCount+m_nCount);
		PhDSPolygon* ps=(PhDSPolygon*)malloc(size);
		ps->fID=phID;
		ctx.pPhShape=ps;
		CMDFillShape::SaveMacDraft(ctx);

		ps->fClosed=m_bClosed;
		ps->fDummy=0;
		ps->fCount=nCount;

		uptr p((char*)ps);
		p.swapDSShape();
		p.swaps(2); // fm
		p.skip(2); p.swaps(1);

		int i=0;
		if(m_nType==kFreeSpline)
		{
			LongPoint lpt1,lpt2,lpt3;
			if(m_bClosed)
			{
				lpt1=CuVertex(GetShapePoint(m_nCount-1),GetShapePoint(0),GetShapePoint(1));
			}
			else lpt1=GetShapePoint(0);
			lpt2=CuVertex(GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
			if(m_nCount<4) lpt3=GetShapePoint(2);
			else lpt3=CuVertex(GetShapePoint(1),GetShapePoint(2),GetShapePoint(3));

			LongPoint lpt12,lpt32;
			guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
			if(!m_bClosed)
			{
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhRealVertex;
				ps->fPoint[i++].fLongPt=lpt1;
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhRealVertex;
				ps->fPoint[i++].fLongPt=lpt1;
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=lpt12;
				p.swapDBPoint(3);
			}
			for(int j=3;j<(m_nCount+(m_bClosed?3:0));j++)
			{
				LongPoint lpt0=lpt2;
				lpt1=lpt32;
				lpt2=lpt3;
				if(j==m_nCount-1 && !m_bClosed) lpt3=GetShapePoint(j);
				else lpt3=CuVertex(GetShapePoint((j-1)%m_nCount),GetShapePoint(j%m_nCount),GetShapePoint((j+1)%m_nCount));
				guBezSplinePts(lpt1,lpt2,lpt3,&lpt12,&lpt32);
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=0x01;
				ps->fPoint[i++].fLongPt=lpt0;
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=lpt1;
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=lpt12;
				p.swapDBPoint(3);
			}
			ps->fPoint[i].fUse=0;
			ps->fPoint[i].fFlags=0x01;
			ps->fPoint[i++].fLongPt=lpt2;
			p.swapDBPoint(1);
			if(!m_bClosed)
			{
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=lpt32;
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhRealVertex;
				ps->fPoint[i++].fLongPt=lpt3;
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhRealVertex;
				ps->fPoint[i++].fLongPt=lpt3;
				p.swapDBPoint(3);
			}
		}
		else
		{
			LongPoint lptCtrl,lpt1;
			if(m_bClosed)
			{
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=0x01;
				ps->fPoint[i++].fLongPt=BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0));
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(0),GetShapePoint(m_nCount-1));
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(0),GetShapePoint(1));
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=0x01;
				ps->fPoint[i++].fLongPt=BezVertex(GetShapePoint(0),GetShapePoint(1));
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(1),GetShapePoint(0));
				p.swapDBPoint(5);
			}
			else
			{
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=0x01;
				ps->fPoint[i++].fLongPt=GetShapePoint(0);;
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(0),GetShapePoint(1));
				p.swapDBPoint(2);
			}
			for(int j=1;j<(m_nCount-(m_bClosed?1:2));j++)
			{
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(j),GetShapePoint(j+1));
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=0x01;
				ps->fPoint[i++].fLongPt=BezVertex(GetShapePoint(j),GetShapePoint(j+1));
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(j+1),GetShapePoint(j));
				p.swapDBPoint(3);
			}
			if(m_bClosed)
			{
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(0));
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=0x01;
				ps->fPoint[i++].fLongPt=BezVertex(GetShapePoint(m_nCount-1),GetShapePoint(0));
				p.swapDBPoint(2);
			}
			else
			{
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=kPhControlVertex;
				ps->fPoint[i++].fLongPt=BezCtrlPt(GetShapePoint(m_nCount-1),GetShapePoint(m_nCount-2));
				ps->fPoint[i].fUse=0;
				ps->fPoint[i].fFlags=0x01;
				ps->fPoint[i++].fLongPt=GetShapePoint(m_nCount-1);
				p.swapDBPoint(2);
			}
		}
		int n=nCount*3+1;
		for(i=0;i<m_nCount;i++)
		{
			ps->fPoint[n+i].fUse=0;
			ps->fPoint[n+i].fFlags=kPhRealVertex;
			ps->fPoint[n+i].fLongPt=m_pt[i];
			p.swapDBPoint(1);
		}

		ushort count=size;
		uptr((char*)&count).swaps(1);
		ctx.f->Write(&count,sizeof(short));
		ctx.f->Write(ps,size);
		SaveMacObjInfo(ctx);
		free(ps);
		return 1;
	}
}

//////////////////////////////////////////////////////////////////////

const WrFixed
	 kWeight0	= 3.0 / 4.0
	,kWeight1	= 1.0 / 8.0
	,kWeight2	= 1.0 / 8.0
	,kWeight01	= 1.0 / 4.0;

LongPoint CMDPolygonShape::BezVertex(const LongPoint& lpt0, const LongPoint &lpt1)
{
	return Average(lpt0,lpt1);
}

LongPoint CMDPolygonShape::BezCtrlPt(const LongPoint& lpt0, const LongPoint& lpt1)
{
	return lpt0*kWeight0+lpt1*kWeight01;
}

LongPoint CMDPolygonShape::CuVertex(const LongPoint& lpt0, const LongPoint& lpt1, const LongPoint& lpt2)
{
	return lpt0*kWeight0+lpt1*kWeight1+lpt2*kWeight2;
}

double CMDPolygonShape::BezEdge(const LongRatio& ratio,const LongPoint& lpt1,const LongPoint& lpt2,const LongPoint& lpt3,const LongPoint& lpt4)
{
	bool function;
	LongPoint pt[4], half1[4], half2[4];
	LongPoint threePt2;
	LongPoint a,b,c;
	DoublePoint da, db, dc;
	double tmp1, tmp2, tmp3;
	double area;

	pt[0]=lpt1;
	pt[1]=lpt2;
	pt[2]=lpt3;
	pt[3]=lpt4;
	pt[1]-=pt[0];
	pt[2]-=pt[0];
	pt[3]-=pt[0];
	pt[0].Clear();
	
	TSinCos sinCos(k360-mtATan2(pt[3])) ;
	pt[1]=mtRotPoint(pt[1],sinCos,pt[0]);
	pt[2]=mtRotPoint(pt[2],sinCos,pt[0]);
	pt[3]=mtRotPoint(pt[3],sinCos,pt[0]);

	function=((pt[1].x >= pt[0].x) && (pt[2].x >= pt[0].x) && (pt[1].x <= pt[3].x) && (pt[2].x <= pt[3].x));
	if ((pt[1].y >= FixedZero && pt[2].y >= FixedZero )	|| (pt[1].y < FixedZero && pt[2].y < FixedZero ))
	{
		function=(pt[1].x <= pt[2].x);
	}
	if (!function)	// curve is not a function, recurse on halves
	{
		guBisectCurve(pt, half1, half2);
		area=ScaleDist(ratio, pt[3].x);
		area*=ScaleDist(ratio, half1[3].y);
		area*=0.5;
		area+=BezEdge(ratio, half1[0], half1[1], half1[2], half1[3]);
		area+=BezEdge(ratio, half2[0], half2[1], half2[2], half2[3]);

	}
	else	// curve is a function, calculate area directly 
	{
/* unoptimized version
		threePt1.x = 3 * pt[1].x;
		threePt1.y = 3 * pt[1].y;
		threePt2.x = 3 * pt[2].x;
		threePt2.y = 3 * pt[2].y;

		a.x = -pt[0].x + threePt1.x - threePt2.x + pt[3].x;
		b.x = threePt0.x - (threePt1.x << 1) + threePt2.x;
		c.x = -threePt0.x + threePt1.x;
		a.y = -pt[0].y + threePt1.y - threePt2.y + pt[3].y;
		b.y = threePt0.y - (threePt1.y << 1) + threePt2.y;
		c.y = -threePt0.y + threePt1.y;
*/
		c = pt[1] * short ( 3 ) ;
		threePt2 = pt[2] * short ( 3 ) ;

		a = c - threePt2 + pt[3] ;
		b = - ( c << 1 ) + threePt2 ;

		a.x *= short ( 3 ) ;
		b.x <<= 1;

		ScaleLPoint(ratio, a, da);
		ScaleLPoint(ratio, b, db);
		ScaleLPoint(ratio, c, dc);

/* unoptimized version
		area = (da.x * da.y) / 6.0 +
			   (da.x * db.y + db.x * da.y) / 5.0 +
			   (da.x * dc.y + db.x * db.y + dc.x * da.y) / 4.0 +
			   (da.x * d0.y + db.x * dc.y + dc.x * db.y) / 3.0 +
			   (db.x * d0.y + dc.x * dc.y) / 2.0 +
			   (dc.x * d0.y);
*/
		area = da.x;		area *= da.y;
		area *= 0.16666666666666666667;
		tmp1 = da.x;		tmp1 *= db.y;
		tmp2 = db.x;		tmp2 *= da.y;		tmp1 += tmp2;
		tmp1 *= 0.2;
		area += tmp1;
		tmp1 = da.x;		tmp1 *= dc.y;
		tmp2 = db.x;		tmp2 *= db.y;		tmp1 += tmp2;
		tmp3 = dc.x;		tmp3 *= da.y;		tmp1 += tmp3;
		tmp1 *= 0.25;
		area += tmp1;
		tmp1 = db.x;		tmp1 *= dc.y;
		tmp2 = dc.x;		tmp2 *= db.y;		tmp1 += tmp2;
		tmp1 *= 0.33333333333333333333;
		area += tmp1;
		tmp1 = dc.x;		tmp1 *= dc.y;
		tmp1 *= 0.5;
		area += tmp1;

	}
	return(area);
}

void CMDPolygonShape::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
	pointd spt;
	for(int i=GetNumShapePoints()-1;i>=0;i--)
	{
		spt=pMapper->LPtoDPF(GetShapePoint(i));
		CRect rc(__point(spt),__point(spt));
		rc.InflateRect(kTrackHandleSize,kTrackHandleSize);
		if(rc.PtInRect(__point(pt)))
		{
			vertexes.Add(i);
			shapes.Add(this);
		}
	}
}
