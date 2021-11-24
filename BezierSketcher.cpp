// BezierSketcher.cpp: implementation of the CBezierSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BezierSketcher.h"

#include "gu.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDBezierShape.h"
#include "DraftContext.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define	kMinDraw ((long)5)

//////////////////////////////////////////////////////////////////////

void CBezierSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnBeginSketch(pDC,pt);
	m_pt[m_nCurPoint++]=pt;
	m_pt[m_nCurPoint]=pt;
	m_bMouseDown=true;
}

CSketcher::ContinueMode CBezierSketcher::QueryContinue()
{
	switch(m_msg.message)
	{
	case WM_KEYDOWN:
		if(!m_bMouseDown && m_msg.wParam==VK_BACK && m_nCurPoint>3)
		{
			m_bRemovePt=true;
			return ContinueAddPt;
		}
	case WM_MOUSEMOVE:
		if(m_nCurPoint>3 && GetAsyncKeyState(VK_SHIFT)<0)
		{
			if(!m_bEraser)
			{
				m_pView->ShowCursorPrompt(false);
				m_pView->ShowCrossCursor(false);
				m_bEraser=true;
			}
		}
		else HideEraser();
		return ContinueUpdatePt;
	case WM_LBUTTONDOWN:
		m_bMouseDown=true;
		return ContinueAddPt;
	case WM_LBUTTONUP:
		m_bMouseDown=false;
		return (m_nCurPoint>1 || !guAlmostEqualPt(m_pt[0],m_pt[1]) || ((CPCDraftApp*)AfxGetApp())->m_bClickToStartPoly)?ContinueAddPt:Cancel;
	default:
		return CSketcher::QueryContinue();
	}
}

void CBezierSketcher::OnAddPt(const LongPoint& ptAdd)
{
	if(m_bRemovePt)
	{
		m_nCurPoint-=2;
		m_bRemovePt=false;
		return;
	}
	if(m_nCurPoint==kMaxPoint-1) m_nCurPoint-=2;
	if(!m_bMouseDown)
	{
		m_pt[++m_nCurPoint]=ptAdd;
		m_pt[++m_nCurPoint]=ptAdd;
	}
}

void CBezierSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
	if(m_bEraser)
	{
		LongPoint delta=Abs(ptUpdate-m_pt[m_nCurPoint-3]);
		WrFixed minDraw=m_pView->m_Mapper.DPtoLP(kMinDraw);
		if(delta.x<minDraw && delta.y<minDraw) m_nCurPoint-=2;
	}
	m_pt[m_nCurPoint]=ptUpdate;
	if(!m_bMouseDown) m_pt[m_nCurPoint-1]=ptUpdate;
}

void CBezierSketcher::OnDrawSketcher(CDC * pDC)
{
	CGDIROP gdiRop(pDC,R2_XORPEN);
	CGDIStockObject gdiPen(pDC,WHITE_PEN);
	LongPoint lpt;

	pointd pt[6];
	lpt.x=(m_pt[0].x<<1)-m_pt[1].x;
	lpt.y=(m_pt[0].y<<1)-m_pt[1].y;
	pt[3]=m_pView->m_Mapper.LPtoDP(lpt);
	pt[4]=m_pView->m_Mapper.LPtoDP(m_pt[0]);
	pt[5]=m_pView->m_Mapper.LPtoDP(m_pt[1]);
   pDC->MoveTo(__point(pt[3]));
   pDC->LineTo(__point(pt[5]));
    int i = 2;
	for(;i<=(m_nCurPoint-1);i+=2)
	{
		pt[0]=pt[3];
		pt[1]=pt[4];
		pt[2]=pt[5];
		lpt.x=(m_pt[i].x<<1)-m_pt[i+1].x;
		lpt.y=(m_pt[i].y<<1)-m_pt[i+1].y;
		pt[3]=m_pView->m_Mapper.LPtoDP(lpt);
		pt[4]=m_pView->m_Mapper.LPtoDP(m_pt[i]);
		pt[5]=m_pView->m_Mapper.LPtoDP(m_pt[i+1]);
      CPoint p[6];
      __copy(p, pt, 6);
      pDC->PolyBezier(p+1,4);
	}
	if(i>2 && pt[3]!=pt[5])
	{
      pDC->MoveTo(__point(pt[3]));
      pDC->LineTo(__point(pt[5]));
	}
}

void CBezierSketcher::OnCancelSketch(CDC* pDC, const LongPoint& pt)
{
	HideEraser();
	CSketcher::OnCancelSketch(pDC, pt);
}

bool CBezierSketcher::OnCompleteSketch(CDC *pDC, const LongPoint& pt)
{
	HideEraser();
	CSketcher::OnCompleteSketch(pDC, pt);
	m_nCurPoint-=3;
	if(m_nCurPoint>2)
	{
		m_pView->SaveUndo(IDS_CREATION);

		CMDBezierShape* pShape=new CMDBezierShape((m_nCurPoint-2)*3/2+1);

		LongPoint lpt;
		pShape->SetShapePoint(0,m_pt[0]);
		int j=1;
		for(int i=1;i<(m_nCurPoint-1);i++)
		{
			if((i%2)==0)
			{
				lpt.x=(m_pt[i].x<<1)-m_pt[i+1].x;
				lpt.y=(m_pt[i].y<<1)-m_pt[i+1].y;
				pShape->SetShapePoint(j++,lpt);
			}
			pShape->SetShapePoint(j++,m_pt[i]);
		}

		pShape->SetPenModel(m_pView->GetDocument()->m_Pen);
	//	pShape->SetFillModel(m_pView->GetDocument()->m_Fill);
		pShape->CalculateExtents();
		pShape->SetShapeDatum(kDefaultDatum);
		m_pView->GetDocument()->CreateShape(pShape);
		m_pView->SelectShape(pShape,false);
		return true;
	}
	return false;
}


void CBezierSketcher::HideEraser()
{
	if(m_bEraser)
	{
		m_pView->ShowCrossCursor();
		m_pView->ShowCursorPrompt();
		m_bEraser=false;
	}
}
