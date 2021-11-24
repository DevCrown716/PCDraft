#include "stdafx.h"
#include "MDRectangle.h"
#include "MDLineShape.h"
#include "MDPolygonShape.h"
#include "MDText.h"

#include "phmath.h"
#include "gu.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "PCDraftView.h"
#include "LinkVertex.h"
#include "dxfutil.h"
#include "Edge.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDRectangle,CMDFillShape,1)

//////////////////////////////////////////////////////////////////////

CMDRectangle::CMDRectangle(RectangleType nType) : m_nType(nType)
{
}

CMDRectangle::CMDRectangle() : m_nType(kRectDiagonal)
{
}

void CMDRectangle::Serialize(CArchive& ar)
{
	int i;

	CMDFillShape::Serialize(ar);
	
	if(ar.IsLoading())
	{
		ar >> i;
		m_nType=RectangleType(i);
		for(i=0;i<4;i++)
			ar >> m_ptCorner[i];
	}
	else
	{
      i = m_nType;
		ar << i;
		for(i=0;i<4;i++)
			ar << m_ptCorner[i];
	}
}

LongPoint CMDRectangle::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<4);
	return m_ptCorner[nIndex];
}

void CMDRectangle::SetShapePoint(int nIndex, const LongPoint &pt)
{
	ASSERT(nIndex>=0 && nIndex<4);
	m_ptCorner[nIndex]=pt;
}

LongPoint CMDRectangle::GetShapeHandle(int nIndex) const
{
	if(nIndex<4) return CMDShape::GetShapeHandle(nIndex);
	ASSERT(nIndex>=4 && nIndex<8);
	return Average(m_ptCorner[nIndex%4],m_ptCorner[(nIndex+1)%4]);
}

CEdge* CMDRectangle::GetShapeEdge(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapeEdges());
	return new CEdge(GetShapePoint(nIndex),GetShapePoint((nIndex+1)%4));
}

void CMDRectangle::DrawShape(CDraftContext* pDC) const
{

   CMDFillShape::DrawFill(pDC);

	pDC->SetPen(m_PenModel);

	DrawShapeOutline(pDC);

}

void CMDRectangle::DrawShapeOutline(CDraftContext* pDC) const
{
	
   pDC->Rectangle(GetShapePoint(0),GetShapePoint(2),m_RotAngle);
   
   if (pDC->path())
   {

      pDC->path()->StartFigure();

   }

   if (pDC->m_DrawPrim.m_helperPoly.has_points())
   {
    
      pDC->m_DrawPrim.m_helperPoly.DrawNow(pDC);
      pDC->m_DrawPrim.m_helperPoly.Reset();

   }

}


bool CMDRectangle::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
	pointd pt1=pMapper->LPtoDPF(GetShapePoint(0));
	pointd pt2=pMapper->LPtoDPF(GetShapePoint(1));
	bool result=guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment);
	pt1=pt2; pt2=pMapper->LPtoDPF(GetShapePoint(2));
	result=result || guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment);
	pt1=pt2; pt2=pMapper->LPtoDPF(GetShapePoint(3));
	result=result || guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment);
	pt1=pt2; pt2=pMapper->LPtoDPF(GetShapePoint(0));
	result=result || guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment);
	return result;
}

void CMDRectangle::Unglue(CMDShapeList* pList) const
{
	for(int i=1;i<=4;i++)
	{
		CMDShape* pShape=new CMDLineShape(GetShapePoint(i-1),GetShapePoint(i%4));
		pShape->SetPenModel(m_PenModel);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pShape);
	}
}

void CMDRectangle::AdjustScaleToRect(const LongRect& lrcFrom,LongRect& lrcTo)
{
	if((m_RotAngle%90)!=FixedZero)
	{
		LongPoint size=lrcFrom.GetSize();
		LongPoint delta=lrcTo.GetSize();
		bool bSignX=(delta.x<FixedZero);
		bool bSignY=(delta.y<FixedZero);
		delta=Abs(delta);
		double xRatio=delta.x.GetRawDouble()/size.x.GetRawDouble();
		double yRatio=delta.y.GetRawDouble()/size.y.GetRawDouble();
		if(Abs(xRatio-1)>Abs(yRatio-1)) delta.y=delta.x*(size.y.GetRawDouble()/size.x.GetRawDouble());
		else delta.x=delta.y*(size.x.GetRawDouble()/size.y.GetRawDouble());
		if(bSignX) delta.x=-delta.x;
		if(bSignY) delta.y=-delta.y;

		lrcTo.right=lrcTo.left+delta.x;
		lrcTo.bottom=lrcTo.top+delta.y;
	}
}

void CMDRectangle::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
	LongPoint ratio=lrcTo.GetSize()/lrcFrom.GetSize();
	CMDShape* pShape;
	if(ratio.x!=ratio.y && (m_RotAngle%90)!=FixedZero && CanAddPt(&pShape))
	{
		pShape->SetShapeDatum(GetShapeDatum());
		pShape->DoShapeScale(lrcFrom,lrcTo,ppNewShape);
		if(m_pAttached)
		{
			pShape->Attach(m_pAttached);
			m_pAttached=NULL;
		}
		gReplacedShapes.SetAt(this,pShape);
		*ppNewShape=pShape;
	}
	else
	{
		LongRect lrc=lrcTo;
		AdjustScaleToRect(lrcFrom,lrc);
		CMDFillShape::DoShapeScale(lrcFrom,lrc,ppNewShape);
		NormalizePoints(m_ptCorner);
	}
}

void CMDRectangle::DoShapeMirror(const LongPoint &lptRef1, const LongPoint &lptRef2)
{
	CMDFillShape::DoShapeMirror(lptRef1,lptRef2);
	while(m_RotAngle>=180) m_RotAngle-=180;
	NormalizePoints(m_ptCorner);
}

void CMDRectangle::DoShapeFlip(bool bVertical,LongPoint* pLpt)
{
	CMDFillShape::DoShapeFlip(bVertical,pLpt);
	NormalizePoints(m_ptCorner);
}

CTrackContext* CMDRectangle::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CRectTrackContext* p=new CRectTrackContext;
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
	}
	return p;
}

void CMDRectangle::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CRectTrackContext* p=(CRectTrackContext*)pTrackContext;
	int nHandle=p->nWhichHandle;
	if(nHandle==kMoveHandle)
	{
		for(int i=0;i<4;i++) p->pt[i]+=pt;
	}
	else
	{
		LongPoint delta=pt - p->pt[nHandle % 4];
		if(m_RotAngle!=0)
		{
			delta=mtRotPoint(delta,-m_RotAngle);
			for(int i=0;i<4;i++)
				p->pt[i]=mtRotPoint(p->pt[i],-m_RotAngle);
		}
		if(nHandle<4 && p->bShift)
		{
			LongPoint pt0=GetShapePoint(0),pt2=GetShapePoint(2);
			if(m_RotAngle!=0)
			{
				pt0=mtRotPoint(pt0,-m_RotAngle);
				pt2=mtRotPoint(pt2,-m_RotAngle);
			}
			LongRect lrcExtents(pt0,pt2);
			LongPoint size=lrcExtents.GetSize();
			LongPoint origin=(m_nType==kRectCenter)?Middle(lrcExtents):lrcExtents.GetCorner(nHandle^2);
			LongPoint delta2=pt-origin;
			bool bNegX=delta2.x<0;
			bool bNegY=delta2.y<0;
			delta2=Abs(delta2);
			double xRatio=delta2.x.GetRawDouble()/size.x.GetRawDouble();
			double yRatio=delta2.y.GetRawDouble()/size.y.GetRawDouble();
			if(xRatio<yRatio) delta2.y=delta2.x*(size.y.GetRawDouble()/size.x.GetRawDouble());
			else delta2.x=delta2.y*(size.x.GetRawDouble()/size.y.GetRawDouble());
			if(bNegX) delta2.x=-delta2.x;
			if(bNegY) delta2.y=-delta2.y;
			delta=delta2-p->pt[nHandle]+origin;
		}

		LongPoint delta0=LongPoint::kZeroPt;
		LongPoint delta2=LongPoint::kZeroPt;

		switch(nHandle)
		{
		case 0:
			delta0=delta;
			if(m_nType==kRectCenter) delta2=-delta;
			break;
		case 1:
			delta0.y=delta.y; delta2.x=delta.x;
			if(m_nType==kRectCenter)
				delta0.x=-delta.x, delta2.y=-delta.y;
			break;
		case 2:
			delta2=delta;
			if(m_nType==kRectCenter) delta0=-delta;
			break;
		case 3:
			delta0.x=delta.x; delta2.y=delta.y;
			if(m_nType==kRectCenter)
				delta0.y=-delta.y, delta2.x=-delta.x;
			break;
		case 4:
			delta0.y=delta.y;
			if(m_nType==kRectCenter) delta2.y=-delta.y;
			break;
		case 5:
			delta2.x=delta.x;
			if(m_nType==kRectCenter) delta0.x=-delta.x;
			break;
		case 6:
			delta2.y=delta.y;
			if(m_nType==kRectCenter) delta0.y=-delta.y;
			break;
		case 7:
			delta0.x=delta.x;
			if(m_nType==kRectCenter) delta2.x=-delta.x;
			break;
		}
		p->pt[0]+=delta0; p->pt[2]+=delta2;
		p->pt[3].x=p->pt[0].x;
		p->pt[1].y=p->pt[0].y;
		p->pt[1].x=p->pt[2].x;
		p->pt[3].y=p->pt[2].y;

		if(m_RotAngle!=0)
		{
			for(int i=0;i<4;i++)
				p->pt[i]=mtRotPoint(p->pt[i],m_RotAngle);
		}
	}
}

void CMDRectangle::TrackShowSize(CTrackContext* pTrackContext)
{
	CRectTrackContext* p=(CRectTrackContext*)pTrackContext;
	p->pView->SetShowSize(0,1,Pythag(p->pt[1]-p->pt[0]));
	p->pView->SetShowSize(1,2,Pythag(p->pt[2]-p->pt[1]));
}

void CMDRectangle::TrackComplete(CTrackContext* pTrackContext)
{
	CRectTrackContext* p=(CRectTrackContext*)pTrackContext;
	int i;
	for(i=0;i<4;i++)
		p->pt[i]=mtRotPoint(p->pt[i],-m_RotAngle/*,p->pt[0]*/);
	LongRect rc;
	rc.Set(p->pt[0],p->pt[2]);
//	rc.Normalize();
	{
		CLinkVertexList& list=p->pView->GetActiveLinkVertexList();
		WrFixed tmp;
		if (rc.left > rc.right)
		{
			tmp = rc.left;
			rc.left = rc.right;
			rc.right = tmp;
			list.SwapShapeVertex(this,0,1);
			list.SwapShapeVertex(this,2,3);
		}
		if (rc.top > rc.bottom)
		{
			tmp = rc.top;
			rc.top = rc.bottom;
			rc.bottom = tmp;
			list.SwapShapeVertex(this,0,3);
			list.SwapShapeVertex(this,1,2);
		}
	}

	for(i=0;i<4;i++)
		p->pt[i]=mtRotPoint(rc.GetCorner(i),m_RotAngle/*,rc.GetCorner(0)*/);
	for(i=0;i<4;i++)
		SetShapePoint(i,p->pt[i]);
	CalculateExtents();
}

void CMDRectangle::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CRectTrackContext* p=(CRectTrackContext*)pTrackContext;
//	pDC->Rectangle(p->pt[0],p->pt[2],);
	pDC->MoveTo(p->pt[0]);
	for(int i=1;i<4;i++)
		pDC->LineTo(p->pt[i]);
	pDC->LineTo(p->pt[0]);

}

double CMDRectangle::GetRawArea(const LongRatio& ratio) const
{
	return ReportCalcWidth(ratio)*ReportCalcHeight(ratio);
}

double CMDRectangle::ReportCalcWidth(const LongRatio& ratio) const
{
	return ScaleDist(ratio,Pythag(GetShapePoint(0)-GetShapePoint(1)));
}

double CMDRectangle::ReportCalcHeight(const LongRatio& ratio) const
{
	return ScaleDist(ratio,Pythag(GetShapePoint(1)-GetShapePoint(2)));
}

double CMDRectangle::ReportCalcLength(const LongRatio& ratio) const
{
	double width=ReportCalcWidth(ratio);
	double height=ReportCalcHeight(ratio);
	return width>height?width:height;
}

double CMDRectangle::ReportCalcPerimeter(const LongRatio& ratio) const
{
	return 2*ReportCalcWidth(ratio)+2*ReportCalcHeight(ratio);
}

const ResizeInfo* CMDRectangle::GetResizeInfo() const
{
static ResizeInfo rectResize =
{
	IDS_RECTANGLE, 2, {IDS_WIDTH,kUnitlessValue, IDS_HEIGHT,kUnitlessValue}
};
	return &rectResize;
}

void CMDRectangle::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	CMDFillShape::GetResizeValue(pValues,nHandle);
	pValues[0].m_Unitless=Pythag(GetShapePoint(0)-GetShapePoint(1));
	pValues[1].m_Unitless=Pythag(GetShapePoint(1)-GetShapePoint(2));
}

void CMDRectangle::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	if(nHandle<0 || nHandle>7) nHandle=2;
	if(nHandle>3)
	{
		if(nHandle==4 || nHandle==6) pValues[0].m_Unitless=Pythag(GetShapePoint(0)-GetShapePoint(1));
		else pValues[1].m_Unitless=Pythag(GetShapePoint(1)-GetShapePoint(2));
		if(nHandle==5 || nHandle==6) nHandle=0;
		else nHandle=2;
	}
	else
	{
		if(nHandle<2) nHandle+=2;
		else nHandle-=2;
	}

	LongPoint pt,ptRef;
	if(m_nType==kRectCenter)
	{
		ptRef=Average(GetShapePoint(0),GetShapePoint(2));
		pt.x=ptRef.x-(pValues[0].m_Unitless>>1);
		pt.y=ptRef.y-(pValues[1].m_Unitless>>1);
		ptRef=mtRotPoint(pt,m_RotAngle,ptRef);
		SetShapePoint(0,ptRef);
		nHandle=0;
	}
	else ptRef=GetShapePoint(nHandle);
	LongRect lrc;
	switch(nHandle)
	{
	case 0:
		lrc.left=ptRef.x;
		lrc.top=ptRef.y;
		lrc.right=ptRef.x+pValues[0].m_Unitless;
		lrc.bottom=ptRef.y+pValues[1].m_Unitless;
		break;
	case 1:
		lrc.left=ptRef.x-pValues[0].m_Unitless;
		lrc.top=ptRef.y;
		lrc.right=ptRef.x;
		lrc.bottom=ptRef.y+pValues[1].m_Unitless;
		break;
	case 2:
		lrc.left=ptRef.x-pValues[0].m_Unitless;
		lrc.top=ptRef.y-pValues[1].m_Unitless;
		lrc.right=ptRef.x;
		lrc.bottom=ptRef.y;
		break;
	case 3:
		lrc.left=ptRef.x;
		lrc.top=ptRef.y-pValues[1].m_Unitless;
		lrc.right=ptRef.x+pValues[0].m_Unitless;
		lrc.bottom=ptRef.y;
		break;
	}

	for(int i=0;i<4;i++)
	{
		SetShapePoint(i,mtRotPoint(lrc.GetCorner(i),m_RotAngle,ptRef));
	}

	CMDFillShape::SetResizeValue(pValues);
}

void CMDRectangle::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
//   	CMDRectangle*	vShPtr;
	CString			strIDName, strBuffer;
	short			vIdx1, vIdx2;
	LongPoint		vVertexPt;
	double			vLnWidth;
	short			vClosed, vPenColIdx;
	short			vDashIdx;


	vDashIdx = GetPenModel().Dashing();
  	findStrFromTab (kDxfPolyline, strIDName, kEntitiesTab);

  	vIdx1 = kEntryStart;
  	vIdx2 = kLayerName;

  	/* Write the Shape name and the Layer name */
	strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", vIdx1, strIDName, vIdx2, pLayerStr);
	pFile->Write(strBuffer, strBuffer.GetLength());
	
  	/* Custom Line Style */
  	if (vDashIdx) {
  		CString strDashName;

		findStrFromTab (kLTYPE, strDashName, kTablesTab);
		strBuffer.Format("%3d\r\n%s%02d\r\n", kLineTypeName, strDashName, vDashIdx);
		pFile->Write(strBuffer, strBuffer.GetLength());
  	}

  	/* Write the Vertices follow flag and Polyline flag */
  	vIdx1 = kEntitiesFollow;
  	vIdx2 = kOtherIntValMin;
  	vClosed = 1;
  	strBuffer.Format("%3d\r\n%3d\r\n%3d\r\n%3d\r\n", vIdx1, 1, vIdx2, vClosed);
	pFile->Write(strBuffer, strBuffer.GetLength());

  	/* Write the Pen Color */
  	vIdx1 = kColorNumber;
  	vPenColIdx = DxfGetColorIndex (&GetPenModel(), ptrPalette);
 	strBuffer.Format("%3d\r\n%3d\r\n", vIdx1, vPenColIdx);
	pFile->Write(strBuffer, strBuffer.GetLength());

  	/* Write the Pen thickness - starting width and the ending width */
  	/* Need to write it if it has line weight > 1 & is not hairline. */
  	if (!(GetPenModel().m_nStyle & HairlineMask) && GetPenModel().m_nWidth > 1) {
		vIdx1 = kOtherFloatValMin;
		vIdx2 = kFlatVal2;
		vLnWidth = (double)(DxfWidthConversion(GetPenModel().m_nWidth)) / vUnitFactor;
		strBuffer.Format("%3d\r\n%.6f\r\n%3d\r\n%.6f\r\n", vIdx1, vLnWidth, vIdx2, vLnWidth);
		pFile->Write(strBuffer, strBuffer.GetLength());
  	}

  	/* Write the vertices of the rectangle */
	for (int i=0; i<4; i++) {
  		vVertexPt.x = GetShapePoint(i).x - pDocExtPtr->left;
  		vVertexPt.y = pDocExtPtr->bottom - GetShapePoint(i).y;
  		DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
				   		&vUnitFactor, 0L, 0, vDashIdx);
	}

	/* Write the Seqend entity */
  	vIdx1 = kEntryStart;
  	vIdx2 = kLayerName;
  	findStrFromTab (kDxfSeqEnd, strIDName, kEntitiesTab);
  	strBuffer.Format("%3d\r\n%s\r\n%3d\r\n%s\r\n", vIdx1, strIDName, vIdx2, pLayerStr);
  	pFile->Write(strBuffer, strBuffer.GetLength());

} /* writeRectShape */

bool CMDRectangle::CanAddPt(CMDShape **ppNewShape) const
{
	CMDShape* pShape=new CMDPolygonShape(CMDPolygonShape::kPolygon,4,true);
	for(int i=0;i<4;i++) pShape->SetShapePoint(i,GetShapePoint(i));
	pShape->SetPenModel(GetPenModel());
	pShape->SetFillModel(GetFillModel());
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	*ppNewShape=pShape;
	return true;
}

void CMDRectangle::RemovePt(int nHandle, CMDShape** ppNewShape)
{
	if(nHandle>=0 && nHandle<4)
	{
		CMDPolygonShape *pPoly=new CMDPolygonShape(CMDPolygonShape::kPolygon,3,true);
		for(int i=0;i<3;i++) pPoly->SetShapePoint(i,GetShapePoint((nHandle+i+1)%4));
		pPoly->SetPenModel(GetPenModel());
		pPoly->SetFillModel(GetFillModel());
		pPoly->CalculateExtents();
		pPoly->SetShapeDatum(kDefaultDatum);
		*ppNewShape=pPoly;
	}
}

int CMDRectangle::SaveMacDraft(PhIOContext& ctx)
{
	PhDSRectangle ps;
	ps.fID=(m_nType==kRectCenter)?PhIDRectCenter:PhIDRectDiagonal;
	ctx.pPhShape=&ps;
	CMDFillShape::SaveMacDraft(ctx);

	uptr p((char*)&ps);
	p.swapDSShape();
	p.swaps(2); // fm
	for(int i=0;i<4;i++)
	{
		ps.fPoint[i].fFlags=kPhRealVertex;
		ps.fPoint[i].fLongPt=m_ptCorner[i];
		ps.fPoint[i].fUse=0;
		p.skip(2); p.swaplpt(1);	// DBPoint
	}

   ushort count=sizeof(PhDSRectangle) << 8;	// swapped
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSRectangle));
	SaveMacObjInfo(ctx);
	return 1;
}




void CMDRectangle::OnUpdateLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex)
{
	if(pCauseShape->IsKindOf(RUNTIME_CLASS(CMDText)))
	{ // get rotation if linked to text
		SetRotation(pCauseShape->GetRotation());
	}
	bool bCenter=(kRectCenter==m_nType);
	if(bCenter) m_nType=kRectDiagonal;
	CMDFillShape::OnUpdateLinkedVertex(pView,nVertex,pCauseShape,nCauseVertex);
	if(bCenter) m_nType=kRectCenter;
}

void CMDRectangle::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
	int nVertex=TestHandles(pt,pMapper);
	if(nVertex>=0 && nVertex<4)
	{
		vertexes.Add(nVertex);
		shapes.Add(this);
	}

}

