#include "stdafx.h"
#include "PCDraft.h"
#include "MDEllipse.h"
#include "MDCircle.h"

#include "Mapper.h"
#include "DraftContext.h"
#include "ArcEdge.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDEllipse,CMDRectangle,1)

//////////////////////////////////////////////////////////////////////
/*
bool CMDEllipse::GetFillRgn(CRgn* pRgn, CDraftContext* pDC) const
{
	pointd pt0=pDC->GetMapper()->LPtoDP(GetShapePoint(0));
	pointd pt2=pDC->GetMapper()->LPtoDP(GetShapePoint(2));
	return pRgn->CreateEllipticRgn(pt0.x,pt0.y,pt2.x,pt2.y)?true:false;
}
*/
LongPoint CMDEllipse::GetCenter() const
{
	LongPoint lptCenter=LongPoint::kZeroPt;
	for(int i=0;i<4;i++)
		lptCenter+=GetShapePoint(i);
	return (lptCenter>>2);
}

LongPoint CMDEllipse::GetRadii() const
{
	return LongPoint(Pythag(GetShapePoint(1)-GetShapePoint(0))>>1,Pythag(GetShapePoint(2)-GetShapePoint(1))>>1);
}

CEdge* CMDEllipse::GetShapeEdge(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapeEdges());
	LongPoint lpt=GetCenter();
	lpt.x+=GetRadii().x;
	return new CArcEdge(GetCenter(),GetRadii(),lpt,lpt,AD_CLOCKWISE,m_RotAngle);
}

void CMDEllipse::DrawShape(CDraftContext* pDC) const
{
	DrawFill(pDC);

	pDC->SetPen(m_PenModel);
	DrawShapeOutline(pDC);
}

void CMDEllipse::DrawShapeOutline(CDraftContext* pDC) const
{
	
   pDC->Ellipse(GetCenter(),GetRadii(),m_RotAngle);
   
}

bool CMDEllipse::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
	LongPoint lptRadii=GetRadii();
	return guPtAtEllip(pt,pMapper->LPtoDPF(GetCenter()),(short)pMapper->LPtoDP(lptRadii.x),(short)pMapper->LPtoDP(lptRadii.y),m_RotAngle,nWidth,Alignment(m_PenModel.m_nAlignment),false);
}

CTrackContext* CMDEllipse::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CEllipseTrackContext* p=new CEllipseTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	for(int i=0;i<4;i++)
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

		for(int i=0;i<4;i++)
			p->pt[i]=mtRotPoint(p->pt[i], pCtx->m_Angle, lptRot);
		p->rotAngle=m_RotAngle+pCtx->m_Angle;
	}
	else p->rotAngle=m_RotAngle;
	return p;
}

void CMDEllipse::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CEllipseTrackContext* p=(CEllipseTrackContext*)pTrackContext;

	LongPoint center=LongPoint::kZeroPt;
	for(int i=0;i<4;i++) center+=p->pt[i];;
	center>>=2;
	LongPoint radii=LongPoint(Pythag(p->pt[1]-p->pt[0])>>1,Pythag(p->pt[2]-p->pt[1])>>1);
	pDC->Ellipse(center,radii,p->rotAngle);
//	pDC->ArcDash(&center,radii.x,radii.y,0,360,p->rotAngle);
}

double CMDEllipse::GetRawArea(const LongRatio& ratio) const
{
	LongPoint radii=GetRadii();
	return ScaleDist(ratio,GetRadii().x)*ScaleDist(ratio,GetRadii().y)*pi;
}

double CMDEllipse::ReportCalcPerimeter(const LongRatio& ratio) const
{
	LongPoint radii=GetRadii();
	double a=mtFixToReal(radii.x);
	double b=mtFixToReal(radii.y);
	return ScaleDist(ratio,pi*sqrt(2*(a*a+b*b)));
}

const ResizeInfo* CMDEllipse::GetResizeInfo() const
{
static ResizeInfo ellipseResize =
{
	IDS_ELLIPSE, 2, {IDS_WIDTH,kUnitlessValue, IDS_HEIGHT,kUnitlessValue}
};
	return &ellipseResize;
}

void CMDEllipse::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
	LongPoint		vVertexPt;
	short			vDashIdx;
	CArray <LongPoint,LongPoint>	vPtHdl;
	short			vIdx1;
	LongPoint		dbPoint;
	PenModel		aPenModel=GetPenModel();

	// check to see if ellipse can be exported as a circle

	if (Abs(GetRadii().x) == Abs(GetRadii().y))
	{
		
		CMDCircle* pCircle = new CMDCircle(CMDCircle::kRadius);
		if (pCircle == 0L)
		{
			TRACE0("Error : insufficient memory \n");
			AfxThrowUserException();
		}

		LongPoint pt, ptCenterPoint;

		ptCenterPoint = GetCenter();
		pt.x = ptCenterPoint.x;
		pt.y = ptCenterPoint.y + Abs(GetRadii().x);

		pCircle->SetShapePoint(0, ptCenterPoint);
		pCircle->SetShapePoint(1, pt);
		
		pCircle->SetPenModel(GetPenModel());
		pCircle->CalculateExtents();
		pCircle->WriteDXF(pFile, pLayerStr,	pDocExtPtr,	vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);

		delete pCircle;

	}
	else
	{
		dbPoint = GetCenter () ;

		DxfGetArcPts (vPtHdl, dbPoint, Abs(GetRadii().x), Abs(GetRadii().y), 0, k360,
				   GetRotation(), true, &aPenModel);

		if (!vPtHdl.GetSize())
			return;
		vDashIdx = aPenModel.Dashing();

  		DxfWritePolyHead (pFile, pLayerStr, &aPenModel, pDocExtPtr, vUnitFactor, 0, ptrPalette);

 		// Write the vertices of the POINT 
  		vVertexPt.x = vPtHdl[0].x - pDocExtPtr->left;
  		vVertexPt.y = pDocExtPtr->bottom - vPtHdl[0].y;
  		DxfWritePtCord (pFile, pLayerStr, &vVertexPt, &vUnitFactor, vDashIdx);

  		for (vIdx1 = 0; vIdx1 < vPtHdl.GetSize(); vIdx1++)
		{
			// Write the vertices 
			vVertexPt.x = vPtHdl[vIdx1].x - pDocExtPtr->left;
			vVertexPt.y = pDocExtPtr->bottom - vPtHdl[vIdx1].y;
			DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
							 &vUnitFactor, 0L, 0, vDashIdx);
  		} // for vIdx1 

 		DxfWriteSeqend (pFile, pLayerStr);
	}
}

int CMDEllipse::SaveMacDraft(PhIOContext& ctx)
{
	PhDSEllipse ps;
	ps.fID=PhIDEllipse;
	ctx.pPhShape=&ps;
	CMDFillShape::SaveMacDraft(ctx);

	uptr p((char*)&ps);
	p.swapDSShape();
	p.swaps(2); // fm

	ps.fMethod=(m_nType==kRectCenter)?kPhCenterRadius:kPhDiameter;
	ps.fDummy=0;
	ps.fCenter.fUse=0;
	ps.fCenter.fFlags=kPhRealVertex;
	ps.fCenter.fLongPt=GetCenter();
	LongPoint lptRadii=GetRadii();
	ps.fHRadius=lptRadii.x;
	ps.fVRadius=lptRadii.y;

	p.skip(2);
	p.skip(2); p.swaplpt(1);	// DBPoint
	p.swapl(2);

	ushort count=sizeof(PhDSEllipse)<<8;	// swapped
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSEllipse));
	SaveMacObjInfo(ctx);
	return 1;
}

void CMDEllipse::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
}
