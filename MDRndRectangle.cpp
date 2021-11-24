// MDRndRectangle.cpp: implementation of the CMDRndRectangle class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDRndRectangle.h"

#include "pcdraftview.h"
#include "DraftContext.h"
#include "MDLineShape.h"
#include "MDArcShape.h"
#include "ArcEdge.h"
#include "BezierEdge.h"
#include "PhIOStruct.h"

#include "phmath.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDRndRectangle,CMDRectangle,1)

//////////////////////////////////////////////////////////////////////

CMDRndRectangle::CMDRndRectangle() : CMDRectangle(CMDRectangle::kRectDiagonal)
{
}

CMDRndRectangle::CMDRndRectangle(CornerType nCornerType) :
	CMDRectangle(CMDRectangle::kRectDiagonal), m_nCornerType(nCornerType)
{
}

void CMDRndRectangle::Serialize(CArchive& ar)
{
	CMDRectangle::Serialize(ar);
	if(ar.IsLoading())
		ar >> m_nCornerType >> m_lptCornerDelta;
	else
		ar << m_nCornerType << m_lptCornerDelta;
}

LongPoint CMDRndRectangle::GetShapeHandle(int nIndex) const
{
	if(nIndex<8) return CMDRectangle::GetShapeHandle(nIndex);
	ASSERT(nIndex==8);
	LongPoint lptDelta=mtRotPoint(m_lptCornerDelta,m_RotAngle);
	return GetShapePoint(0)+lptDelta;
}

CEdge* CMDRndRectangle::GetShapeEdge(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapeEdges());

	LongPoint lptDelta=m_lptCornerDelta;
	LongPoint pt[12];
	CalcRotPoints(m_ptCorner,lptDelta,m_RotAngle,pt);

	if(nIndex&1)	// arcs
	{
		nIndex=((nIndex>>1)+1)*3;
		return new CArcEdge(pt[nIndex-1],lptDelta,pt[nIndex-2],pt[nIndex%12],AD_CLOCKWISE,m_nCornerType==kCornerElliptical?m_RotAngle:0);
	}
	else			// lines
	{
		nIndex=(nIndex>>1)*3;
		return new CEdge(pt[nIndex],pt[nIndex+1]);
	}
}

void CMDRndRectangle::DrawShape(CDraftContext* pDC) const
{
	DrawFill(pDC);
	pDC->SetPen(m_PenModel);
   if (pDC->shouldUseDash())
   {
      pDC->m_DrawPrim.m_bUsePolyHelperIfDashed = true;
      DrawShapeOutline(pDC);
      pDC->m_DrawPrim.m_bUsePolyHelperIfDashed = false;
   }
   else
   {
      UsePath usepath(pDC);
      pDC->SetOutlinePen();
      DrawShapeOutline(pDC);
      pDC->SetOutlinePen(false);
      UseRect userect(pDC, (CMDShape *)this);

      pDC->SetPen(m_PenModel);

      //pDC->PaintRgn(&rgn); comment for GDI+

      //pDC->PaintPenPath();/

      pDC->graphics()->DrawPath(pDC->m_PenPlus.get(), pDC->path());


   }
}

void CMDRndRectangle::DrawShapeOutline(CDraftContext* pDC) const
{
	LongPoint lptDelta=m_lptCornerDelta;
	LongPoint lptDiag=Abs(mtRotPoint(GetShapePoint(2)-GetShapePoint(0),-m_RotAngle));
	AdjustCornerDelta(lptDelta,lptDiag);

	pDC->RoundRect(GetShapePoint(0),GetShapePoint(2),lptDelta,m_RotAngle);
}

bool CMDRndRectangle::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	bool bFound=false;
	CEdge* pEdge=NULL;
	for(int i=0;i<GetNumShapeEdges();i++)
	{
		pEdge=GetShapeEdge(i);
		bFound=pEdge->PtAtEdge(pt,pMapper);
		delete pEdge;
		pEdge=NULL;
		if(bFound) break;
	}
	return bFound;
}

void CMDRndRectangle::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
	LongRect lrc=lrcTo;
	AdjustScaleToRect(lrcFrom,lrc);

	LongRatio xRatio,yRatio;
	xRatio.fN=(lrc.right-lrc.left).GetFixed();
	xRatio.fD=(lrcFrom.right-lrcFrom.left).GetFixed();
	yRatio.fN=(lrc.bottom-lrc.top).GetFixed();
	yRatio.fD=(lrcFrom.bottom-lrcFrom.top).GetFixed();

	LongPoint lpt;
	lpt.x=Abs(m_lptCornerDelta.x*xRatio);
	lpt.y=Abs(m_lptCornerDelta.x*yRatio);
	if(lpt.x==lpt.y) m_lptCornerDelta.x=lpt.x;
	else m_lptCornerDelta.x.SetFromRawDoubleTruncated(sqrt(lpt.x.GetRawDouble()*lpt.y.GetRawDouble()));

	lpt.x=Abs(m_lptCornerDelta.y*xRatio);
	lpt.y=Abs(m_lptCornerDelta.y*yRatio);
	if(lpt.x==lpt.y) m_lptCornerDelta.y=lpt.y;
	else m_lptCornerDelta.y.SetFromRawDoubleTruncated(sqrt(lpt.x.GetRawDouble()*lpt.y.GetRawDouble()));

	CMDFillShape::DoShapeScale(lrcFrom,lrc,ppNewShape);
	NormalizePoints(m_ptCorner);

	LongPoint lptDiag=Abs(mtRotPoint(GetShapePoint(2)-GetShapePoint(0),-m_RotAngle));
	AdjustCornerDelta(m_lptCornerDelta,lptDiag);
}

void CMDRndRectangle::DoShapeScale(const LongRatio& ratio,double factor)
{
	if(factor) m_lptCornerDelta*=factor;
	m_lptCornerDelta*=ratio;
	CMDFillShape::DoShapeScale(ratio,factor);
}

void CMDRndRectangle::Unglue(CMDShapeList* pList) const
{
	for (int i=0; i<GetNumShapeEdges(); i++) {
		CEdge* pEdge=GetShapeEdge(i);
		CMDShape* pShape=pEdge->CreateShape();
		delete pEdge;
		pShape->SetPenModel(m_PenModel);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pShape);
	}
}

void CMDRndRectangle::AdjustCornerDelta(LongPoint &lptDelta,const LongPoint& lptDiag) const
{
	if(lptDelta.x<=0) lptDelta.x=1;
	else
	{
		Coord maxDeltaX=lptDiag.x>>1;
		if(lptDelta.x>maxDeltaX) lptDelta.x=maxDeltaX;
	}
	if(lptDelta.y<=0) lptDelta.y=1;
	else
	{
		Coord maxDeltaY=lptDiag.y>>1;
		if(lptDelta.y>maxDeltaY) lptDelta.y=maxDeltaY;
	}
	if(m_nCornerType!=kCornerElliptical) lptDelta.x=lptDelta.y=Min(lptDelta.x,lptDelta.y);
}

CTrackContext* CMDRndRectangle::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CRndRectTrackContext* p=new CRndRectTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	for(int i=0;i<4;i++)
		p->pt[i]=GetShapePoint(i);
	p->ptCorner=m_lptCornerDelta;

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

void CMDRndRectangle::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CRndRectTrackContext* p=(CRndRectTrackContext*)pTrackContext;
	if(pTrackContext->nWhichHandle==8)
	{
		p->ptCorner=mtRotPoint(pt-p->pt[0],-p->rotAngle);
		LongPoint lptDiag=Abs(mtRotPoint(p->pt[2]-p->pt[0],-p->rotAngle));
		AdjustCornerDelta(p->ptCorner,lptDiag);
	}
	else
	{
		LongPoint lptOldDiag=Abs(mtRotPoint(p->pt[2]-p->pt[0],-p->rotAngle));
		CMDRectangle::TrackFeedback(pt, pTrackContext);
		LongPoint lptDiag=Abs(mtRotPoint(p->pt[2]-p->pt[0],-p->rotAngle));
		if(m_nCornerType==kCornerProportional)
		{
			p->ptCorner.x-=((Min(lptOldDiag.x,lptOldDiag.y)-Min(lptDiag.x,lptDiag.y))>>2);
			p->ptCorner.y=p->ptCorner.x;
		}
		else p->ptCorner=m_lptCornerDelta;
		AdjustCornerDelta(p->ptCorner,lptDiag);
	}
}

void CMDRndRectangle::TrackShowSize(CTrackContext* pTrackContext)
{
	CRndRectTrackContext* p=(CRndRectTrackContext*)pTrackContext;
	p->pView->SetShowSize(0,1,Pythag(p->pt[1]-p->pt[0]));
	p->pView->SetShowSize(2,2,Pythag(p->pt[2]-p->pt[1]));
	if(m_nCornerType==kCornerElliptical)
	{
		p->pView->SetShowSize(1,5,p->ptCorner.x);
		p->pView->SetShowSize(3,6,p->ptCorner.y);
	}
	else p->pView->SetShowSize(1,4,p->ptCorner.x);
}

void CMDRndRectangle::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CRndRectTrackContext* p=(CRndRectTrackContext*)pTrackContext;
	pDC->RoundRect(p->pt[0],p->pt[2],p->ptCorner,p->rotAngle);
}

void CMDRndRectangle::TrackComplete(CTrackContext* pTrackContext)
{
	CRndRectTrackContext* p=(CRndRectTrackContext*)pTrackContext;
	m_lptCornerDelta=p->ptCorner;
	CMDRectangle::TrackComplete(pTrackContext);
}

double CMDRndRectangle::GetRawArea(const LongRatio& ratio) const
{
	double area=CMDRectangle::GetRawArea(ratio);
	double subarea=ScaleDist(ratio,m_lptCornerDelta.x<<1)*ScaleDist(ratio,m_lptCornerDelta.y<<1) * -0.21460183660255169034;	// PI4 - 1.0
	return area+subarea;
}

double CMDRndRectangle::ReportCalcPerimeter(const LongRatio& ratio) const
{
	LongPoint lptCorner=m_lptCornerDelta;
	double a=mtFixToReal(lptCorner.x);
	double b=mtFixToReal(lptCorner.y);
	lptCorner<<=1;
	return 2*ScaleDist(ratio,Pythag(GetShapePoint(0)-GetShapePoint(1))-lptCorner.x)
			+2*ScaleDist(ratio,Pythag(GetShapePoint(1)-GetShapePoint(2))-lptCorner.y)
			+ScaleDist(ratio,pi*sqrt(2*(a*a+b*b)));
}

const ResizeInfo* CMDRndRectangle::GetResizeInfo() const
{
static ResizeInfo rndResize[2] =
{
	{	IDS_RNDRECT,   3, {IDS_WIDTH,kUnitlessValue, IDS_HEIGHT,kUnitlessValue, IDS_R,kUnitlessValue} },
	{	IDS_ELLIP_RECT,4, {IDS_WIDTH,kUnitlessValue, IDS_HEIGHT,kUnitlessValue, IDS_XR,kUnitlessValue, IDS_YR,kUnitlessValue} }
};
	return &rndResize[m_nCornerType==kCornerElliptical?1:0];
}

void CMDRndRectangle::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	CMDRectangle::GetResizeValue(pValues,nHandle);
	pValues[2].m_Unitless=m_lptCornerDelta.x;
	pValues[3].m_Unitless=m_lptCornerDelta.y;
}

void CMDRndRectangle::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	m_lptCornerDelta.x=pValues[2].m_Unitless;
	if(m_nCornerType!=kCornerElliptical) m_lptCornerDelta.y=m_lptCornerDelta.x;
	else m_lptCornerDelta.y=pValues[3].m_Unitless;
	CMDRectangle::SetResizeValue(pValues,nHandle);
	LongPoint lptDiag=Abs(mtRotPoint(GetShapePoint(2)-GetShapePoint(0),-m_RotAngle));
	AdjustCornerDelta(m_lptCornerDelta,lptDiag);
	CalculateExtents();
}

void CMDRndRectangle::CalcRotPoints(const LongPoint pt[], LongPoint ptDelta, Angle nAngle, LongPoint ptRot[])
{
/*
		   r0------1r
		  r          r
0--1     10 11     2 3
|  |  >>  |          |
3--2      9 8      5 4
		  r          r
		   r7------6r
*/
	LongPoint ptDeltaX=Abs(LongPoint(ptDelta.x,0));/*Abs(mtRotPoint(LongPoint(ptDelta.x,0),nAngle));*/
	LongPoint ptDeltaY=Abs(LongPoint(0,ptDelta.y));/*Abs(mtRotPoint(LongPoint(0,ptDelta.y),nAngle));*/

	LongPoint pt1[4];
	int i=0;
	for(i=0;i<4;i++)
		pt1[i]=mtRotPoint(pt[i],-nAngle);
	LongRect rc;
	rc.Set(pt1[0],pt1[2]);
	rc.Normalize();
	for(i=0;i<4;i++)
		pt1[i]=rc.GetCorner(i);

	ptRot[10]=pt1[0]+ptDeltaY;
	ptRot[11]=pt1[0]+ptDeltaX+ptDeltaY;
	ptRot[0] =pt1[0]+ptDeltaX;
	ptRot[1] =pt1[1]-ptDeltaX;
	ptRot[2] =pt1[1]-ptDeltaX+ptDeltaY;
	ptRot[3] =pt1[1]+ptDeltaY;
	ptRot[4] =pt1[2]-ptDeltaY;
	ptRot[5] =pt1[2]-ptDeltaY-ptDeltaX;
	ptRot[6] =pt1[2]-ptDeltaX;
	ptRot[7] =pt1[3]+ptDeltaX;
	ptRot[8] =pt1[3]+ptDeltaX-ptDeltaY;
	ptRot[9] =pt1[3]-ptDeltaY;
	for(i=0;i<12;i++)
		ptRot[i]=mtRotPoint(ptRot[i],nAngle);
}

void CMDRndRectangle::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
	LongPoint			vVertexPt;
	LongPoint			vPoint1,vPoint2,vPoint3,vPoint4,
						pPoint,vDelta1,vDelta2;
	register short		i,penWidth;
	LongPoint			longMajor, longMinor, axisPt;
	LongPoint			vSPoint1,vSPoint2,vSPoint3,vSPoint4;
	Coord				leftTopShift, rightBottomShift;
	short				halfPen;
	Coord				lHalfPen;
	CBezierEdge*		pEdge;
	
	short vDashIdx = GetPenModel().Dashing();
	LongPoint diagonals = m_lptCornerDelta;
	longMajor.x = diagonals.x;
	longMajor.y = 0;
	longMinor.y = diagonals.y;
	longMinor.x = 0;
	axisPt.Clear () ;
	if ( GetRotation() != 0 )
	{
		TSinCos sinCos ( GetRotation() ) ;
		// rotate major & minor diameter 
		longMajor = mtRotPoint ( longMajor, sinCos ) ;
		longMinor = mtRotPoint ( longMinor, sinCos ) ;
	}

	pPoint = GetShapePoint(3);
	penWidth = DxfWidthConversion(GetPenModel().m_nWidth);

	halfPen = (penWidth - 1) >> 1;
	lHalfPen = (_guLOrd (penWidth)) >> 1;
	if ( penWidth > 2 && GetPenModel().m_nAlignment != kCenterLine )
	{
		leftTopShift = rightBottomShift = (WrFixed)halfPen;

		if ( GetPenModel().m_nAlignment == kInsideLine )
			leftTopShift = -leftTopShift;
		else
			rightBottomShift = -rightBottomShift;

		pPoint.x-= leftTopShift;		
		pPoint.y -= rightBottomShift;
	}

	vPoint1 = pPoint - longMinor ;
  	DxfWritePolyHead (pFile, pLayerStr, &GetPenModel(), pDocExtPtr, vUnitFactor, 1, ptrPalette);

  	// Write the vertices of the POINT 
  	vVertexPt.x = mtFixToReal(GetShapePoint(1).x) - pDocExtPtr->left;
  	vVertexPt.y = pDocExtPtr->bottom - mtFixToReal(GetShapePoint(1).x);
  	DxfWritePtCord (pFile, pLayerStr, &vVertexPt, &vUnitFactor, vDashIdx);

	for (i = 0; i < 4; i++)
	{
		pPoint = GetShapePoint(i);

		if ( penWidth > 2 && GetPenModel().m_nAlignment != kCenterLine )
		{
			switch ( i )
			{
			case 0:
				pPoint.x -= leftTopShift;
				pPoint.y -= leftTopShift;
				break;
			case 1:
				pPoint.x -= rightBottomShift;
				pPoint.y -= leftTopShift;
				break;
			case 2:
				pPoint.x -= rightBottomShift;
				pPoint.y -= rightBottomShift;
				break;
			case 3:
				pPoint.x -= leftTopShift;
				pPoint.y -= rightBottomShift;
				break;
			}
		}

		switch (i)
		{
		case 0:
			vDelta1 = longMinor ;
			vDelta2 = longMajor ;
			break;
		case 1:
			vDelta1 = - longMajor ;
			vDelta2 = longMinor ;
			break;
		case 2:
			vDelta1 = - longMinor ;
			vDelta2 = - longMajor ;
			break;
		case 3:
			vDelta1 = longMajor ;
			vDelta2 = - longMinor ;
			break;
		}
		vPoint2 	= pPoint + vDelta1 ;

		vVertexPt.x = vPoint1.x - pDocExtPtr->left;
		vVertexPt.y = pDocExtPtr->bottom - vPoint1.y;
		DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
						 &vUnitFactor, 0L, 0, vDashIdx);

		vVertexPt.x = vPoint2.x - pDocExtPtr->left;
		vVertexPt.y = pDocExtPtr->bottom - vPoint2.y;
		DxfWriteVertex (pFile, pLayerStr, &vVertexPt,
						 &vUnitFactor, 0L, 0, vDashIdx);

		vPoint1 = vPoint2;
		vPoint2 = pPoint + vDelta1 / short(2) ;
		vPoint3 = pPoint + vDelta2 / short(2) ;
		vPoint4 = pPoint + vDelta2;
		vSPoint1 = vPoint1;
		vSPoint2 = vPoint2;
		vSPoint3 = vPoint3;
		vSPoint4 = vPoint4;

		try
		{
			pEdge = new CBezierEdge(vSPoint1, vSPoint2, vSPoint3, vSPoint4);
			DxfWriteOneBezierSeg (pFile, pLayerStr, pDocExtPtr, vUnitFactor, nZoomFactor, pEdge);
			delete pEdge;
		}
		catch (CException* e)
		{
			if (pEdge)
				delete pEdge;
			throw e;
		}
		
		vPoint1 = vPoint4;
	}

  	DxfWriteSeqend (pFile, pLayerStr);
}

int CMDRndRectangle::SaveMacDraft(PhIOContext& ctx)
{
	PhDSRndRectangle ps;
	ps.fID=PhIDRndRectangle;
	ctx.pPhShape=&ps;
	CMDFillShape::SaveMacDraft(ctx);

	uptr p((char*)&ps);
	p.swapDSShape();
	p.swaps(2); // fm
	LongPoint axisPt=Average(m_ptCorner[1],m_ptCorner[3]);
	for(int i=0;i<4;i++)
	{
		ps.fPoint[i].fFlags=kPhRealVertex;
		ps.fPoint[i].fLongPt=mtRotPoint(m_ptCorner[i],-m_RotAngle,axisPt);
		ps.fPoint[i].fUse=0;
		p.skip(2); p.swaplpt(1);	// DBPoint
	}
	switch(m_nCornerType)
	{
	case kCornerProportional:
		ps.fCornerType=kPhProportionalCorner;
		break;
	case kCornerCircle:
		ps.fCornerType=kPhConstantCorner;
		break;
	case kCornerElliptical:
		ps.fCornerType=kPhEllipticalCorner;
		break;
	}
	ps.fDiagonals.fFlags=0;
	ps.fDiagonals.fLongPt=m_lptCornerDelta<<1;
	ps.fDiagonals.fUse=0;
	p.skip(2);
	p.skip(2); p.swaplpt(1);	// DBPoint

	ushort count=sizeof(PhDSRndRectangle)<<8;	// swapped
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSRndRectangle));
	SaveMacObjInfo(ctx);
	return 1;
}
