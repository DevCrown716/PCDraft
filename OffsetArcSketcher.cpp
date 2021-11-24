#include "stdafx.h"
#include "OffsetArcSketcher.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDArcShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CSketcher::ContinueMode COffsetArcSketcher::QueryContinue()
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

void COffsetArcSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC,pt);
	Coord nRadius;
	switch(m_pBaseShape->m_nMode)
	{
	case CMDArcShape::kRadius:
		m_ptCenter=m_pBaseShape->GetCenter();
		break;
	case CMDArcShape::kThreePoint:
		guCircBy3Pt(&m_ptCenter,&nRadius,m_pBaseShape->GetShapePoint(0),
			m_pBaseShape->GetShapePoint(1),m_pBaseShape->GetShapePoint(2));
		break;
	}
	OnUpdatePt(pt);
	m_pView->SetCursorPrompt(IDS_ANCHOR);
	m_pView->SetSnapMode(kSnap);
}

void COffsetArcSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	Coord newRadius=Pythag(ptUpdate-m_ptCenter);
	m_pView->SnapLength(newRadius);
	m_pView->SetShowSize(0,4,newRadius);

	LongPoint ptRot=m_ptCenter;
	ptRot.x+=newRadius;
	for(int i=0,j=m_pBaseShape->GetNumShapePoints();i<j;i++)
	{
		LongPoint ptShape=m_pBaseShape->GetShapePoint(i);
		if(ptShape!=m_ptCenter)
			m_pt[i]=mtRotPoint(ptRot,mtAngle(m_ptCenter,ptShape),m_ptCenter);
		else
			m_pt[i]=m_ptCenter;
	}
}

void COffsetArcSketcher::OnDrawSketcher(CDC * pDC)
{
	Coord nRadius=Pythag(m_pt[2]-m_ptCenter);
	LongRect rcBound=BoundRectOfCircle(m_ptCenter,nRadius);
	CGDIArcDirection ad(pDC,AD_COUNTERCLOCKWISE);
	switch(m_pBaseShape->m_nMode)
	{
	case CMDArcShape::kRadius:
		if(m_pBaseShape->m_nArcDirection==AD_COUNTERCLOCKWISE) pDC->Arc(m_pView->m_Mapper.LPtoDP(rcBound),m_pView->m_Mapper.LPtoDP(m_pt[1]),m_pView->m_Mapper.LPtoDP(m_pt[2]));
		else  pDC->Arc(m_pView->m_Mapper.LPtoDP(rcBound),m_pView->m_Mapper.LPtoDP(m_pt[2]),m_pView->m_Mapper.LPtoDP(m_pt[1]));
		break;
	case CMDArcShape::kThreePoint:
		if(m_pBaseShape->m_nArcDirection==AD_COUNTERCLOCKWISE) pDC->Arc(m_pView->m_Mapper.LPtoDP(rcBound),m_pView->m_Mapper.LPtoDP(m_pt[0]),m_pView->m_Mapper.LPtoDP(m_pt[2]));
		else  pDC->Arc(m_pView->m_Mapper.LPtoDP(rcBound),m_pView->m_Mapper.LPtoDP(m_pt[2]),m_pView->m_Mapper.LPtoDP(m_pt[0]));
		break;
	}
}

void COffsetArcSketcher::OnCancelSketch(CDC* pDC, const LongPoint& pt)
{
	m_pView->SetSnapMode(kNoSnap);
	CSketcher::OnCancelSketch(pDC, pt);
}

bool COffsetArcSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	m_pView->SetSnapMode(kNoSnap);
	if(m_ptCenter!=m_ptRadius)
	{
		m_pView->SaveUndo(IDS_CREATION);
		CMDArcShape* pShape=new CMDArcShape(m_pBaseShape->m_nMode);
		pShape->m_nArcDirection=m_pBaseShape->m_nArcDirection;
		for(int i=0,j=pShape->GetNumShapePoints();i<j;i++)
			pShape->SetShapePoint(i,m_pt[i]);
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
