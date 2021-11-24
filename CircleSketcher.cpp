// CircleSketcher.cpp: implementation of the CCircleSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CircleSketcher.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDCircle.h"
#include "DraftContext.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
void CCircleSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC,pt);
	m_pt[0]=m_pt[1]=pt;
	m_nStage=0;
}

LongPoint CCircleSketcher::AdjustPosition(LongPoint ptTo)
{
	if(GetKeyState(VK_SHIFT)<0)
		guGetHV45LPt(&ptTo,&m_pt[m_nStage]);
	return ptTo;
}

CSketcher::ContinueMode CCircleSketcher::QueryContinue()
{
	if(m_nMode==kThreePoint)
	{
		switch(m_msg.message)
		{
		case WM_LBUTTONUP:
			return ContinueAddPt;
		case WM_LBUTTONDOWN:
			return Complete;
		}
	}
	return CSketcher::QueryContinue();
}

void CCircleSketcher::OnAddPt(const LongPoint& ptAdd)
{
	m_nStage++;
	m_pt[m_nStage+1]=ptAdd;
}

void CCircleSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_pt[m_nStage+1]=ptUpdate;
	if(m_nMode!=kThreePoint)
	{
		m_pView->SetShowSize(0,(m_nMode==kRadius)?4:3,Pythag(m_pt[1]-m_pt[0]));
	}
}

void CCircleSketcher::OnDrawSketcher(CDC * pDC)
{
	LongPoint ptCenter;
	LongRect  rcBoundRect;
	Coord nExtent;
	nExtent=(Coord)Pythag(m_pt[m_nStage+1]-m_pt[0]);
	bool bDrawEllipse=true;
	switch(m_nMode)
	{
	case kDiameter:
		ptCenter=Average(m_pt[0],m_pt[1]);
		rcBoundRect.left=ptCenter.x-(nExtent>>1);
		rcBoundRect.top=ptCenter.y-(nExtent>>1);
		rcBoundRect.right=rcBoundRect.left+nExtent;
		rcBoundRect.bottom=rcBoundRect.top+nExtent;
		break;
	case kRadius:
		rcBoundRect.left=m_pt[0].x-nExtent;
		rcBoundRect.top=m_pt[0].y-nExtent;
		rcBoundRect.right=m_pt[0].x+nExtent;
		rcBoundRect.bottom=m_pt[0].y+nExtent;
		break;
	case kThreePoint:
		if(m_nStage)
		{
			bDrawEllipse=guCircBy3Pt(&ptCenter,&nExtent,m_pt[0],m_pt[1],m_pt[2]);
			rcBoundRect.left=ptCenter.x-nExtent;
			rcBoundRect.top=ptCenter.y-nExtent;
			rcBoundRect.right=ptCenter.x+nExtent;
			rcBoundRect.bottom=ptCenter.y+nExtent;
		}
		break;
	}
	if(bDrawEllipse && (m_nMode!=kThreePoint || m_nStage))
      pDC->Ellipse(m_pView->m_Mapper.LPtoDP(rcBoundRect));
   pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_pt[0]));
   pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pt[1]));
	if(m_nStage) pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pt[2]));
}

bool CCircleSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	if(m_pt[0]!=m_pt[1])
	{
		if(m_nMode==kThreePoint)
		{
			Coord nRadius;
			LongPoint lptCenter;
			if(!guCircBy3Pt(&lptCenter,&nRadius,m_pt[0],m_pt[1],m_pt[2])) return true;
		}

		m_pView->SaveUndo(IDS_CREATION);
		
		CMDCircle* pShape;
		pShape=new CMDCircle(m_nMode);
		pShape->SetShapePoint(0,m_pt[0]);
		pShape->SetShapePoint(1,m_pt[1]);
		if(m_nMode==kThreePoint) pShape->SetShapePoint(2,m_pt[2]);

		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		pShape->SetFillModel(m_pView->GetDocument()->m_Fill);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		return true;
	}
	return false;
}
