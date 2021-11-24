// RotateSketcher.cpp: implementation of the CRotateSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PCDraft.h"
#include "RotateSketcher.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraftView.h"

#include "DraftContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

CRotateSketcher::CRotateSketcher(Angle angle, bool bAboutCenter, LongPoint* pExtentCenter) : m_nMode(0), m_bAboutCenter(bAboutCenter), m_pExtentCenter(pExtentCenter)
{
	mtAng2DMS(angle,&m_nDegree,&m_nMinute,&m_nSecond);
	m_startAngle=angle;
}

void CRotateSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC, pt);
	m_ptCenter=m_pt=pt;
	m_Angle=mtDMS2Ang(m_nDegree,m_nMinute,m_nSecond);
	if(m_bAboutCenter)
	{
		m_ptCenter.Offset(m_pView->m_Mapper.DPtoLP((long)-20),0);
		m_ptCenter=mtRotPoint(m_ptCenter,m_Angle,m_pt);
	}
}

CSketcher::ContinueMode CRotateSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_KEYDOWN:
		if((m_msg.wParam==VK_SHIFT && m_nMode<2) || (m_msg.wParam==VK_COMMAND && m_nMode>0))
		{
			return ContinueAddPt;
		}

	default:
		return CSketcher::QueryContinue();
	}
}

void CRotateSketcher::OnAddPt(const LongPoint& ptAdd)
{
	if(m_msg.wParam==VK_SHIFT) m_nMode++;
	else m_nMode--;
}

void CRotateSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	m_pt=ptUpdate;
	Angle arcAngle=mtATan2(m_pt-m_ptCenter);
	short degree=mtFixToShort(arcAngle),increment;

	switch(m_nMode)
	{
	case 0:		// degree
		increment=(GetAsyncKeyState(VK_COMMAND)<0)?1:5;
		m_nDegree=(degree/increment)*increment;
		break;
	case 1:		// minute
		m_nMinute=(degree/6)%60;
		break;
	case 2:		// second
		m_nSecond=(degree/6)%60;
		break;
	}
	m_Angle=mtDMS2Ang(m_nDegree,m_nMinute,m_nSecond);
	m_pView->SetShowSize(0,-1,m_Angle);
}

#define	kBoxWidth		25
#define	kDeltaCorner	9
#define	kDeltaCircle	5

void CRotateSketcher::OnDrawSketcher(CDC * pDC)
{
	pointd ptCenter=m_pView->m_Mapper.LPtoDP(m_ptCenter);
	CRect rc=BoundRectOfCircle(ptCenter,kBoxWidth-kDeltaCircle-1);

	{
		CPen pen(PS_DOT,1,RGB(0,0,0));
		CGDIObject<CPen> op(pDC,&pen);
		pDC->Ellipse(&rc);
	}

	rc.InflateRect(kDeltaCircle,kDeltaCircle);

	pDC->MoveTo(ptCenter.x, rc.top);
	pDC->LineTo(ptCenter.x, rc.top+kDeltaCorner);
	pDC->MoveTo(ptCenter.x, rc.bottom);
	pDC->LineTo(ptCenter.x, rc.bottom-kDeltaCorner);
	pDC->MoveTo(rc.left, ptCenter.y);
	pDC->LineTo(rc.left+kDeltaCorner, ptCenter.y);
	pDC->MoveTo(rc.right,ptCenter.y);
	pDC->LineTo(ptCenter);

	short hOffset=rc.right+8, vOffset=ptCenter.y-4;

	if(m_nMode)
	{
		rc.SetRect(0,0,1,4);			/* draw minute mark */
		rc.OffsetRect(hOffset+8,vOffset);
		pDC->Rectangle(rc);

		if(m_nMode==2)					/* add one to make second */
		{
			rc.SetRect(3,0,4,4);
			rc.OffsetRect(hOffset+8,vOffset);
			pDC->Rectangle(rc);
		}
	}
	else
	{
		pDC->MoveTo(rc.left+kDeltaCorner, rc.top+kDeltaCorner);
		pDC->LineTo(rc.left+kDeltaCorner+kDeltaCorner, rc.top+kDeltaCorner+kDeltaCorner);
		pDC->MoveTo(rc.right-kDeltaCorner, rc.bottom-kDeltaCorner);
		pDC->LineTo(rc.right-kDeltaCorner-kDeltaCorner, rc.bottom-kDeltaCorner-kDeltaCorner);
		pDC->MoveTo(rc.left+kDeltaCorner, rc.bottom-kDeltaCorner);
		pDC->LineTo(rc.left+kDeltaCorner+kDeltaCorner, rc.bottom-kDeltaCorner-kDeltaCorner);
		pDC->MoveTo(rc.right-kDeltaCorner, rc.top+kDeltaCorner);
		pDC->LineTo(rc.right-kDeltaCorner-kDeltaCorner, rc.top+kDeltaCorner+kDeltaCorner);

		rc.SetRect(0,0,4,4);			/* degree */
		rc.OffsetRect(hOffset+8,vOffset);
		pDC->Ellipse(rc);
	}

	rc.SetRect(0,0,7,8);				/* draw zero */
	rc.OffsetRect(hOffset,vOffset);
	pDC->Ellipse(&rc);
//	pDC->TextOut(hOffset,vOffset,"0");

	pDC->MoveTo(ptCenter);
	pDC->LineTo(m_pView->m_Mapper.LPtoDP(m_pt));

	CRotateContext rotContext;
	rotContext.m_Angle=m_Angle-m_startAngle;
	rotContext.m_nAbout=(m_bAboutCenter && !m_pExtentCenter)?kAboutCenter:kAboutRefPt;
	rotContext.m_ptOrigin=m_pExtentCenter?*m_pExtentCenter:m_ptCenter;
	{
		CGDIBackMode bm(pDC,TRANSPARENT);
		CPen pen(PS_DOT,1,RGB(0,0,0));
		CGDIObject<CPen> op(pDC,&pen);
		CDraftContext dc(pDC, &m_pView->m_Mapper);
      dc.m_bUseGDI = true;
      dc.SetOutlinePen();
		m_pView->RotateFeedback(&dc, &rotContext);
	}
}

bool CRotateSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	m_pView->BeginSaveUndo();
	if(m_pView->DoShapeRotate(m_Angle-m_startAngle, (m_bAboutCenter && !m_pExtentCenter)?kAboutCenter:kAboutRefPt, m_pExtentCenter?*m_pExtentCenter:m_ptCenter))
	{
		m_pView->CommitSaveUndo(IDS_ROTATE);
		m_pView->ForceResizeUpdate();
	}
	else
	{
		m_pView->AbortSaveUndo(true);
		AfxMessageBox(IDS_UNABLE_COMPLETE_ACTION);
	}
	return true;
}
