// MDParallelLine.cpp: implementation of the CMDParallelLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MDParallelLine.h"
#include "MDParallelPolygon.h"

#include "PCDraftDoc.h"
#include "gu.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "MDLineShape.h"
#include "PhIOStruct.h"
#include "Edge.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDParallelLine,CMDLineShape,1)

//////////////////////////////////////////////////////////////////////

CMDParallelLine::CMDParallelLine(const LongPoint& lptFrom,const LongPoint& lptTo,const ParallelAttrs& Attrs) : CMDLineShape(lptFrom,lptTo)
{
	m_Attrs=Attrs;
}

void CMDParallelLine::Serialize(CArchive& ar)
{
	CMDLineShape::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_Attrs;
	}
	else
	{
		ar << m_Attrs;
	}
}

LongRect CMDParallelLine::CalculateBasicExtents() const
{
	LongBoundRect lbr;
	LongPoint pp[4];
	guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),pp,pp+1,pp+2,pp+3);
	for(int i=0;i<4;i++)
		lbr|=pp[i];
	return lbr;
}

CEdge* CMDParallelLine::GetShapeEdgeFromPt(const pointd &pt,const CMapper* pMapper) const
{
	LongPoint pp1,pp2,pp3,pp4;
	guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp1,&pp2,&pp3,&pp4);
	if(guPtAtLine(pt,pMapper->LPtoDPF(pp1),pMapper->LPtoDPF(pp2),1,kCenterLine))
	{
		return new CEdge(pp1,pp2);
	}
	else if(guPtAtLine(pt,pMapper->LPtoDPF(pp3),pMapper->LPtoDPF(pp4),1,kCenterLine))
	{
		return new CEdge(pp3,pp4);
	}
	return NULL;
}

void CMDParallelLine::DrawShape(CDraftContext* pDC) const
{
// draw parallel fill
	CRgn Rgn;
	//if(pDC->BeginPath())
	{

		pDC->SetOutlinePen();
      pDC->path_new(false);

		LongPoint pp1,pp2,pp3,pp4;
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp1,&pp2,&pp3,&pp4);
		pDC->MoveTo(pp1);
		pDC->LineTo(pp2);
		pDC->LineTo(pp4);
		pDC->LineTo(pp3);
		pDC->LineTo(pp1);

		//if(pDC->EndPath(&Rgn))
		{
			pDC->SetFill(m_Attrs.m_Fill);
			//pDC->PaintRgn(&Rgn);
         pDC->PaintPath();
		}
	}

	pDC->SetPen(m_PenModel);
	DrawShapeOutline(pDC);
}

void CMDParallelLine::DrawShapeOutline(CDraftContext* pDC) const
{
	pDC->ParallelLine(GetShapePoint(0),GetShapePoint(1),m_Attrs);
}

bool CMDParallelLine::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	pointd pt1=pMapper->LPtoDPF(GetShapePoint(0));
	pointd pt2=pMapper->LPtoDPF(GetShapePoint(1));
	int nWidth=m_Attrs.m_fWidth.GetLong();
	return guPtAtLine(pt,pt1,pt2,nWidth,m_Attrs.m_nAlign);
}

void CMDParallelLine::DoShapeScale(const LongRatio& ratio,double factor)
{
	if(factor) m_Attrs.m_fWidth*=factor;
	m_Attrs.m_fWidth*=ratio;
	CMDLineShape::DoShapeScale(ratio,factor);
}

void CMDParallelLine::Unglue(CMDShapeList* pList) const
{
	LongPoint pp1,pp2,pp3,pp4;
	guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp1,&pp2,&pp3,&pp4);

	CMDLineShape* pLine=new CMDLineShape(pp1,pp2);
	pLine->SetPenModel(m_PenModel);
	pLine->CalculateExtents();
	pLine->SetShapeDatum(kDefaultDatum);
	pList->AddTail(pLine);

	if(m_Attrs.m_nEndCap&2)
	{
		pLine=new CMDLineShape(pp2,pp4);
		pLine->ModifyPenModel(m_PenModel,pmAllButCaps);
		pLine->CalculateExtents();
		pLine->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pLine);
	}

	pLine=new CMDLineShape(pp4,pp3);
	pLine->ModifyPenModel(m_PenModel,pmAllButCaps);
	pLine->CalculateExtents();
	pLine->SetShapeDatum(kDefaultDatum);
	pList->AddTail(pLine);

	if(m_Attrs.m_nEndCap&1)
	{
		pLine=new CMDLineShape(pp3,pp1);
		pLine->ModifyPenModel(m_PenModel,pmAllButCaps);
		pLine->CalculateExtents();
		pLine->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pLine);
	}
}

void  CMDParallelLine::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CLineTrackContext* p=(CLineTrackContext*)pTrackContext;
	LongPoint pp1,pp2,pp3,pp4;
	guParallelLn(p->pt[0],p->pt[1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp1,&pp2,&pp3,&pp4);
	pDC->MoveTo(pp1);
	pDC->LineTo(pp2);
	if(m_Attrs.m_nEndCap&2) pDC->LineTo(pp4);
	else pDC->MoveTo(pp4);
	pDC->LineTo(pp3);
	if(m_Attrs.m_nEndCap&1) pDC->LineTo(pp1);
}

void CMDParallelLine::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
	CMDShape* pShape;
	if (CanUnglue()) {
		CMDShapeList shapeList;
		Unglue(&shapeList);
		ASSERT(shapeList.GetCount());
		while (shapeList.GetCount()) {
			pShape = shapeList.RemoveHead();
			pShape ->WriteDXF(pFile, pLayerStr,	pDocExtPtr,	vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);
			delete pShape;
		}
	}
}

bool CMDParallelLine::CanAddPt(CMDShape **ppNewShape) const
{
	CMDShape* pShape=new CMDParallelPolygon(2,false,GetParallelAttrs());
	for(int i=0;i<2;i++) pShape->SetShapePoint(i,GetShapePoint(i));
	pShape->SetPenModel(GetPenModel());
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	*ppNewShape=pShape;
	return true;
}

void CMDParallelLine::MakeSelfContained(CPCDraftDoc *pDoc,bool CopyDIB)
{
	CMDShape::MakeSelfContained(pDoc,CopyDIB);
	switch(m_Attrs.m_Fill.m_nFillType)
	{
	case kFillColor:
		if(m_Attrs.m_Fill.m_crColor & PALETTEINDEX(0))
		{
			PALETTEENTRY pe;
			pDoc->m_Palette.GetPaletteEntries(m_Attrs.m_Fill.m_nIndex&0xFF,1,&pe);
			m_Attrs.m_Fill.m_crColor=RGB(pe.peRed,pe.peGreen,pe.peBlue);
		}
		break;
	case kPenPat:
		{
			if(CopyDIB && (m_Attrs.m_Fill.m_nIndex > 70)) {
				m_Attrs.m_Fill.m_nFillType=kFillPatEmbed;
				m_Attrs.m_Fill.m_hDib=pDoc->m_Patterns[m_Attrs.m_Fill.m_nIndex].CreateDIBSection();;
			}
		}
		break;
	}
}

int CMDParallelLine::SaveMacDraft(PhIOContext& ctx)
{
	PhDSParallelLine ps;
	ps.fID=PhIDParallelLine;
	ctx.pPhShape=(PhDSFillShape*)&ps;
	CMDShape::SaveMacDraft(ctx);

	uptr p((char*)&ps);
	p.swapDSShape();

	ctx.ConvertParaAttr(m_Attrs,ps.fParallelAttr);
	ps.fConAngle=0;

	p.swaps(2); p.swapl(1); p.swaps(1); // parallel attr
	p.swaps(1);

	LongPoint pp1,pp2,pp3,pp4;
	guParallelLn(m_pt[0],m_pt[1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp1,&pp2,&pp3,&pp4);

	ps.fPoint[0].fFlags=kPhRealVertex;
	ps.fPoint[0].fLongPt=m_pt[0];
	ps.fPoint[0].fUse=0;
	ps.fPoint[1].fFlags=kPhCanSelect|kPhNoHighlight;
	ps.fPoint[1].fLongPt=pp1;
	ps.fPoint[1].fUse=0;
	ps.fPoint[2].fFlags=kPhCanSelect|kPhNoHighlight;
	ps.fPoint[2].fLongPt=pp3;
	ps.fPoint[2].fUse=0;
	ps.fPoint[3].fFlags=kPhRealVertex;
	ps.fPoint[3].fLongPt=m_pt[1];
	ps.fPoint[3].fUse=0;
	ps.fPoint[4].fFlags=kPhCanSelect|kPhNoHighlight;
	ps.fPoint[4].fLongPt=pp2;
	ps.fPoint[4].fUse=0;
	ps.fPoint[5].fFlags=kPhCanSelect|kPhNoHighlight;
	ps.fPoint[5].fLongPt=pp4;
	ps.fPoint[5].fUse=0;
	p.swapDBPoint(6);

	ushort count=sizeof(PhDSParallelLine)<<8;	// swapped
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSParallelLine));
	SaveMacObjInfo(ctx);
	return 1;
}

CMDShape* CMDParallelLine::ShapeFromEdge(CEdge* pEdge,int& nFromVertex,int& nToVertex)
{
	LongPoint pp1,pp2,pp3,pp4;
	guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp1,&pp2,&pp3,&pp4);

	CEdge* pShapeEdge1=new CEdge(pp1,pp2);
	CEdge* pShapeEdge2=new CEdge(pp3,pp4);

	if(pEdge->IsEqual(pShapeEdge1) || pEdge->IsEqual(pShapeEdge2))
	{
		nFromVertex=0;
		nToVertex=1;
		delete pShapeEdge1;
		delete pShapeEdge2;
		return this;
	}
	delete pShapeEdge1;
	delete pShapeEdge2;
	return NULL;
}

void CMDParallelLine::ProcessShapesForCopy(CVersionDoc *pDoc, int* used)
{
	CMDShape::ProcessShapesForCopy(pDoc, used);
	if(m_Attrs.m_Fill.m_nFillType == kFillPat) used[m_Attrs.m_Fill.m_nIndex] = 1;
}

void CMDParallelLine::ProcessShapesAfterCopy(CPCDraftDoc *pDoc, int *used)
{
	CMDShape::ProcessShapesAfterCopy(pDoc, used);
	//Check pen and fill models. If PenPatternEmbed - convert to palette entry.
	if(m_Attrs.m_Fill.m_nFillType == kFillPat) {
		m_Attrs.m_Fill.m_nIndex = used[m_Attrs.m_Fill.m_nIndex ];
	}
	if(m_Attrs.m_Fill.m_nFillType == kFillPatEmbed) {
		m_Attrs.m_Fill.m_nFillType = kFillPat;
		HBITMAP hDib = m_Attrs.m_Fill.m_hDib;
		int found = 0;
		int hash = CalculateDIBHash(hDib);
		for(int k=1; k <= pDoc->m_numDIBPatterns; k++) {
			if(hash == pDoc->m_PatternsHash[k-1]) {
				HBITMAP hDib2 = pDoc->m_Patterns[k-1].CreateDIBSection();
				if(!CompareDIBs(hDib, hDib2)) {
					::DeleteObject(hDib2);
					found = k; break;
				}
				::DeleteObject(hDib2);
			}
		}
		if(found) {
			m_Attrs.m_Fill.m_nIndex = found-1;
			::DeleteObject(hDib);
		} else {
			m_Attrs.m_Fill.m_nIndex = pDoc->m_numDIBPatterns;
			pDoc->m_PatternsHash[pDoc->m_numDIBPatterns] = CalculateDIBHash(hDib);
			pDoc->m_Patterns[pDoc->m_numDIBPatterns++].FromDIBSection(hDib);
			::DeleteObject(hDib);
		}
	}
}

void CMDParallelLine::ProcessShapesAfterSymLibraryCopy(CPackedItem * pitem,int *used)
{
	CMDShape::ProcessShapesAfterSymLibraryCopy(pitem, used);
   if(m_Attrs.m_Fill.m_nFillType == kFillPat)
	{
      if(pitem[m_Attrs.m_Fill.m_nIndex].m_etype == packed_type_dib)
      {
         m_Attrs.m_Fill.m_nFillType = kFillPatEmbed;
         m_Attrs.m_Fill.m_hDib = pitem[m_Attrs.m_Fill.m_nIndex].m_pdib->CreateDIBSection();
      }
      else if(pitem[m_Attrs.m_Fill.m_nIndex].m_etype == packed_type_gradient)
      {
         m_Attrs.m_Fill.m_nIndex = used[m_Attrs.m_Fill.m_nIndex];
      }
	}
}