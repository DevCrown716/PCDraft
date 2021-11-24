// ConstrLineSketcher.cpp: implementation of the CConstrLineSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ConstrLineSketcher.h"

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

void CConstrLineSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_ptTo=ptUpdate;
	bool bShift=GetKeyState(VK_SHIFT)<0;
	if(m_nConstraintAngle)
	{
		if(!bShift)
		{
			double m;
			short shortAngle;
			shortAngle=RoundToShort(mtAngle(m_ptFrom,m_ptTo));
			shortAngle=((shortAngle+(m_nConstraintAngle>>1)+1)/m_nConstraintAngle)*m_nConstraintAngle;
			shortAngle%=360;
			Angle longAngle=mtDMS2Ang(shortAngle,0,0);
			if((shortAngle%45)==0)
			{
				if((shortAngle%180)==0)
					m_ptTo.y=m_ptFrom.y;
				else if((shortAngle%90)==0)
					m_ptTo.x=m_ptFrom.x;
				guGetHV45LPt(&m_ptTo,&m_ptFrom);
			}
			else
			{
				m=mtSin(longAngle)/mtCos(longAngle);
				m_ptTo.y=(m*m_ptTo.x+m_ptFrom.y-m*m_ptFrom.x);
			}
		}
	}
	else if(bShift) guGetHV45LPt(&m_ptTo,&m_ptFrom);
	if(GetKeyState(VK_OPTION)<0) m_pView->SnapLPtToLength(m_ptFrom,m_ptTo);

	m_pView->SetShowSize(0,1,Pythag(m_ptTo-m_ptFrom));
	m_pView->SetShowSize(1,-1,mtAngle(m_ptFrom,m_ptTo));
}


bool CConstrLineSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	if(m_ptFrom!=m_ptTo)
	{
		m_pView->SaveUndo(IDS_CREATION);
		CMDLineShape* pShape=new CMDLineShape(m_ptFrom,m_ptTo,m_nConstraintAngle);
		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		return true;
	}
	return false;
}
