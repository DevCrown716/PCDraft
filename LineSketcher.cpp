// LineSketcher.cpp: implementation of the CLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LineSketcher.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDLineShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CLineSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC, pt);
	m_ptFrom=m_ptTo=pt;
}

void CLineSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_ptTo=ptUpdate;
	m_pView->GetDocument()->SnapToAngle(m_ptTo,m_ptFrom,GetKeyState(VK_SHIFT)<0);
	if(GetKeyState(VK_OPTION)<0) m_pView->SnapLPtToLength(m_ptFrom,m_ptTo);

	m_pView->SetShowSize(0,1,Pythag(m_ptTo-m_ptFrom));
	m_pView->SetShowSize(1,-1,mtAngle(m_ptFrom,m_ptTo));
}

void CLineSketcher::OnDrawSketcher(CDC * pDC)
{
	if(m_ptTo!=m_ptFrom)
	{
		pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_ptFrom));
		pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_ptTo));
	}
}

CSketcher::ContinueMode CLineSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_LBUTTONDOWN:
		return Complete;

	case WM_LBUTTONUP:
		return ((m_ptFrom==m_ptTo) && ((CPCDraftApp*)AfxGetApp())->m_bClickToStartPoly)?Continue:Complete;

	case WM_KEYDOWN:
		if(m_msg.wParam!=VK_ESCAPE) return ContinueUpdatePt;
		return Cancel;
	}
	return CSketcher::QueryContinue();
}

bool CLineSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	if(m_ptFrom!=m_ptTo)
	{
		m_pView->SaveUndo(IDS_CREATION);
		CMDLineShape* pShape=new CMDLineShape(m_ptFrom,m_ptTo);
		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		return true;
	}
	return false;
}
