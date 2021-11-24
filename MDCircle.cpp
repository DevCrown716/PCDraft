// MDCircle.cpp: implementation of the CMDCircle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDCircle.h"

#include "gu.h"
#include "phmath.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "ArcEdge.h"
#include "dxfutil.h"
#include "PCDraftView.h"
#include "MDEllipse.h"
#include "MDArea.h"
#include "PhIOStruct.h"
#include "LinkVertex.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDCircle,CMDFillShape,1)

//////////////////////////////////////////////////////////////////////

CMDCircle::CMDCircle() : m_nType(kRadius)
{
}

CMDCircle::CMDCircle(int nType) : m_nType(nType)
{
}

void CMDCircle::Serialize(CArchive& ar)
{
	CMDFillShape::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_nType >> m_pt[0] >> m_pt[1];
		if(m_nType==kThreePoint)
			ar >> m_pt[2];
	}
	else
	{
		ar << m_nType << m_pt[0] << m_pt[1];
		if(m_nType==kThreePoint)
			ar << m_pt[2];
	}
}

LongRect CMDCircle::CalculateBasicExtents() const
{
	LongPoint ptCenter;
	Coord radius;
	switch(m_nType)
	{
	case kDiameter:
		ptCenter=Average(GetShapePoint(0),GetShapePoint(1));
		radius=Pythag(GetShapePoint(1)-GetShapePoint(0))/short(2);
		break;
	case kRadius:
		ptCenter=GetShapePoint(0);
		radius=Pythag(GetShapePoint(1)-GetShapePoint(0));
		break;
	case kThreePoint:
		guCircBy3Pt(&ptCenter,&radius,GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
		break;
	}
	return BoundRectOfCircle(ptCenter,radius);
}

LongPoint CMDCircle::GetCenter() const
{
	LongPoint ptCenter;
	Coord radius;
	switch(m_nType)
	{
	case kDiameter:
		ptCenter=Average(GetShapePoint(0),GetShapePoint(1));
		break;
	case kRadius:
		ptCenter=GetShapePoint(0);
		break;
	case kThreePoint:
		guCircBy3Pt(&ptCenter,&radius,GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
		break;
	}
	return ptCenter;
}

LongPoint CMDCircle::GetRadii() const
{
	LongPoint ptCenter;
	Coord radius;
	switch(m_nType)
	{
	case kDiameter:
		radius=Pythag(GetShapePoint(1)-GetShapePoint(0))/short(2);
		break;
	case kRadius:
		radius=Pythag(GetShapePoint(1)-GetShapePoint(0));
		break;
	case kThreePoint:
		guCircBy3Pt(&ptCenter,&radius,GetShapePoint(0),GetShapePoint(1),GetShapePoint(2));
		break;
	}
	return LongPoint(radius,radius);
}

int CMDCircle::GetNumShapePoints() const
{
	return m_nType==kThreePoint?3:2;
}

LongPoint CMDCircle::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<3);
	return m_pt[nIndex];
}

void CMDCircle::SetShapePoint(int nIndex, const LongPoint &lpt)
{
	ASSERT(nIndex>=0 && nIndex<3);
	m_pt[nIndex]=lpt;
}

CEdge* CMDCircle::GetShapeEdge(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapeEdges());
	LongPoint lpt=GetCenter();
	lpt.x+=GetRadii().x;
	return new CArcEdge(GetCenter(),GetRadii(),lpt,lpt,AD_CLOCKWISE);
}

void CMDCircle::DrawShape(CDraftContext* pDC) const
{
	DrawFill(pDC);

	pDC->SetPen(m_PenModel);
	DrawShapeOutline(pDC);
}

void CMDCircle::DrawShapeOutline(CDraftContext* pDC) const
{
	pDC->Ellipse(GetCenter(),GetRadii(),0);
}

bool CMDCircle::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
	LongRect lRadii(LongPoint::kZeroPt,GetRadii());
	CRect radii=pMapper->LPtoDP(lRadii);
   pointd pCenter;
   pMapper->LPtoDP(pCenter, GetCenter());
	return guPtAtEllip(pt,pCenter,radii.Width(),radii.Height(),0.0,nWidth,Alignment(m_PenModel.m_nAlignment),false);
}

int	CMDCircle::TestHandles(const pointd& pt, CMapper* pMapper) const
{
	for(int i=0;i<GetNumShapePoints();i++)
	{
      pointd p;
      p = pMapper->LPtoDPF(GetShapePoint(i));

		if(TestHandle(p,pt))
			return i;
	}
	return -1;
}

void CMDCircle::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
	LongPoint ratio=lrcTo.GetSize()/lrcFrom.GetSize();
	if(ratio.x!=ratio.y)
	{
		CMDEllipse* pShape=new CMDEllipse(CMDRectangle::kRectDiagonal);
		LongRect lrc(GetCenter()-GetRadii(),GetCenter()+GetRadii());
		for(int i=0;i<4;i++)
			pShape->SetShapePoint(i,lrc.GetCorner(i));
		pShape->SetPenModel(GetPenModel());
		pShape->SetFillModel(GetFillModel());
		pShape->SetShapeDatum(GetShapeDatum());
		pShape->CalculateExtents();
		pShape->DoShapeScale(lrcFrom,lrcTo,ppNewShape);
		if(m_pAttached)
		{
			pShape->Attach(m_pAttached);
			m_pAttached=NULL;
		}
		*ppNewShape=pShape;
	}
	CMDShape::DoShapeScale(lrcFrom,lrcTo,ppNewShape);
}

CTrackContext* CMDCircle::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CCircleTrackContext* p=new CCircleTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	for(int i=0;i<3;i++)
		p->pt[i]=GetShapePoint(i);
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
		for(int i=0;i<3;i++)
			p->pt[i]=mtRotPoint(p->pt[i], pCtx->m_Angle, lptRot);
	}
	return p;
}

void CMDCircle::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CCircleTrackContext* p=(CCircleTrackContext*)pTrackContext;
	if(p->nWhichHandle==kMoveHandle)
	{
		for(int i=0;i<3;i++) p->pt[i]+=pt;
	}
	else p->pt[p->nWhichHandle]=pt;
}

void CMDCircle::TrackShowSize(CTrackContext* pTrackContext)
{
	CCircleTrackContext* p=(CCircleTrackContext*)pTrackContext;
	if(m_nType!=kThreePoint)
	{
		p->pView->SetShowSize(0,(m_nType==kRadius)?4:3,Pythag(p->pt[1]-p->pt[0]));
	}
}

void CMDCircle::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CCircleTrackContext* p=(CCircleTrackContext*)pTrackContext;
	LongPoint ptCenter;
	Coord radius;
	switch(m_nType)
	{
	case kDiameter:
		ptCenter=Average(p->pt[0],p->pt[1]);
		radius=Pythag(p->pt[1]-p->pt[0])/short(2);
		if(p->nWhichHandle>=0)
			pDC->Line(p->pt[0],p->pt[1]);
		break;
	case kRadius:
		ptCenter=p->pt[0];
		radius=Pythag(p->pt[1]-p->pt[0]);
		if(p->nWhichHandle>=0)
			pDC->Line(p->pt[0],p->pt[1]);
		break;
	case kThreePoint:
		if(!guCircBy3Pt(&ptCenter,&radius,p->pt[0],p->pt[1],p->pt[2])) return;
		if(p->nWhichHandle>=0)
		{
			pDC->MoveTo(p->pt[0]);
			pDC->LineTo(p->pt[1]);
			pDC->LineTo(p->pt[2]);
		}
		break;
	}
	pDC->Ellipse(ptCenter,LongPoint(radius,radius),FixedZero);
}

void CMDCircle::TrackComplete(CTrackContext* pTrackContext)
{
	CCircleTrackContext* p=(CCircleTrackContext*)pTrackContext;
	if(m_nType==kThreePoint)
	{
		Coord nRadius;
		LongPoint lptCenter;
		if(!guCircBy3Pt(&lptCenter,&nRadius,p->pt[0],p->pt[1],p->pt[2])) return;
	}
	for(int i=0;i<3;i++)
		SetShapePoint(i,p->pt[i]);
	CalculateExtents();
}

double CMDCircle::GetRawArea(const LongRatio& ratio) const
{
	return ScaleDist(ratio,GetRadii().x)*ScaleDist(ratio,GetRadii().y)*pi;
}

double CMDCircle::ReportCalcPerimeter(const LongRatio& ratio) const
{
	return ScaleDist(ratio,2*pi*mtFixToReal(GetRadii().x));
}

const ResizeInfo* CMDCircle::GetResizeInfo() const
{
static ResizeInfo circleResize[3] =
{
	{	IDS_CIRCLE,   1, {IDS_DIA,kUnitlessValue} },
	{	IDS_CIRCLE,	  1, {IDS_RADIUS,kUnitlessValue} },
	{	IDS_CIRCLE3P, 6, {IDS_1X,kUnitlessValue, IDS_Y,kUnitlessValue, IDS_2X,kUnitlessValue, IDS_Y,kUnitlessValue, IDS_3X,kUnitlessValue, IDS_Y,kUnitlessValue} }
};
	return &circleResize[m_nType];
}

void CMDCircle::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	CMDFillShape::GetResizeValue(pValues,nHandle);
	switch(m_nType)
	{
	case kDiameter:
	case kRadius:
		pValues[0].m_Unitless=Pythag(GetShapePoint(1)-GetShapePoint(0));
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

void CMDCircle::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	switch(m_nType)
	{
	case kDiameter:
	case kRadius:
		{
			if(nHandle<0 || nHandle>1) nHandle=1;
			Angle angle=mtAngle(GetShapePoint(0),GetShapePoint(1));
			LongPoint lpt=GetShapePoint(1-nHandle);
			if(nHandle)	lpt.x+=pValues[0].m_Unitless;
			else lpt.x-=pValues[0].m_Unitless;
			SetShapePoint(nHandle,mtRotPoint(lpt,angle,GetShapePoint(1-nHandle)));
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
		}
		break;
	}
	CMDFillShape::SetResizeValue(pValues);
}

void CMDCircle::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{

	CString strIDName, strBuffer;
	short		vIdx1, vIdx2;
	LongPoint	vVertexPt;
	double		vRadius;
	short		vPenColIdx;
	short		vDashIdx = GetPenModel().Dashing();

  	findStrFromTab (kDxfCircle, strIDName, kEntitiesTab);

  	vIdx1 = kEntryStart;
  	vIdx2 = kLayerName;

  	/* Write the Shape name and the Layer name */
	strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", vIdx1, strIDName, vIdx2, pLayerStr);
	pFile->Write(strBuffer, strBuffer.GetLength());

	/* Write the Pen Color */
  	vIdx1 = kColorNumber;
  	vPenColIdx = DxfGetColorIndex (&GetPenModel(), ptrPalette);
	strBuffer.Format("%3d\r\n%3d\r\n", vIdx1, vPenColIdx);
	pFile->Write(strBuffer, strBuffer.GetLength());

 	/* Write the center of the circle */
	LongPoint vCenterPt = GetCenter();
  	vVertexPt.x = vCenterPt.x - pDocExtPtr->left;
  	vVertexPt.y = pDocExtPtr->bottom - vCenterPt.y;

  	DxfWritePtCord (pFile, pLayerStr, &vVertexPt, &vUnitFactor, vDashIdx);

  	/* Write the radius of circle */
  	vIdx1 = kOtherFloatValMin;
  	vRadius = mtFixToReal(GetRadii().x) / vUnitFactor;
	strBuffer.Format("%3d\r\n%.6f\r\n", vIdx1, vRadius);
	pFile->Write(strBuffer, strBuffer.GetLength());
}

int CMDCircle::SaveMacDraft(PhIOContext& ctx)
{
	PhDSCircle ps;
	ps.fID=PhIDCircle;
	ctx.pPhShape=&ps;
	CMDFillShape::SaveMacDraft(ctx);

	uptr p((char*)&ps);
	p.swapDSShape();
	p.swaps(2); // fm

	ps.fCenter.fFlags=kPhRealVertex;
	ps.fCenter.fLongPt=GetCenter();
	ps.fCenter.fUse=0;
	ps.fRadius=GetRadii().x;
	switch(m_nType)
	{
	case kRadius:
		ps.fMethod=kPhCenterRadius;
		ps.fPoint[0].fFlags=kPhRealVertex;
		ps.fPoint[0].fLongPt=m_pt[1];
		ps.fPoint[0].fUse=0;
		break;
	case kDiameter:
		ps.fMethod=kPhDiameter;
		ps.fPoint[0].fFlags=kPhRealVertex;
		ps.fPoint[0].fLongPt=m_pt[0];
		ps.fPoint[0].fUse=0;
		ps.fPoint[1].fFlags=kPhRealVertex;
		ps.fPoint[1].fLongPt=m_pt[1];
		ps.fPoint[1].fUse=0;
		break;
	case kThreePoint:
		ps.fMethod=kPh3Point;
		ps.fPoint[0].fFlags=kPhRealVertex;
		ps.fPoint[0].fLongPt=m_pt[0];
		ps.fPoint[0].fUse=0;
		ps.fPoint[1].fFlags=kPhRealVertex;
		ps.fPoint[1].fLongPt=m_pt[1];
		ps.fPoint[1].fUse=0;
		ps.fPoint[2].fFlags=kPhRealVertex;
		ps.fPoint[2].fLongPt=m_pt[2];
		ps.fPoint[2].fUse=0;
		break;
	}

	p.skip(2);
	p.skip(2); p.swaplpt(1);	// DBPoint
	p.swapl(1);
	p.skip(2); p.swaplpt(1);	// DBPoint
	p.skip(2); p.swaplpt(1);	// DBPoint
	p.skip(2); p.swaplpt(1);	// DBPoint

	ushort count=sizeof(PhDSCircle)<<8;	// swapped
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSCircle));
	SaveMacObjInfo(ctx);
	return 1;
}

void CMDCircle::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
	if(kRadius!=m_nType)
		return;
	int nHandle=TestHandles(pt,pMapper);
	if(0==nHandle)
	{
		vertexes.Add(0);
		shapes.Add(this);
	}
}
