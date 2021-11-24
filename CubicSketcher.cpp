// CubicSketcher.cpp: implementation of the CCubicSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CubicSketcher.h"

#include "gu.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDCubicShape.h"
#include "DraftContext.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CCubicSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC,pt);
	m_pt[m_nCurPoint++]=pt;
	m_pt[m_nCurPoint]=pt;
}

CSketcher::ContinueMode CCubicSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_KEYUP:
		return (m_msg.wParam==VK_BACK)?Continue:ContinueUpdatePt;
	case WM_KEYDOWN:
		if(m_msg.wParam==VK_BACK && m_nCurPoint>1)
		{
			m_bRemovePt=true;
			return ContinueAddPt;
		}
		return Continue;
	case WM_MOUSEMOVE:
		return ContinueUpdatePt;
	case WM_LBUTTONDOWN:
		return ContinueAddPt;
	case WM_LBUTTONUP:
		return (guAlmostEqualPt(m_pt[m_nCurPoint-1],m_pt[m_nCurPoint]))?Continue:ContinueAddPt;
	case WM_LBUTTONDBLCLK:
		return m_nCurPoint>2?Complete:Continue;
	default:
		return CSketcher::QueryContinue();
	}
}

void CCubicSketcher::OnAddPt(const LongPoint& ptAdd)
{
	if(m_bRemovePt)
	{
		m_pt[--m_nCurPoint]=ptAdd;
		m_bRemovePt=false;
	}
	else
	{
		if(m_nCurPoint<kMaxPoint) m_nCurPoint++;
		m_pt[m_nCurPoint]=ptAdd;
	}
}

void CCubicSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_pt[m_nCurPoint]=ptUpdate;
}

void CCubicSketcher::OnDrawSketcher(CDC * pDC)
{
	pointd pt[7];
	if(m_nCurPoint<2)
	{
      pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_pt[0]));
      pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pt[1]));
	}
	else
	{
		LongPoint lpt12,lpt32;
		guBezSplinePts(m_pt[0],m_pt[1],m_pt[2],&lpt12,&lpt32);
		pt[0]=m_pView->m_Mapper.LPtoDP(m_pt[0]);
		pt[1]=pt[0];
		pt[2]=m_pView->m_Mapper.LPtoDP(lpt12);
		pt[3]=m_pView->m_Mapper.LPtoDP(m_pt[1]);
		pt[4]=m_pView->m_Mapper.LPtoDP(lpt32);
//		pt[5]=pt[6];
		pt[6]=m_pView->m_Mapper.LPtoDP(m_pt[2]);
      CPoint p[7];
      __copy(p, pt, 7);
		pDC->PolyBezier(p,4);
		for(int i=3;i<=m_nCurPoint;i++)
		{
			guBezSplinePts(m_pt[i-2],m_pt[i-1],m_pt[i],&lpt12,&lpt32);
			pt[0]=pt[3];
			pt[1]=pt[4];
			pt[2]=m_pView->m_Mapper.LPtoDP(lpt12);
			pt[3]=pt[6];
			pt[4]=m_pView->m_Mapper.LPtoDP(lpt32);
//			pt[5]=pt[6];
			pt[6]=m_pView->m_Mapper.LPtoDP(m_pt[i]);
         __copy(p, pt, 7);

         pDC->PolyBezier(p,4);

		}
		pt[5]=pt[6];
      __copy(p, pt, 7);
      pDC->PolyBezier(p+3,4);
	}
}

bool CCubicSketcher::OnCompleteSketch(CDC *pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);

	m_nCurPoint--;
	bool bClosed=m_pt[0]==m_pt[m_nCurPoint];
	if(bClosed) m_nCurPoint--;

	if(m_nCurPoint>(bClosed?1:0))
	{
		m_pView->SaveUndo(IDS_CREATION);
		CMDCubicShape* pShape=new CMDCubicShape(m_nCurPoint+1,bClosed);

		for(int i=0;i<=m_nCurPoint;i++)
			pShape->SetShapePoint(i,m_pt[i]);

		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		if(bClosed) pShape->SetFillModel(m_pView->GetDocument()->m_Fill);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);

		return true;
	}
	return false;
}
