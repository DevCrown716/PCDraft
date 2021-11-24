// MDAngularDim.cpp: implementation of the CMDAngularDim class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "MDAngularDim.h"

#include "gu.h"
#include "phmath.h"
#include "DraftContext.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDDimStr.h"

#include "MDArcShape.h"
#include "MDLineShape.h"
#include "DSShape.h"
#include "PhIOStruct.h"
#include "ArcEdge.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_SERIAL(CMDAngularDim,CMDAssocDim,1)

//////////////////////////////////////////////////////////////////////
CAngDimTrackContext::~CAngDimTrackContext()
{
	if(pDimStr) {
		delete pDimStr;
		pDimStr=NULL;
	}
}

CMDAngularDim::CMDAngularDim(const DimStandard& DimStd,const DimFormat& DimFmt,BYTE nCap1,BYTE nCap2,bool bSmallerArc) : CMDAssocDim(DimStd,DimFmt,nCap1,nCap2), m_adp(&m_DimStd,&m_DimFmt)
{
	m_bSmallerArc=bSmallerArc;
}

void CMDAngularDim::Serialize(CArchive& ar)
{
	CMDAssocDim::Serialize(ar);
	if(ar.IsLoading())
	{
        BYTE dataGet;
        ar >> dataGet; 
        m_bSmallerArc = dataGet == TRUE;
		ar >> m_adp.lptFrom >> m_adp.lptTo >> m_adp.lptAnchor >> m_adp.lptCenter;

		ar >> m_adp.lptDimStart >> m_adp.lptDimEnd >> m_adp.lptExtent >> m_adp.lptLeader;
		ar >> m_adp.lptWit1Start >> m_adp.lptWit1End >> m_adp.lptWit2Start >> m_adp.lptWit2End;
		ar >> m_adp.lptText >> m_adp.nDSAlign >> m_adp.angText >> m_adp.nDirection;
		m_adp.pDimStd=&m_DimStd;
		m_adp.pDimFmt=&m_DimFmt;
	}
	else
	{
		ar << (BYTE)(m_bSmallerArc?1:0);
		ar << m_adp.lptFrom << m_adp.lptTo << m_adp.lptAnchor << m_adp.lptCenter;

		ar << m_adp.lptDimStart << m_adp.lptDimEnd << m_adp.lptExtent << m_adp.lptLeader;
		ar << m_adp.lptWit1Start << m_adp.lptWit1End << m_adp.lptWit2Start << m_adp.lptWit2End;
		ar << m_adp.lptText << m_adp.nDSAlign << m_adp.angText << m_adp.nDirection;
	}
}

LongRect CMDAngularDim::CalculateBasicExtents() const
{
	LongBoundRect lbr;
	LongPoint lptRadii;
	lptRadii.x=lptRadii.y=Pythag(m_adp.lptAnchor-m_adp.lptCenter);
	CEdge* pEdge=new CArcEdge(m_adp.lptCenter,lptRadii,m_adp.lptDimStart,m_adp.lptExtent,m_adp.nDirection);
	lbr|=pEdge->GetExtents();
	delete pEdge;
	for(int i=0;i<GetNumShapePoints()-1;i++) lbr|=GetShapePoint(i);
	lbr|=m_pDimStr->CalculateBasicExtents();
	return lbr;
}

LongPoint CMDAngularDim::GetShapePoint(int nIndex) const
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapePoints());
	switch(nIndex)
	{
	case 0:
		return m_adp.lptFrom;
	case 1:
		return m_adp.lptTo;
	case 2:
		return m_adp.lptAnchor;
	case 3:
		return m_adp.lptDimStart;
	case 4:
		return m_adp.lptDimEnd;
	case 5:
		return m_adp.lptExtent;
	case 6:
		return m_adp.lptLeader;
	case 7:
		return m_adp.lptText;
	case 8:
		return m_adp.lptWit1Start;
	case 9:
		return m_adp.lptWit1End;
	case 10:
		return m_adp.lptWit2Start;
	case 11:
		return m_adp.lptWit2End;
	case 12:
		return m_adp.lptCenter;
	}
	return LongPoint::kZeroPt;
}

void CMDAngularDim::SetShapePoint(int nIndex, const LongPoint &lpt)
{
	ASSERT(nIndex>=0 && nIndex<GetNumShapePoints());
	switch(nIndex)
	{
	case 0:
		m_adp.lptFrom=lpt;
		break;
	case 1:
		m_adp.lptTo=lpt;
		break;
	case 2:
		m_adp.lptAnchor=lpt;
		break;
	case 3:
		m_adp.lptDimStart=lpt;
		break;
	case 4:
		m_adp.lptDimEnd=lpt;
		break;
	case 5:
		m_adp.lptExtent=lpt;
		break;
	case 6:
		m_adp.lptLeader=lpt;
		break;
	case 7:
		m_adp.lptText=lpt;
		break;
	case 8:
		m_adp.lptWit1Start=lpt;
		break;
	case 9:
		m_adp.lptWit1End=lpt;
		break;
	case 10:
		m_adp.lptWit2Start=lpt;
		break;
	case 11:
		m_adp.lptWit2End=lpt;
		break;
	case 12:
		m_adp.lptCenter=lpt;
		break;
	}
}

void CMDAngularDim::DrawShape(CDraftContext* pDC) const
{
	Gdiplus::GraphicsPath* pClip=m_pDimStr->GetClip(pDC);
	pDC->SetPen(m_PenModel);
   
   auto bOutlinePen = pDC->IsOutlinePen();

   pDC->SetOutlinePen();

   try
   {

      pDC->DrawAngDim(m_adp, m_nDimCap1, m_nDimCap2, pClip);

   }
   catch (...)
   {


   }
   pDC->SetOutlinePen(bOutlinePen);


	if(pClip) delete pClip;
//	DrawShapeOutline(pDC);
	DrawDimStr(pDC);
}

void CMDAngularDim::DrawShapeOutline(CDraftContext* pDC) const
{
	pDC->DrawAngDim(m_adp,m_nDimCap1,m_nDimCap2);
}

bool CMDAngularDim::PointInShape(const pointd& pt, CMapper* pMapper) const
{
	int nWidth=pMapper->ScreenPenW(m_PenModel.m_nWidth);
	pointd pt1,pt2;
	pMapper->LPtoDP(pt1, m_adp.lptWit1Start);
	pMapper->LPtoDP(pt2, m_adp.lptWit1End);
	if(guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment)) return true;
	pMapper->LPtoDP(pt1, m_adp.lptWit2Start);
	pMapper->LPtoDP(pt2,m_adp.lptWit2End);
	if(guPtAtLine(pt,pt1,pt2,nWidth,m_PenModel.m_nAlignment)) return true;

   pointd ptCenter;
   pMapper->LPtoDP(ptCenter,m_adp.lptCenter);
	short radius=(short)pMapper->LPtoDP(Pythag(m_adp.lptDimStart-m_adp.lptCenter));
   LongPoint lp;
   lp=pMapper->DPtoLP(pt);
	Angle angle=mtAngle(m_adp.lptCenter, lp);
	Angle startAngle=mtFixAngle(mtAngle(m_adp.lptCenter, m_adp.lptDimStart)-angle);
	Angle endAngle=mtFixAngle(mtAngle(m_adp.lptCenter, m_adp.lptExtent)-angle);
	if((endAngle>startAngle) == (m_adp.nDirection==AD_CLOCKWISE) && guPtAtEllip(pt,ptCenter,radius,radius,0,nWidth,Alignment(m_PenModel.m_nAlignment),false)) return true;

	return false;
}

void CMDAngularDim::UpdateDependentShapes(CPCDraftView *pView)
{
	CMDAssocDim::UpdateDependentShapes(pView);
	if(m_pDimStr)
	{
		CString sValue,sTol1,sTol2;
		if(m_DimFmt.bUseLabel) sValue=m_DimFmt.sLabel;
		else
		{
			CDimUnit* pdu=pView->GetDocument()->GetDimUnit();
			Angle arcAngle=Abs(mtAngle(m_adp.lptCenter,m_adp.lptFrom)-mtAngle(m_adp.lptCenter,m_adp.lptTo));
			if(arcAngle<180 != m_bSmallerArc) arcAngle=360-arcAngle;
			switch(m_DimFmt.nTolerance)
			{
			case kNoTolerance:
				sValue=pdu->makeADistStr(arcAngle,false);
				break;
			case kOne:
				sValue=pdu->makeADistStr(arcAngle,false);
				//sTol1="±"+pdu->makeADistStr(m_DimFmt.Tolerance1,false); Dennis
#ifdef _JAPANESE
				sTol1 = "}" + pdu->makeADistStr(m_DimFmt.Tolerance1, false);
#else
				sTol1 = "±" + pdu->makeADistStr(m_DimFmt.Tolerance1, false);
#endif
				break;
			case kTwo:
				sValue=pdu->makeADistStr(arcAngle,false);
				sTol1="+"+pdu->makeADistStr(m_DimFmt.Tolerance1,false);
				sTol2="-"+pdu->makeADistStr(m_DimFmt.Tolerance2,false);
				break;
			case kLimits:
				sTol1=pdu->makeADistStr(arcAngle+m_DimFmt.Tolerance1,false);
				sTol2=pdu->makeADistStr(arcAngle-m_DimFmt.Tolerance2,false);
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

		m_pDimStr->SetRotation(m_adp.angText);
		m_pDimStr->SetAlign(m_adp.nDSAlign);
		m_pDimStr->SetShapePoint(0,m_adp.lptText);
		m_pDimStr->CalculateExtents();
	}
}

void CMDAngularDim::Unglue(CMDShapeList* pList) const
{
	CMDShape* pShape;
	if(m_adp.pDimFmt->bUseWitness)
	{
		if(m_adp.lptWit1Start!=m_adp.lptWit1End)
		{
			pShape=new CMDLineShape(m_adp.lptWit1Start,m_adp.lptWit1End);
			pShape->SetPenModel(GetPenModel());
			pShape->CalculateExtents();
			pShape->SetShapeDatum(kDefaultDatum);
			pList->AddTail(pShape);
		}
		if(m_adp.lptWit2Start!=m_adp.lptWit2End)
		{
			pShape=new CMDLineShape(m_adp.lptWit2Start,m_adp.lptWit2End);
			pShape->SetPenModel(GetPenModel());
			pShape->CalculateExtents();
			pShape->SetShapeDatum(kDefaultDatum);
			pList->AddTail(pShape);
		}
	}

	pShape=new CMDArcShape;
	pShape->SetShapePoint(0,m_adp.lptCenter);
	pShape->SetShapePoint(1,m_adp.lptDimStart);
	pShape->SetShapePoint(2,m_adp.lptDimEnd);
	((CMDArcShape*)pShape)->m_nArcDirection=m_adp.nDirection;
	pShape->SetPenModel(GetPenModel());
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	pList->AddTail(pShape);

	if(m_adp.lptDimEnd!=m_adp.lptExtent)
	{
		pShape=new CMDArcShape;
		pShape->SetShapePoint(0,m_adp.lptCenter);
		pShape->SetShapePoint(1,m_adp.lptDimEnd);
		pShape->SetShapePoint(2,m_adp.lptExtent);
		((CMDArcShape*)pShape)->m_nArcDirection=m_adp.nDirection;
		pShape->SetPenModel(GetPenModel());
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		pList->AddTail(pShape);
	}
}

CTrackContext* CMDAngularDim::TrackCreateContext(int nWhichHandle, LPARAM lParam,CMapper* pMapper)
{
	CAngDimTrackContext* p=new CAngDimTrackContext;
	p->nWhichHandle=nWhichHandle;
	p->lParam=lParam;
	p->adp=m_adp;
	p->bSmallerArc=m_bSmallerArc;
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
		p->adp.lptFrom=mtRotPoint(p->adp.lptFrom, pCtx->m_Angle, lptRot);
		p->adp.lptTo=mtRotPoint(p->adp.lptTo, pCtx->m_Angle, lptRot);
		p->adp.lptAnchor=mtRotPoint(p->adp.lptAnchor, pCtx->m_Angle, lptRot);
		p->adp.lptCenter=mtRotPoint(p->adp.lptCenter, pCtx->m_Angle, lptRot);
		p->pDimStr->SetShapePoint(0,mtRotPoint(p->pDimStr->GetShapePoint(0), pCtx->m_Angle, lptRot));
	}
	CalcPoints(p->bSmallerArc,p->adp,pMapper);
	return p;
}

void CMDAngularDim::TrackFeedback(LongPoint pt, CTrackContext* pTrackContext)
{
	int nHandle=pTrackContext->nWhichHandle;
	CAngDimTrackContext* p=(CAngDimTrackContext*)pTrackContext;
	switch(nHandle)
	{
	case kMoveHandle:
		p->adp.lptFrom+=pt;
		p->adp.lptTo+=pt;
		p->adp.lptAnchor+=pt;
		p->adp.lptCenter+=pt;
		break;
	case 0:
		p->adp.lptFrom=pt;
		break;
	case 1:
		p->adp.lptTo=pt;
		break;
	case 2:
		if(p->bShift)
		{
			p->adp.lptAnchor=m_adp.lptCenter+mtRotPoint(LongPoint(Pythag(m_adp.lptAnchor-m_adp.lptCenter),0),mtAngle(m_adp.lptCenter,pt));
		}
		else p->adp.lptAnchor=pt;
		break;
	}
	CalcPoints(p->bSmallerArc,p->adp,&p->pView->m_Mapper);

	CString sValue,sTol1,sTol2;
	if(m_DimFmt.bUseLabel) sValue=m_DimFmt.sLabel;
	else
	{
		CDimUnit* pdu=pTrackContext->pView->GetDocument()->GetDimUnit();
		Angle arcAngle=Abs(mtAngle(p->adp.lptCenter,p->adp.lptFrom)-mtAngle(p->adp.lptCenter,p->adp.lptTo));
		if(arcAngle<180 != p->bSmallerArc) arcAngle=360-arcAngle;
		switch(m_DimFmt.nTolerance)
		{
		case kNoTolerance:
			sValue=pdu->makeADistStr(arcAngle,false);
			break;
		case kOne:
			sValue=pdu->makeADistStr(arcAngle,false);
			//sTol1="±"+pdu->makeADistStr(m_DimFmt.Tolerance1,false); Dennis
#ifdef _JAPANESE
			sTol1 = "}" + pdu->makeADistStr(m_DimFmt.Tolerance1, false);
#else
			sTol1 = "±" + pdu->makeADistStr(m_DimFmt.Tolerance1, false);
#endif
			break;
		case kTwo:
			sValue=pdu->makeADistStr(arcAngle,false);
			sTol1="+"+pdu->makeADistStr(m_DimFmt.Tolerance1,false);
			sTol2="-"+pdu->makeADistStr(m_DimFmt.Tolerance2,false);
			break;
		case kLimits:
			sTol1=pdu->makeADistStr(arcAngle+m_DimFmt.Tolerance1,false);
			sTol2=pdu->makeADistStr(arcAngle+m_DimFmt.Tolerance2,false);
			break;
		}
	}

	CDC* pDC=p->pView->GetDC();
	{
		CDraftContext dc(pDC,&p->pView->m_Mapper);
		dc.SetDimStandard(&m_DimStd);
		p->pDimStr->SetValues(&dc,sValue,sTol1,sTol2);
	}
	p->pView->ReleaseDC(pDC);

	p->pDimStr->SetRotation(p->adp.angText);
	p->pDimStr->SetAlign(p->adp.nDSAlign);
	p->pDimStr->SetShapePoint(0,p->adp.lptText);
	p->pDimStr->CalculateExtents();
}

void CMDAngularDim::TrackComplete(CTrackContext* pTrackContext)
{
	CAngDimTrackContext* p=(CAngDimTrackContext*)pTrackContext;
	m_adp=p->adp;
	m_bSmallerArc=p->bSmallerArc;
	delete m_pDimStr;
	m_pDimStr=p->pDimStr;
	p->pDimStr = NULL;
	CalculateExtents();
}

void CMDAngularDim::TrackWireFrame(CDraftContext* pDC, CTrackContext* pTrackContext)
{
	CAngDimTrackContext* p=(CAngDimTrackContext*)pTrackContext;

	pDC->DrawAngDim(p->adp);
	p->pDimStr->DrawShapeOutline(pDC);
}

const ResizeInfo* CMDAngularDim::GetResizeInfo() const
{
static ResizeInfo rectResize =
{
	IDS_ANGDIM, 3, {IDS_1A,kAngleValue, IDS_ENDA,kAngleValue, IDS_RADIUS,kUnitlessValue}
};
	return &rectResize;
}

void CMDAngularDim::GetResizeValue(ResizeValue* pValues,int& nHandle)
{
	CMDShape::GetResizeValue(pValues,nHandle);
	pValues[0].m_Angle=mtAngle(m_adp.lptCenter,m_adp.lptFrom);
	pValues[1].m_Angle=mtAngle(m_adp.lptCenter,m_adp.lptTo);
	pValues[2].m_Unitless=Pythag(m_adp.lptAnchor-m_adp.lptCenter);
}

void CMDAngularDim::SetResizeValue(ResizeValue* pValues,int nHandle)
{
	Angle angle=mtAngle(m_adp.lptCenter,m_adp.lptAnchor);
	LongPoint lpt=m_adp.lptCenter;
	lpt.x+=pValues[2].m_Unitless;
	m_adp.lptAnchor=mtRotPoint(lpt,angle,m_adp.lptCenter);

	lpt=m_adp.lptCenter;
	lpt.x+=Pythag(m_adp.lptFrom-m_adp.lptCenter);
	m_adp.lptFrom=mtRotPoint(lpt,pValues[0].m_Angle,m_adp.lptCenter);

	lpt=m_adp.lptCenter;
	lpt.x+=Pythag(m_adp.lptTo-m_adp.lptCenter);
	m_adp.lptTo=mtRotPoint(lpt,pValues[1].m_Angle,m_adp.lptCenter);

	CMDShape::SetResizeValue(pValues);
}

void CMDAngularDim::CalcPoints(CMapper* pMapper)
{
	CalcPoints(m_bSmallerArc,m_adp,pMapper);
}

void CMDAngularDim::ReCalc(CMapper* pMapper)
{
	CalcPoints(m_bSmallerArc,m_adp,pMapper);
	m_pDimStr->SetRotation(m_adp.angText);
	m_pDimStr->SetAlign(m_adp.nDSAlign);
	m_pDimStr->SetShapePoint(0,m_adp.lptText);
	m_pDimStr->CalculateExtents();
	CalculateExtents();
}

void CMDAngularDim::WriteDXF(CFile* pFile,	CString& pLayerStr,	LongRectPtr pDocExtPtr,	double vUnitFactor, int nZoomFactor, Coord vDisplayResValue, CPalette *ptrPalette) const
{

	CMDArcShape* dimArcHdl= 0L;
	CMDLineShape* dimLineHdl = 0L;
	LongPoint dimStart, dimEnd, centerPt;
	LongPoint ref1Pt, ref2Pt;
	LongPoint capAPt1, capAPt2, capBPt1, capBPt2;
	PenModel penModel, capsPenModel;
	short cap1, cap2; //, capA, capB, caps;
//	bool drawCap2;
	Coord outsideLineLen, radius;
	Angle startLAngle, endLAngle, startAngle;
	Angle arcAngle, capsAngle;
	LongPoint lptDimLineStart,lptDimLineEnd;
	LongPoint lptWit1Start,lptWit1End;
	LongPoint lptWit2Start,lptWit2End;
	LongPoint lptExtLineStart,lptExtLineEnd;

	try {
		
		outsideLineLen = FixedZero ;

		if (!(dimArcHdl = new CMDArcShape) ) {
			TRACE0("DXF write error. Write CMDAngularDim\n");
			AfxThrowUserException();
		}

	
		if (!(dimLineHdl = new CMDLineShape)) {
			TRACE0("DXF write error. Write CMDAngularDim\n");
			AfxThrowUserException();
		}

		penModel = capsPenModel = GetPenModel();
		cap1 = m_nDimCap1;
		cap2 = m_nDimCap2;

		Angle startAngle=mtATan2(m_adp.lptDimStart-m_adp.lptCenter);
		Angle endAngle=mtATan2(m_adp.lptDimEnd-m_adp.lptCenter);

/*calc points
		Coord wlGap,wlExt;
		wlGap=m_DimStd.WitnessLineGap*vDisplayResValue;
		wlExt=m_DimStd.WitnessExt*vDisplayResValue;
		LongPoint	lpt1 = GetShapePoint(0), 
					lpt2=GetShapePoint(1), 
					lptAnchor = GetShapePoint(2),
					lptCenter = GetShapePoint(12);
	
		Angle startAngle=mtATan2(lpt1-lptCenter);
		Angle endAngle=mtATan2(lpt2-lptCenter);
		Coord radius=Pythag(lptAnchor-lptCenter);
		Coord startRadius=Pythag(lpt1-lptCenter);
		Coord endRadius=Pythag(lpt2-lptCenter);

		TSinCos sinCos(startAngle);
		LongPoint lpt=lptCenter;
		lpt.x+=radius;
		lptDimLineStart=mtRotPoint(lpt,sinCos,lptCenter);

		lpt=lptCenter;
		lpt.x+=startRadius+(startRadius<radius?wlGap:-wlGap);
		lptWit1Start=mtRotPoint(lpt,sinCos,lptCenter);
		if(Abs(startRadius-radius)>wlGap) {
			lpt=lptCenter;
			lpt.x+=radius+(startRadius<radius?wlExt:-wlExt);
			lptWit1End=mtRotPoint(lpt,sinCos,lptCenter);
		}
		else 
			lptWit1End=lptWit1Start;

		sinCos=TSinCos(endAngle);
		lpt=lptCenter;
		lpt.x+=radius;
		lptDimLineEnd=mtRotPoint(lpt,sinCos,lptCenter);

		lpt=lptCenter;
		lpt.x+=endRadius+(endRadius<radius?wlGap:-wlGap);
		lptWit2Start=mtRotPoint(lpt,sinCos,lptCenter);
		if(Abs(endRadius-radius)>wlGap) {
			lpt=lptCenter;
			lpt.x+=radius+(endRadius<radius?wlExt:-wlExt);
			lptWit2End=mtRotPoint(lpt,sinCos,lptCenter);
		}
		else 
			lptWit2End=lptWit2Start;

*/
		dimStart = m_adp.lptDimStart;
		dimEnd	 = m_adp.lptDimEnd;

//		PenModel penModel=GetPenModel();
//		penModel.m_nStyle&=~CapBothMask;
//		SetPenModel(penModel);

		capsAngle = (startAngle < FixedZero) ? -k90 : k90;
		capAPt1 = m_adp.lptDimStart;
		capAPt2 = mtRotPoint ( m_adp.lptCenter, capsAngle, m_adp.lptDimStart);

		capsAngle = (endAngle < FixedZero) ? k90 : -k90;
		capBPt1 = m_adp.lptDimEnd;
		capBPt2 = mtRotPoint ( m_adp.lptCenter, capsAngle, m_adp.lptDimEnd );



		if (cap1)
			DxfDrawEndcap (pFile, pLayerStr, &capAPt1, &capAPt2, cap1, kEnd1,
						   &capsPenModel, false, outsideLineLen,
						   pDocExtPtr, vUnitFactor, nZoomFactor, ptrPalette);

		if (cap2)
			DxfDrawEndcap (	pFile, pLayerStr, &capBPt1, &capBPt2, cap2, kEnd1,
							&capsPenModel, false, outsideLineLen,
		  					pDocExtPtr, vUnitFactor, nZoomFactor, ptrPalette);

		
		if ( m_adp.lptWit1Start != m_adp.lptWit1End ) {
			dimLineHdl->SetShapePoint(0,m_adp.lptWit1Start) ;
			dimLineHdl->SetShapePoint(1,m_adp.lptWit1End) ;
			dimLineHdl->WriteDXF(pFile,	pLayerStr,	pDocExtPtr,	vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);
		}

		if ( m_adp.lptWit2Start != m_adp.lptWit2End )	{
			dimLineHdl->SetShapePoint (0, m_adp.lptWit2Start ) ;
			dimLineHdl->SetShapePoint( 1,m_adp.lptWit2End ) ;
			dimLineHdl->WriteDXF (pFile, pLayerStr,	pDocExtPtr,	vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);
		}

		FillModel fillModel;
		fillModel=GetFillModel();
		fillModel.m_nFillType = fillModel.m_nIndex = 0;
		penModel=GetPenModel();
		dimArcHdl->SetPenModel(penModel);
		dimArcHdl->SetFillModel(fillModel);
		
		dimArcHdl->m_nArcDirection=m_adp.nDirection;
		dimArcHdl->SetShapePoint(0,m_adp.lptCenter);
		dimArcHdl->SetShapePoint(1,m_adp.lptDimStart);
		dimArcHdl->SetShapePoint(2,m_adp.lptDimEnd);

		CMDAssocDim::WriteDXF(pFile, 	pLayerStr,	pDocExtPtr,	vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);
		
		dimArcHdl->WriteDXF(pFile, 	pLayerStr,	pDocExtPtr,	vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);

		if(m_adp.lptDimEnd!=m_adp.lptExtent) {
			dimArcHdl->SetShapePoint(1,m_adp.lptDimEnd);
			dimArcHdl->SetShapePoint(2,m_adp.lptExtent);
			dimArcHdl->WriteDXF(pFile, 	pLayerStr,	pDocExtPtr,	vUnitFactor, nZoomFactor, vDisplayResValue, ptrPalette);
		}


/*
	caps = GetPenModel().EndCap1();
	capA = (caps == kCap2AsCap1) ? cap2 : cap1;
	capAPt1 = dimStart;

	capAPt2 = mtRotPoint ( centerPt, capsAngle, capAPt1 );

	drawCap2 = false;
	if ( caps == kCapsBoth) {
		capBPt1 = dimEnd;
		capsAngle = (arcAngle < FixedZero ) ? -k90 : k90;

		capBPt2 = mtRotPoint ( centerPt, capsAngle, capBPt1 );
		capB = cap2;
		drawCap2 = true;
	}

	caps = dimHdl->cCaps2;

	if (caps != kNoArc) {
		startAngle = capsAngle = dimHdl->cStartAngle2;
		arcAngle = dimHdl->cArcAngle2;

		if ( arcAngle < FixedZero ) {
			startLAngle = mtFixAngle (k90 - (startAngle + arcAngle));
			endLAngle = mtFixAngle (startLAngle + dimHdl->cArcAngle2);
		}
		else {
			startLAngle = mtFixAngle (k90 - startAngle);
			endLAngle = mtFixAngle (startLAngle - arcAngle);
		}
		dimArcHdl->cStartAng = startLAngle;
		dimArcHdl->cEndAng = endLAngle;

		if ((vErrCode = writeArc (pFileHdl, pLayerStr, dimArcHdl,
							  		pDocExtPtr, pUnitFactor)) != kDxfNoErr)
			goto ERR_EXIT;

		if (caps != kNoCaps) {
			capBPt1 = dimEnd;
			capsAngle = ( arcAngle < FixedZero ) ? k90 : -k90;
			capB = (caps == kCap2AsCap1) ? cap2 : cap1;

			capBPt2 = mtRotPoint ( centerPt, capsAngle, capBPt1 );
			drawCap2 = true;
		}
	}

	DxfDrawEndcap (pFile, pLayerStr, &capAPt1, &capAPt2, capA, kEnd1,
				   &capsPenModel, false, outsideLineLen,
				   pDocExtPtr, pUnitFactor);

	if (drawCap2)
		DxfDrawEndcap (		pFile, pLayerStr, &capBPt1, &capBPt2, capB, kEnd1,
							&capsPenModel, false, outsideLineLen,
		  					pDocExtPtr, pUnitFactor);
	}
*/
//	if ((vErrCode = writeAssocDimText (pFileHdl, pLayerHdl, pLayerStr, dimHdl,
//		 												 pDocExtPtr, pUnitFactor)) == kDxfErr)
//			goto ERR_EXIT;
//	}
		if (dimArcHdl)
			delete dimArcHdl;
		if (dimLineHdl)
			delete dimLineHdl;
	}
	catch (CException* e) {
		if (dimArcHdl)
			delete dimArcHdl;
		if (dimLineHdl)
			delete dimLineHdl;
		throw e;
	}

}

//////////////////////////////////////////////////////////////////////

void CMDAngularDim::CalcPoints(bool bSmallerArc,AngDimPoints& adp,CMapper* pMapper)
{
	Angle anchorAngle;
	Angle startAngle=mtATan2(adp.lptFrom-adp.lptCenter);
	Angle endAngle=mtATan2(adp.lptTo-adp.lptCenter);

	Angle arcAngle=endAngle-startAngle;
	Angle midAngle;

	if(Abs(arcAngle)<180 == bSmallerArc)
	{
		adp.nDirection=arcAngle<0?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
	}
	else
	{
		adp.nDirection=arcAngle<0?AD_COUNTERCLOCKWISE:AD_CLOCKWISE;
	}

	Coord radius=Pythag(adp.lptAnchor-adp.lptCenter);
	if(adp.pDimFmt->bTextCentered)
	{
		anchorAngle=startAngle+endAngle;

		if(adp.nDirection==AD_CLOCKWISE)
		{
			if(startAngle<=endAngle) anchorAngle+=360;
		}
		else
		{
			if(startAngle>=endAngle) anchorAngle+=360;
		}
		anchorAngle=mtFixAngle(anchorAngle>>1);
		adp.lptAnchor=adp.lptCenter+mtRotPoint(LongPoint(Pythag(adp.lptAnchor-adp.lptCenter),0),anchorAngle);
	}
	else anchorAngle=mtATan2(adp.lptAnchor-adp.lptCenter);

	Coord startRadius=Pythag(adp.lptFrom-adp.lptCenter);
	Coord endRadius=Pythag(adp.lptTo-adp.lptCenter);

	TSinCos startSinCos(startAngle);
	LongPoint lpt=adp.lptCenter;
	lpt.x+=radius;
	adp.lptDimStart=mtRotPoint(lpt,startSinCos,adp.lptCenter);

	TSinCos endSinCos(endAngle);
	lpt=adp.lptCenter;
	lpt.x+=radius;
	adp.lptDimEnd=mtRotPoint(lpt,endSinCos,adp.lptCenter);

	Coord wlGap,wlExt;
	wlGap=pMapper->Scalar2Long(adp.pDimStd->WitnessLineGap);
	wlExt=pMapper->Scalar2Long(adp.pDimStd->WitnessExt);

	lpt=adp.lptCenter;
	lpt.x+=startRadius+(startRadius<radius?wlGap:-wlGap);
	adp.lptWit1Start=mtRotPoint(lpt,startSinCos,adp.lptCenter);
	if(Abs(startRadius-radius)>wlGap)
	{
		lpt=adp.lptCenter;
		lpt.x+=radius+(startRadius<radius?wlExt:-wlExt);
		adp.lptWit1End=mtRotPoint(lpt,startSinCos,adp.lptCenter);
	}
	else adp.lptWit1End=adp.lptWit1Start;

	lpt=adp.lptCenter;
	lpt.x+=endRadius+(endRadius<radius?wlGap:-wlGap);
	adp.lptWit2Start=mtRotPoint(lpt,endSinCos,adp.lptCenter);
	if(Abs(endRadius-radius)>wlGap)
	{
		lpt=adp.lptCenter;
		lpt.x+=radius+(endRadius<radius?wlExt:-wlExt);
		adp.lptWit2End=mtRotPoint(lpt,endSinCos,adp.lptCenter);
	}
	else adp.lptWit2End=adp.lptWit2Start;

	startAngle=mtFixAngle(startAngle-anchorAngle);
	endAngle=mtFixAngle(endAngle-anchorAngle);

	if((adp.nDirection==AD_CLOCKWISE) == (startAngle>endAngle))
	{
		adp.lptExtent=adp.lptCenter;
		adp.lptExtent.x+=radius;
		adp.lptExtent=mtRotPoint(adp.lptExtent,anchorAngle,adp.lptCenter);

		if(((adp.nDirection==AD_CLOCKWISE) && (endAngle>(360-startAngle)))
			|| ((adp.nDirection!=AD_CLOCKWISE) && (startAngle<(360-endAngle))))
		{
			LongPoint lpt=adp.lptDimStart;
			adp.lptDimStart=adp.lptDimEnd;
			adp.lptDimEnd=lpt;
			adp.nDirection=(adp.nDirection==AD_CLOCKWISE)?AD_COUNTERCLOCKWISE:AD_CLOCKWISE;
		}
	}
	else adp.lptExtent=adp.lptDimEnd;	// no extention
	adp.lptLeader=adp.lptExtent;
	adp.lptText=adp.lptAnchor;
	adp.nDSAlign=kDSCenter;
	adp.angText=0;
}

int CMDAngularDim::SaveMacDraft(PhIOContext& ctx)
{
	PhDSAngularDim ps;
	ps.fID=PhIDAngularDim;
	ctx.pPhShape=(PhDSFillShape*)&ps;
	CMDAssocDim::SaveMacDraft(ctx);

	ps.refPt1.fUse=0;
	ps.refPt1.fFlags=kPhRealVertex;
	ps.refPt1.fLongPt=m_adp.lptFrom;
	ps.refPt2.fUse=0;
	ps.refPt2.fFlags=kPhRealVertex;
	ps.refPt2.fLongPt=m_adp.lptTo;
	ps.dimLineStart.fUse=0;
	ps.dimLineStart.fFlags=0;
	ps.dimLineStart.fLongPt=m_adp.lptDimStart;
	ps.dimLineEnd.fUse=0;
	ps.dimLineEnd.fFlags=0;
	ps.dimLineEnd.fLongPt=m_adp.lptDimEnd;
	ps.anchorPt.fUse=0;
	ps.anchorPt.fFlags=kPhRealVertex;
	ps.anchorPt.fLongPt=m_adp.lptAnchor;
	ps.extPt.fUse=0;
	ps.extPt.fFlags=0;
	ps.extPt.fLongPt=m_adp.lptExtent;
	ps.centerPt.fUse=0;
	ps.centerPt.fFlags=kPhRealVertex|kPhNoHighlight;
	ps.centerPt.fLongPt=m_adp.lptCenter;
	ps.witnessStart1.fUse=0;
	ps.witnessStart1.fFlags=0;
	ps.witnessStart1.fLongPt=m_adp.lptWit1Start;
	ps.witnessEnd1.fUse=0;
	ps.witnessEnd1.fFlags=0;
	ps.witnessEnd1.fLongPt=m_adp.lptWit1End;
	ps.witnessStart2.fUse=0;
	ps.witnessStart2.fFlags=0;
	ps.witnessStart2.fLongPt=m_adp.lptWit2Start;
	ps.witnessEnd2.fUse=0;
	ps.witnessEnd2.fFlags=0;
	ps.witnessEnd2.fLongPt=m_adp.lptWit2End;
	ps.fRadius=Pythag(m_adp.lptAnchor-m_adp.lptCenter);
	ps.fStartAngle1=mtATan2(m_adp.lptFrom-m_adp.lptCenter);
	ps.fArcAngle1=mtATan2(m_adp.lptTo-m_adp.lptCenter)-ps.fStartAngle1;
	ps.fCaps1=0;
	ps.fStartAngle2=mtATan2(m_adp.lptTo-m_adp.lptCenter);
	ps.fArcAngle2=mtATan2(m_adp.lptExtent-m_adp.lptCenter)-ps.fStartAngle2;
	ps.fCaps2=0;

	uptr p((char*)&ps);
	p.swapDSShape();

	p.swaps(2); // fm
	p.swaps(3);	p.skip(12);
	p.swaps(1); p.skip(6); p.swaps(4); p.swapl(3); // dim fmt
	p.swaps(1); p.swapl(10); // dim std
	p.swaplpt(8); 
	p.skip(kDimStrSize+kDimTolStrSize+kDimTolStrSize);
	p.swapDBPoint(11);
	p.swapl(3); p.swaps(1); p.swapl(2); p.swaps(1);

	short count=sizeof(PhDSAngularDim);
	uptr(&count).swaps(1);
	ctx.f->Write(&count,sizeof(short));
	ctx.f->Write(&ps,sizeof(PhDSAngularDim));
	SaveMacDimLabel(ctx);

	SaveMacObjInfo(ctx);
	return 1;
}

void CMDAngularDim::GetVertexesFromPt(CUIntArray& vertexes,CMDShapeArray& shapes,const pointd& pt, CMapper* pMapper)
{
	int nHandle=TestHandles(pt,pMapper);
	if(nHandle>=0 && nHandle<2)
	{
		vertexes.Add(nHandle);
		shapes.Add(this);
	}
}

void CMDAngularDim::OnUpdateLineLinkedVertex(CPCDraftView* pView,int nVertex,CMDShape* pCauseShape,int nCauseVertex1,int nCauseVertex2)
{
	LongPoint lptCenter;
	if(guLongLnLnPt(pCauseShape->GetShapePoint(nCauseVertex1),pCauseShape->GetShapePoint(nCauseVertex2),GetShapePoint(1-nVertex),GetShapePoint(12),&lptCenter))
	{
		SetShapePoint(12,lptCenter);
	}
	else
	{
		SetShapePoint(12,Average(GetShapePoint(nVertex),GetShapePoint(1-nVertex)));
	}
	{
		CTrackContext* pTrackContext=TrackCreateContext(nVertex, 0, &pView->m_Mapper);
		if(pTrackContext)
		{
			pTrackContext->pView=pView;
			TrackFeedback(pCauseShape->GetShapeHandle(nCauseVertex1),pTrackContext);
			TrackComplete(pTrackContext);
			delete pTrackContext;
		}
	}
}


bool CMDAngularDim::TestSelect(const pointd& pt, CMapper* pMapper) const
{

   return CMDAssocDim::PointInFill(pt, pMapper);

}
