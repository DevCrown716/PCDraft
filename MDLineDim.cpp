// MDLineDim.cpp: implementation of the CMDLineDim class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDLineDim.h"
#include "LinkVertex.h"

#include "phmath.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDDimStr.h"
#include "MDLineShape.h"
#include "MDDimLine.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDLineDim,CMDAssocDim,1)

//////////////////////////////////////////////////////////////////////

CLineDimTrackContext::~CLineDimTrackContext()
{
	if(pDimStr) {
		delete pDimStr;
		pDimStr=NULL;
	}
}

CMDLineDim::CMDLineDim(const DimStandard& DimStd,const DimFormat& DimFmt,BYTE nCap1,BYTE nCap2,int nType,Angle angle) : CMDAssocDim(DimStd,DimFmt,nCap1,nCap2),m_nType(nType), m_Angle(angle), m_ldp(&m_DimStd,&m_DimFmt)
{
}

void CMDLineDim::Serialize(CArchive& ar)
{
	CMDAssocDim::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_nType >> m_Angle;
		ar >> m_ldp.lptFrom >> m_ldp.lptTo >> m_ldp.lptAnchor;

		ar >> m_ldp.lptDimStart >> m_ldp.lptDimEnd >> m_ldp.lptExtent >> m_ldp.lptLeader;
		ar >> m_ldp.lptWit1Start >> m_ldp.lptWit1End >> m_ldp.lptWit2Start >> m_ldp.lptWit2End;
		ar >> m_ldp.lptText >> m_ldp.nDSAlign >> m_ldp.angText;
		m_ldp.pDimStd=&m_DimStd;
		m_ldp.pDimFmt=&m_DimFmt;
	}
	else
	{
		ar << m_nType << m_Angle;
		ar << m_ldp.lptFrom << m_ldp.lptTo << m_ldp.lptAnchor;

		ar << m_ldp.lptDimStart << m_ldp.lptDimEnd << m_ldp.lptExtent << m_ldp.lptLeader;
		ar << m_ldp.lptWit1Start << m_ldp.lptWit1End << m_ldp.lptWit2Start << m_ldp.lptWit2End;
		ar << m_ldp.lptText << m_ldp.nDSAlign << m_ldp.angText;
	}
}

LongRect CMDLineDim::CalculateBasicExtents() const
{
	LongRect lrc=CMDAssocDim::CalculateBasicExtents();
	lrc|=m_pDimStr->CalculateBasicExtents();
	return lrc;
}

LongPoint CMDLineDim::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapePoints());
	switch(nIndex)
	{
	case 0:
		return m_ldp.lptFrom;
	case 1:
		return m_ldp.lptTo;
	case 2:
		return m_ldp.lptAnchor;
	case 3:
		return m_ldp.lptDimStart;
	case 4:
		return m_ldp.lptDimEnd;
	case 5:
		return m_ldp.lptExtent;
	case 6:
		return m_ldp.lptLeader;
	case 7:
		return m_ldp.lptText;
	case 8:
		return m_ldp.lptWit1Start;
	case 9:
		return m_ldp.lptWit1End;
	case 10:
		return m_ldp.lptWit2Start;
	case 11:
		return m_ldp.lptWit2End;
	}
	return LongPoint::kZeroPt;
}

void CMDLineDim::SetShapePoint(int nIndex, const LongPoint &lpt)
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapePoints());
	switch(nIndex)
	{
	case 0:
		m_ldp.lptFrom=lpt;
		break;
	case 1:
		m_ldp.lptTo=lpt;
		break;
	case 2:
		m_ldp.lptAnchor=lpt;
		break;
	case 3:
		m_ldp.lptDimStart=lpt;
		break;
	case 4:
		m_ldp.lptDimEnd=lpt;
		break;
	case 5:
		m_ldp.lptExtent=lpt;
		break;
	case 6:
		m_ldp.lptLeader=lpt;
		break;
	case 7:
		m_ldp.lptText=lpt;
		break;
	case 8:
		m_ldp.lptWit1Start=lpt;
		break;
	case 9:
		m_ldp.lptWit1End=lpt;
		break;
	case 10:
		m_ldp.lptWit2Start=lpt;
		break;
	case 11:
		m_ldp.lptWit2End=lpt;
		break;
	}
}

void CMDLineDim::DrawShape(CDraftContext* pDC) const
{
	Gdiplus::GraphicsPath * pClip=m_pDimStr->GetClip(pDC);
	pDC->SetPen(m_PenModel);
	pDC->DrawLineDim(m_ldp,m_nDimCap1,m_nDimCap2,pClip);
	if(pClip) delete pClip;
	//DrawShapeOutline(pDC);
	DrawDimStr(pDC);
}

void CMDLineDim::DrawShapeOutline(CDraftContext* pDC) const
{
	pDC->DrawLineDim(m_ldp,m_nDimCap1,m_nDimCap2);
}

bool CMDLineDim::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
	pointd pt1,pt2;
	pt1=pMapper->LPtoDPF(m_ldp.lptDimStart);
	pt2=pMapper->LPtoDPF(m_ldp.lptDimEnd);
	if(guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment)) return true;
	pt1=pMapper->LPtoDPF(m_ldp.lptExtent);
	if(guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment)) return true;
	pt2=pMapper->LPtoDPF(m_ldp.lptLeader);
	if(guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment)) return true;
	pt1=pMapper->LPtoDPF(m_ldp.lptWit1Start);
	pt2=pMapper->LPtoDPF(m_ldp.lptWit1End);
	if(guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment)) return true;
	pt1=pMapper->LPtoDPF(m_ldp.lptWit2Start);
	pt2=pMapper->LPtoDPF(m_ldp.lptWit2End);
	if(guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment)) return true;

	return false;
}

void CMDLineDim::UpdateDependentShapes(CPCDraftView *pView)
{
	CMDAssocDim::UpdateDependentShapes(pView);
	if(m_pDimStr)
	{
		CString sValue,sTol1,sTol2;
		if(m_DimFmt.bUseLabel) sValue=m_DimFmt.sLabel;
		else
		{
			CPCDraftDoc* pDoc=pView->GetDocument();
			LongRatio ratio,tolRatio;
			pDoc->GetActiveScaleRatio(ratio);
			tolRatio.fN=1; tolRatio.fD=1;
			CDimUnit* pdu=pDoc->GetDimUnit();

			Coord dimValue=Pythag(m_ldp.lptDimEnd-m_ldp.lptDimStart);
			if(m_DimFmt.nRoundoff>0 && m_ldp.pDimFmt->nTolerance<3)
			{
				LongRatio snapRatio;
				pDoc->GetSnapRatio(m_DimFmt.nRoundoff,snapRatio);
				pView->SnapValue(dimValue,snapRatio);
			}

			switch(m_DimFmt.nTolerance)
			{
			case kNoTolerance:
				sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
				break;
			case kOne:
				sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
				//sTol1="±"+pdu->numUser2Str(pdu->numDB2User(tolRatio,pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)),true);  Dennis
#ifdef _JAPANESE
				sTol1 = "}" + pdu->numUser2Str(pdu->numDB2User(tolRatio, pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)), true);
#else
				sTol1 = "±" + pdu->numUser2Str(pdu->numDB2User(tolRatio, pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)), true);
#endif
				break;
			case kTwo:
				sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
				sTol1="+"+pdu->numUser2Str(pdu->numDB2User(tolRatio,pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)),true);
				sTol2="-"+pdu->numUser2Str(pdu->numDB2User(tolRatio,pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance2)),true);
				break;
			case kLimits:
				sTol1=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue+pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)),true);
				sTol2=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue-pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance2)),true);
				break;
			}
		}
		CDC* pDC=pView->GetDC();
		{
			CDraftContext dc(pDC,&pView->m_Mapper);
			dc.SetDimStandard(&m_DimStd);
			m_pDimStr->SetValues(&dc,sValue,sTol1,sTol2);
		}
		pView->ReleaseDC(pDC);
		m_pDimStr->SetRotation(m_ldp.angText);
		m_pDimStr->SetAlign(m_ldp.nDSAlign);
		m_pDimStr->SetShapePoint(0,m_ldp.lptText);
		m_pDimStr->CalculateExtents();
	}
}

void CMDLineDim::DoShapeRotate(Angle angle, short nAbout,const LongPoint& lptRef)
{
	CMDShape::DoShapeRotate(angle,nAbout,lptRef);
	if(m_nType<kSlope) m_RotAngle=0;
	else m_Angle=mtFixAngle(m_Angle+angle);
}

void CMDLineDim::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
	CMDAssocDim::DoShapeScale(lrcFrom,lrcTo,ppNewShape);
	if(m_nType>kVert)
		m_Angle=mtAngle(m_ldp.lptFrom,m_ldp.lptTo);
}

void CMDLineDim::DoShapeMirror(const LongPoint& lptRef1,const LongPoint& lptRef2)
{
	if(m_nType>kVert)
	{
		Angle refAngle=mtAngle(lptRef1,lptRef2);
		m_Angle=mtFixAngle(2*refAngle-m_Angle);
	}
	CMDShape::DoShapeMirror(lptRef1,lptRef2);
}

void CMDLineDim::DoShapeFlip(bool bVertical,LongPoint* pLpt)
{
	CMDAssocDim::DoShapeFlip(bVertical,pLpt);
	// flip angle
	m_Angle=mtFixAngle((bVertical?180:0)-m_Angle);
}

void CMDLineDim::Unglue(CMDShapeList* pList) const
{
	CMDShape* pShape;
	if(m_ldp.pDimFmt->bUseWitness)
	{
		if(m_ldp.lptWit1Start!=m_ldp.lptWit1End)
		{
			pShape=new CMDLineShape(m_ldp.lptWit1Start,m_ldp.lptWit1End);
			pShape->SetPenModel(GetPenModel());
			pShape->CalculateExtents();
			pShape->SetShapeDatum(kDefaultDatum);
			pList->AddTail(pShape);
		}
		if(m_ldp.lptWit2Start!=m_ldp.lptWit2End)
		{
			pShape=new CMDLineShape(m_ldp.lptWit2Start,m_ldp.lptWit2End);
			pShape->SetPenModel(GetPenModel());
			pShape->CalculateExtents();
			pShape->SetShapeDatum(kDefaultDatum);
			pList->AddTail(pShape);
		}
	}

	pShape=new CMDDimLine(m_ldp.lptDimStart,m_ldp.lptDimEnd,(m_DimFmt.nTextDisplay<kBelow)?m_DimFmt.nTextDisplay:kAbove);
	CMDDimStr* pDimStr=new CMDDimStr(true,pShape);
	pShape->Attach(pDimStr);
	pDimStr->SetShapePoint(0,Average(m_ldp.lptDimStart,m_ldp.lptDimEnd));
	pDimStr->SetShapeDatum(kDefaultDatum);
	pDimStr->CalculateExtents();
	pList->AddTail(pDimStr);

	PenModel pm=GetPenModel();
	pm.m_nEndCap1=m_nDimCap1;
	pm.m_nEndCap2=m_nDimCap2;
	pShape->SetPenModel(pm);
	pShape->SetFillModel(m_pDimStr->GetFillModel());
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	pList->AddTail(pShape);

	if(m_ldp.lptDimEnd!=m_ldp.lptExtent)
	{
		pShape=new CMDLineShape(m_ldp.lptDimEnd,m_ldp.lptExtent);
		pShape->SetPenModel(GetPenModel());
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pShape);

		if(m_ldp.lptExtent!=m_ldp.lptLeader)
		{
			pShape=new CMDLineShape(m_ldp.lptExtent,m_ldp.lptLeader);
			pShape->SetPenModel(GetPenModel());
			pShape->CalculateExtents();
			pShape->SetShapeDatum(kDefaultDatum);
			pList->AddTail(pShape);
		}
	}
}

CTrackContext* CMDLineDim::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CLineDimTrackContext* p=new CLineDimTrackContext(&m_DimStd,&m_DimFmt);
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	p->angle=m_Angle;
	p->ldp=m_ldp;
	p->pDimStr=(CMDDimStr*)m_pDimStr->CreateCopy();

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
		p->ldp.lptFrom=mtRotPoint(p->ldp.lptFrom, pCtx->m_Angle, lptRot);
		p->ldp.lptTo=mtRotPoint(p->ldp.lptTo, pCtx->m_Angle, lptRot);
		p->ldp.lptAnchor=mtRotPoint(p->ldp.lptAnchor, pCtx->m_Angle, lptRot);
		if(m_nType>kVert) p->angle=mtFixAngle(p->angle+pCtx->m_Angle);
		CalcPoints(m_nType,p->angle,p->ldp,pMapper);
		p->pDimStr->SetRotation(p->ldp.angText);
		p->pDimStr->SetAlign(p->ldp.nDSAlign);
		p->pDimStr->SetShapePoint(0,p->ldp.lptText);
		p->pDimStr->CalculateExtents();
	}
	CalcPoints(m_nType,p->angle,p->ldp,pMapper);
	return p;
}

void CMDLineDim::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	CLineDimTrackContext* p=(CLineDimTrackContext*)pTrackContext;
	int nHandle=p->nWhichHandle;
	switch(nHandle)
	{
	case kMoveHandle:
		p->ldp.lptFrom+=pt;
		p->ldp.lptTo+=pt;
		p->ldp.lptAnchor+=pt;
		break;
	case 0:
		p->ldp.lptFrom=pt;
		break;
	case 1:
		p->ldp.lptTo=pt;
		break;
	case 2:
		if(p->bShift)
		{
			if(m_nType<kSlope) SetCursor(AfxGetApp()->LoadCursor(m_nType==kHoriz?IDC_HCONSTRAIN:IDC_VCONSTRAIN));
			guNearPtLnPt(pt,m_ldp.lptDimStart,m_ldp.lptDimEnd,&p->ldp.lptAnchor);
		}
		else
		{
			if(m_nType<kSlope) SetCursor(AfxGetApp()->LoadCursor(IDC_SMALL_CROSS));
			p->ldp.lptAnchor=pt;
		}
		break;
	}
	if(m_nType==kSlope && nHandle<2) p->angle=mtAngle(p->ldp.lptFrom,p->ldp.lptTo);
	if(CalcPoints(m_nType,p->angle,p->ldp,&p->pView->m_Mapper))
	{
		p->bSwappedVertexes=!pTrackContext->bSwappedVertexes;
		if(nHandle<2) p->nWhichHandle=1-nHandle;
	}

	CString sValue,sTol1,sTol2;
	if(m_DimFmt.bUseLabel) sValue=m_DimFmt.sLabel;
	else
	{
		CPCDraftDoc* pDoc=p->pView->GetDocument();
		LongRatio ratio,tolRatio;
		pDoc->GetActiveScaleRatio(ratio);
		tolRatio.fN=1; tolRatio.fD=1;
		CDimUnit* pdu=pDoc->GetDimUnit();

		Coord dimValue=Pythag((p->ldp.lptDimEnd)-(p->ldp.lptDimStart));
		if(m_DimFmt.nRoundoff>0 && m_DimFmt.nTolerance<3)
		{
			LongRatio snapRatio;
			pDoc->GetSnapRatio(m_DimFmt.nRoundoff,snapRatio);
			p->pView->SnapValue(dimValue,snapRatio);
		}
		switch(m_DimFmt.nTolerance)
		{
		case kNoTolerance:
			sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
			break;
		case kOne:
			sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
#ifdef _JAPANESE
			sTol1 = "}" + pdu->numUser2Str(pdu->numDB2User(tolRatio, pTrackContext->pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)), true);
#else
			sTol1="±"+pdu->numUser2Str(pdu->numDB2User(tolRatio,pTrackContext->pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)),true);
#endif
			break;
		case kTwo:
			sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
			sTol1="+"+pdu->numUser2Str(pdu->numDB2User(tolRatio,pTrackContext->pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)),true);
			sTol2="-"+pdu->numUser2Str(pdu->numDB2User(tolRatio,pTrackContext->pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance2)),true);
			break;
		case kLimits:
			sTol1=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue)+pTrackContext->pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1),true);
			sTol2=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue)-pTrackContext->pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance2),true);
			break;
		}
	}

	CDC* pDC=pTrackContext->pView->GetDC();
	{
		CDraftContext dc(pDC,&pTrackContext->pView->m_Mapper);
		dc.SetDimStandard(&m_DimStd);
		p->pDimStr->SetValues(&dc,sValue,sTol1,sTol2);
	}
	pTrackContext->pView->ReleaseDC(pDC);

	p->pDimStr->SetRotation(p->ldp.angText);
	p->pDimStr->SetAlign(p->ldp.nDSAlign);
	p->pDimStr->SetShapePoint(0,p->ldp.lptText);
	p->pDimStr->CalculateExtents();
}

void CMDLineDim::TrackShowSize(CTrackContext* pTrackContext)
{
	CLineDimTrackContext* p=(CLineDimTrackContext*)pTrackContext;
	switch(m_nType)
	{
	case kHoriz:
		p->pView->SetShowSize(0,1,Abs(p->ldp.lptDimEnd.x-p->ldp.lptDimStart.x));
		break;
	case kVert:
		p->pView->SetShowSize(0,2,Abs(p->ldp.lptDimEnd.y-p->ldp.lptDimStart.y));
		break;
	case kSlope:
		p->pView->SetShowSize(0,7,Pythag(p->ldp.lptDimStart-p->ldp.lptDimEnd));
		p->pView->SetShowSize(1,-1,p->angle);
		break;
	case kPerp:
		p->pView->SetShowSize(0,8,Abs(mtRotPoint(p->ldp.lptTo,m_Angle,p->ldp.lptFrom).x-p->ldp.lptFrom.x));
		p->pView->SetShowSize(1,-1,m_Angle);
		break;
	}
}

void CMDLineDim::TrackComplete(CTrackContext* pTrackContext)
{
	CLineDimTrackContext* p=(CLineDimTrackContext*)pTrackContext;
	m_ldp=p->ldp;
	if(m_nType==kSlope) m_Angle=p->angle;
	delete m_pDimStr;
	m_pDimStr=p->pDimStr;
	p->pDimStr = NULL;
	CalculateExtents();
	if(p->bSwappedVertexes) p->pView->GetActiveLinkVertexList().SwapShapeVertex(this,0,1);
}

void CMDLineDim::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CLineDimTrackContext* p=(CLineDimTrackContext*)pTrackContext;
	pDC->DrawLineDim(p->ldp);
	p->pDimStr->DrawShapeOutline(pDC);
}

double CMDLineDim::ReportCalcLength(const LongRatio& ratio) const
{
	return ScaleDist(ratio,Pythag(m_ldp.lptDimEnd-m_ldp.lptDimStart));
}

const ResizeInfo* CMDLineDim::GetResizeInfo() const
{
static ResizeInfo dimResize =
{
	IDS_DIMENSION, 1, {IDS_LENGTH,kUnitlessValue}
};
	return &dimResize;
}

void CMDLineDim::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	CMDAssocDim::GetResizeValue(pValues,nHandle);
	pValues[0].m_Unitless=Pythag(m_ldp.lptDimEnd-m_ldp.lptDimStart);
}

void CMDLineDim::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	CMDAssocDim::SetResizeValue(pValues);

	if(nHandle<0 || nHandle>1) nHandle=1;
	Angle ang=mtAngle(m_ldp.lptDimStart,m_ldp.lptDimEnd);
	if(nHandle)	m_ldp.lptTo+=(m_ldp.lptDimStart-m_ldp.lptDimEnd)+mtRotPoint(LongPoint(pValues[0].m_Unitless,0),ang);
	else m_ldp.lptFrom+=(m_ldp.lptDimEnd-m_ldp.lptDimStart)+mtRotPoint(LongPoint(-pValues[0].m_Unitless,0),ang);
}

bool CMDLineDim::CalcPoints(CMapper* pMapper)
{
	return CalcPoints(m_nType,m_Angle,m_ldp,pMapper);
}

void CMDLineDim::ReCalc(CMapper* pMapper)
{
	CalcPoints(m_nType,m_Angle,m_ldp,pMapper);
	m_pDimStr->SetRotation(m_ldp.angText);
	m_pDimStr->SetAlign(m_ldp.nDSAlign);
	m_pDimStr->SetShapePoint(0,m_ldp.lptText);
	m_pDimStr->CalculateExtents();
	CalculateExtents();
}

//////////////////////////////////////////////////////////////////////

bool CMDLineDim::CalcPoints(bool bHoriz,LineDimPoints& ldp,CMapper* pMapper)
{
	bool bVertexSwap=false;

	XYSelect x,y;
	if(bHoriz)
	{
		x=xSel;
		y=ySel;
	}
	else
	{
		x=ySel;
		y=xSel;
	}

	ldp.lptDimStart[x]=ldp.lptWit1Start[x]=ldp.lptWit1End[x]=ldp.lptFrom[x];
	ldp.lptDimEnd[x]=ldp.lptWit2Start[x]=ldp.lptWit2End[x]=ldp.lptTo[x];
	ldp.lptDimStart[y]=ldp.lptDimEnd[y]=ldp.lptWit1End[y]=ldp.lptWit2End[y]=ldp.lptAnchor[y];

	Coord wlGap,wlExt;
	wlGap=pMapper->Scalar2Long(ldp.pDimStd->WitnessLineGap);
	wlExt=pMapper->Scalar2Long(ldp.pDimStd->WitnessExt);

	if(ldp.lptAnchor[y] < (ldp.lptFrom[y] - wlGap))
	{
		ldp.lptWit1Start[y] = ldp.lptFrom[y] - wlGap;
		ldp.lptWit1End[y] = ldp.lptDimStart[y] - wlExt;
	}
	else if(ldp.lptAnchor[y] > (ldp.lptFrom[y] + wlGap))
	{
		ldp.lptWit1Start[y] = ldp.lptFrom[y] + wlGap;
		ldp.lptWit1End[y] = ldp.lptDimStart[y] + wlExt;
	}
	else // no witness line 1
	{
		ldp.lptWit1End[y] = ldp.lptAnchor[y];
		ldp.lptWit1Start[y] = ldp.lptAnchor[y];
	}

	// get second witness line (right)
	if(ldp.lptAnchor[y] < (ldp.lptTo[y] - wlGap))
	{
		ldp.lptWit2Start[y] = ldp.lptTo[y] - wlGap;
		ldp.lptWit2End[y] = ldp.lptDimEnd[y] - wlExt;
	}
	else if(ldp.lptAnchor[y] > (ldp.lptTo[y] + wlGap))
	{
		ldp.lptWit2Start[y] = ldp.lptTo[y] + wlGap;
		ldp.lptWit2End[y] = ldp.lptDimEnd[y] + wlExt;
	}
	else // no witness line 2
	{
		ldp.lptWit2End[y] = ldp.lptAnchor[y];
		ldp.lptWit2Start[y] = ldp.lptAnchor[y];
	}

	ldp.lptExtent = ldp.lptAnchor;

	if((ldp.lptDimStart[x] > ldp.lptDimEnd[x]) && (ldp.lptAnchor[x] > ldp.lptDimStart[x])
		|| (ldp.lptDimStart[x] < ldp.lptDimEnd[x]) && (ldp.lptAnchor[x] < ldp.lptDimStart[x]))
	{
		LongPoint lpt=ldp.lptDimStart;
		ldp.lptDimStart=ldp.lptDimEnd;
		ldp.lptDimEnd=lpt;
		lpt=ldp.lptFrom;
		ldp.lptFrom=ldp.lptTo;
		ldp.lptTo=lpt;
		bVertexSwap=true;
	}
	else
	{
		if(ldp.lptDimStart[x] != ldp.lptDimEnd[x] && (!((ldp.lptDimStart[x] < ldp.lptDimEnd[x]) && (ldp.lptAnchor[x] > ldp.lptDimEnd[x])
			|| (ldp.lptDimStart[x] > ldp.lptDimEnd[x]) && (ldp.lptAnchor[x] < ldp.lptDimEnd[x]))))
		{
			ldp.lptExtent = ldp.lptDimEnd;	// no extension
		}
	}
	return bVertexSwap;
}

bool CMDLineDim::CalcPoints(int nType,Angle angle,LineDimPoints& ldp,CMapper* pMapper)
{
	bool bVertexSwap=false;
	switch(nType)
	{
	case kHoriz:
		bVertexSwap=CalcPoints(true,ldp,pMapper);
		break;

	case kVert:
		bVertexSwap=CalcPoints(false,ldp,pMapper);
		break;

	case kSlope:
	case kPerp:
		{
			// rotate
			TSinCos minusSinCos(-angle);
			LongPoint lptRef=ldp.lptFrom;
			ldp.lptTo=mtRotPoint(ldp.lptTo, minusSinCos, lptRef);
			ldp.lptAnchor=mtRotPoint(ldp.lptAnchor, minusSinCos, lptRef);

			// do as for horizontal
			bVertexSwap=CalcPoints(true,ldp,pMapper);

			// rotate back
			TSinCos sinCos(angle);
			if(lptRef!=ldp.lptFrom) ldp.lptFrom=mtRotPoint(ldp.lptFrom, sinCos, lptRef);
			else ldp.lptTo=mtRotPoint(ldp.lptTo, sinCos, lptRef);
			ldp.lptAnchor=mtRotPoint(ldp.lptAnchor, sinCos, lptRef);
			ldp.lptDimStart=mtRotPoint(ldp.lptDimStart, sinCos, lptRef);
			ldp.lptDimEnd=mtRotPoint(ldp.lptDimEnd, sinCos, lptRef);
			ldp.lptExtent=mtRotPoint(ldp.lptExtent, sinCos, lptRef);
			ldp.lptWit1Start=mtRotPoint(ldp.lptWit1Start, sinCos, lptRef);
			ldp.lptWit1End=mtRotPoint(ldp.lptWit1End, sinCos, lptRef);
			ldp.lptWit2Start=mtRotPoint(ldp.lptWit2Start, sinCos, lptRef);
			ldp.lptWit2End=mtRotPoint(ldp.lptWit2End, sinCos, lptRef);
		}
		break;
	}

	if(ldp.pDimFmt->bTextCentered)
	{
		ldp.lptAnchor=Average(ldp.lptDimStart,ldp.lptDimEnd);
		ldp.lptExtent=ldp.lptDimEnd;
	}

	ldp.lptText=ldp.lptAnchor;
	if(ldp.pDimFmt->nTextDisplay)
	{
		ldp.lptLeader=ldp.lptExtent;
		ldp.angText=mtAngle(ldp.lptDimStart,ldp.lptDimEnd);
		if(ldp.angText>90 && ldp.angText<=270) ldp.angText=mtFixAngle(ldp.angText-180);
		switch(ldp.pDimFmt->nTextDisplay)
		{
		case 1:		// Along
			ldp.nDSAlign=kDSCenter;
			break;
		case 2:		// Above
			ldp.nDSAlign=kDSBottom;
			ldp.lptText.y-=4;
			break;
		case 3:		// Below
			ldp.nDSAlign=kDSTop;
			ldp.lptText.y+=4;
			break;
		}
		ldp.lptText=mtRotPoint(ldp.lptText,ldp.angText,ldp.lptAnchor);
	}
	else
	{
		ldp.angText=0;
		LongRect lrcExtent;
		lrcExtent.SetToBound(ldp.lptDimStart,ldp.lptDimEnd);
		lrcExtent.Inset(-FixedMinDelta,-FixedMinDelta);

		if(lrcExtent.Contains(ldp.lptAnchor))
		{
			ldp.lptExtent=ldp.lptLeader=ldp.lptDimEnd;
			ldp.nDSAlign=kDSCenter;
		}
		else
		{
			ldp.lptLeader=ldp.lptExtent;
			if(nType!=kHoriz && ldp.pDimFmt->nLeader)
			{
				Coord leader=pMapper->Scalar2Long(ldp.pDimStd->LeaderLen);
				if(ldp.pDimFmt->nLeader==kRight)
				{
					leader=-leader;
					ldp.nDSAlign=kDSRight;
				}
				else ldp.nDSAlign=kDSLeft;
				ldp.lptLeader.x+=leader;
				ldp.lptText=ldp.lptLeader;
			}
			else
			{
				if(nType==kVert) ldp.nDSAlign=(ldp.lptDimStart.y<ldp.lptDimEnd.y)?kDSTop:kDSBottom;
				else
				{
					Angle angle=mtAngle(ldp.lptDimStart,ldp.lptDimEnd);
					ldp.nDSAlign=(angle>90 && angle<=270)?kDSRight:kDSLeft;
				}
				ldp.lptText=ldp.lptAnchor;
			}
		}
	}
	return bVertexSwap;
}

WrFixed CMDLineDim::GetDimValue()
{
	WrFixed value=0;;
	switch(m_nType)
	{
	case kHoriz:
		value=Abs(GetShapePoint(1).x-GetShapePoint(0).x);
		break;

	case kVert:
		value=Abs(GetShapePoint(1).y-GetShapePoint(0).y);
		break;

	case kSlope:
		value=Pythag(GetShapePoint(1)-GetShapePoint(0));
		break;
	}
	return value;
}

void CMDLineDim::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{
   	short				vErrCode = kDxfNoErr;
	PenModel			aPenModel, capsPenModel;
	short				penW;
	Coord				lPen, halfWidth;
	LongPoint			lPt1, lPt2, trueLPt1, trueLPt2;
	LongPoint			p1Line, p2Line, p1, p2;
	short				vDashIdx;
	short				cap1, cap2;
	Angle				rot;
	unsigned short		map;
	Coord				outsideLineLen;
	bool				arrowsInside, drawn = false;


//	id = pShHdl->cID;
//	if (pShHdl->cFormat.dfTextDisplay != kHorizontal)
//		rot = pShHdl->cRot;
//	else
//		rot = FixedZero ;

//	if ((rot >= (k180 - FixedHalf)) && (rot <= (k180 + FixedHalf)))	// 180Ejust like 0
//		rot = FixedZero ;

//	if (!(( rot > FixedHalf ) || (rot < -FixedHalf)))	// within 1/2Eof 0 is 0
//		rot = FixedZero ;

	map = 0;
	outsideLineLen =  0;
	arrowsInside = m_DimFmt.bArrowsInside;
	aPenModel = GetPenModel();

	if (!arrowsInside)
	{
		outsideLineLen = m_DimStd.OutsideLineLen*vDisplayResValue;
		map |= kDimEndmarkOut;
	}

	vDashIdx = aPenModel.Dashing();
	cap1 = m_nDimCap1;
	cap2 = m_nDimCap2;
	penW = DxfWidthConversion(aPenModel.m_nWidth);
	lPen = _guLOrd (penW);
	halfWidth = (lPen >> 1);

	CMapper mapper;

	lPt1 = m_ldp.lptDimStart;
	lPt2 = m_ldp.lptDimEnd;

	trueLPt1 = lPt1;
	trueLPt2 = lPt2;

	if (arrowsInside)	// draw dimension line
	{
		if (cap1 || cap2)
			DxfLineCapped (&lPt1, &lPt2, &capsPenModel, kBothEnd, &lPt1, &lPt2, nZoomFactor);

		if (aPenModel.m_nAlignment != kCenterLine)
			DxfLineBorderPos (&aPenModel, false, &lPt1, &lPt2, &lPt1, &lPt2, nZoomFactor);

		p1Line = lPt1;
		p2Line = lPt2;
		p1 = p1Line;
		p2 = p2Line;

		if (aPenModel.Dashing())
			DxfMakeButtCaps(penW, p1Line, p2Line, &p1, &p2);

		if (! cap1)		p1Line = p1;
		if (! cap2)		p2Line = p2;

  		DxfWriteALine ( pFile, pLayerStr, &aPenModel, &p1Line, &p2Line,
						pDocExtPtr, vUnitFactor, ptrPalette);
	}


//if (LINEARDIM(id))
//
		// draw witness lines
    lPt1= m_ldp.lptWit1Start ;
	lPt2 = m_ldp.lptWit1End ;
	if ( lPt1 != lPt2 )	{
		p1Line = lPt1;
		p2Line = lPt2;
		DxfWriteALine (pFile, pLayerStr, &aPenModel, &p1Line, &p2Line,
					pDocExtPtr, vUnitFactor, ptrPalette);
	}

	lPt1 = m_ldp.lptWit2Start ;
	lPt2 = m_ldp.lptWit2End ;
	if ( lPt1 != lPt2 )	{
		p1Line = lPt1;
		p2Line = lPt2;
		DxfWriteALine (pFile, pLayerStr, &aPenModel, &p1Line, &p2Line,
						pDocExtPtr, vUnitFactor, ptrPalette);
	}
// end lineardim check

	// draw extension line if any

	lPt1 = m_ldp.lptDimEnd;
	lPt2 = m_ldp.lptExtent;
	if ( lPt1 != lPt2 )	{
		p1Line = lPt1;
		p2Line = lPt2;
  		DxfWriteALine (pFile, pLayerStr, &aPenModel, &p1Line, &p2Line,
						pDocExtPtr, vUnitFactor, ptrPalette);
	}

	// draw leader line if any
	lPt1 = m_ldp.lptExtent;//dbPeekLPoint ( pShHdl->GetLeaderLineStart () ) ;
	lPt2 = m_ldp.lptLeader;//dbPeekLPoint ( pShHdl->GetLeaderLineEnd () ) ; ????
	if ( lPt1 != lPt2 )
	{
		p1Line = lPt1;
		p2Line = lPt2;
  		DxfWriteALine (pFile, pLayerStr, &aPenModel, &p1Line, &p2Line,
						pDocExtPtr, vUnitFactor, ptrPalette);
	}

	// draw endcaps
	lPt1 = m_ldp.lptDimStart;
	lPt2 = m_ldp.lptDimEnd;
	trueLPt1 = lPt1;
	trueLPt2 = lPt2;

	if (cap1) {
		map |= kEnd1;
		DxfDrawEndcap (pFile, pLayerStr, &trueLPt1, &trueLPt2, cap1, map,
					   &capsPenModel, false, outsideLineLen,
					   pDocExtPtr, vUnitFactor, nZoomFactor, ptrPalette);
	}
	if (cap2) {
		map &= ~kEnd1;
		map |= kEnd2;
		DxfDrawEndcap (pFile, pLayerStr, &trueLPt1, &trueLPt2, cap2, map,
					  	&capsPenModel, false, outsideLineLen,
					  	pDocExtPtr, vUnitFactor, nZoomFactor, ptrPalette);
	}

	CMDAssocDim::WriteDXF(pFile, 	pLayerStr,	pDocExtPtr,	vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);

/* draw dimension strings
	if ( rot != 0)
	{	if ((vErrCode = writeRotLineDimStr (pFileHdl, pLayerHdl, pLayerStr, pShHdl,
													   		pDocExtPtr, pUnitFactor)) == kDxfErr)
			goto ERR_EXIT;
	}
	else
	{
		if ((vErrCode = writeAssocDimText (pFileHdl, pLayerHdl, pLayerStr, pShHdl,
		 												  		pDocExtPtr, pUnitFactor)) == kDxfErr)
			goto ERR_EXIT;
	}
*/
}

int CMDLineDim::SaveMacDraft(PhIOContext& ctx)
{
	PhDSLineDim ps;
	switch(m_nType)
	{
	case kHoriz:
		ps.fID=PhIDHLineDim;
		break;
	case kVert:
		ps.fID=PhIDVLineDim;
		break;
	case kSlope:
		ps.fID=PhIDSLineDim;
		break;
	case kPerp:
		ps.fID=PhIDPLineDim;
		break;
	}
	ctx.pPhShape=(PhDSFillShape*)&ps;
	Angle savedAngle=m_RotAngle;
	m_RotAngle=m_Angle;
	CMDAssocDim::SaveMacDraft(ctx);
	m_RotAngle=savedAngle;

	ps.refPt1.fUse=0;
	ps.refPt1.fFlags=kPhRealVertex;
	ps.refPt1.fLongPt=m_ldp.lptFrom;
	ps.refPt2.fUse=0;
	ps.refPt2.fFlags=kPhRealVertex;
	ps.refPt2.fLongPt=m_ldp.lptTo;
	ps.anchorPt.fUse=0;
	ps.anchorPt.fFlags=kPhRealVertex;
	ps.anchorPt.fLongPt=m_ldp.lptAnchor;
	ps.witnessStart1.fUse=0;
	ps.witnessStart1.fFlags=kPhRealVertex;
	ps.witnessStart1.fLongPt=m_ldp.lptWit1Start;
	ps.witnessEnd1.fUse=0;
	ps.witnessEnd1.fFlags=kPhRealVertex;
	ps.witnessEnd1.fLongPt=m_ldp.lptWit1End;
	ps.witnessStart2.fUse=0;
	ps.witnessStart2.fFlags=kPhRealVertex;
	ps.witnessStart2.fLongPt=m_ldp.lptWit2Start;
	ps.witnessEnd2.fUse=0;
	ps.witnessEnd2.fFlags=kPhRealVertex;
	ps.witnessEnd2.fLongPt=m_ldp.lptWit2End;
	ps.dimLineStart.fUse=0;
	ps.dimLineStart.fFlags=kPhRealVertex;
	ps.dimLineStart.fLongPt=m_ldp.lptDimStart;
	ps.dimLineEnd.fUse=0;
	ps.dimLineEnd.fFlags=kPhRealVertex;
	ps.dimLineEnd.fLongPt=m_ldp.lptDimEnd;
	ps.extLineStart.fUse=0;
	ps.extLineStart.fFlags=kPhRealVertex;
	ps.extLineStart.fLongPt=m_ldp.lptDimEnd;
	ps.extLineEnd.fUse=0;
	ps.extLineEnd.fFlags=kPhRealVertex;
	ps.extLineEnd.fLongPt=m_ldp.lptExtent;
	ps.leaderLineStart.fUse=0;
	ps.leaderLineStart.fFlags=kPhRealVertex;
	ps.leaderLineStart.fLongPt=m_ldp.lptExtent;
	ps.leaderLineEnd.fUse=0;
	ps.leaderLineEnd.fFlags=kPhRealVertex;
	ps.leaderLineEnd.fLongPt=m_ldp.lptLeader;

	uptr p((char*)&ps);
	p.swapDSShape();

	p.swaps(2); // fm
	p.swaps(3);	p.skip(12);
	p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
	p.swaps(1); p.swapl(10); // dim std
	p.swaplpt(8); 
	p.skip(kDimStrSize+kDimTolStrSize+kDimTolStrSize);
	p.swapDBPoint(13);

	short count=sizeof(PhDSLineDim);
	uptr(&count).swaps(1);
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSLineDim));
	SaveMacDimLabel(ctx);

	SaveMacObjInfo(ctx);
	return 1;
}

void CMDLineDim::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
	int nHandle=TestHandles(pt,pMapper);
	if(nHandle>=0 && nHandle<2)
	{
		vertexes.Add(nHandle);
		shapes.Add(this);
	}
}

void CMDLineDim::OnUpdateLineLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex1,int nCauseVertex2)
{
	if(m_nType==kPerp)
	{
		m_Angle=mtFixAngle(mtAngle(pCauseShape->GetShapePoint(nCauseVertex1),pCauseShape->GetShapePoint(nCauseVertex2))+90);
		LongPoint lptPerp;
		guNearPtLnPt(GetShapePoint(nVertex),pCauseShape->GetShapePoint(nCauseVertex1),pCauseShape->GetShapePoint(nCauseVertex2),&lptPerp);
		CTrackContext* pTrackContext=TrackCreateContext(nVertex, 0, &pView->m_Mapper);
		if(pTrackContext)
		{
			pTrackContext->pView=pView;
			TrackFeedback(pCauseShape->GetShapePoint(nCauseVertex1),pTrackContext);
			TrackComplete(pTrackContext);
			delete pTrackContext;
		}
	}
	else CMDAssocDim::OnUpdateLineLinkedVertex(pView,nVertex,pCauseShape,nCauseVertex1,nCauseVertex2);
}
