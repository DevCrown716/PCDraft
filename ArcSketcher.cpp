// ArcSketcher.cpp: implementation of the CArcSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ArcSketcher.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDArcShape.h"
#include "DraftContext.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
void CArcSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt) 
{ 
	CSketcher::OnBeginSketch(pDC,pt); 
	m_pt[0]=m_pt[1]=m_pt[3]=pt; 
	m_nStage=0; 
	m_nArcDirection=AD_COUNTERCLOCKWISE;
	m_bSnapLength=(GetAsyncKeyState(VK_COMMAND)>=0);
} 

LongPoint CArcSketcher::AdjustPosition(LongPoint ptTo) 
{ 
	if(m_nMode!=kElliptic || m_nStage)
	{
		m_pView->GetDocument()->SnapToAngle(ptTo,m_pt[m_nMode!=kThreePoint?0:m_nStage],GetKeyState(VK_SHIFT)<0);
	}
	return ptTo; 
} 

CSketcher::ContinueMode CArcSketcher::QueryContinue() 
{ 
	switch(m_msg.message) 
	{ 
	case WM_LBUTTONUP: 
		if(m_nMode==kElliptic && guAlmostEqualPt(m_pt[0],m_pt[3])) return Cancel;
		return guAlmostEqualPt(m_pt[0],m_pt[1])?Cancel:ContinueAddPt; 
	case WM_LBUTTONDOWN: 
		return Complete; 
	} 
	return CSketcher::QueryContinue(); 
} 

void CArcSketcher::OnAddPt(const LongPoint& ptAdd) 
{ 
	m_nStage++; 
	m_pt[m_nStage+1]=ptAdd; 
	m_pView->ClearShowSize();
} 

void CArcSketcher::OnUpdatePt(const LongPoint& ptUpdate) 
{
	if(m_nStage && (m_nMode==kRadius || m_nMode==kElliptic))
	{
		Angle ang1=mtAngle(m_pt[0],m_pt[1]);
		Angle angDeltaBefore=mtFixAngle(mtAngle(m_pt[0],m_pt[2])-ang1);
		Angle angDeltaAfter=mtFixAngle(mtAngle(m_pt[0],ptUpdate)-ang1);

		if(angDeltaBefore>k270 && angDeltaAfter<k90)
			m_nArcDirection=AD_COUNTERCLOCKWISE;
		else if(angDeltaBefore<k90 && angDeltaAfter>k270)
			m_nArcDirection=AD_CLOCKWISE;
	}
	else if(m_nStage && m_nMode==kThreePoint)
	{
		Angle ang=mtFixAngle(mtAngle(m_pt[0],m_pt[1])-mtAngle(m_pt[0],ptUpdate));
		m_nArcDirection=ang<k180?AD_CLOCKWISE:AD_COUNTERCLOCKWISE;
	}
	else if(!m_nStage && m_nMode==kElliptic && GetKeyState(VK_SHIFT)>=0)
	{
		m_pt[3]=m_pt[1];
	}
	m_pt[m_nStage+1]=ptUpdate; 

	if(m_nStage)
	{
		if(m_nMode==kThreePoint)
		{
			LongPoint lptCenter; 
			Coord nRadius; 
			if(guCircBy3Pt(&lptCenter,&nRadius,m_pt[0],m_pt[1],m_pt[2]))
			{
				Angle angle=mtFixAngle(mtAngle(lptCenter,m_pt[2])-mtAngle(lptCenter,m_pt[0]));
				if(m_nArcDirection==AD_CLOCKWISE) angle=360-angle;
				m_pView->SetShowSize(0,-1,angle);
			}
		}
		else
		{
			Angle toAngle=mtAngle(m_pt[0],m_pt[2]);
			Angle angle=mtFixAngle(toAngle-mtAngle(m_pt[0],m_pt[1]));
			if(m_nArcDirection==AD_CLOCKWISE) angle=360-angle;
			m_pView->SetShowSize(0,-1,toAngle);
			m_pView->SetShowSize(1,-3,angle);
		}
	}
	else
	{
		m_bSnapLength=(GetAsyncKeyState(VK_COMMAND)>=0);
		switch(m_nMode)
		{
		case kRadius:
			{
				Coord nRadius=Pythag(m_pt[1]-m_pt[0]);
				if(m_bSnapLength) m_pView->SnapLength(nRadius);
				m_pView->SetShowSize(0,4,nRadius);
				m_pView->SetShowSize(1,-1,mtAngle(m_pt[0],m_pt[1]));
			}
			break;
		case kElliptic:
			m_pView->SetShowSize(0,1,Abs(m_pt[3].x-m_pt[0].x));
			m_pView->SetShowSize(1,2,Abs(m_pt[3].y-m_pt[0].y));
			m_pView->SetShowSize(2,-1,mtAngle(m_pt[0],m_pt[1]));
			break;
		}
	}
} 

void CArcSketcher::OnDrawSketcher(CDC * pDC) 
{ 
	CGDIROP gdiRop(pDC,R2_XORPEN);
	CGDIStockObject gdiPen(pDC,WHITE_PEN);
	CGDIStockObject gdiBrush(pDC,NULL_BRUSH);

	switch(m_nMode) 
	{ 
	case kRadius: 
	case kElliptic: 
		{ 
			LongRect rc; 
			if(m_nMode==kRadius) 
			{ 
				Coord nRadius=Pythag(m_pt[1]-m_pt[0]);
				if(m_bSnapLength) m_pView->SnapLength(nRadius);
				rc=BoundRectOfCircle(m_pt[0],nRadius);
			} 
			else 
			{ 
				rc=BoundRectOfEllipse(m_pt[0],m_pt[3]-m_pt[0]);
			} 

			if(!m_nStage) 
			{
				LongPoint pt=m_pt[1];
				if(m_nMode==kElliptic)
					pt=CMDArcShape::SnapPtToEllipse(m_pt[0],Abs(m_pt[3]-m_pt[0]),pt);
            pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_pt[0]));
            pDC->LineTo(m_pView->m_Mapper.LPtoDP(pt));
            pDC->Ellipse(m_pView->m_Mapper.LPtoDP(rc));
			}
			else 
			{
				pointd ptAngStart=m_pView->m_Mapper.LPtoDP(m_pt[1]);
				pointd ptAngEnd=m_pView->m_Mapper.LPtoDP(m_pt[2]);
				if(ptAngStart!=ptAngEnd)
				{
					CGDIArcDirection ad(pDC,AD_COUNTERCLOCKWISE);
					if(m_nArcDirection==AD_COUNTERCLOCKWISE) pDC->Arc(m_pView->m_Mapper.LPtoDP(rc),__point(ptAngStart),__point(ptAngEnd));
					else  pDC->Arc(m_pView->m_Mapper.LPtoDP(rc),__point(ptAngEnd),__point(ptAngStart));
               pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_pt[0]));
               pDC->LineTo(__point(ptAngEnd));
				}
			} 
		} 
		break; 
	case kThreePoint: 
		{ 
      pDC->MoveTo(m_pView->m_Mapper.LPtoDP(m_pt[0]));
      pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pt[1]));
			if(m_nStage) 
			{ 
				LongPoint ptCenter; 
				Coord nRadius; 
            pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pt[2]));
				if(guCircBy3Pt(&ptCenter,&nRadius,m_pt[0],m_pt[1],m_pt[2]))
				{ 
					CGDIArcDirection ad(pDC,AD_COUNTERCLOCKWISE);
					LongRect rc=BoundRectOfCircle(ptCenter,nRadius);
					if(m_nArcDirection==AD_COUNTERCLOCKWISE) pDC->Arc(m_pView->m_Mapper.LPtoDP(rc),m_pView->m_Mapper.LPtoDP(m_pt[0]),m_pView->m_Mapper.LPtoDP(m_pt[2]));
					else  pDC->Arc(m_pView->m_Mapper.LPtoDP(rc),m_pView->m_Mapper.LPtoDP(m_pt[2]),m_pView->m_Mapper.LPtoDP(m_pt[0]));
					
				} 
			} 
		} 
		break; 
	} 
} 

bool CArcSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt) 
{ 
	CSketcher::OnCompleteSketch(pDC, pt);
	if(mtAngle(m_pt[0],m_pt[1])!=mtAngle(m_pt[0],m_pt[2])) //m_pt[0]!=m_pt[1]
	{
		switch(m_nMode)
		{
		case kRadius:
			{
				if(m_bSnapLength) m_pView->SnapLPtToLength(m_pt[0],m_pt[1]);
				Angle ang=mtAngle(m_pt[0],m_pt[2]);
				m_pt[2].x=Pythag(m_pt[1]-m_pt[0]); m_pt[2].y=0;
				m_pt[2]=m_pt[0]+mtRotPoint(m_pt[2], ang);
			}
			break;
		case kElliptic:
			m_pt[1]=CMDArcShape::SnapPtToEllipse(m_pt[0],Abs(m_pt[3]-m_pt[0]),m_pt[1]);
			m_pt[2]=CMDArcShape::SnapPtToEllipse(m_pt[0],Abs(m_pt[3]-m_pt[0]),m_pt[2]);
			break;
		case kThreePoint:
			{
				Coord nRadius;
				LongPoint lptCenter;
				if(!guCircBy3Pt(&lptCenter,&nRadius,m_pt[0],m_pt[1],m_pt[2])) return true;
			}
			break;
		}

		m_pView->SaveUndo(IDS_CREATION);

		CMDArcShape* pShape;
		pShape=new CMDArcShape(m_nMode);
		pShape->m_nArcDirection=m_nArcDirection;
		pShape->SetShapePoint(0,m_pt[0]);
		pShape->SetShapePoint(1,m_pt[1]);
		pShape->SetShapePoint(2,m_pt[2]);
		if(m_nMode==kElliptic)
			pShape->SetShapePoint(3,m_pt[0]+Abs(m_pt[3]-m_pt[0]));

		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		FillModel fm;
		fm.m_nFillType=kNoFill;
		pShape->SetFillModel(fm);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		return true;
	}
	return false;
} 
