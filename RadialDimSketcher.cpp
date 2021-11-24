// RadialDimSketcher.cpp: implementation of the CRadialDimSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "RadialDimSketcher.h"

#include "MDShape.h"
#include "DimStructs.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "gu.h"
#include "phmath.h"
#include "MDRadialDim.h"
#include "MDDimStr.h"
#include "DraftContext.h"
#include "Layer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CRadialDimSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt) 
{ 
	CSketcher::OnBeginSketch(pDC,pt); 
	m_pView->SetCursorPrompt(IDS_ANCHOR);

	m_Radius=m_pShape->GetRadii().x;
	OnUpdatePt(pt);

	m_pDimStr=new CMDDimStr();
	CPCDraftDoc* pDoc=m_pView->GetDocument();
	m_pDimStr->ModifyTextParams(pDoc->m_TextParams,tpUseFont,NULL);

	UpdateDimStr(pDC);
} 

LongPoint CRadialDimSketcher::AdjustPosition(LongPoint ptTo) 
{ 
	if(GetKeyState(VK_SHIFT)<0) 
		guGetHV45LPt(&ptTo,&m_pShape->GetCenter());
	return ptTo; 
} 

CSketcher::ContinueMode CRadialDimSketcher::QueryContinue() 
{ 
	switch(m_msg.message) 
	{ 
	case WM_LBUTTONUP: 
	case WM_LBUTTONDBLCLK:
		return Continue; 
	case WM_LBUTTONDOWN: 
		return Complete; 
	} 
	return CSketcher::QueryContinue(); 
} 

void CRadialDimSketcher::OnUpdatePt(const LongPoint& ptUpdate) 
{
	m_lptAnchor=ptUpdate;

	LongPoint radii=m_pShape->GetRadii();
	if(radii.x!=radii.y)	// elliptic
	{
		LongPoint lptCenter=m_pShape->GetCenter();
		m_lptAnchor=mtRotPoint(m_lptAnchor,-m_pShape->GetRotation(),lptCenter);
		Angle theta=mtAngle(lptCenter,m_lptAnchor);
		if((theta > k45 && theta <= k135) || (theta > k225 && theta <= k315))
		{
			if(m_Radius!=radii.y)
			{
				m_Radius=radii.y;
				m_bUpdateDimStr=true;
			}
			m_lptAnchor.x=lptCenter.x;
		}
		else
		{
			if(m_Radius!=radii.x)
			{
				m_Radius=radii.x;
				m_bUpdateDimStr=true;
			}
			m_lptAnchor.y=lptCenter.y;
		}
		m_lptAnchor=mtRotPoint(m_lptAnchor,m_pShape->GetRotation(),lptCenter);
	}
} 

void CRadialDimSketcher::OnDrawSketcher(CDC * pDC) 
{ 
	RadialDimPoints rdp(m_pShape->GetCenter(),m_Radius,m_lptAnchor,m_pDimStd,m_pDimFmt);
	CMDRadialDim::CalcPoints(m_bDiameter,rdp,&m_pView->m_Mapper);

	if(m_bUpdateDimStr) UpdateDimStr(pDC);

	pDC->MoveTo(m_pView->m_Mapper.LPtoDP(rdp.lptDimStart));
	pDC->LineTo(m_pView->m_Mapper.LPtoDP(rdp.lptDimEnd));
	if(rdp.lptDimEnd!=rdp.lptExtent)
	{
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(rdp.lptExtent));
		if(rdp.lptExtent!=rdp.lptLeader) pDC->LineTo(m_pView->m_Mapper.LPtoDP(rdp.lptLeader));
	}
	m_pDimStr->SetRotation(rdp.angText);
	m_pDimStr->SetAlign(rdp.nDSAlign);
	m_pDimStr->SetShapePoint(0,rdp.lptText);
	m_pDimStr->CalculateExtents();

	CDraftContext dc(pDC,&m_pView->m_Mapper);
   dc.m_bUseGDI = true;
   dc.SetOutlinePen();
	m_pDimStr->DrawShapeOutline(&dc);
}

bool CRadialDimSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt) 
{ 
	CSketcher::OnCompleteSketch(pDC, pt);
	m_pView->SaveUndo(IDS_CREATION);

	CPCDraftDoc* pDoc=m_pView->GetDocument();

	CMDRadialDim* pShape=new CMDRadialDim(*m_pDimStd,*m_pDimFmt,pDoc->m_nDimCap1,pDoc->m_nDimCap2,m_bDiameter,m_pShape->GetCenter(),m_Radius,m_lptAnchor);
	if(m_bForceArrowsInside) pShape->m_DimFmt.bArrowsInside=false;
	pShape->CalcPoints(&m_pView->m_Mapper);

	PenModel pm=m_pView->GetDocument()->m_Pen;
	if(m_bForceHairline) pm.m_nWidth=0x81; //hairline
	m_pDimStr->SetFillModel(m_pView->GetDocument()->m_Fill);
	pShape->SetDimStr(m_pDimStr);
	pShape->SetPenModel(pm);
	pShape->CalculateExtents();
	pShape->SetShapeDatum(kDefaultDatum);
	pDoc->CreateShape(pShape);
	m_pView->SelectShape(pShape,false);

#ifndef _PERSONAL_EDITION
	if(((CPCDraftApp*)AfxGetApp())->m_bLinkedDimensions)
		m_pView->LinkCenterVertexes(m_pShape,pShape);
#endif

	m_pView->SetCursorPrompt(IDS_CLICKCIRCLEARC);
	return true;
} 

void CRadialDimSketcher::OnCancelSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCancelSketch(pDC,pt);
	
	if(m_pDimStr) delete m_pDimStr;

	m_pView->SetCursorPrompt(IDS_CLICKCIRCLEARC);
}

void CRadialDimSketcher::UpdateDimStr(CDC *pDC)
{
	CPCDraftDoc* pDoc=m_pView->GetDocument();
	CString sValue,sTol1,sTol2;
	if(m_pDimFmt->bUseLabel) sValue=m_pDimFmt->sLabel;
	else
	{
		LongRatio ratio,tolRatio;
		pDoc->GetActiveScaleRatio(ratio);
		tolRatio.fN=1; tolRatio.fD=1;
		CDimUnit* pdu=pDoc->GetDimUnit();
		Coord dimValue=m_bDiameter?m_Radius<<1:m_Radius;
		switch(m_pDimFmt->nTolerance)
		{
		case kNoTolerance:
			sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
			break;
		case kOne:
			sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
#ifdef _JAPANESE			
			sTol1 = "}" + pdu->numUser2Str(pdu->numDB2User(tolRatio, m_pView->m_Mapper.Scalar2Long(m_pDimFmt->Tolerance1)), true);
#else
			sTol1="±"+pdu->numUser2Str(pdu->numDB2User(tolRatio,m_pView->m_Mapper.Scalar2Long(m_pDimFmt->Tolerance1)),true);
#endif
			break;
		case kTwo:
			sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
			sTol1="+"+pdu->numUser2Str(pdu->numDB2User(tolRatio,m_pView->m_Mapper.Scalar2Long(m_pDimFmt->Tolerance1)),true);
			sTol2="-"+pdu->numUser2Str(pdu->numDB2User(tolRatio,m_pView->m_Mapper.Scalar2Long(m_pDimFmt->Tolerance2)),true);
			break;
		case kLimits:
			sTol1=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue+m_pView->m_Mapper.Scalar2Long(m_pDimFmt->Tolerance1)),true);
			sTol2=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue-m_pView->m_Mapper.Scalar2Long(m_pDimFmt->Tolerance2)),true);
			break;
		}

		if(sValue.GetLength())
		{
			CString strSymbol;
			switch(m_pDimFmt->nRDSymbol)
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

	CDraftContext dc(pDC,&m_pView->m_Mapper);
	dc.SetDimStandard(m_pDimStd);
	m_pDimStr->SetValues(&dc,sValue,sTol1,sTol2);
	m_pDimStr->SetShapePoint(0,m_lptAnchor);
	m_pDimStr->CalculateExtents();
	m_bUpdateDimStr=false;
}
