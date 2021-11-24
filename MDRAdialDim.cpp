// MDRadialDim.cpp: implementation of the CMDRAdialDim class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDRadialDim.h"

#include "phmath.h"
#include "Mapper.h"
#include "DraftContext.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDDimStr.h"
#include "MDDimLine.h"
#include "Layer.h"
#include "PhIOStruct.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDRadialDim,CMDAssocDim,1)

//////////////////////////////////////////////////////////////////////
CRadialDimTrackContext::~CRadialDimTrackContext()
{
	if(pDimStr) {
		delete pDimStr;
		pDimStr=NULL;
	}
}

CMDRadialDim::CMDRadialDim(const DimStandard& DimStd, const DimFormat& DimFmt, BYTE nCap1, BYTE nCap2, bool bDiameter, const LongPoint& lptCenter, Coord radius, const LongPoint& lptAnchor) : CMDAssocDim(DimStd,DimFmt,nCap1,nCap2), m_rdp(&m_DimStd,&m_DimFmt)
{
	m_bDiameter=bDiameter;
	m_rdp.lptFrom=lptCenter;
	m_rdp.Radius=radius;
	m_rdp.lptAnchor=lptAnchor;
}

void CMDRadialDim::Serialize(CArchive& ar)
{
	CMDAssocDim::Serialize(ar);
	if(ar.IsLoading())
	{
		ar >> m_rdp.lptFrom >> m_rdp.lptAnchor;
		ar >> m_rdp.lptDimStart >> m_rdp.lptDimEnd >> m_rdp.lptExtent >> m_rdp.lptLeader;
		ar >> m_rdp.lptText >> m_rdp.nDSAlign >> m_rdp.angText;
        BYTE getData;
        ar >> m_rdp.Radius >> getData;
        m_bDiameter = getData == TRUE;
		m_rdp.pDimStd=&m_DimStd;
		m_rdp.pDimFmt=&m_DimFmt;
	}
	else
	{
		ar << m_rdp.lptFrom << m_rdp.lptAnchor;
		ar << m_rdp.lptDimStart << m_rdp.lptDimEnd << m_rdp.lptExtent << m_rdp.lptLeader;
		ar << m_rdp.lptText << m_rdp.nDSAlign << m_rdp.angText;
		ar << m_rdp.Radius << (BYTE) ( m_bDiameter ? 1 : 0);
	}
}

void CMDRadialDim::CalcCenterDatum(LongPoint& lpt)
{
	lpt=m_rdp.lptFrom;
}

LongRect CMDRadialDim::CalculateBasicExtents() const
{
	LongRect lrc=CMDAssocDim::CalculateBasicExtents();
	lrc|=m_pDimStr->CalculateBasicExtents();
	return lrc;
}

LongPoint CMDRadialDim::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapePoints());
	switch(nIndex)
	{
	case 0:
		return m_rdp.lptFrom;
	case 1:
		return m_rdp.lptAnchor;
	case 2:
		return m_rdp.lptAnchor;
	case 3:
		return m_rdp.lptDimStart;
	case 4:
		return m_rdp.lptDimEnd;
	case 5:
		return m_rdp.lptExtent;
	case 6:
		return m_rdp.lptLeader;
	case 7:
		return m_rdp.lptText;
	}
	return LongPoint::kZeroPt;
}

void CMDRadialDim::SetShapePoint(int nIndex, const LongPoint &lpt)
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapePoints());
	switch(nIndex)
	{
	case 0:
		m_rdp.lptFrom=lpt;
		break;
	case 1:
		break;
	case 2:
		m_rdp.lptAnchor=lpt;
		break;
	case 3:
		m_rdp.lptDimStart=lpt;
		break;
	case 4:
		m_rdp.lptDimEnd=lpt;
		break;
	case 5:
		m_rdp.lptExtent=lpt;
		break;
	case 6:
		m_rdp.lptLeader=lpt;
		break;
	case 7:
		m_rdp.lptText=lpt;
		break;
	}
}

LongPoint CMDRadialDim::GetShapeHandle(int nIndex) const
{
	switch(nIndex)
	{
	case 0:
		return m_bDiameter?m_rdp.lptFrom+mtRotPoint(LongPoint(m_rdp.Radius,0),mtAngle(m_rdp.lptAnchor,m_rdp.lptFrom)):m_rdp.lptFrom;
	case 1:
		return m_rdp.lptFrom+mtRotPoint(LongPoint(m_rdp.Radius,0),mtAngle(m_rdp.lptFrom,m_rdp.lptAnchor));
	}

	return CMDAssocDim::GetShapeHandle(nIndex);
}

void CMDRadialDim::DrawShape(CDraftContext* pDC) const
{
	Gdiplus::GraphicsPath* pClip=m_pDimStr->GetClip(pDC);
	pDC->SetPen(m_PenModel);
	pDC->DrawRadialDim(m_rdp,m_bDiameter?m_nDimCap1:0,m_nDimCap2,pClip);
	if(pClip) delete pClip;
//	DrawShapeOutline(pDC);
	DrawDimStr(pDC);
}

void CMDRadialDim::DrawShapeOutline(CDraftContext* pDC) const
{
	pDC->DrawRadialDim(m_rdp,m_bDiameter?m_nDimCap1:0,m_nDimCap2);
}

bool CMDRadialDim::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
	pointd pt1,pt2;
	pt1=pMapper->LPtoDPF(m_rdp.lptDimStart);
	pt2=pMapper->LPtoDPF(m_rdp.lptDimEnd);
	if(guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment)) return true;
	pt1=pMapper->LPtoDPF(m_rdp.lptExtent);
	if(guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment)) return true;
	pt2=pMapper->LPtoDPF(m_rdp.lptLeader);
	if(guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment)) return true;

	return false;
}

void CMDRadialDim::DoShapeScale(const LongRect& lrcFrom,const LongRect& lrcTo,CMDShape** ppNewShape)
{
	LongRatio xRatio,yRatio;
	xRatio.fN=(lrcTo.right-lrcTo.left).GetFixed();
	xRatio.fD=(lrcFrom.right-lrcFrom.left).GetFixed();
	yRatio.fN=(lrcTo.bottom-lrcTo.top).GetFixed();
	yRatio.fD=(lrcFrom.bottom-lrcFrom.top).GetFixed();
	LongPoint lptRadius=mtRotPoint(LongPoint(m_rdp.Radius,0),mtAngle(m_rdp.lptFrom,m_rdp.lptAnchor));
	lptRadius.x*=xRatio;
	lptRadius.y*=yRatio;
	m_rdp.Radius=Pythag(lptRadius);

	CMDAssocDim::DoShapeScale(lrcFrom,lrcTo,ppNewShape);
}

void CMDRadialDim::DoShapeScale(const LongRatio& ratio,double factor)
{
	m_rdp.Radius*=ratio;
	if(factor) m_rdp.Radius*=factor;
	CMDAssocDim::DoShapeScale(ratio,factor);
}

void CMDRadialDim::DoShapeRotate(Angle angle, short nAbout,const LongPoint& lptRef)
{
	LongPoint lptRot;
	switch(nAbout)
	{
	case kAboutCenter:
		lptRot=m_rdp.lptFrom;
		break;
	case kAboutDatum:
		lptRot=GetShapeDatum();
		break;
	case kAboutRefPt:
		lptRot=lptRef;
		break;
	default:
		ASSERT(0);
	}

	angle=mtFixAngle(angle);

	m_rdp.lptFrom=mtRotPoint(m_rdp.lptFrom,angle,lptRot);
	m_rdp.lptAnchor=mtRotPoint(m_rdp.lptAnchor,angle,lptRot);
	if(nAbout!=kAboutDatum) m_lptDatum=mtRotPoint(m_lptDatum,angle,lptRot);
	CalculateExtents();
}

void CMDRadialDim::UpdateDependentShapes(CPCDraftView *pView)
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
			Coord dimValue=m_bDiameter?m_rdp.Radius<<1:m_rdp.Radius;
			if(m_DimFmt.nRoundoff>0 && m_DimFmt.nTolerance<3)
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
				//sTol1="±"+pdu->numUser2Str(pdu->numDB2User(tolRatio,pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)),true); Dennis
#ifdef _JAPANESE
				sTol1 = "Å}" + pdu->numUser2Str(pdu->numDB2User(tolRatio, pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)), true);
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

			if(sValue.GetLength())
			{
				CString strSymbol;
				switch(m_DimFmt.nRDSymbol)
				{
				case kLeading:
					strSymbol.LoadString(m_bDiameter?IDS_LEADINGDIAMETER:IDS_LEADINGRADIUS);
					sValue=strSymbol+sValue;
					break;
				case kTrailing:
					strSymbol.LoadString(m_bDiameter?IDS_TRAILINGDIAMETER:IDS_TRAILINGRADIUS);
					sValue+=strSymbol;
					break;
				}
			}
		}
		CDC* pDC=pView->GetDC();	
		{
			CDraftContext dc(pDC,&pView->m_Mapper);
			dc.SetDimStandard(&m_DimStd);
			m_pDimStr->SetValues(&dc,sValue,sTol1,sTol2);
		}
		pView->ReleaseDC(pDC);

		m_pDimStr->SetRotation(m_rdp.angText);
		m_pDimStr->SetAlign(m_rdp.nDSAlign);
		m_pDimStr->SetShapePoint(0,m_rdp.lptText);
		m_pDimStr->CalculateExtents();
	}
}

void CMDRadialDim::Unglue(CMDShapeList* pList) const
{
	CMDShape* pShape;

	pShape=new CMDDimLine(m_rdp.lptDimStart,m_rdp.lptDimEnd,(m_DimFmt.nTextDisplay<kBelow)?m_DimFmt.nTextDisplay:kAbove);
	CMDDimStr* pDimStr=new CMDDimStr(true,pShape);
	pShape->Attach(pDimStr);
	pDimStr->SetShapePoint(0,Average(m_rdp.lptDimStart,m_rdp.lptDimEnd));
	pDimStr->SetShapeDatum(kDefaultDatum);
	pDimStr->CalculateExtents();
	pList->AddTail(pDimStr);

	PenModel pm=GetPenModel();
	if(m_bDiameter)	pm.m_nEndCap1=m_nDimCap1;
	pm.m_nEndCap2=m_nDimCap2;
	pShape->SetPenModel(pm);
	pShape->SetFillModel(m_pDimStr->GetFillModel());
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	pList->AddTail(pShape);

	if(m_rdp.lptDimEnd!=m_rdp.lptExtent)
	{
		pShape=new CMDLineShape(m_rdp.lptDimEnd,m_rdp.lptExtent);
		pShape->SetPenModel(GetPenModel());
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pShape);

		if(m_rdp.lptExtent!=m_rdp.lptLeader)
		{
			pShape=new CMDLineShape(m_rdp.lptExtent,m_rdp.lptLeader);
			pShape->SetPenModel(GetPenModel());
			pShape->CalculateExtents();
			pShape->SetShapeDatum(kDefaultDatum);
			pList->AddTail(pShape);
		}
	}
}

CTrackContext* CMDRadialDim::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CRadialDimTrackContext* p=new CRadialDimTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	p->rdp=m_rdp;
	p->pDimStr=(CMDDimStr*)m_pDimStr->CreateCopy();
	if(nWhichHandle==kRotateHandle)
	{
		CRotateContext* pCtx=(CRotateContext*)lParam;
		LongPoint lptRot;
		switch(pCtx->m_nAbout)
		{
		case kAboutCenter:
			lptRot=m_rdp.lptFrom;
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
		p->rdp.lptFrom=mtRotPoint(p->rdp.lptFrom, pCtx->m_Angle, lptRot);
		p->rdp.lptAnchor=mtRotPoint(p->rdp.lptAnchor, pCtx->m_Angle, lptRot);
		p->pDimStr->SetShapePoint(0,mtRotPoint(p->pDimStr->GetShapePoint(0), pCtx->m_Angle, lptRot));
	}
	CalcPoints(m_bDiameter,p->rdp,pMapper);
	return p;
}

void CMDRadialDim::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	int nHandle=pTrackContext->nWhichHandle;
	CRadialDimTrackContext* p=(CRadialDimTrackContext*)pTrackContext;
	if(nHandle==kMoveHandle)
	{
		p->rdp.lptFrom+=pt;
		p->rdp.lptAnchor+=pt;
		CalcPoints(m_bDiameter,p->rdp,&p->pView->m_Mapper);
		p->pDimStr->DoShapeOffset(pt);
	}
	else
	{
		Angle angle=mtAngle(m_rdp.lptFrom,pt);
		CMDShape* pLinkedShape;
		if(nHandle==2)
		{
			p->rdp.lptAnchor=pt;
			if(p->pView->IsLinkedShape(this,kCenterHandle,&pLinkedShape))
				CalcRadius(p->rdp,pLinkedShape);
		}
		else
		{
			if(m_bDiameter)
			{
				switch(nHandle)
				{
				case 0:
					p->rdp.lptAnchor=m_rdp.lptFrom+mtRotPoint(LongPoint(-Pythag(m_rdp.lptAnchor-m_rdp.lptFrom),0),angle);
					break;
				case 1:
					p->rdp.lptAnchor=m_rdp.lptFrom+mtRotPoint(LongPoint(Pythag(m_rdp.lptAnchor-m_rdp.lptFrom),0),angle);
					break;
				}
				if(p->pView->IsLinkedShape(this,kCenterHandle,&pLinkedShape))
					CalcRadius(p->rdp,pLinkedShape);
				else
				{
					p->rdp.lptFrom=m_rdp.lptFrom+mtRotPoint(LongPoint(-m_rdp.Radius,0),angle);
					p->rdp.lptFrom=Average(p->rdp.lptFrom,pt);
					p->rdp.Radius=Pythag(pt-p->rdp.lptFrom);
				}
			}
			else if(nHandle==1)
			{
				p->rdp.lptAnchor=m_rdp.lptFrom+mtRotPoint(LongPoint(Pythag(m_rdp.lptAnchor-m_rdp.lptFrom),0),angle);
				if(p->pView->IsLinkedShape(this,kCenterHandle,&pLinkedShape))
					CalcRadius(p->rdp,pLinkedShape);
				else p->rdp.Radius=Pythag(pt-m_rdp.lptFrom);
			}
		}
		CalcPoints(m_bDiameter,p->rdp,&p->pView->m_Mapper);

		CString sValue,sTol1,sTol2;
		if(m_DimFmt.bUseLabel) sValue=m_DimFmt.sLabel;
		else
		{
			CPCDraftDoc* pDoc=pTrackContext->pView->GetDocument();
			LongRatio ratio,tolRatio;
			pDoc->GetActiveScaleRatio(ratio);
			tolRatio.fN=1; tolRatio.fD=1;
			CDimUnit* pdu=pDoc->GetDimUnit();
			Coord dimValue=m_bDiameter?p->rdp.Radius<<1:p->rdp.Radius;
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
				sTol1 = "Å}" + pdu->numUser2Str(pdu->numDB2User(tolRatio, pTrackContext->pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)), true);
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
				sTol1=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue+pTrackContext->pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance1)),true);
				sTol2=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue-pTrackContext->pView->m_Mapper.Scalar2Long(m_DimFmt.Tolerance2)),true);
				break;
			}

			if(sValue.GetLength())
			{
				CString strSymbol;
				switch(m_DimFmt.nRDSymbol)
				{
				case kLeading:
					strSymbol.LoadString(m_bDiameter?IDS_LEADINGDIAMETER:IDS_LEADINGRADIUS);
					sValue=strSymbol+sValue;
					break;
				case kTrailing:
					strSymbol.LoadString(m_bDiameter?IDS_TRAILINGDIAMETER:IDS_TRAILINGRADIUS);
					sValue+=strSymbol;
					break;
				}
			}
		}
		CDC* pDC=pTrackContext->pView->GetDC();
		{
			CDraftContext dc(pDC,&pTrackContext->pView->m_Mapper);
			dc.SetDimStandard(&m_DimStd);
			p->pDimStr->SetValues(&dc,sValue,sTol1,sTol2);
		}
		pTrackContext->pView->ReleaseDC(pDC);

		p->pDimStr->SetRotation(p->rdp.angText);
		p->pDimStr->SetAlign(p->rdp.nDSAlign);
		p->pDimStr->SetShapePoint(0,p->rdp.lptText);
		p->pDimStr->CalculateExtents();
	}
}

void CMDRadialDim::TrackShowSize(CTrackContext* pTrackContext)
{
	CRadialDimTrackContext* p=(CRadialDimTrackContext*)pTrackContext;
}

void CMDRadialDim::TrackComplete(CTrackContext* pTrackContext)
{
	CRadialDimTrackContext* p=(CRadialDimTrackContext*)pTrackContext;
	m_rdp=p->rdp;
	delete m_pDimStr;
	m_pDimStr=p->pDimStr;
	p->pDimStr = NULL;
	CalculateExtents();
}

void CMDRadialDim::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CRadialDimTrackContext* p=(CRadialDimTrackContext*)pTrackContext;

	pDC->DrawRadialDim(p->rdp);
	p->pDimStr->DrawShapeOutline(pDC);
}

const ResizeInfo* CMDRadialDim::GetResizeInfo() const
{
static ResizeInfo dimResize =
{
	IDS_DIMENSION, 1, {IDS_LENGTH,kUnitlessValue}
};
	return &dimResize;
}

void CMDRadialDim::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	CMDAssocDim::GetResizeValue(pValues,nHandle);
	pValues[0].m_Unitless=m_bDiameter?m_rdp.Radius<<1:m_rdp.Radius;
}

void CMDRadialDim::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	m_rdp.Radius=m_bDiameter?(pValues[0].m_Unitless>>1):pValues[0].m_Unitless;
	CMDAssocDim::SetResizeValue(pValues);
}

void CMDRadialDim::CalcPoints(CMapper* pMapper)
{
	CalcPoints(m_bDiameter,m_rdp,pMapper);
}

void CMDRadialDim::ReCalc(CMapper* pMapper)
{
	CalcPoints(m_bDiameter,m_rdp,pMapper);
	m_pDimStr->SetRotation(m_rdp.angText);
	m_pDimStr->SetAlign(m_rdp.nDSAlign);
	m_pDimStr->SetShapePoint(0,m_rdp.lptText);
	m_pDimStr->CalculateExtents();
	CalculateExtents();
}

//////////////////////////////////////////////////////////////////////

void CMDRadialDim::CalcPoints(bool bDiameter,RadialDimPoints& rdp,CMapper* pMapper)
{
	rdp.lptDimStart=rdp.lptDimEnd=rdp.lptFrom;

	Angle theta=mtAngle(rdp.lptDimStart,rdp.lptAnchor);
	rdp.lptDimEnd.x+=rdp.Radius;
	rdp.lptDimEnd=mtRotPoint(rdp.lptDimEnd,theta,rdp.lptFrom);

	if(bDiameter)
	{
		rdp.lptDimStart.x-=rdp.Radius;	// place at 180ÅE rotate by theta
		rdp.lptDimStart=mtRotPoint(rdp.lptDimStart,theta,rdp.lptFrom);
	}
	rdp.lptExtent=rdp.Radius<Pythag(rdp.lptAnchor-rdp.lptFrom)?rdp.lptAnchor:rdp.lptDimEnd;

	if(rdp.pDimFmt->bTextCentered)
	{
		rdp.lptAnchor=Average(rdp.lptDimStart,rdp.lptDimEnd);
		rdp.lptExtent=rdp.lptDimEnd;
	}

	rdp.lptText=rdp.lptAnchor;
	if(rdp.pDimFmt->nTextDisplay)
	{
		rdp.lptLeader=rdp.lptExtent;
		rdp.angText=mtAngle(rdp.lptDimStart,rdp.lptDimEnd);
		if(rdp.angText>90 && rdp.angText<=270) rdp.angText=mtFixAngle(rdp.angText-180);
		switch(rdp.pDimFmt->nTextDisplay)
		{
		case 1:		// Along
			rdp.nDSAlign=kDSCenter;
			break;
		case 2:		// Above
			rdp.nDSAlign=kDSBottom;
			rdp.lptText.y-=4;
			break;
		case 3:		// Below
			rdp.nDSAlign=kDSTop;
			rdp.lptText.y+=4;
			break;
		}
		rdp.lptText=mtRotPoint(rdp.lptText,rdp.angText,rdp.lptAnchor);
	}
	else
	{
		rdp.angText=0;
		LongRect lrcExtent;
		lrcExtent.SetToBound(rdp.lptDimStart,rdp.lptDimEnd);
		lrcExtent.Inset(-FixedMinDelta,-FixedMinDelta);

		if(lrcExtent.Contains(rdp.lptAnchor))
		{
			rdp.lptExtent=rdp.lptLeader=rdp.lptDimEnd;
			rdp.lptText=rdp.lptAnchor;
			rdp.nDSAlign=kDSCenter;
		}
		else
		{
			rdp.lptExtent=rdp.lptLeader=rdp.lptAnchor;

			if(rdp.pDimFmt->nLeader==kNoLeader)
			{
				Angle angle=mtAngle(rdp.lptDimEnd,rdp.lptDimStart);
				rdp.nDSAlign=(angle>90 && angle<=270)?kDSRight:kDSLeft;
			}
			else
			{
				if(rdp.pDimFmt->nTextDisplay==kHorizontal)
				{
					Coord leader=pMapper->Scalar2Long(rdp.pDimStd->LeaderLen);
					if(rdp.pDimFmt->nLeader==kRight)
					{
						leader=-leader;
						rdp.nDSAlign=kDSRight;
					}
					else rdp.nDSAlign=kDSLeft;
					rdp.lptLeader.x+=leader;
				}
			}
			rdp.lptText=rdp.lptLeader;
		}
	}
}

void CMDRadialDim::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
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

//	if ((rot >= (k180 - FixedHalf)) && (rot <= (k180 + FixedHalf)))	// 180ÅEjust like 0
//		rot = FixedZero ;

//	if (!(( rot > FixedHalf ) || (rot < -FixedHalf)))	// within 1/2ÅEof 0 is 0
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
	penW = aPenModel.m_nWidth;
	lPen = _guLOrd (penW);
	halfWidth = (lPen >> 1);

//	RadialDimPoints rdp(m_lptCenter,m_Radius,m_lptAnchor,&m_DimStd,&m_DimFmt);
//	CMapper mapper;
//	CalcPoints(m_bDiameter,rdp,&mapper);

	lPt1 = m_rdp.lptDimStart;
	lPt2 = m_rdp.lptDimEnd;

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


	// draw extension line if any
	
	lPt1 = m_rdp.lptDimEnd;
	lPt2 = m_rdp.lptExtent;
	if ( lPt1 != lPt2 )	{
		p1Line = lPt1;
		p2Line = lPt2;
  		DxfWriteALine (pFile, pLayerStr, &aPenModel, &p1Line, &p2Line,
						pDocExtPtr, vUnitFactor, ptrPalette);
	}

	// draw leader line if any
	lPt1 = m_rdp.lptExtent;//dbPeekLPoint ( pShHdl->GetLeaderLineStart () ) ;
	lPt2 = m_rdp.lptLeader;//dbPeekLPoint ( pShHdl->GetLeaderLineEnd () ) ; ????
	if ( lPt1 != lPt2 )
	{
		p1Line = lPt1;
		p2Line = lPt2;
  		DxfWriteALine (pFile, pLayerStr, &aPenModel, &p1Line, &p2Line,
						pDocExtPtr, vUnitFactor, ptrPalette);
	}

	// draw endcaps
	lPt1 = m_rdp.lptDimStart;
	lPt2 = m_rdp.lptDimEnd;
	trueLPt1 = lPt1;
	trueLPt2 = lPt2;

	if (m_bDiameter && cap1) {
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

int CMDRadialDim::SaveMacDraft(PhIOContext& ctx)
{
	PhDSLineDim ps;
	ps.fID=m_bDiameter?PhIDDRadialDim:PhIDRRadialDim;
	ctx.pPhShape=(PhDSFillShape*)&ps;
	CMDAssocDim::SaveMacDraft(ctx);

	ps.refPt1.fUse=0;
	ps.refPt1.fFlags=kPhRealVertex;
	ps.refPt1.fLongPt=m_rdp.lptFrom;
	ps.refPt2.fUse=0;
	ps.refPt2.fFlags=kPhRealVertex;
	ps.refPt2.fLongPt=m_rdp.lptFrom;;
	ps.anchorPt.fUse=0;
	ps.anchorPt.fFlags=kPhRealVertex;
	ps.anchorPt.fLongPt=m_rdp.lptAnchor;
	ps.witnessStart1.fUse=0;
	ps.witnessStart1.fFlags=kPhRealVertex;
	ps.witnessStart1.fLongPt=LongPoint::kZeroPt;
	ps.witnessEnd1.fUse=0;
	ps.witnessEnd1.fFlags=kPhRealVertex;
	ps.witnessEnd1.fLongPt=LongPoint::kZeroPt;
	ps.witnessStart2.fUse=0;
	ps.witnessStart2.fFlags=kPhRealVertex;
	ps.witnessStart2.fLongPt=LongPoint::kZeroPt;
	ps.witnessEnd2.fUse=0;
	ps.witnessEnd2.fFlags=kPhRealVertex;
	ps.witnessEnd2.fLongPt=LongPoint::kZeroPt;
	ps.dimLineStart.fUse=0;
	ps.dimLineStart.fFlags=kPhRealVertex;
	ps.dimLineStart.fLongPt=m_rdp.lptDimStart;
	ps.dimLineEnd.fUse=0;
	ps.dimLineEnd.fFlags=kPhRealVertex;
	ps.dimLineEnd.fLongPt=m_rdp.lptDimEnd;
	ps.extLineStart.fUse=0;
	ps.extLineStart.fFlags=kPhRealVertex;
	ps.extLineStart.fLongPt=m_rdp.lptDimEnd;
	ps.extLineEnd.fUse=0;
	ps.extLineEnd.fFlags=kPhRealVertex;
	ps.extLineEnd.fLongPt=m_rdp.lptExtent;
	ps.leaderLineStart.fUse=0;
	ps.leaderLineStart.fFlags=kPhRealVertex;
	ps.leaderLineStart.fLongPt=m_rdp.lptExtent;
	ps.leaderLineEnd.fUse=0;
	ps.leaderLineEnd.fFlags=kPhRealVertex;
	ps.leaderLineEnd.fLongPt=m_rdp.lptLeader;

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

void CMDRadialDim::OnUpdateLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex)
{
	if(pCauseShape==this) return;
	if(nVertex==kCenterHandle)
	{
		CTrackContext* pTrackContext=TrackCreateContext(2, 0, &pView->m_Mapper);
		if(pTrackContext)
		{
			pTrackContext->pView=pView;
			((CRadialDimTrackContext*)pTrackContext)->rdp.lptFrom=pCauseShape->GetCenter();
//			CalcRadius(((CRadialDimTrackContext*)pTrackContext)->rdp,pCauseShape);
			TrackFeedback(((CRadialDimTrackContext*)pTrackContext)->rdp.lptAnchor,pTrackContext);
			TrackComplete(pTrackContext);
			delete pTrackContext;
		}
	}
	else CMDAssocDim::OnUpdateLinkedVertex(pView,nVertex,pCauseShape,nCauseVertex);
}

void CMDRadialDim::GetVertexesFromPt(CUIntArray& vertexes, CMDShapeArray& shapes, const pointd& pt, CMapper* pMapper)
{
	if(m_bDiameter)
		return;
	int nHandle=TestHandles(pt,pMapper);
	if(0==nHandle)
	{
		vertexes.Add(0);
		shapes.Add(this);
	}
}

void CMDRadialDim::CalcRadius(RadialDimPoints& rdp,CMDShape *pShape)
{
	LongPoint radii=pShape->GetRadii();
	if(radii.x==radii.y) rdp.Radius=radii.x;
	else
	{
/*		Angle radAngle=mtEATan2(rdp.lptAnchor-rdp.lptFrom,radii.x,radii.y);
		radAngle-=pShape->GetRotation();	// the angle is now relative to the ellipse

		radii.x=mtFixMulDbl(radii.x,mtCos(radAngle));
		radii.y=mtFixMulDbl(radii.y,mtSin(radAngle));

		rdp.Radius=Pythag(radii);
*/
		Angle radAngle=mtATan2(rdp.lptAnchor-rdp.lptFrom);
		radAngle-=pShape->GetRotation();	// the angle is now relative to the ellipse

		LongPoint rad(256.0,0.0);	// h value doesnt matter much as long as it is > 0
		rad=mtRotPoint(rad,radAngle);
		
		Angle vAng=mtEATan2(rad,radii.x,radii.y);

		rad.x=mtFixMulDbl(radii.x,mtCos(vAng));
		rad.y=mtFixMulDbl(radii.y,mtSin(vAng));

		rad=mtRotPoint(rad,pShape->GetRotation());
		rdp.Radius=Pythag(rad);
	}
}
