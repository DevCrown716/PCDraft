// LineDimSketcher.cpp: implementation of the CLineDimSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "LineDimSketcher.h"

#include "phmath.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDDimStr.h"
#include "MDLineDim.h"
#include "DraftContext.h"
#include "Layer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CLineDimSketcher::CLineDimSketcher(DimStandard* pDimStd,DimFormat* pDimFmt,int nMode,int nType, CEdge* pEdge) : m_ldp(pDimStd,pDimFmt),m_nMode(nMode),m_nType(nType),m_pEdge(pEdge),m_bNextDim(false),m_bForceHairline(GetAsyncKeyState(VK_SHIFT)<0?true:false),m_bForceArrowsInside(GetAsyncKeyState(VK_OPTION)<0?true:false)
{
	m_Angle=m_pEdge?mtFixAngle(mtAngle(pEdge->m_lptFrom,pEdge->m_lptTo)+90):0;
}

void CLineDimSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC, pt);
	m_bAnchor=false;
	
	m_pDimStr=NULL;

	if(m_bNextDim)
	{
		if(m_nMode==1) // chained
		{
			m_ldp.lptFrom=m_ldp.lptTo;
			m_lptPrevAnchor=m_ldp.lptAnchor;
		}
		OnAddPt(pt);
	}
	else
	{
		m_ldp.lptFrom=pt;
		if(m_nType==kPerp) m_pView->SetSnapMode(kSnap);
	}
	m_ldp.lptTo=pt;
}

CSketcher::ContinueMode CLineDimSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_MOUSEMOVE:
		return ContinueUpdatePt;
	case WM_LBUTTONDOWN:
		return m_bAnchor?Complete:ContinueAddPt;
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		return Continue;
	default:
		return CSketcher::QueryContinue();
	}
}

void CLineDimSketcher::OnAddPt(const LongPoint& ptAdd)
{
	m_bAnchor=true;
	m_pView->SetCursorPrompt(IDS_ANCHOR);

	m_ldp.lptTo=m_ldp.lptAnchor=ptAdd;
	if(m_nType==kSlope) m_Angle=mtAngle(m_ldp.lptFrom,m_ldp.lptTo);

	CMDLineDim::CalcPoints(m_nType,m_Angle,m_ldp,&m_pView->m_Mapper);

	CPCDraftDoc* pDoc=m_pView->GetDocument();
	m_pDimStr=new CMDDimStr;

	CString sValue,sTol1,sTol2;
	if(m_ldp.pDimFmt->bUseLabel) sValue=m_ldp.pDimFmt->sLabel;
	else
	{
		LongRatio ratio,tolRatio;
		pDoc->GetActiveScaleRatio(ratio);
		tolRatio.fN=1; tolRatio.fD=1;
		CDimUnit* pdu=pDoc->GetDimUnit();

		Coord dimValue=Pythag(m_ldp.lptDimEnd-m_ldp.lptDimStart);
		switch(m_ldp.pDimFmt->nTolerance)
		{
		case kNoTolerance:
			sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
			break;
		case kOne:
			sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
			//sTol1="±"+pdu->numUser2Str(pdu->numDB2User(tolRatio,m_pView->m_Mapper.Scalar2Long(m_ldp.pDimFmt->Tolerance1)),true); Dennis
#ifdef _JAPANESE		
			sTol1 = "}" + pdu->numUser2Str(pdu->numDB2User(tolRatio, m_pView->m_Mapper.Scalar2Long(m_ldp.pDimFmt->Tolerance1)), true);
#else
			sTol1 = "±" + pdu->numUser2Str(pdu->numDB2User(tolRatio, m_pView->m_Mapper.Scalar2Long(m_ldp.pDimFmt->Tolerance1)), true);
#endif
			break;
		case kTwo:
			sValue=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue),false);
			sTol1="+"+pdu->numUser2Str(pdu->numDB2User(tolRatio,m_pView->m_Mapper.Scalar2Long(m_ldp.pDimFmt->Tolerance1)),true);
			sTol2="-"+pdu->numUser2Str(pdu->numDB2User(tolRatio,m_pView->m_Mapper.Scalar2Long(m_ldp.pDimFmt->Tolerance2)),true);
			break;
		case kLimits:
			sTol1=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue+m_pView->m_Mapper.Scalar2Long(m_ldp.pDimFmt->Tolerance1)),true);
			sTol2=pdu->numUser2Str(pdu->numDB2User(ratio,dimValue-m_pView->m_Mapper.Scalar2Long(m_ldp.pDimFmt->Tolerance2)),true);
			break;
		}
	}

	CDC* pDC=m_pView->GetDC();
	{
		CDraftContext dc(pDC,&m_pView->m_Mapper);
		dc.SetDimStandard(m_ldp.pDimStd);
		m_pDimStr->SetValues(&dc,sValue,sTol1,sTol2);
		//m_pDimStr->ModifyTextParams(pDoc->m_TextParams,tpUseFont,&dc);
	}
	m_pDimStr->SetRotation(m_ldp.angText);
	m_pDimStr->SetAlign(m_ldp.nDSAlign);
	m_pDimStr->SetShapePoint(0,m_ldp.lptText);
	m_pDimStr->CalculateExtents();
}

void CLineDimSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	if(m_bAnchor)
	{
		m_ldp.lptAnchor=ptUpdate;
		if(m_nMode==1 && m_bNextDim && GetAsyncKeyState(VK_SHIFT)<0)
		{
			switch(m_nType)
			{
			case kHoriz:
				m_ldp.lptAnchor.y=m_lptPrevAnchor.y;
				break;
			case kVert:
				m_ldp.lptAnchor.x=m_lptPrevAnchor.x;
				break;
			case kSlope:
				break;
			}
		}

		CMDLineDim::CalcPoints(m_nType,m_Angle,m_ldp,&m_pView->m_Mapper);
		
		switch(m_nType)
		{
		case kHoriz:
			m_pView->SetShowSize(1,2,Abs(m_ldp.lptAnchor.y-m_ldp.lptFrom.y));
			break;
		case kVert:
			m_pView->SetShowSize(1,1,Abs(m_ldp.lptAnchor.x-m_ldp.lptFrom.x));
			break;
		case kSlope:
			m_pView->SetShowSize(0,8,Abs(mtRotPoint(m_ldp.lptAnchor,m_Angle,m_ldp.lptFrom).x-m_ldp.lptFrom.x));
			break;
		}

		m_pDimStr->SetRotation(m_ldp.angText);
		m_pDimStr->SetAlign(m_ldp.nDSAlign);
		m_pDimStr->SetShapePoint(0,m_ldp.lptText);
		m_pDimStr->CalculateExtents();
	}
	else
	{
		m_ldp.lptTo=ptUpdate;
		switch(m_nType)
		{
		case kHoriz:
			m_pView->SetShowSize(0,1,Abs(m_ldp.lptTo.x-m_ldp.lptFrom.x));
			break;
		case kVert:
			m_pView->SetShowSize(0,2,Abs(m_ldp.lptTo.y-m_ldp.lptFrom.y));
			break;
		case kSlope:
			m_Angle=mtAngle(m_ldp.lptFrom,m_ldp.lptTo);
			m_pView->SetShowSize(0,7,Pythag(m_ldp.lptTo-m_ldp.lptFrom));
			m_pView->SetShowSize(1,-1,m_Angle);
			break;
		case kPerp:
			m_pView->SetShowSize(0,8,Abs(mtRotPoint(m_ldp.lptTo,m_Angle,m_ldp.lptFrom).x-m_ldp.lptFrom.x));
			m_pView->SetShowSize(1,-1,m_Angle);
			break;
		}
	}
}

void CLineDimSketcher::OnDrawSketcher(CDC * pDC)
{
	DrawMarker(pDC,m_pView->m_Mapper.LPtoDP(m_ldp.lptFrom));
	if(m_bAnchor)
	{
		DrawMarker(pDC,m_pView->m_Mapper.LPtoDP(m_ldp.lptTo));

		if(m_ldp.lptWit1Start!=m_ldp.lptWit1End)
		{
			pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_ldp.lptWit1Start));
			pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_ldp.lptWit1End));
		}
		if(m_ldp.lptWit2Start!=m_ldp.lptWit2End)
		{
			pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_ldp.lptWit2Start));
			pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_ldp.lptWit2End));
		}

		pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_ldp.lptDimStart));
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_ldp.lptDimEnd));
		if(m_ldp.lptDimEnd!=m_ldp.lptExtent)
		{
			pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_ldp.lptExtent));
			if(m_ldp.lptExtent!=m_ldp.lptLeader) pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_ldp.lptLeader));
		}

		CDraftContext dc(pDC,&m_pView->m_Mapper);
      dc.m_bUseGDI = true; // Temporary Fix
      dc.SetOutlinePen();
		m_pDimStr->DrawShapeOutline(&dc);
	}
}

bool CLineDimSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	m_pView->SaveUndo(IDS_CREATION);

	CPCDraftDoc* pDoc=m_pView->GetDocument();

	CMDLineDim* pShape=new CMDLineDim(*m_ldp.pDimStd,*m_ldp.pDimFmt,pDoc->m_nDimCap1,pDoc->m_nDimCap2,m_nType,m_Angle);
	pShape->SetShapePoint(0,m_ldp.lptFrom);
	pShape->SetShapePoint(1,m_ldp.lptTo);
	pShape->SetShapePoint(2,m_ldp.lptAnchor);
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
	m_pView->SelectShape(pShape,m_bNextDim);

#ifndef _PERSONAL_EDITION
	if(((CPCDraftApp*)AfxGetApp())->m_bLinkedDimensions)
	{
		if(m_nType==kPerp)
			m_pView->LinkLineVertexes(pShape,0,m_pEdge,false);
		else
		{
			m_pView->LinkVertexes(m_pView->m_Mapper.LPtoDP(m_ldp.lptFrom));
			m_pView->LinkVertexes(m_pView->m_Mapper.LPtoDP(m_ldp.lptTo));
		}
	}
#endif

	if(m_nMode) m_bNextDim=true;
//	else 
		m_pView->SetCursorPrompt(IDS_1POINT);
	return true;
}

void CLineDimSketcher::OnCancelSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCancelSketch(pDC,pt);

	if(m_pDimStr) delete m_pDimStr;

	m_pView->SetCursorPrompt(IDS_1POINT);
}

void CLineDimSketcher::DrawMarker(CDC *pDC, const pointd &pt)
{
	CRect rc(pt.x-4,pt.y-4,pt.x+5,pt.y+5);

	CPen pen;
	pen.CreateStockObject(WHITE_PEN);
	CGDIObject<CPen> gdiPen(pDC,&pen);

	pDC->MoveTo(pt.x-5,pt.y-5);
	pDC->LineTo(pt.x+6,pt.y+6);
	pDC->MoveTo(pt.x+5,pt.y-5);
	pDC->LineTo(pt.x-6,pt.y+6);

	CBrush brush;
	brush.CreateStockObject(NULL_BRUSH);
	CGDIObject<CBrush> gdiBrush(pDC,&brush);

	pDC->Ellipse(&rc);
}
