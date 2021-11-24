// TangentLineSketcher.cpp: implementation of the CTangentLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pcdraft.h"
#include "TangentLineSketcher.h"

#include "phmath.h"
#include "Layer.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDLineShape.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CTangentLineSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC, pt);
	m_lpt=pt;
	m_pView->SetCursorPrompt(IDS_2CIRCLEARC);
}

bool CTangentLineSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);

	m_pView->SetCursorPrompt(IDS_2CIRCLEARC);

	LongPoint lptFrom=m_lpt;
	LongPoint lptTo=pt;
	if(ComputeTangent(lptFrom,lptTo))
	{

		m_pView->SaveUndo(IDS_CREATION);
		CMDLineShape* pShape=new CMDLineShape(lptFrom,lptTo);
		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		return true;
	}
	else AfxMessageBox(IDS_TANGENT_NOT_EXIST);
	return false;
}

void CTangentLineSketcher::OnCancelSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCancelSketch(pDC,pt);
	m_pView->SetCursorPrompt(IDS_1CIRCLEARC);
}

CSketcher::ContinueMode CTangentLineSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_LBUTTONDOWN:
		{
			pointd pt=m_msg.pt;
			m_pView->ScreenToClient(&pt);
			pt+=m_pView->GetDeviceScrollPosition();
			CMDShape* pShape=m_pView->GetDocument()->GetActiveLayer()->ShapeFromPoint(pt,&m_pView->m_Mapper,true,true);
			if(pShape && pShape->IsRadial())
			{
				if(m_pShapeFrom==pShape)
				{
					AfxMessageBox(IDS_TANGENT_NOT_EXIST);
					m_pView->m_Mistakes.Reset();
					return Cancel;
				}
				LongPoint radii=pShape->GetRadii();
				if(radii.x==radii.y)	// circular only
				{
					m_pShapeTo=pShape;
					m_pView->m_Mistakes.Reset();
					return Complete;
				}
				else m_pView->m_Mistakes.ShowMessage(IDS_SELECT_CIRC_ARC);
			}
			else m_pView->m_Mistakes.ShowMessage(IDS_SELECT_CIRC_ARC);
		}
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		return Continue;
	default:
		return CSketcher::QueryContinue();
	}
}

bool CTangentLineSketcher::ComputeTangent(LongPoint &lptFrom, LongPoint &lptTo)
{
	LongPoint centerPt1=m_pShapeFrom->GetCenter();
	LongPoint centerPt2=m_pShapeTo->GetCenter();

	Coord centersDist=Pythag(centerPt1-centerPt2);

	Coord radius1=m_pShapeFrom->GetRadii().x;
	Coord radius2=m_pShapeTo->GetRadii().x;

	// looking for an easy test for concentric circles...
	if(centersDist < Abs(radius1-radius2)) return false;

	Angle centersAngle;
	if(radius1>=radius2)
		centersAngle=mtFastATan2(centerPt1-centerPt2);
	else
		centersAngle=mtFastATan2(centerPt2-centerPt1);

	Angle hit1Angle=mtFastATan2(lptFrom-centerPt1);
	Angle hit1RelAngle=mtFixAngle(hit1Angle-centersAngle);
	bool hit1Side=(hit1RelAngle < k180)?false:true;

	Angle hit2Angle=mtFastATan2(lptTo-centerPt2);
	Angle hit2RelAngle=mtFixAngle(hit2Angle-centersAngle);
	bool hit2Side=(hit2RelAngle < k180)?false:true;

	WrFixed tAngleSin;
	if(hit1Side == hit2Side) tAngleSin=Abs(radius1-radius2)/centersDist;
	else tAngleSin=(radius1+radius2)/centersDist;

	Angle tangentAngle=mtRealToFix(kRadToAng*asin(mtFixToReal(tAngleSin)));
	Angle t1Angle,t2Angle;

	if(hit1Side==hit2Side)
	{
		if(hit1Side) t1Angle=t2Angle=centersAngle-k90-tangentAngle;
		else t1Angle=t2Angle=centersAngle+k90+tangentAngle;
	}
	else
	{
		if(hit1Side)
		{
			if(radius1>=radius2)
			{
				t1Angle=centersAngle-k90-tangentAngle;
				t2Angle=centersAngle+k90-tangentAngle;
			}
			else
			{
				t1Angle=centersAngle-k90+tangentAngle;
				t2Angle=centersAngle+k90+tangentAngle;
			}
		}
		else
		{
			if(radius1>=radius2)
			{
				t1Angle=centersAngle+k90+tangentAngle;
				t2Angle=centersAngle-k90+tangentAngle;
			}
			else
			{
				t1Angle=centersAngle+k90-tangentAngle;
				t2Angle=centersAngle-k90-tangentAngle;
			}
		}
	}

	lptFrom=centerPt1;
	lptFrom.x+=radius1;
	lptFrom=mtRotPoint(lptFrom, t1Angle, centerPt1);

	lptTo=centerPt2;
	lptTo.x+=radius2;
	lptTo=mtRotPoint(lptTo, t2Angle, centerPt2);

	return true;
}
