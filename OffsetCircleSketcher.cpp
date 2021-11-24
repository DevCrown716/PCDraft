#include "stdafx.h"
#include "OffsetCircleSketcher.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDCircle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSketcher::ContinueMode COffsetCircleSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_MOUSEMOVE:
		return ContinueUpdatePt;
	case WM_LBUTTONDOWN:
		return Complete;
	case WM_KEYDOWN:
		if(m_msg.wParam!=VK_ESCAPE) break;
	case WM_RBUTTONDOWN:
		return Cancel;
	}
	return Continue;
}

void COffsetCircleSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC,pt);
	Coord nRadius;
	switch(m_pBaseShape->m_nType)
	{
	case CMDCircle::kRadius:
		m_ptCenter=m_pBaseShape->GetShapePoint(0);
		break;
	case CMDCircle::kDiameter:
		m_ptCenter=Average(m_pBaseShape->GetShapePoint(0),m_pBaseShape->GetShapePoint(1));
		break;
	case CMDCircle::kThreePoint:
		guCircBy3Pt(&m_ptCenter,&nRadius,m_pBaseShape->GetShapePoint(0),
			m_pBaseShape->GetShapePoint(1),m_pBaseShape->GetShapePoint(2));
		break;
	}
	m_ptRadius=pt;
	m_pView->SetCursorPrompt(IDS_ANCHOR);
	m_pView->SetSnapMode(kSnap);
}

void COffsetCircleSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_ptRadius=ptUpdate;
	m_pView->SnapLPtToLength(m_ptCenter,m_ptRadius);
	m_pView->SetShowSize(0,4,Pythag(m_ptRadius-m_ptCenter));
}

void COffsetCircleSketcher::OnDrawSketcher(CDC * pDC)
{
	Coord nRadius=Pythag(m_ptRadius-m_ptCenter);
	LongRect rcBound=BoundRectOfCircle(m_ptCenter,nRadius);
	pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_ptCenter));
	pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_ptRadius));
	pDC->Ellipse(m_pView->m_Mapper.LPtoDP(rcBound));
}

void COffsetCircleSketcher::OnCancelSketch(CDC* pDC, const LongPoint& pt)
{
	m_pView->SetSnapMode(kNoSnap);
	CSketcher::OnCancelSketch(pDC, pt);
}

bool COffsetCircleSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	m_pView->SetSnapMode(kNoSnap);
	if(m_ptCenter!=m_ptRadius)
	{
		m_pView->SaveUndo(IDS_CREATION);
		CMDCircle* pShape=new CMDCircle(m_pBaseShape->m_nType);
		switch(m_pBaseShape->m_nType)
		{
		case CMDCircle::kRadius:
			pShape->SetShapePoint(0,m_ptCenter);
			pShape->SetShapePoint(1,m_ptRadius);
			break;
		case CMDCircle::kDiameter:
			m_ptRadius-=m_ptCenter;
			pShape->SetShapePoint(0,m_ptCenter-m_ptRadius);
			pShape->SetShapePoint(1,m_ptCenter+m_ptRadius);
			break;
		case CMDCircle::kThreePoint:
			{
				LongPoint ptRot(m_ptCenter.x+Pythag(m_ptRadius-m_ptCenter),m_ptCenter.y);
				for(int i=0;i<3;i++)
					pShape->SetShapePoint(i,mtRotPoint(ptRot,mtAngle(m_ptCenter,m_pBaseShape->GetShapePoint(i)),m_ptCenter));
			}
			break;
		}
		pShape->SetPenModel(m_pBaseShape->GetPenModel());
		pShape->SetFillModel(FillModel());
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		return true;
	}
	return false;
}
