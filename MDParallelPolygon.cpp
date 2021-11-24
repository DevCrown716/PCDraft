// MDParallelPolygon.cpp: implementation of the CMDParallelPolygon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MDParallelPolygon.h"

#include "PCDraftDoc.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "MDLineShape.h"
#include "PhIOStruct.h"
#include "Edge.h"
#include "MDParallelLine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDParallelPolygon,CMDFillShape,1)

//////////////////////////////////////////////////////////////////////

void CMDParallelPolygon::Serialize(CArchive& ar)
{
	CMDPolygonShape::Serialize(ar);

	if(ar.IsLoading())
	{
		ar >> m_Attrs;
	}
	else
	{
		ar << m_Attrs;
	}
}

LongRect CMDParallelPolygon::CalculateBasicExtents() const
{
	LongBoundRect lbr;
	LongPointArray pt[2];
	CalcPoints(pt);
	for(int i=0;i<pt[0].GetSize();i++)
	{
		lbr|=pt[0][i];
		lbr|=pt[1][i];
	}
	return lbr;
}

CEdge* CMDParallelPolygon::GetShapeEdgeFromPt(const pointd &pt,const CMapper* pMapper) const
{
	LongPoint pp11,pp12,pp13,pp14;
	LongPoint pp21,pp22,pp23,pp24;
	if(m_bClosed)
	{
		guParallelLn(GetShapePoint(m_nCount-1),GetShapePoint(0),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp11,&pp12,&pp13,&pp14);
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);
		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);
	}
	else
	{
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);
	}
	for(int i=2;i<(m_nCount+(m_bClosed?2:0));i++)
	{
		pp11=pp12;
		pp12=pp22;
		pp13=pp14;
		pp14=pp24;

		guParallelLn(GetShapePoint((i-1)%m_nCount),GetShapePoint(i%m_nCount),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);

		if(guPtAtLine(pt,pMapper->LPtoDPF(pp11),pMapper->LPtoDPF(pp12),1,kCenterLine))
		{
			return new CEdge(pp11,pp12);
		}
		else if(guPtAtLine(pt,pMapper->LPtoDPF(pp13),pMapper->LPtoDPF(pp14),1,kCenterLine))
		{
			return new CEdge(pp13,pp14);
		}
	}
	if(m_bClosed)
	{
	}
	else
	{
		if(guPtAtLine(pt,pMapper->LPtoDPF(pp12),pMapper->LPtoDPF(pp22),1,kCenterLine))
		{
			return new CEdge(pp12,pp22);
		}
		else if(guPtAtLine(pt,pMapper->LPtoDPF(pp14),pMapper->LPtoDPF(pp24),1,kCenterLine))
		{
			return new CEdge(pp14,pp24);
		}
	}
	return NULL;
}

void CMDParallelPolygon::DrawShape(CDraftContext* pDC) const
{
   DrawFill(pDC);

// draw parallel fill
	{
      UsePath usepath(pDC);
		pDC->SetOutlinePen();

		CRgn CumRgn;
		CumRgn.CreateRectRgn(0,0,0,0);

		LongPoint pp11,pp12,pp13,pp14;
		LongPoint pp21,pp22,pp23,pp24;
		if(m_bClosed)
		{
			guParallelLn(GetShapePoint(m_nCount-1),GetShapePoint(0),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp11,&pp12,&pp13,&pp14);
			guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);
			guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
			guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);
		}
		else
		{
			guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);
		}

		for(int i=2;i<(m_nCount+(m_bClosed?2:0));i++)
		{
			pp11=pp12;
			pp12=pp22;
			pp13=pp14;
			pp14=pp24;

			guParallelLn(GetShapePoint((i-1)%m_nCount),GetShapePoint(i%m_nCount),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

			guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
			guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);
			{
				CRgn Rgn;
				if(pDC->BeginPath())
				{
					pDC->MoveTo(pp11);
					pDC->LineTo(pp12);
					pDC->LineTo(pp14);
					pDC->LineTo(pp13);
					pDC->LineTo(pp11);

					if(pDC->EndPath(&Rgn))
					{
						if(CumRgn.m_hObject==NULL) CumRgn.CopyRgn(&Rgn);
						else CumRgn.CombineRgn(&CumRgn,&Rgn,RGN_OR);
					}
				}
			}
		}
		if(m_bClosed)
		{
		}
		else
		{
			CRgn Rgn;
			if(pDC->BeginPath())
			{
				pDC->MoveTo(pp12);
				pDC->LineTo(pp22);
				pDC->LineTo(pp24);
				pDC->LineTo(pp14);
				pDC->LineTo(pp12);

				if(pDC->EndPath(&Rgn))
				{
					if(CumRgn.m_hObject==NULL) CumRgn.CopyRgn(&Rgn);
					else CumRgn.CombineRgn(&CumRgn,&Rgn,RGN_OR);
				}
			}
		}
		pDC->SetFill(m_Attrs.m_Fill);
		//pDC->PaintRgn(&CumRgn);
      pDC->PaintPath();
	}

	pDC->SetPen(m_PenModel);
	DrawShapeOutline(pDC);
}

void CMDParallelPolygon::CalcPoints(LongPointArray *pt) const
{
	pt[0].SetSize(m_nCount);
	pt[1].SetSize(m_nCount);

	LongPoint pp11,pp12,pp13,pp14;
	LongPoint pp21,pp22,pp23,pp24;
	if(m_bClosed)
	{
		guParallelLn(GetShapePoint(m_nCount-1),GetShapePoint(0),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp11,&pp12,&pp13,&pp14);
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);
		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);
	}
	else
	{
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);
	}
	pt[0][0]=pp12;
	pt[1][0]=pp14;
	for(int i=2;i<(m_nCount+(m_bClosed?2:0));i++)
	{
		pp11=pp12;
		pp12=pp22;
		pp13=pp14;
		pp14=pp24;

		int j=(i-1)%m_nCount;
		guParallelLn(GetShapePoint(j),GetShapePoint(i%m_nCount),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);

		pt[0][j]=pp12;
		pt[1][j]=pp14;
	}

	if(!m_bClosed)
	{
		pt[0][m_nCount-1]=pp22;
		pt[1][m_nCount-1]=pp24;
	}
}

void CMDParallelPolygon::DrawShapeOutline(CDraftContext* pDC) const
{
	LongPointArray pt[2];
	pt[0].SetSize(m_nCount);
	pt[1].SetSize(m_nCount);

	LongPoint pp11,pp12,pp13,pp14;
	LongPoint pp21,pp22,pp23,pp24;
	if(m_bClosed)
	{
		guParallelLn(GetShapePoint(m_nCount-1),GetShapePoint(0),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp11,&pp12,&pp13,&pp14);
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);
		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);
	}
	else
	{
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);
		if(m_Attrs.m_nEndCap&1)
		{
			pDC->ParallelCaps(GetShapePoint(0),GetShapePoint(1),m_Attrs,kEnd1);
//			pDC->MoveTo(pp12);
//			pDC->OutsideLineTo(pp14);
		}
	}
	pt[0][0]=pp12;
	pt[1][0]=pp14;
	for(int i=2;i<(m_nCount+(m_bClosed?2:0));i++)
	{
		pp11=pp12;
		pp12=pp22;
		pp13=pp14;
		pp14=pp24;

		int j=(i-1)%m_nCount;
		guParallelLn(GetShapePoint(j),GetShapePoint(i%m_nCount),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);

		pt[0][j]=pp12;
		pt[1][j]=pp14;
		if(m_Attrs.m_nJoint)
		{
			pDC->Line(pp12,pp14);
			pDC->ResetJoint();
		}
	}

	if(!m_bClosed)
	{
		pt[0][m_nCount-1]=pp22;
		pt[1][m_nCount-1]=pp24;
		if(m_Attrs.m_nEndCap&2)
		{
			pDC->ParallelCaps(GetShapePoint(m_nCount-2),GetShapePoint(m_nCount-1),m_Attrs,kEnd2);
//			pDC->MoveTo(pp22);
//			pDC->OutsideLineTo(pp24);
		}
	}
	pDC->PolyLine(pt[0],m_nCount,m_bClosed);
	pDC->ResetJoint();
	pDC->PolyLine(pt[1],m_nCount,m_bClosed);
	pDC->ResetJoint();
}

bool CMDParallelPolygon::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	int nWidth=m_Attrs.m_fWidth.GetLong();
	for(int i=0;i<(m_nCount-1);i++)
	{
		if(guPtAtLine(pt,pMapper->LPtoDPF(GetShapePoint(i)),pMapper->LPtoDPF(GetShapePoint(i+1)),nWidth,m_Attrs.m_nAlign))
		{
			return true;
		}
	}
	if(m_bClosed && guPtAtLine(pt,pMapper->LPtoDPF(GetShapePoint(0)),pMapper->LPtoDPF(GetShapePoint(m_nCount-1)),nWidth,m_PenModel.m_nAlignment)) return true;
	return false;
}

void CMDParallelPolygon::DoShapeScale(const LongRatio& ratio,double factor)
{
	if(factor) m_Attrs.m_fWidth*=factor;
	m_Attrs.m_fWidth*=ratio;
	CMDPolygonShape::DoShapeScale(ratio,factor);
}

void CMDParallelPolygon::Unglue(CMDShapeList* pList) const
{
	CMDLineShape* pLine;
	CMDPolygonShape* pPoly1=new CMDPolygonShape(kPolygon,m_nCount,m_bClosed);
	CMDPolygonShape* pPoly2=new CMDPolygonShape(kPolygon,m_nCount,m_bClosed);

	LongPoint pp11,pp12,pp13,pp14;
	LongPoint pp21,pp22,pp23,pp24;

	if(m_bClosed)
	{
		guParallelLn(GetShapePoint(m_nCount-1),GetShapePoint(0),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp11,&pp12,&pp13,&pp14);
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);
		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);
	}
	else
	{
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);

		if(m_Attrs.m_nEndCap&1)
		{
			pLine=new CMDLineShape(pp12,pp14);
			pLine->ModifyPenModel(m_PenModel,pmAllButCaps);
			pLine->CalculateExtents();
			pLine->SetShapeDatum(kDefaultDatum);
			pList->AddTail(pLine);
		}
	}
	pPoly1->SetShapePoint(0,pp12);
	pPoly2->SetShapePoint(0,pp14);

	for(int i=2;i<(m_nCount+(m_bClosed?2:0));i++)
	{
		pp11=pp12;
		pp12=pp22;
		pp13=pp14;
		pp14=pp24;

		int j=(i-1)%m_nCount;
		guParallelLn(GetShapePoint(j),GetShapePoint(i%m_nCount),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);

		pPoly1->SetShapePoint(j,pp12);
		pPoly2->SetShapePoint(j,pp14);
		if(m_Attrs.m_nJoint)
		{
			pLine=new CMDLineShape(pp12,pp14);
			pLine->ModifyPenModel(m_PenModel,pmAllButCaps);
			pLine->CalculateExtents();
			pLine->SetShapeDatum(kDefaultDatum);
			pList->AddTail(pLine);
		}
	}
	if(!m_bClosed)
	{
		pPoly1->SetShapePoint(m_nCount-1,pp22);
		pPoly2->SetShapePoint(m_nCount-1,pp24);
		if(m_Attrs.m_nEndCap&1)
		{
			pLine=new CMDLineShape(pp22,pp24);
			pLine->ModifyPenModel(m_PenModel,pmAllButCaps);
			pLine->CalculateExtents();
			pLine->SetShapeDatum(kDefaultDatum);
			pList->AddTail(pLine);
		}
	}

	LongRatio ratio={1,1};
	if(pPoly1->GetRawArea(ratio)>pPoly2->GetRawArea(ratio))
	{
		CMDPolygonShape* pPoly=pPoly1;
		pPoly1=pPoly2;
		pPoly2=pPoly;
	}

	pPoly1->ModifyPenModel(m_PenModel,pmAllButCaps);
	pPoly1->SetFillModel(GetFillModel());
	pPoly1->CalculateExtents();
	pPoly1->SetShapeDatum(kDefaultDatum);
	pList->AddTail(pPoly1);
	pPoly2->ModifyPenModel(m_PenModel,pmAllButCaps);
	pPoly2->SetFillModel(m_Attrs.m_Fill);
	pPoly2->CalculateExtents();
	pPoly2->SetShapeDatum(kDefaultDatum);
	pList->AddTail(pPoly2);
}

void CMDParallelPolygon::RemovePt(int nHandle, CMDShape** ppNewShape)
{
	if(nHandle>=0 && nHandle<m_nCount)
	{
		m_pt.RemoveAt(nHandle);
		m_nCount--;
		if(m_nCount==2)
		{
			m_bClosed=false;
			m_FillModel.m_nFillType=kNoFill;

			CMDLineShape* pLine=new CMDParallelLine(GetShapePoint(0),GetShapePoint(1),m_Attrs);
			pLine->SetPenModel(GetPenModel());
			pLine->CalculateExtents();
			pLine->SetShapeDatum(kDefaultDatum);
			*ppNewShape=pLine;
		}
	}
}

void CMDParallelPolygon::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CPolyTrackContext* p=(CPolyTrackContext*)pTrackContext;

	LongPoint pp11,pp12,pp13,pp14;
	LongPoint pp21,pp22,pp23,pp24;
	if(m_bClosed)
	{
		guParallelLn(p->pt[m_nCount-1],p->pt[0],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp11,&pp12,&pp13,&pp14);
		guParallelLn(p->pt[0],p->pt[1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);
		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);
	}
	else
	{
		guParallelLn(p->pt[0],p->pt[1],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);
		if(m_Attrs.m_nEndCap&1)
		{
			pDC->MoveTo(pp12);
			pDC->LineTo(pp14);
		}
	}

	for(int i=2;i<(m_nCount+(m_bClosed?2:0));i++)
	{
		pp11=pp12;
		pp12=pp22;
		pp13=pp14;
		pp14=pp24;

		guParallelLn(p->pt[(i-1)%m_nCount],p->pt[i%m_nCount],m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);

		pDC->MoveTo(pp11);
		pDC->LineTo(pp12);
		pDC->MoveTo(pp13);
		pDC->LineTo(pp14);
		if(m_Attrs.m_nJoint)
		{
			pDC->MoveTo(pp12);
			pDC->LineTo(pp14);
		}
	}
	if(!m_bClosed)
	{
		pDC->MoveTo(pp12);
		pDC->LineTo(pp22);
		pDC->MoveTo(pp14);
		pDC->LineTo(pp24);
		if(m_Attrs.m_nEndCap&2)
		{
			pDC->MoveTo(pp22);
			pDC->LineTo(pp24);
		}
	}
	if(p->nWhichHandle>=0) TrackAnnot(pDC,p);
}

void CMDParallelPolygon::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
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

void CMDParallelPolygon::MakeSelfContained(CPCDraftDoc *pDoc,bool CopyDIB)
{
	CMDFillShape::MakeSelfContained(pDoc,CopyDIB);
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

int CMDParallelPolygon::SaveMacDraft(PhIOContext& ctx)
{
	int size=sizeof(PhDSParallelPolygon)+sizeof(PhDBPoint)*(m_nCount*3-1);
	PhDSParallelPolygon* ps=(PhDSParallelPolygon*)malloc(size);

	ps->fID=PhIDParallelPolygon;
	ctx.pPhShape=ps;
	CMDFillShape::SaveMacDraft(ctx);

	ps->fClosed=m_bClosed;
	ps->fDummy=0;
	ps->fCount=m_nCount;
	ctx.ConvertParaAttr(m_Attrs,ps->fParallelAttr);

	uptr p((char*)ps);
	p.swapDSShape();
	p.swaps(2); // fm
	p.swaps(2); p.swapl(1); p.swaps(1); // parallel attr
	p.skip(2); p.swaps(1);

	LongPoint pp11,pp12,pp13,pp14;
	LongPoint pp21,pp22,pp23,pp24;
	if(m_bClosed)
	{
		guParallelLn(GetShapePoint(m_nCount-1),GetShapePoint(0),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp11,&pp12,&pp13,&pp14);
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);
		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);
	}
	else
	{
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);
	}

	ps->fPoint[0].fFlags=kPhRealVertex;
	ps->fPoint[0].fLongPt=m_pt[0];
	ps->fPoint[0].fUse=0;
	ps->fPoint[m_nCount].fFlags=kPhCanSelect|kPhNoHighlight;
	ps->fPoint[m_nCount].fLongPt=pp12;
	ps->fPoint[m_nCount].fUse=0;
	ps->fPoint[m_nCount*2].fFlags=kPhCanSelect|kPhNoHighlight;
	ps->fPoint[m_nCount*2].fLongPt=pp14;
	ps->fPoint[m_nCount*2].fUse=0;

    int i = 1;
	for(;i<(m_nCount-(m_bClosed?0:1));i++)
	{
		pp11=pp12;
		pp12=pp22;
		pp13=pp14;
		pp14=pp24;

		guParallelLn(GetShapePoint(i),GetShapePoint((i+1)%m_nCount),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);

		ps->fPoint[i].fFlags=kPhRealVertex;
		ps->fPoint[i].fLongPt=m_pt[i];
		ps->fPoint[i].fUse=0;
		ps->fPoint[i+m_nCount].fFlags=kPhCanSelect|kPhNoHighlight;
		ps->fPoint[i+m_nCount].fLongPt=pp12;
		ps->fPoint[i+m_nCount].fUse=0;
		ps->fPoint[i+m_nCount*2].fFlags=kPhCanSelect|kPhNoHighlight;
		ps->fPoint[i+m_nCount*2].fLongPt=pp14;
		ps->fPoint[i+m_nCount*2].fUse=0;
	}
	if(m_bClosed)
	{
	}
	else
	{
		ps->fPoint[i].fFlags=kPhRealVertex;
		ps->fPoint[i].fLongPt=m_pt[i];
		ps->fPoint[i].fUse=0;
		ps->fPoint[i+m_nCount].fFlags=kPhCanSelect|kPhNoHighlight;
		ps->fPoint[i+m_nCount].fLongPt=pp22;
		ps->fPoint[i+m_nCount].fUse=0;
		ps->fPoint[i+m_nCount*2].fFlags=kPhCanSelect|kPhNoHighlight;
		ps->fPoint[i+m_nCount*2].fLongPt=pp24;
		ps->fPoint[i+m_nCount*2].fUse=0;
	}

	p.swapDBPoint(m_nCount*3);
	
	ushort count=size;
	uptr((char*)&count).swaps(1);
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(ps,size);
	SaveMacObjInfo(ctx);
	free(ps);
	return 1;
}

CMDShape* CMDParallelPolygon::ShapeFromEdge(CEdge* pEdge,int& nFromVertex,int& nToVertex)
{
	LongPoint pp11,pp12,pp13,pp14;
	LongPoint pp21,pp22,pp23,pp24;
	if(m_bClosed)
	{
		guParallelLn(GetShapePoint(m_nCount-1),GetShapePoint(0),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp11,&pp12,&pp13,&pp14);
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);
		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);
	}
	else
	{
		guParallelLn(GetShapePoint(0),GetShapePoint(1),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp12,&pp22,&pp14,&pp24);
	}
	for(int i=2;i<(m_nCount+(m_bClosed?2:0));i++)
	{
		pp11=pp12;
		pp12=pp22;
		pp13=pp14;
		pp14=pp24;

		guParallelLn(GetShapePoint((i-1)%m_nCount),GetShapePoint(i%m_nCount),m_Attrs.m_fWidth,LineAttribute(m_Attrs.m_nAlign),&pp21,&pp22,&pp23,&pp24);

		guLongLnLnPt(pp11,pp12,pp21,pp22,&pp12);
		guLongLnLnPt(pp13,pp14,pp23,pp24,&pp14);

		CEdge* pShapeEdge1=new CEdge(pp11,pp12);
		CEdge* pShapeEdge2=new CEdge(pp13,pp14);
		if(pEdge->IsEqual(pShapeEdge1) || pEdge->IsEqual(pShapeEdge2))
		{
			nFromVertex=i-2;
			nToVertex=(i-1)%m_nCount;
			delete pShapeEdge1;
			delete pShapeEdge2;
			return this;
		}
		delete pShapeEdge1;
		delete pShapeEdge2;
	}
	if(m_bClosed)
	{
	}
	else
	{
		CEdge* pShapeEdge1=new CEdge(pp12,pp22);
		CEdge* pShapeEdge2=new CEdge(pp14,pp24);
		if(pEdge->IsEqual(pShapeEdge1) || pEdge->IsEqual(pShapeEdge2))
		{
			nFromVertex=m_nCount-2;
			nToVertex=m_nCount-1;
			delete pShapeEdge1;
			delete pShapeEdge2;
			return this;
		}
		delete pShapeEdge1;
		delete pShapeEdge2;
	}
	return NULL;
}

void CMDParallelPolygon::ProcessShapesForCopy(CVersionDoc *pDoc, int* used)
{
	CMDFillShape::ProcessShapesForCopy(pDoc, used);
	if(m_Attrs.m_Fill.m_nFillType == kFillPat) used[m_Attrs.m_Fill.m_nIndex] = 1;
}

void CMDParallelPolygon::ProcessShapesAfterCopy(CPCDraftDoc *pDoc, int *used)
{
	CMDFillShape::ProcessShapesAfterCopy(pDoc, used);
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

void CMDParallelPolygon::ProcessShapesAfterSymLibraryCopy(CPackedItem * pitem,int *used)
{
	
   CMDFillShape::ProcessShapesAfterSymLibraryCopy(pitem, used);

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