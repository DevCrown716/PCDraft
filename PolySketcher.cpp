// PolySketcher.cpp: implementation of the CPolySketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PolySketcher.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDPolygonShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CPolySketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC,pt);
	m_pt[m_nCurPoint++]=pt;
	m_pt[m_nCurPoint]=pt;
}

CSketcher::ContinueMode CPolySketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_KEYDOWN:
		if(m_msg.wParam==VK_BACK && m_nCurPoint>1)
		{
			m_bRemovePt=true;
			return ContinueAddPt;
		}
		return Continue;
	case WM_LBUTTONDOWN:
		if(m_pt[0]==m_pt[m_nCurPoint])
		{
			if(m_nCurPoint>2)
			{
				m_bClosed=true;
				return Complete;
			}
			else return Cancel;
		}
		return ContinueUpdatePt;
	case WM_LBUTTONUP:
		{
			bool bNearPoints=IsNearPoints(m_pt[0],m_pt[1]);
			if(m_nCurPoint==1 && bNearPoints && ((CPCDraftApp*)AfxGetApp())->m_bClickToStartPoly) return Continue;
			return (m_nCurPoint>1 || !bNearPoints)?ContinueAddPt:Cancel;
		}
	default:
		return CSketcher::QueryContinue();
	}
}

LongPoint CPolySketcher::AdjustPosition(LongPoint ptTo)
{
/*	if(GetKeyState(VK_SHIFT)<0)
		guGetHV45LPt(&ptTo,&m_pt[m_nCurPoint-1]);
	if(GetKeyState(VK_OPTION)<0)
		m_pView->SnapLPtToLength(m_pt[m_nCurPoint-1],ptTo);
*/	return ptTo;
}

void CPolySketcher::OnAddPt(const LongPoint& ptAdd)
{
	if(m_bRemovePt ||(m_nCurPoint && IsNearPoints(m_pt[m_nCurPoint-1],ptAdd)))
	{
		m_nCurPoint--;
		m_bRemovePt=false;
	}
	else
	{
		if(m_nCurPoint<kMaxPoint) m_nCurPoint++;
		m_pt[m_nCurPoint]=ptAdd;
	}
}

void CPolySketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_pt[m_nCurPoint]=ptUpdate;
	m_pView->GetDocument()->SnapToAngle(m_pt[m_nCurPoint],m_pt[m_nCurPoint-1],GetKeyState(VK_SHIFT)<0);
	if(GetKeyState(VK_OPTION)<0) m_pView->SnapLPtToLength(m_pt[m_nCurPoint-1],m_pt[m_nCurPoint]);
	m_pView->SetShowSize(0,1,Pythag(m_pt[m_nCurPoint]-m_pt[m_nCurPoint-1]));
	m_pView->SetShowSize(1,-1,mtAngle(m_pt[m_nCurPoint-1],m_pt[m_nCurPoint]));
}

void CPolySketcher::OnDrawSketcher(CDC * pDC)
{
	CGDIROP gdiRop(pDC,R2_XORPEN);
	CGDIStockObject gdiPen(pDC,WHITE_PEN);
	pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_pt[0]));
	for(int i=1;i<=m_nCurPoint;i++)
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pt[i]));
}

bool CPolySketcher::OnCompleteSketch(CDC *pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	if(m_nCurPoint>1)
	{
		m_pView->SaveUndo(IDS_CREATION);

		CMDPolygonShape* pShape=new CMDPolygonShape(CMDPolygonShape::kPolygon,m_nCurPoint,m_bClosed);

		for(int i=0;i<m_nCurPoint;i++)
			pShape->SetShapePoint(i,m_pt[i]);

		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		if(m_bClosed) pShape->SetFillModel(m_pView->GetDocument()->m_Fill);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		return true;
	}
	return false;
}

bool CPolySketcher::IsNearPoints(const LongPoint &lpt1, const LongPoint &lpt2)
{
	short nPenHalf=(m_pView->GetDocument()->m_Pen.m_nWidth&0x1F)>>1;
	if(nPenHalf<3) nPenHalf=3;
	return Pythag(lpt2-lpt1)<m_pView->m_Mapper.DPtoLP((long)nPenHalf);
}
