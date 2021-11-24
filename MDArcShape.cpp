#include "stdafx.h"
#include "resource.h"
#include "MDArcShape.h"

#include "gu.h"
#include "phmath.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "ArcEdge.h"
#include "PCDraftView.h"
#include "PhIOStruct.h"

#include "dxfutil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDArcShape, CMDFillShape,1)

//////////////////////////////////////////////////////////////////////

void CMDArcShape::Serialize(CArchive& ar)
{
	CMDFillShape::Serialize(ar);
	if(ar.IsLoading())
		ar >> m_nMode >> m_pt[0] >> m_pt[1] >> m_pt[2] >> m_pt[3] >> m_nArcDirection;
	else
		ar << m_nMode << m_pt[0] << m_pt[1] << m_pt[2] << m_pt[3] << m_nArcDirection;
}

LongRect CMDArcShape::CalculateBasicExtents() const
{
	CEdge* pEdge=GetShapeEdge(0);
	LongRect ext=pEdge->GetExtents();
	delete pEdge;

	LongPoint lptCenter;
	if(m_nMode==kThreePoint)
	{
		Coord radius;
		guCircBy3Pt(&lptCenter,&radius,GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
	}
	else lptCenter=GetShapePoint(0);
	ext|=lptCenter;
	return ext;
}

LongPoint CMDArcShape::GetCenter() const
{
	LongPoint lptCenter;
	Coord radius;
	switch(m_nMode)
	{
	case kRadius:
	case kElliptic:
		lptCenter=GetShapePoint(0);
		break;
	case kThreePoint:
		guCircBy3Pt(&lptCenter,&radius,GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
		break;
	}
	return lptCenter;
}

LongPoint CMDArcShape::GetRadii() const
{
	LongPoint lptCenter;
	Coord radius;
	switch(m_nMode)
	{
	case kRadius:
		radius=Pythag(GetShapePoint(1)-GetShapePoint(0));
		break;
	case kElliptic:
		return Abs(mtRotPoint(GetShapePoint(3)-GetShapePoint(0),-m_RotAngle));
		break;
	case kThreePoint:
		guCircBy3Pt(&lptCenter,&radius,GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
		break;
	}
	return LongPoint(radius,radius);
}

LongPoint CMDArcShape::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapePoints());
	return m_pt[nIndex];
}

void CMDArcShape::SetShapePoint(int nIndex, const LongPoint &lpt)
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapePoints());
	m_pt[nIndex]=lpt;
}

CEdge* CMDArcShape::GetShapeEdge(int nIndex) const
{
	ASSERT(nIndex==0);
	LongPoint lptCenter,lptRadii;
	CArcEdge* pEdge=NULL;
	switch(m_nMode)
	{
	case kRadius:
		lptRadii.x=lptRadii.y=Pythag(GetShapePoint(1)-GetShapePoint(0));
		pEdge=new CArcEdge(GetShapePoint(0),lptRadii,GetShapePoint(1),GetShapePoint(2),m_nArcDirection);
		break;
	case kElliptic:
		lptRadii=GetRadii();
		pEdge=new CArcEdge(GetShapePoint(0),lptRadii,GetShapePoint(1),GetShapePoint(2),m_nArcDirection,m_RotAngle);
		break;
	case kThreePoint:
		guCircBy3Pt(&lptCenter,&lptRadii.x,GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
		lptRadii.y=lptRadii.x;
		pEdge=new CArcEdge(lptCenter,lptRadii,GetShapePoint(0),GetShapePoint(2),m_nArcDirection);
		break;
	}
	return pEdge;
}

void CMDArcShape::DrawShape(CDraftContext* pDC) const
{
	DrawFill(pDC);

	pDC->SetPen(m_PenModel);
	DrawShapeOutline(pDC);
}

void CMDArcShape::DrawShapeOutline(CDraftContext* pDC) const
{
	LongPoint lptCenter,lptRadii;
	LongPoint lptFrom,lptTo;
	lptTo=GetShapePoint(2);
	switch(m_nMode)
	{
	case kRadius:
		lptCenter=GetShapePoint(0);
		lptFrom=GetShapePoint(1);
		lptRadii.x=lptRadii.y=Pythag(lptFrom-lptCenter);
		pDC->Arc(lptCenter,lptRadii,lptFrom,lptTo,m_nArcDirection,0,0,0,true, true);
		break;
	case kElliptic:
		lptCenter=GetShapePoint(0);
		lptFrom=GetShapePoint(1);
		lptRadii=GetRadii();
		pDC->Arc(lptCenter,lptRadii,lptFrom,lptTo,m_nArcDirection,m_RotAngle,0,0,true, true);
		break;
	case kThreePoint:
		lptFrom=GetShapePoint(0);
		guCircBy3Pt(&lptCenter,&lptRadii.x,lptFrom,GetShapePoint(1),lptTo);
		lptRadii.y=lptRadii.x;
		pDC->Arc(lptCenter,lptRadii,lptFrom,lptTo,m_nArcDirection,0,0,0,true, true);
		break;
	}
}

void CMDArcShape::DrawShapeOutlineForFill(CDraftContext* pDC) const
{
	DrawShapeOutline(pDC);
	pDC->LineTo(GetCenter());
	pDC->LineTo(GetShapePoint(m_nMode==kThreePoint?0:1));
}

bool CMDArcShape::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	LongPoint lptCenter,lptRadii;
	LongPoint lptFrom,lptTo;

	lptTo=GetShapePoint(2);
	if(m_nMode==kThreePoint)
	{
		lptFrom=GetShapePoint(0);
		guCircBy3Pt(&lptCenter,&lptRadii.x,lptFrom,GetShapePoint(1),lptTo);
		lptRadii.y=lptRadii.x;
	}
	else
	{
		lptCenter=GetCenter();
		lptRadii=GetRadii();
		lptFrom=GetShapePoint(1);
	}
	short nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
   pointd p1;
   pMapper->LPtoDP(p1,lptCenter);
	if(guPtAtEllip(pt,p1,(short)pMapper->LPtoDP(lptRadii.x),(short)pMapper->LPtoDP(lptRadii.y),m_RotAngle,nWidth,Alignment(m_PenModel.m_nAlignment),m_FillModel.m_nFillType!=kNoFill))
	{
		if(lptFrom==lptTo) return true;	// whole ellipse
      LongPoint lp2;
      lp2=pMapper->DPtoLP(pt);
		Angle angle = mtFixAngle(mtAngle(lptCenter, lp2));
		Angle startAngle = mtFixAngle(mtAngle(lptCenter, lptFrom));
		Angle endAngle = mtFixAngle(mtAngle(lptCenter, lptTo));
		if(m_nArcDirection==AD_CLOCKWISE) return CArcEdge::AngleBetween(angle,endAngle,startAngle);
		else return CArcEdge::AngleBetween(angle,startAngle,endAngle);
	}
	return false;
}

int CMDArcShape::GetNumShapeHandles() const
{
	return m_nMode!=kThreePoint?4:3;
}

LongPoint CMDArcShape::GetShapeHandle(int nIndex) const
{
	if(nIndex==3)
	{
		if(m_nMode==kRadius)
		{
			Angle a1,a2;
			a1=mtATan2(m_pt[1]-m_pt[0]);
			a2=mtATan2(m_pt[2]-m_pt[0]);
			if(m_nArcDirection==AD_COUNTERCLOCKWISE && a2<a1)
				a2+=k360;
			else if(m_nArcDirection==AD_CLOCKWISE && a2>a1)
				a2-=k360;
			return mtRotPoint(m_pt[1],(a2-a1)>>1,m_pt[0]);
		}
		else
		{
			ASSERT(m_nMode==kElliptic);
			const_cast<CMDArcShape*>(this)->m_ptCenterHandle=GetShapeEdge(0)->MidPoint();
			return m_ptCenterHandle;
		}
	}
	else
		return GetShapePoint(nIndex); 
}

int	CMDArcShape::TestHandles(const pointd& pt, CMapper* pMapper) const
{
	for(int i=0;i<GetNumShapeHandles();i++)
	{
      pointd p;
      pMapper->LPtoDP(p, GetShapeHandle(i));
		if(TestHandle(p,pt))
			return i;
	}
	return -1; // miss it
}

void CMDArcShape::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
	if(m_nMode!=kElliptic)
	{
		LongPoint ratio=lrcTo.GetSize()/lrcFrom.GetSize();
		if(ratio.x!=ratio.y)
		{
			LongPoint lptRadii=GetRadii();
			if(m_nMode==kThreePoint)
			{
				LongPoint lptCenter=GetCenter();
				SetShapePoint(1,GetShapePoint(0));
				SetShapePoint(0,lptCenter);
			}
			m_nMode=kElliptic;
			SetShapePoint(3,GetShapePoint(0)+lptRadii);
		}
	}
	CMDShape::DoShapeScale(lrcFrom,lrcTo,ppNewShape);
	if(m_nMode==kElliptic)
	{
		LongPoint lptCenter=GetCenter();
		LongPoint lptRadii=GetRadii();
		SetShapePoint(1,mtRotPoint(SnapPtToEllipse(lptCenter,lptRadii,mtRotPoint(GetShapePoint(1),-m_RotAngle,lptCenter)),m_RotAngle,lptCenter));
		SetShapePoint(2,mtRotPoint(SnapPtToEllipse(lptCenter,lptRadii,mtRotPoint(GetShapePoint(2),-m_RotAngle,lptCenter)),m_RotAngle,lptCenter));
	}
}

void CMDArcShape::DoShapeMirror(const LongPoint &lptRef1, const LongPoint &lptRef2)
{
	m_nArcDirection=m_nArcDirection==AD_CLOCKWISE?AD_COUNTERCLOCKWISE:AD_CLOCKWISE;
	CMDFillShape::DoShapeMirror(lptRef1,lptRef2);
}

void CMDArcShape::DoShapeFlip(bool bVertical,LongPoint* pLpt)
{
	m_nArcDirection=m_nArcDirection==AD_CLOCKWISE?AD_COUNTERCLOCKWISE:AD_CLOCKWISE;
	CMDFillShape::DoShapeFlip(bVertical,pLpt);
}

CTrackContext* CMDArcShape::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CArcTrackContext* p=new CArcTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	for(int i=0;i<GetNumShapePoints();i++)
		p->pt[i]=GetShapePoint(i);
	p->nArcDirection=m_nArcDirection;
	if(m_nMode!=kThreePoint && (nWhichHandle==1 || nWhichHandle==2)) p->ptMouse=p->pt[nWhichHandle];

	if(nWhichHandle==kRotateHandle)
	{
		CRotateContext* pCtx=(CRotateContext*)lParam;
		LongPoint lptRot;
		switch(pCtx->m_nAbout)
		{
		case kAboutCenter:
			lptRot=GetCenter(); //Middle(GetExtents());
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
		for(int i=0;i<GetNumShapePoints();i++)
			p->pt[i]=mtRotPoint(p->pt[i], pCtx->m_Angle, lptRot);
		p->rotAngle=m_RotAngle+pCtx->m_Angle;
	}
	else p->rotAngle=m_RotAngle;
	return p;
}

void CMDArcShape::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CArcTrackContext* p=(CArcTrackContext*)pTrackContext;
	int nHandle=p->nWhichHandle;
	if(nHandle==kMoveHandle)
	{
		for(int i=0;i<GetNumShapePoints();i++) p->pt[i]+=pt;
	}
	else
	{
		switch(m_nMode)
		{
		case kRadius:
			switch(nHandle)
			{
			case 0:
				{
					LongPoint delta=pt-p->pt[0];
					for(int i=0;i<3;i++)
						p->pt[i]+=delta;
				}
				break;
			case 1:
			case 2:
				{
					Angle ang1=mtFixAngle(mtAngle(p->pt[0],p->pt[p->nWhichHandle==1?2:1]));
					Angle angDeltaBefore=mtFixAngle(mtAngle(m_pt[0],p->pt[p->nWhichHandle])-ang1);
					Angle angDeltaAfter=mtFixAngle(mtAngle(m_pt[0],pt)-ang1);

					if(angDeltaBefore>k270 && angDeltaAfter<k90)
						p->nArcDirection=p->nWhichHandle==2?AD_COUNTERCLOCKWISE:AD_CLOCKWISE;
					else if(angDeltaBefore<k90 && angDeltaAfter>k270)
						p->nArcDirection=p->nWhichHandle==2?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;

					Coord nRadius=Pythag(p->pt[p->nWhichHandle]-p->pt[0]);
					Angle ang=mtAngle(p->pt[0],pt);
					p->pt[p->nWhichHandle].x=p->pt[0].x+mtCos(ang)*nRadius;
					p->pt[p->nWhichHandle].y=p->pt[0].y+mtSin(ang)*nRadius;
					p->ptMouse=pt;
				}
				break;
			case 3:
				{
					Coord nRadius=Pythag(pt-p->pt[0]);
					p->pView->SnapLength(nRadius);
					if(nRadius>0)
					{
						// use original points to avoid flooring
						Angle ang1=mtATan2(m_pt[1]-m_pt[0]);
						Angle ang2=mtATan2(m_pt[2]-m_pt[0]);
						LongPoint pt2(nRadius,0);
						p->pt[1]=p->pt[0]+mtRotPoint(pt2, ang1);
						p->pt[2]=p->pt[0]+mtRotPoint(pt2, ang2);
					}
				}
				break;
			}
			break;
		case kElliptic:
			switch(nHandle)
			{
			case 0:
				{
					LongPoint delta=pt-p->pt[0];
					for(int i=0;i<4;i++)
						p->pt[i]+=delta;
				}
				break;
			case 1:
			case 2:
				{
					LongPoint ptRadii=Abs(mtRotPoint(p->pt[3]-p->pt[0],-p->rotAngle));
					Angle ang1=mtFixAngle(mtEATan2(p->pt[p->nWhichHandle==1?2:1]-p->pt[0],ptRadii.x,ptRadii.y));
					Angle angDeltaBefore=mtFixAngle(mtEATan2(p->pt[p->nWhichHandle]-m_pt[0],ptRadii.x,ptRadii.y)-ang1);
					Angle angDeltaAfter=mtFixAngle(mtEATan2(pt-m_pt[0],ptRadii.x,ptRadii.y)-ang1);
					if(angDeltaBefore>k270 && angDeltaAfter<k90)
						p->nArcDirection=p->nWhichHandle==2?AD_COUNTERCLOCKWISE:AD_CLOCKWISE;
					else if(angDeltaBefore<k90 && angDeltaAfter>k270)
						p->nArcDirection=p->nWhichHandle==2?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
					p->pt[p->nWhichHandle]=mtRotPoint(SnapPtToEllipse(p->pt[0],ptRadii, mtRotPoint(pt,-p->rotAngle,p->pt[0])),p->rotAngle,p->pt[0]);
					p->ptMouse=pt;
				}
				break;
			case 3:
				{
					LongPoint ptHandle=mtRotPoint(m_ptCenterHandle,-p->rotAngle,p->pt[0]);
					LongPoint delta=mtRotPoint(pt,-p->rotAngle,p->pt[0])-ptHandle;
					if(ptHandle.x<m_pt[0].x) delta.x=-delta.x;
					if(ptHandle.y<m_pt[0].y) delta.y=-delta.y;
					p->pt[3]=m_pt[3]+mtRotPoint(delta,p->rotAngle);;
					LongPoint lptRadii=Abs(mtRotPoint(p->pt[3]-p->pt[0],-p->rotAngle));
					p->pt[1]=mtRotPoint(SnapPtToEllipse(p->pt[0],lptRadii,mtRotPoint(p->pt[1],-p->rotAngle,p->pt[0])),p->rotAngle,p->pt[0]);
					p->pt[2]=mtRotPoint(SnapPtToEllipse(p->pt[0],lptRadii,mtRotPoint(p->pt[2],-p->rotAngle,p->pt[0])),p->rotAngle,p->pt[0]);
					p->ptMouse=pt;
				}
				break;
			}
			break;
		case kThreePoint:
			p->pt[nHandle]=pt;
			Angle ang=mtFixAngle(mtAngle(p->pt[0],p->pt[1])-mtAngle(p->pt[0],p->pt[2]));
			p->nArcDirection=ang<k180?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
			break;
		}
	}
}

void CMDArcShape::TrackShowSize(CTrackContext* pTrackContext)
{
	CArcTrackContext* p=(CArcTrackContext*)pTrackContext;
	switch(m_nMode)
	{
	case kRadius:
		switch(p->nWhichHandle)
		{
		case 1:
		case 2:
			{
				Angle toAngle=mtAngle(p->pt[0],p->pt[p->nWhichHandle]);
				Angle angle=mtFixAngle(toAngle-mtAngle(m_pt[0],m_pt[3-p->nWhichHandle]));
				if(p->nArcDirection==(p->nWhichHandle==2?AD_CLOCKWISE:AD_COUNTERCLOCKWISE)) angle=360-angle;
				p->pView->SetShowSize(0,-1,toAngle);
				p->pView->SetShowSize(1,-3,angle);
			}
			break;
		case 3:
			p->pView->SetShowSize(0,4,Pythag(p->pt[1]-p->pt[0]));
			break;
		}
		break;
	case kElliptic:
		switch(p->nWhichHandle)
		{
		case 1:
		case 2:
			{
				Angle toAngle=mtAngle(p->pt[0],p->pt[p->nWhichHandle]);
				Angle angle=mtFixAngle(toAngle-mtAngle(m_pt[0],m_pt[3-p->nWhichHandle]));
				if(p->nArcDirection==(p->nWhichHandle==2?AD_CLOCKWISE:AD_COUNTERCLOCKWISE)) angle=360-angle;
				p->pView->SetShowSize(0,-1,toAngle);
				p->pView->SetShowSize(1,-3,angle);
			}
			break;
		case 3:
			{
				LongPoint lptRadii=Abs(mtRotPoint(p->pt[3]-p->pt[0],-p->rotAngle));
				p->pView->SetShowSize(0,1,lptRadii.x);
				p->pView->SetShowSize(1,2,lptRadii.y);
			}
			break;
		}
		break;
	case kThreePoint:
		{
			Angle ang=mtFixAngle(mtAngle(p->pt[0],p->pt[1])-mtAngle(p->pt[0],p->pt[2]));
			p->pView->SetShowSize(0,-1,ang);
		}
		break;
	}
}

void CMDArcShape::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CArcTrackContext* p=(CArcTrackContext*)pTrackContext;
	LongRect rc;

	LongPoint lptCenter,lptRadii;
	LongPoint lptFrom,lptTo;
	lptTo=p->pt[2];
	switch(m_nMode)
	{
	case kRadius:
		lptCenter=p->pt[0];
		lptFrom=p->pt[1];
		lptRadii.x=lptRadii.y=Pythag(lptFrom-lptCenter);
		if(p->nWhichHandle==1 || p->nWhichHandle==2) pDC->Line(lptCenter,p->ptMouse);
		pDC->Arc(lptCenter,lptRadii,lptFrom,lptTo,m_nArcDirection,0,0,0,true);
		break;
	case kElliptic:
		lptCenter=p->pt[0];
		lptFrom=p->pt[1];
		lptRadii=Abs(mtRotPoint(p->pt[3]-lptCenter,-p->rotAngle));
		if(p->nWhichHandle==1 || p->nWhichHandle==2) pDC->Line(lptCenter,p->ptMouse);
		pDC->Arc(lptCenter,lptRadii,lptFrom,lptTo,m_nArcDirection,p->rotAngle,0,0,true);
		break;
	case kThreePoint:
		lptFrom=p->pt[0];
		if(guCircBy3Pt(&lptCenter,&lptRadii.x,lptFrom,p->pt[1],lptTo))
		{
			lptRadii.y=lptRadii.x;
			pDC->Arc(lptCenter,lptRadii,lptFrom,lptTo,p->nArcDirection,0,0,0,true);
			if(p->nWhichHandle>=0)
			{
				pDC->LineTo(p->pt[1]);
				pDC->LineTo(lptFrom);
			}
		}
		break;
	}
}

void CMDArcShape::TrackComplete(CTrackContext* pTrackContext)
{
	CArcTrackContext* p=(CArcTrackContext*)pTrackContext;
	if(m_nMode==kThreePoint)
	{
		Coord nRadius;
		LongPoint lptCenter;
		if(!guCircBy3Pt(&lptCenter,&nRadius,p->pt[0],p->pt[1],p->pt[2])) return;
	}
	for(int i=0;i<3;i++) SetShapePoint(i,p->pt[i]);
	if(m_nMode==kElliptic) SetShapePoint(3,p->pt[0]+mtRotPoint(Abs(mtRotPoint(p->pt[3]-p->pt[0],-p->rotAngle)),p->rotAngle));
	m_nArcDirection=p->nArcDirection;
	CalculateExtents();
	p->pView->ClearShowSize();
}

double CMDArcShape::GetRawArea(const LongRatio& ratio) const
{
	LongPoint lptCenter,lptRadii;
	Angle nStartAngle, nEndAngle, nAngle;
	GetArcInfo(m_nMode, m_pt, m_nArcDirection, &lptCenter, &lptRadii, &nStartAngle, &nEndAngle,m_RotAngle);
	double area=ScaleDist(ratio,lptRadii.x)*ScaleDist(ratio,lptRadii.y)*_M_PI; //pi;
	area*=mtFixAngle(nEndAngle-nStartAngle).GetRawDouble()/k360.GetRawDouble();
	return area;
}

void CMDArcShape::GetArcInfo(int nMode, const LongPoint pt[], int nArcDirection, LongPoint* ptCenter, LongPoint* radii, Angle* nStartAngle, Angle* nEndAngle,Angle nRotAngle)
{
	switch(nMode)
	{
	case kRadius:
		*ptCenter=pt[0];
		radii->x=radii->y=Pythag(*ptCenter-pt[1]);
		*nStartAngle=mtFixAngle(mtAngle(*ptCenter,pt[1]));
		*nEndAngle=mtFixAngle(mtAngle(*ptCenter,pt[2]));
		break;
	case kElliptic:
		*ptCenter=pt[0];
		*radii=Abs(mtRotPoint(pt[3]-pt[0],-nRotAngle));
		*nStartAngle=mtFixAngle(mtAngle(*ptCenter,pt[1]));
		*nEndAngle=mtFixAngle(mtAngle(*ptCenter,pt[2]));
		break;
	case kThreePoint:
		guCircBy3Pt(ptCenter,&radii->x,pt[0],pt[1],pt[2]);
		radii->y=radii->x;
		*nStartAngle=mtFixAngle(mtAngle(*ptCenter,pt[0]));
		*nEndAngle=mtFixAngle(mtAngle(*ptCenter,pt[2]));
		break;
	}
	// mtFixAngle return [0..k360)
	// let's make startangle<endangle
	if(nArcDirection==AD_CLOCKWISE)
	{
		Angle t=*nStartAngle;
		*nStartAngle=*nEndAngle;
		*nEndAngle=t;
	}
	if(*nStartAngle>*nEndAngle)
		*nStartAngle-=k360;
}

LongPoint CMDArcShape::SnapPtToEllipse(LongPoint ptCenter, LongPoint ptRadii, LongPoint pt)
{
	Angle ang=mtEATan2(pt-ptCenter,ptRadii.x,ptRadii.y);
	LongPoint endPt;
	endPt.x=ptCenter.x+WrFixedFromRawDoubleTruncated(ptRadii.x.GetRawDouble()*mtCos(WrFixed(ang)));
	endPt.y=ptCenter.y+WrFixedFromRawDoubleTruncated(ptRadii.y.GetRawDouble()*mtSin(WrFixed(ang)));
	return endPt;
}

const ResizeInfo* CMDArcShape::GetResizeInfo() const
{
static ResizeInfo arcResize[3] =
{
	{	IDS_ARC,   3, {IDS_1A,kAngleValue, IDS_ENDA,kAngleValue, IDS_RADIUS,kUnitlessValue} },
	{	IDS_EARC,  4, {IDS_1A,kAngleValue, IDS_ENDA,kAngleValue, IDS_XR,kUnitlessValue, IDS_YR,kUnitlessValue} },
	{	IDS_ARC3P, 6, {IDS_1X,kUnitlessValue, IDS_Y,kUnitlessValue, IDS_2X,kUnitlessValue, IDS_Y,kUnitlessValue, IDS_3X,kUnitlessValue, IDS_Y,kUnitlessValue} }
};
	return &arcResize[m_nMode];
}

void CMDArcShape::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	CMDFillShape::GetResizeValue(pValues,nHandle);
	switch(m_nMode)
	{
	case kRadius:
		pValues[0].m_Angle=mtAngle(GetShapePoint(0),GetShapePoint(1));
		pValues[1].m_Angle=mtAngle(GetShapePoint(0),GetShapePoint(2));
		pValues[2].m_Unitless=Pythag(GetShapePoint(1)-GetShapePoint(0));
		break;
	case kElliptic:
		{
			pValues[0].m_Angle=mtAngle(GetShapePoint(0),GetShapePoint(1));
			pValues[1].m_Angle=mtAngle(GetShapePoint(0),GetShapePoint(2));
			LongPoint radii=GetRadii();
			pValues[2].m_Unitless=radii.x;
			pValues[3].m_Unitless=radii.y;
		}
		break;
	case kThreePoint:
		pValues[0].m_Unitless=GetShapePoint(0).x;
		pValues[1].m_Unitless=GetShapePoint(0).y;
		pValues[2].m_Unitless=GetShapePoint(1).x;
		pValues[3].m_Unitless=GetShapePoint(1).y;
		pValues[4].m_Unitless=GetShapePoint(2).x;
		pValues[5].m_Unitless=GetShapePoint(2).y;
		break;
	}
}

void CMDArcShape::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	switch(m_nMode)
	{
	case kRadius:
		{
			Coord radius=pValues[2].m_Unitless;
			if(radius<kOnePixel) radius=kOnePixel;
			LongPoint lpt=GetShapePoint(0);
			lpt.x+=radius;
			SetShapePoint(1,mtRotPoint(lpt,pValues[0].m_Angle,GetShapePoint(0)));

			lpt=GetShapePoint(0);
			lpt.x+=radius;
			SetShapePoint(2,mtRotPoint(lpt,pValues[1].m_Angle,GetShapePoint(0)));
		}
		break;
	case kElliptic:
		{
			LongPoint radii(pValues[2].m_Unitless,pValues[3].m_Unitless);
			if(radii.x<kOnePixel) radii.x=kOnePixel;
			if(radii.y<kOnePixel) radii.y=kOnePixel;
			SetShapePoint(3,GetShapePoint(0)+radii);

			LongPoint lpt=GetShapePoint(0);
			lpt.x+=radii.x;
			SetShapePoint(1,SnapPtToEllipse(GetShapePoint(0),radii,mtRotPoint(lpt,pValues[0].m_Angle,GetShapePoint(0))));

			lpt=GetShapePoint(0);
			lpt.x+=pValues[2].m_Unitless;
			SetShapePoint(2,SnapPtToEllipse(GetShapePoint(0),radii,mtRotPoint(lpt,pValues[1].m_Angle,GetShapePoint(0))));
		}
		break;
	case kThreePoint:
		{
			Coord nRadius;
			LongPoint lptCenter;
			if(!guCircBy3Pt(&lptCenter,&nRadius,LongPoint(pValues[0].m_Unitless,pValues[1].m_Unitless),LongPoint(pValues[2].m_Unitless,pValues[3].m_Unitless),LongPoint(pValues[4].m_Unitless,pValues[5].m_Unitless)))
			{
				MessageBeep(MB_ICONEXCLAMATION);
				return;
			}

			SetShapePoint(0,LongPoint(pValues[0].m_Unitless,pValues[1].m_Unitless));
			SetShapePoint(1,LongPoint(pValues[2].m_Unitless,pValues[3].m_Unitless));
			SetShapePoint(2,LongPoint(pValues[4].m_Unitless,pValues[5].m_Unitless));
			m_nArcDirection=mtFixAngle(mtAngle(GetShapePoint(0),GetShapePoint(1))-mtAngle(GetShapePoint(0),GetShapePoint(2)))<k180?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
		}
		break;
	}
	CMDFillShape::SetResizeValue(pValues);
}

void CMDArcShape::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
	CString		strIDName, strBuffer;
	short		vPenColIdx;
	short		vIdx1, vIdx2, vDashIdx;
	LongPoint	vVertexPt, vLPt1;
	double		vRadius, vAngle;
	Angle		vStAng, vEndAng;

	PenModel aPenModel=GetPenModel();
	vDashIdx = aPenModel.Dashing();

	if(m_nMode==kElliptic)
	{
		LongPoint dbPoint = GetCenter () ;
		CArray <LongPoint,LongPoint> vPtHdl;
		Angle startAng, endAng;

		LongPoint radii=Abs(GetRadii());
		Angle pRot=GetRotation();
		startAng = guLPtToLAngle(radii.x,radii.y,mtRotPoint(GetShapePoint(1),-pRot,dbPoint),dbPoint);
		endAng = guLPtToLAngle(radii.x,radii.y,mtRotPoint(GetShapePoint(2),-pRot,dbPoint),dbPoint);

		if(m_nArcDirection==AD_CLOCKWISE)
		{
			Angle temp=endAng;
			endAng=startAng;
			startAng=temp;
		}
		while(endAng<startAng) endAng+=k360;

		DxfGetArcPts (vPtHdl, dbPoint, radii.x, radii.y, startAng, endAng, pRot, true, &aPenModel);

		if (!vPtHdl.GetSize())
			return;

  		DxfWritePolyHead (pFile, pLayerStr, &aPenModel,
						   pDocExtPtr, vUnitFactor, 0, ptrPalette);

 		// Write the vertices of the POINT 
  		vVertexPt.x = vPtHdl[0].x - pDocExtPtr->left;
  		vVertexPt.y = pDocExtPtr->bottom - vPtHdl[0].y;
  		DxfWritePtCord (pFile, pLayerStr, &vVertexPt,
						 &vUnitFactor, vDashIdx);

  		for (vIdx1 = 0; vIdx1 < vPtHdl.GetSize(); vIdx1++) {
			// Write the vertices 
			vVertexPt.x = vPtHdl[vIdx1].x - pDocExtPtr->left;
			vVertexPt.y = pDocExtPtr->bottom - vPtHdl[vIdx1].y;
			DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
							 &vUnitFactor, 0L, 0, vDashIdx);
  		} // for vIdx1 

 		DxfWriteSeqend (pFile, pLayerStr);
	} /* writeEArc */
	else
	{
  		findStrFromTab (kDxfArc, strIDName, kEntitiesTab);

  		vIdx1 = kEntryStart;
  		vIdx2 = kLayerName;

		vLPt1 = GetShapePoint(0);
  		vEndAng = mtAngle(GetShapePoint(0), GetShapePoint(1));
  		vStAng  = mtAngle(GetShapePoint(0),GetShapePoint(2));

  		/* Write the Shape name and the Layer name */
  		strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", vIdx1, strIDName, vIdx2, pLayerStr);
		pFile->Write(strBuffer, strBuffer.GetLength());
  		
  		/* Write the Pen Color */
  		vIdx1 = kColorNumber;
  		vPenColIdx = DxfGetColorIndex (&GetPenModel(), ptrPalette);
		strBuffer.Format("%3d\r\n%3d\r\n", vIdx1, vPenColIdx);
		pFile->Write(strBuffer, strBuffer.GetLength());
  		
 		/* Write the center point of the Arc */
  		vVertexPt.x = vLPt1.x - pDocExtPtr->left;
  		vVertexPt.y = pDocExtPtr->bottom - vLPt1.y;
  		DxfWritePtCord (pFile, pLayerStr, &vVertexPt, &vUnitFactor, vDashIdx);
		
  		/* Write the radius of Arc */
  		vIdx1 = kOtherFloatValMin;
  		vRadius = mtFixToReal(GetRadii().x) / vUnitFactor;
		strBuffer.Format("%3d\r\n%.6f\r\n", vIdx1, vRadius);
		pFile->Write(strBuffer, strBuffer.GetLength());
  		
  		/* Write the Start angle of Arc */
  		vIdx1 = kAngleMin;
		vAngle = mtFixToReal ( (m_nArcDirection==AD_CLOCKWISE) ? vStAng : vEndAng);
		strBuffer.Format("%3d\r\n%.6f\r\n", vIdx1, vAngle);
		pFile->Write(strBuffer, strBuffer.GetLength());
  		
  		/* Write the End angle of Arc */
  		vIdx1 = kAngleMax;
  		vAngle = mtFixToReal ((m_nArcDirection==AD_CLOCKWISE) ? vEndAng : vStAng);
  		strBuffer.Format("%3d\r\n%.6f\r\n", vIdx1, vAngle);
		pFile->Write(strBuffer, strBuffer.GetLength());
	} /* writeArc */
}

int CMDArcShape::SaveMacDraft(PhIOContext& ctx)
{
	if(m_nMode==kElliptic)
	{
		PhDSEArc ps;
		ps.fID=PhIDEArc;
		ctx.pPhShape=&ps;
		CMDFillShape::SaveMacDraft(ctx);

		ps.fMethod=kPhEllipse;
		ps.fDummy=0;
		ps.fCenter.fUse=0;
		ps.fCenter.fFlags=kPhRealVertex;
		ps.fCenter.fLongPt=GetCenter();
		LongPoint radii=GetRadii();
		ps.fHRadius=radii.x;
		ps.fVRadius=radii.y;

		ps.fPoint[0].fUse=0;
		ps.fPoint[0].fFlags=kPhRealVertex;
		ps.fPoint[1].fUse=0;
		ps.fPoint[1].fFlags=kPhRealVertex;

		if(m_nArcDirection==AD_CLOCKWISE)
		{
			ps.fPoint[0].fLongPt=m_pt[2];
			ps.fPoint[1].fLongPt=m_pt[1];
		}
		else
		{
			ps.fPoint[0].fLongPt=m_pt[1];
			ps.fPoint[1].fLongPt=m_pt[2];
		}
		ps.fStartAng=mtFixAngle(k90-guLPtToLAngle(ps.fHRadius,ps.fVRadius,ps.fPoint[0].fLongPt,ps.fCenter.fLongPt));
		ps.fEndAng=mtFixAngle(k90-guLPtToLAngle(ps.fHRadius,ps.fVRadius,ps.fPoint[1].fLongPt,ps.fCenter.fLongPt));

		Angle midAngle=ps.fEndAng-ps.fStartAng;
		if(midAngle<0) midAngle=360-midAngle;
		midAngle=mtFixAngle(ps.fStartAng+(midAngle>>1));
		ps.fPoint[2].fUse=0;
		ps.fPoint[2].fFlags=kPhRealVertex;
		ps.fPoint[2].fLongPt.x=ps.fCenter.fLongPt.x+mtFixMulDbl(ps.fHRadius,mtCos(midAngle));
		ps.fPoint[2].fLongPt.y=ps.fCenter.fLongPt.y+mtFixMulDbl(ps.fVRadius,mtSin(midAngle));
		ps.fPoint[2].fLongPt=mtRotPoint(ps.fPoint[2].fLongPt,m_RotAngle,ps.fCenter.fLongPt);

		uptr p((char*)&ps);
		p.swapDSShape();
		p.swaps(2); // fm

		p.skip(2);
		p.swapDBPoint(1);
		p.swapl(4);
		p.swapDBPoint(3);

		ushort count=sizeof(PhDSEArc)<<8;	// swapped
		ctx.f->Write(&count,sizeof(short));
		ctx.f->Write(&ps,sizeof(PhDSEArc));
	}
	else
	{
		PhDSArc ps;
		ps.fID=(m_nMode==kRadius)?PhIDArc:PhID3Arc;
		ctx.pPhShape=&ps;
		CMDFillShape::SaveMacDraft(ctx);

		ps.fMethod=(m_nMode==kRadius)?kPhCenterRadius:kPh3Point;
		ps.fDummy=0;
		ps.fCenter.fUse=0;
		ps.fCenter.fFlags=kPhRealVertex;
		ps.fCenter.fLongPt=GetCenter();
		ps.fRadius=GetRadii().x;

		switch(m_nMode)
		{
		case kRadius:
			{
				ps.fPoint[0].fUse=0;
				ps.fPoint[0].fFlags=kPhRealVertex;
				ps.fPoint[0].fLongPt=m_pt[1];
				ps.fPoint[1].fUse=0;
				ps.fPoint[1].fFlags=kPhRealVertex;
				ps.fPoint[1].fLongPt=m_pt[2];
				ps.fPoint[2].fUse=0;
				ps.fPoint[2].fFlags=0;
				ps.fPoint[2].fLongPt=LongPoint::kZeroPt;
			}
			break;
		case kThreePoint:
			{
				ps.fPoint[0].fUse=0;
				ps.fPoint[0].fFlags=kPhRealVertex;
				ps.fPoint[0].fLongPt=m_pt[0];
				ps.fPoint[1].fUse=0;
				ps.fPoint[1].fFlags=kPhRealVertex;
				ps.fPoint[1].fLongPt=m_pt[2];
				ps.fPoint[2].fUse=0;
				ps.fPoint[2].fFlags=kPhRealVertex;
				ps.fPoint[2].fLongPt=m_pt[1];
			}
			break;
		}
		if(m_nArcDirection==AD_CLOCKWISE)
		{
			ps.fStartAng=mtFixAngle(k90-mtAngle(ps.fCenter.fLongPt,ps.fPoint[1].fLongPt));
			ps.fEndAng=mtFixAngle(k90-mtAngle(ps.fCenter.fLongPt,ps.fPoint[0].fLongPt));
		}
		else
		{
			ps.fStartAng=mtFixAngle(k90-mtAngle(ps.fCenter.fLongPt,ps.fPoint[0].fLongPt));
			ps.fEndAng=mtFixAngle(k90-mtAngle(ps.fCenter.fLongPt,ps.fPoint[1].fLongPt));
		}

		uptr p((char*)&ps);
		p.swapDSShape();
		p.swaps(2); // fm

		p.skip(2);
		p.swapDBPoint(1);
		p.swapl(3);
		p.swapDBPoint(3);

		ushort count=sizeof(PhDSArc)<<8;	// swapped
		ctx.f->Write(&count,sizeof(short));
		ctx.f->Write(&ps,sizeof(PhDSArc));
	}
	SaveMacObjInfo(ctx);
	return 1;
}

void CMDArcShape::GetVertexesFromPt(CUIntArray& vertexes,CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
	if(kThreePoint==m_nMode)
		return;
	int nHandle=TestHandles(pt,pMapper);
	if(0==nHandle)
	{
		vertexes.Add(0);
		shapes.Add(this);
	}
}
