// AngularDimSketcher.cpp: implementation of the CAngularDimSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "AngularDimSketcher.h"

#include "phmath.h"
#include "Layer.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDDimStr.h"
#include "MDAngularDim.h"
#include "DraftContext.h"
#include "Edge.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CAngularDimSketcher::CAngularDimSketcher(DimStandard* pDimStd,DimFormat* pDimFmt,CEdge* pEdgeFrom) : m_pDimStd(pDimStd),m_pDimFmt(pDimFmt), m_pEdgeFrom(pEdgeFrom), m_pEdgeTo(NULL)
{
	m_bAnchor=false;
	m_bSmallerArc=true;
	m_bForceHairline=GetAsyncKeyState(VK_SHIFT)<0?true:false;
	m_bForceArrowsInside=GetAsyncKeyState(VK_OPTION)<0?true:false;
	m_bEdgeFromSwap=false;
	m_bEdgeToSwap=false;
}

CAngularDimSketcher::~CAngularDimSketcher()
{
}

void CAngularDimSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC, pt);
	m_lptFrom=pt;
	m_pView->SetCursorPrompt(IDS_2LINEEDGE);
	m_pDimStr=NULL;
}

CSketcher::ContinueMode CAngularDimSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_MOUSEMOVE:
		return m_bAnchor?ContinueUpdatePt:Continue;
	case WM_LBUTTONDOWN:
		if(m_bAnchor) return Complete;
		else
		{
         pointd pt=__pointf(m_msg.pt);
			m_pView->ScreenToClient(&pt);
			pt+=m_pView->GetDeviceScrollPosition();
			CMDShape* pShape=m_pView->GetDocument()->GetActiveLayer()->ShapeFromPoint(pt,&m_pView->m_Mapper,true,true);
			if(pShape)
			{
				CEdge* pEdge=pShape->GetShapeEdgeFromPt(pt,&m_pView->m_Mapper);
				if(pEdge && pEdge->m_nType==CEdge::kLineEdge)
				{
					if(m_pEdgeFrom->IsEqual(pEdge)) return Cancel;
					m_pView->m_Mistakes.Reset();
					m_pEdgeTo=pEdge;
					m_nSketchMode&=~NoSnap;
					return ContinueAddPt;
				}
				else m_pView->m_Mistakes.ShowMessage(IDS_SELECT_SHAPE_EDGE);
			}
			else m_pView->m_Mistakes.ShowMessage(IDS_SELECT_SHAPE_EDGE);
			return Continue;
		}
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		return Continue;
	default:
		return CSketcher::QueryContinue();
	}
}

void CAngularDimSketcher::OnAddPt(const LongPoint& ptAdd)
{
	m_lptTo=ptAdd;

	LongPoint lptHit1,lptHit2;
	guNearPtLnPt(m_lptFrom,m_pEdgeFrom->m_lptFrom,m_pEdgeFrom->m_lptTo,&lptHit1);
	guNearPtLnPt(m_lptTo,m_pEdgeTo->m_lptFrom,m_pEdgeTo->m_lptTo,&lptHit2);

	if(guLongLnLnPt(m_pEdgeFrom->m_lptFrom,m_pEdgeFrom->m_lptTo,m_pEdgeTo->m_lptFrom,m_pEdgeTo->m_lptTo,&m_lptCenter))
	{
		m_bEdgeFromSwap=GetDimLineEndPt(lptHit1,m_lptCenter,m_pEdgeFrom->m_lptFrom,m_pEdgeFrom->m_lptTo,m_lptFrom);
		m_bEdgeToSwap=GetDimLineEndPt(lptHit2,m_lptCenter,m_pEdgeTo->m_lptFrom,m_pEdgeTo->m_lptTo,m_lptTo);
	}
	else
	{
		m_lptFrom=m_pEdgeFrom->m_lptFrom;
		if(Abs(m_pEdgeFrom->m_lptTo.x-m_lptTo.x)>Abs(m_lptFrom.x-m_lptTo.x))
		{
			m_lptFrom=m_pEdgeFrom->m_lptTo;
			m_bEdgeFromSwap=true;
		}
		m_lptTo=m_pEdgeTo->m_lptFrom;
		if(Abs(m_pEdgeTo->m_lptTo.x-m_lptFrom.x)>Abs(m_lptFrom.x-m_lptTo.x))
		{
			m_lptTo=m_pEdgeTo->m_lptTo;
			m_bEdgeToSwap=true;
		}
		m_lptCenter=Average(m_lptFrom,m_lptTo);
	}
	m_lptAnchor=ptAdd;
	m_bAnchor=true;
	m_pView->SetCursorPrompt(IDS_ANCHOR);

	m_pDimStr=new CMDDimStr();
	UpdateDimStr();
}

void CAngularDimSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_lptAnchor=ptUpdate;
	m_bSmallerArc=GetAsyncKeyState(VK_OPTION)<0?false:true;

	UpdateDimStr();
}

void CAngularDimSketcher::OnDrawSketcher(CDC * pDC)
{
	if(m_bAnchor)
	{
		AngDimPoints adp(m_lptFrom,m_lptTo,m_lptCenter,m_lptAnchor,m_pDimStd,m_pDimFmt);
		CMDAngularDim::CalcPoints(m_bSmallerArc,adp,&m_pView->m_Mapper);

		if(adp.lptWit1Start!=adp.lptWit1End)
		{
         pDC->MoveTo(m_pView->m_Mapper.LPtoDP(adp.lptWit1Start));
         pDC->LineTo(m_pView->m_Mapper.LPtoDP(adp.lptWit1End));
		}
		if(adp.lptWit2Start!=adp.lptWit2End)
		{
         pDC->MoveTo(m_pView->m_Mapper.LPtoDP(adp.lptWit2Start));
         pDC->LineTo(m_pView->m_Mapper.LPtoDP(adp.lptWit2End));
		}

		LongRect lrc;
		Coord radius=Pythag(adp.lptDimStart-m_lptCenter);
		lrc=BoundRectOfCircle(m_lptCenter,radius);
		CRect rc=m_pView->m_Mapper.LPtoDP(lrc);
		
		CGDIArcDirection ad(pDC,AD_COUNTERCLOCKWISE);
		pointd ptAngStart=m_pView->m_Mapper.LPtoDP(adp.lptDimStart);
		pointd ptAngEnd=m_pView->m_Mapper.LPtoDP(adp.lptDimEnd);
		if(ptAngStart!=ptAngEnd)
		{
			if(adp.nDirection==AD_COUNTERCLOCKWISE) pDC->Arc(&rc,__point(ptAngStart),__point(ptAngEnd));
			else  pDC->Arc(&rc,__point(ptAngEnd),__point(ptAngStart));
		}

		if(!guAlmostEqualPt(adp.lptDimEnd,adp.lptExtent))
		{
			ptAngStart=ptAngEnd;
			ptAngEnd=m_pView->m_Mapper.LPtoDP(adp.lptExtent);
			if(ptAngStart!=ptAngEnd)
			{
				if(adp.nDirection==AD_COUNTERCLOCKWISE) pDC->Arc(&rc,__point(ptAngStart),__point(ptAngEnd));
				else  pDC->Arc(&rc,__point(ptAngEnd),__point(ptAngStart));
			}
		}

#undef new
		CDraftContext dc(pDC,&m_pView->m_Mapper);
      dc.m_bUseGDI = true;
      dc.SetOutlinePen();
		m_pDimStr->DrawShapeOutline(&dc);
	}
}

bool CAngularDimSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	m_pView->SaveUndo(IDS_CREATION);

	CPCDraftDoc* pDoc=m_pView->GetDocument();

	AngDimPoints adp(m_lptFrom,m_lptTo,m_lptCenter,m_lptAnchor,m_pDimStd,m_pDimFmt);
	CMDAngularDim::CalcPoints(m_bSmallerArc,adp,&m_pView->m_Mapper);
	if(Pythag(m_lptFrom-m_lptCenter)>Pythag(adp.lptDimStart-m_lptCenter)) m_lptFrom=adp.lptDimStart;
	if(Pythag(m_lptTo-m_lptCenter)>Pythag(adp.lptDimEnd-m_lptCenter)) m_lptTo=adp.lptDimEnd;

	CMDAngularDim* pShape=new CMDAngularDim(*m_pDimStd,*m_pDimFmt,pDoc->m_nDimCap1,pDoc->m_nDimCap2,m_bSmallerArc);
	pShape->SetShapePoint(0,m_lptFrom);
	pShape->SetShapePoint(1,m_lptTo);
	pShape->SetShapePoint(2,m_lptAnchor);
	pShape->SetShapePoint(12,m_lptCenter);
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
	{
		m_pView->LinkLineVertexes(pShape,0,m_pEdgeFrom,m_bEdgeFromSwap);
		m_pView->LinkLineVertexes(pShape,1,m_pEdgeTo,m_bEdgeToSwap);
	}
#endif

	m_pView->SetCursorPrompt(IDS_1LINEEDGE);
	return true;
}

void CAngularDimSketcher::OnCancelSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCancelSketch(pDC,pt);

	if(m_pDimStr) delete m_pDimStr;

	m_pView->SetCursorPrompt(IDS_1LINEEDGE);
}

bool CAngularDimSketcher::GetDimLineEndPt(const LongPoint &lptHit, const LongPoint &lptCenter, const LongPoint &lptFrom, const LongPoint &lptTo, LongPoint &lpt)
{
	lpt=lptHit;
	if(Abs(lptFrom.x-lptTo.x) > Abs(lptFrom.y-lptTo.y))
	{
		if(lptHit.x>lptCenter.x)
		{
			if(lptFrom.x>lptCenter.x && lptFrom.x>lpt.x) lpt=lptFrom;
			if(lptTo.x>lptCenter.x && lptTo.x>lpt.x)
			{
				lpt=lptTo;
				return true;
			}
		}
		else
		{
			if(lptFrom.x<lptCenter.x && lptFrom.x<lpt.x) lpt=lptFrom;
			if(lptTo.x<lptCenter.x && lptTo.x<lpt.x)
			{
				lpt=lptTo;
				return true;
			}
		}
	}
	else
	{
		if(lptHit.y>lptCenter.y)
		{
			if(lptFrom.y>lptCenter.y && lptFrom.y>lpt.y) lpt=lptFrom;
			if(lptTo.y>lptCenter.y && lptTo.y>lpt.y)
			{
				lpt=lptTo;
				return true;
			}
		}
		else
		{
			if(lptFrom.y<lptCenter.y && lptFrom.y<lpt.y) lpt=lptFrom;
			if(lptTo.y<lptCenter.y && lptTo.y<lpt.y)
			{
				lpt=lptTo;
				return true;
			}
		}
	}
	return false;
}

void CAngularDimSketcher::UpdateDimStr()
{
	if(m_pDimStr==NULL) return; // nothing to update
	CString sValue,sTol1,sTol2;
	if(m_pDimFmt->bUseLabel) sValue=m_pDimFmt->sLabel;
	else
	{
		CDimUnit* pdu=m_pView->GetDocument()->GetDimUnit();
		Angle arcAngle=Abs(mtAngle(m_lptCenter,m_lptTo)-mtAngle(m_lptCenter,m_lptFrom));
		if(arcAngle<180 != m_bSmallerArc) arcAngle=360-arcAngle;
		switch(m_pDimFmt->nTolerance)
		{
		case kNoTolerance:
			sValue=pdu->makeADistStr(arcAngle,false);
			break;
		case kOne:
			sValue=pdu->makeADistStr(arcAngle,false);
			//sTol1="±"+pdu->makeADistStr(m_pDimFmt->Tolerance1,false); Dennis
#ifdef _JAPANESE
			sTol1 = "}" + pdu->makeADistStr(m_pDimFmt->Tolerance1, false);
#else
			sTol1 = "±" + pdu->makeADistStr(m_pDimFmt->Tolerance1, false);
#endif
			break;
		case kTwo:
			sValue=pdu->makeADistStr(arcAngle,false);
			sTol1="+"+pdu->makeADistStr(m_pDimFmt->Tolerance1,false);
			sTol2="-"+pdu->makeADistStr(m_pDimFmt->Tolerance2,false);
			break;
		case kLimits:
			sTol1=pdu->makeADistStr(arcAngle+m_pDimFmt->Tolerance1,false);
			sTol2=pdu->makeADistStr(arcAngle+m_pDimFmt->Tolerance2,false);
			break;
		}
	}

	CDC* pDC=m_pView->GetDC();
	{
		CDraftContext dc(pDC,&m_pView->m_Mapper);
		dc.SetDimStandard(m_pDimStd);
		m_pDimStr->SetValues(&dc,sValue,sTol1,sTol2);
		m_pDimStr->ModifyTextParams(m_pView->GetDocument()->m_TextParams,tpUseFont,&dc);
	}
	m_pView->ReleaseDC(pDC);
	m_pDimStr->SetShapePoint(0,m_lptAnchor);
	m_pDimStr->CalculateExtents();
}
