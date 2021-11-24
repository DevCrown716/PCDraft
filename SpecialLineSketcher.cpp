// SpecialLineSketcher.cpp: implementation of the CSpecialLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PCDraft.h"
#include "SpecialLineSketcher.h"

#include "math.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "Edge.h"
#include "MDLineShape.h"
#include "Layer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CSpecialLineSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CLineSketcher::OnBeginSketch(pDC, pt);
	m_pView->SetCursorPrompt(IDS_CLICKPOINT);
	m_pView->SetSnapMode(kSnap);
}

CSketcher::ContinueMode CSpecialLineSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_LBUTTONDOWN:
		return (m_nType!=kTangent || m_ptFrom!=m_ptTo)?Complete:Continue;
	case WM_LBUTTONUP:
		return Continue;
	default:
		return CSketcher::QueryContinue();
	}
}

Angle DUBLmtATan2(double x,double y)
{
	Angle vAngle;

	if ( x == 0L  && y == 0L) return FixedZero ;
	if ((vAngle = mtRealToFix (atan2 (y,x) * kRadToAng)) < FixedZero )
		vAngle += k360;

	return ( (k360 - (vAngle % k360)) % k360 );		/* flip angle */
}

void CSpecialLineSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_ptTo=ptUpdate;
	switch(m_nType)
	{
	case kPoint:
		m_ptFrom=m_lpt;
		break;
	case kEdgePerp:
		m_pEdge->PerpPt(m_ptTo,&m_ptFrom);
		break;
	case kArcPerp:
		{
			Angle angle;
			angle=mtATan2(m_ptTo-m_lpt);
			LongPoint ptBase=m_lpt;
			ptBase.x+=m_Radius;
			m_ptFrom=mtRotPoint(ptBase,angle,m_lpt);
		}
		break;
	case kTangent:
		{
			Coord dist=Pythag(m_ptTo-m_lpt);
			if(dist<=m_Radius) m_ptFrom=m_ptTo;
			else
			{
				double sideA=m_Radius.GetRawDouble();
				double sideC=dist.GetRawDouble();
				double sideB=sqrt(sideC*sideC-sideA*sideA);

				Angle theta=DUBLmtATan2(sideA,sideB);
				Angle angle=mtATan2(m_ptTo-m_lpt);
				if(GetAsyncKeyState(VK_OPTION)<0) angle+=theta;
				else angle-=theta;
				LongPoint ptBase=m_lpt;
				ptBase.x+=m_Radius;
				m_ptFrom=mtRotPoint(ptBase,angle,m_lpt);
			}
		}
		break;
	case kOffset:
		{
			guNearPtLnPt(m_ptTo,m_pEdge->m_lptFrom,m_pEdge->m_lptTo,&m_ptFrom);
			if(GetKeyState(VK_OPTION)<0)
				m_pView->SnapLPtToLength(m_ptFrom,m_ptTo);

			m_pView->SetShowSize(0,8,Pythag(m_ptTo-m_ptFrom));
			m_pView->SetShowSize(1,-1,mtAngle(m_ptFrom,m_ptTo));

			LongPoint delta=m_ptTo-m_ptFrom;
			m_ptFrom=m_pEdge->m_lptFrom+delta;
			m_ptTo=m_pEdge->m_lptTo+delta;
			return;
		}
		break;
	}

	if(m_nType!=kTangent && GetKeyState(VK_OPTION)<0)
		m_pView->SnapLPtToLength(m_ptFrom,m_ptTo);
	m_pView->SetShowSize(0,1,Pythag(m_ptTo-m_ptFrom));
	m_pView->SetShowSize(1,-1,mtAngle(m_ptFrom,m_ptTo));
}

void CSpecialLineSketcher::OnCancelSketch(CDC* pDC, const LongPoint& pt)
{
	m_pView->SetSnapMode(kNoSnap);
	CLineSketcher::OnCancelSketch(pDC, pt);
}

bool CSpecialLineSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	m_pView->SetSnapMode(kNoSnap);
	if(kOffset==m_nType)
	{
		CSketcher::OnCompleteSketch(pDC, pt);
		if(m_ptFrom!=m_ptTo)
		{
			m_pView->SaveUndo(IDS_CREATION);
			CMDLineShape* pShape=new CMDLineShape(m_ptFrom,m_ptTo);
			int nLineVertex1,nLineVertex2;
			CMDShape* pFromShape=m_pView->GetDocument()->GetActiveLayer()->ShapeFromEdge(m_pEdge,nLineVertex1,nLineVertex2);
			if(pFromShape) pShape->SetPenModel(pFromShape->GetPenModel());
			pShape->CalculateExtents();
			pShape->SetShapeDatum(kDefaultDatum);
			m_pView->GetDocument()->CreateShape(pShape);
			m_pView->SelectShape(pShape,false);
			return true;
		}
		return false;
	}
	return CLineSketcher::OnCompleteSketch(pDC, pt);
}
