// MDAssocDim.cpp: implementation of the CMDAssocDim class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDAssocDim.h"

#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "DraftContext.h"
#include "DimUnit.h"
#include "MDDimStr.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDAssocDim,CMDShape,1)

//////////////////////////////////////////////////////////////////////

CMDAssocDim::CMDAssocDim() :  m_nDimCap1(6), m_nDimCap2(6)
{
}

CMDAssocDim::CMDAssocDim(const DimStandard& DimStd,const DimFormat& DimFmt,BYTE nCap1,BYTE nCap2) : m_DimStd(DimStd), m_DimFmt(DimFmt), m_nDimCap1(nCap1), m_nDimCap2(nCap2)
{
	m_pDimStr=new CMDDimStr;
}

CMDAssocDim::~CMDAssocDim()
{
	if(m_pDimStr) {
		delete m_pDimStr;
		m_pDimStr = NULL;
	}
}

void CMDAssocDim::Serialize(CArchive& ar)
{
	CMDShape::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_DimStd;
		ar >> m_DimFmt;
		ar >> m_nDimCap1 >> m_nDimCap2;
		ar >> m_pDimStr;
	}
	else
	{
		ar << m_DimStd;
		ar << m_DimFmt;
		ar << m_nDimCap1 << m_nDimCap2;
		ar << m_pDimStr;
	}
}

void CMDAssocDim::ExtendForPen(LongRect& lrcExtent) const
{
	CMDShape::ExtendForPen(lrcExtent);
	const DimPoints* pdp=GetDimPoints();
	if(pdp)
	{
		Coord adjust=0;
		if(m_nDimCap1)
		{
			LongPoint lpt=pdp->lptDimStart;
			adjust=Coord(gEndcaps[getEndcapIndex(m_nDimCap1,m_PenModel.m_nWidth&0x1F)].width);
			LongRect lrc;
			lrc.Set(lpt.x - adjust, lpt.y - adjust, lpt.x + adjust +1 , lpt.y + adjust +1);
			lrcExtent|=lrc;
		}
		if(m_nDimCap2)
		{
			LongPoint lpt=pdp->lptDimEnd;
			adjust=Coord(gEndcaps[getEndcapIndex(m_nDimCap2,m_PenModel.m_nWidth&0x1F)].width);
			LongRect lrc;
			lrc.Set(lpt.x - adjust, lpt.y - adjust, lpt.x + adjust +1 , lpt.y + adjust +1);
			lrcExtent|=lrc;
		}
		if(!pdp->pDimFmt->bArrowsInside)
		{
			adjust=Coord(pdp->pDimStd->OutsideLineLen*kEngDecRes); // max resolution
			lrcExtent.Inset(-adjust,-adjust);
		}
	}
}

void CMDAssocDim::CalculateExtents()
{
	if(m_pDimStr) m_pDimStr->CalculateExtents();
	CMDShape::CalculateExtents();
}

void CMDAssocDim::SetPenModel(const PenModel& pm)
{
	if(m_pDimStr) m_pDimStr->SetPenModel(pm);
	CMDShape::SetPenModel(pm);
}

bool CMDAssocDim::ModifyPenModel(const PenModel &pm, int nWhat)
{
	if(m_pDimStr) m_pDimStr->ModifyPenModel(pm,nWhat);
	return CMDShape::ModifyPenModel(pm,nWhat);
}

void CMDAssocDim::SetPenModelDirect(const PenModel& penModel)
{
	m_PenModel=penModel;
	m_nDimCap1=(BYTE)penModel.EndCap1();
	m_nDimCap2=(BYTE)penModel.EndCap2();
	m_PenModel.m_nEndCaps=0;
}

void CMDAssocDim::SetFillModel(const FillModel& fm)
{
	if(m_pDimStr) m_pDimStr->SetFillModel(fm);
}

const TextParams& CMDAssocDim::GetTextParams() const
{
	if(m_pDimStr) return m_pDimStr->GetTextParams();
	return CMDShape::GetTextParams();
}

void CMDAssocDim::ModifyTextParams(const TextParams& tp, int nWhat, CDraftContext* pDC)
{
	if(m_pDimStr)
	{
		pDC->SetDimStandard(&m_DimStd);
		m_pDimStr->ModifyTextParams(tp,nWhat,pDC);
		pDC->SetDimStandard(NULL);
	}
}

void CMDAssocDim::DrawHandles(CDraftContext* pDC) const
{
	CMDShape::DrawHandles(pDC);
	if(m_pDimStr) m_pDimStr->DrawHandles(pDC);
}

bool CMDAssocDim::PointInFill(const pointd& pt, CMapper* pMapper) const
{
	if(m_pDimStr) return m_pDimStr->PointInFill(pt,pMapper);
	return false;
}

int	CMDAssocDim::TestHandles(const pointd& pt, CMapper* pMapper) const
{
	if(m_pDimStr && m_pDimStr->TestSelect(pt,pMapper)) return 2; // anchor
	else return CMDShape::TestHandles(pt,pMapper);
}

void CMDAssocDim::DoShapeOffset(const LongPoint& delta)
{
	if(m_pDimStr) m_pDimStr->DoShapeOffset(delta);
	CMDShape::DoShapeOffset(delta);
}

void CMDAssocDim::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
	if(m_pDimStr) m_pDimStr->DoShapeScale(lrcFrom,lrcTo,ppNewShape);
	CMDShape::DoShapeScale(lrcFrom,lrcTo,ppNewShape);
}

void CMDAssocDim::DoShapeScale(const LongRatio& ratio,double factor)
{
	if(m_pDimStr) m_pDimStr->DoShapeScale(ratio,factor);
	CMDShape::DoShapeScale(ratio,factor);
}

void CMDAssocDim::DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2)
{
	if(m_pDimStr) m_pDimStr->DoShapeMirror(lptRef1,lptRef2);
	CMDShape::DoShapeMirror(lptRef1,lptRef2);
}

void CMDAssocDim::MakeSelfContained(CPCDraftDoc *pDoc,bool CopyDIB)
{
	CMDShape::MakeSelfContained(pDoc,CopyDIB);
	if(m_pDimStr) m_pDimStr->MakeSelfContained(pDoc,CopyDIB);
}

void CMDAssocDim::UpdateDependentShapes(CPCDraftView *pView)
{
	m_DimStd.ConvertForUnits(pView->GetDocument()->m_bEnglishUnits?kDsEnglish:kDsMetric);
}

void CMDAssocDim::DrawDimStr(CDraftContext *pDC) const
{
	if(m_pDimStr)
	{
		pDC->SetDimStandard(&m_DimStd);
		m_pDimStr->DrawShape(pDC);
		if(m_DimFmt.bTextFramed) m_pDimStr->DrawShapeOutline(pDC);
		pDC->SetDimStandard(NULL);
	}
}

void CMDAssocDim::SetDimStr(CMDDimStr* pDimStr)
{
	if(m_pDimStr) delete m_pDimStr;
	m_pDimStr=pDimStr;
}

void CMDAssocDim::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
    if (m_pDimStr){
        if (m_DimFmt.bTextFramed)
            m_pDimStr->DXFWriteFrame(pFile, pLayerStr, pDocExtPtr, vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);
        m_pDimStr->WriteDXF(pFile, pLayerStr, pDocExtPtr, vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);
    }
}

int CMDAssocDim::SaveMacDraft(PhIOContext& ctx)
{
	FillModel fm;
	CString sValue,sTol1,sTol2;
	TextParams tp;
	if(m_pDimStr)
	{
		fm=m_pDimStr->GetFillModel();
		tp=m_pDimStr->GetTextParams();
		m_pDimStr->GetValues(sValue,sTol1,sTol2);
	}

	CMDShape::SaveMacDraft(ctx);
	PhDSAssocDim* ps=(PhDSAssocDim*)ctx.pPhShape;
	ctx.ConvertFillModel(fm,ps->fFillModel);
	
	ps->fPenModel.fPenStyle&=0xFF00;
	ps->fPenModel.fPenStyle|=(m_nDimCap1&0xF);
	ps->fPenModel.fPenStyle|=(m_nDimCap2&0xF)<<4;

	ps->fStatus=kPhDimVisible;
	Style nStyle;
	ctx.ConvertLogFont(tp.m_lf,ps->fFont,ps->fFontSize,nStyle);
	for(int i=0;i<12;i++) ps->fValue[i]=0;
	ctx.ConvertDimFmt(m_DimFmt,ps->format);
// roundoff
	ps->fShapeFlags&=0xF0FF;
	ps->fShapeFlags|=((m_DimFmt.nRoundoff&0xF)<<8);

	ctx.ConvertDimStd(m_DimStd,ps->standard);
	ps->textExtent=LongRect(0,0,0,0);
	ps->dimStrDraw=LongRect(0,0,0,0);
	ps->tol1Draw=LongRect(0,0,0,0);
	ps->tol2Draw=LongRect(0,0,0,0);

	ctx.ConvertC2P(sValue,ps->dimStr,kDimStrSize);
	ctx.ConvertC2P(sTol1,ps->tol1Str,kDimTolStrSize);
	ctx.ConvertC2P(sTol2,ps->tol2Str,kDimTolStrSize);
	return 0;
}

void CMDAssocDim::SaveMacDimLabel(PhIOContext &ctx)
{
	if(m_DimFmt.bUseLabel && !m_DimFmt.sLabel.IsEmpty())
	{
		short count=m_DimFmt.sLabel.GetLength()+1;
		uptr(&count).swaps(1);
		ctx.f->Write(&count,sizeof(short));
		count=m_DimFmt.sLabel.GetLength();
		ctx.f->Write(&count,1);
		ctx.f->Write(m_DimFmt.sLabel,count);
	}
}

void CMDAssocDim::ProcessShapesForCopy(CVersionDoc *pDoc, int* used)
{
	CMDShape::ProcessShapesForCopy(pDoc, used);
	if(m_pDimStr) m_pDimStr->ProcessShapesForCopy(pDoc, used);
}

void CMDAssocDim::ProcessShapesAfterCopy(CPCDraftDoc *pDoc, int *used)
{
	CMDShape::ProcessShapesAfterCopy(pDoc, used);
	if(m_pDimStr) m_pDimStr->ProcessShapesAfterCopy(pDoc, used);
}

void CMDAssocDim::ProcessShapesAfterSymLibraryCopy(CPackedItem * pitem,int *used)
{
	CMDShape::ProcessShapesAfterSymLibraryCopy(pitem, used);
	if(m_pDimStr) m_pDimStr->ProcessShapesAfterSymLibraryCopy(pitem, used);
}