// EllipticSketcher.cpp: implementation of the CEllipticSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EllipticSketcher.h"

#include "gu.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDEllipse.h"
#include "DraftContext.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CEllipticSketcher::OnDrawSketcher(CDC * pDC)
{
	
   Gdiplus::RectF rc;
   MakeRect(rc);
   CRect r;
   r.left = lround(rc.X);
   r.top = lround(rc.Y);
   r.right = lround(rc.X + rc.Width) + 1;
   r.bottom = lround(rc.Y + rc.Height) + 1;  
   pDC->Ellipse(&r);
   //HDC hdc = pDC->GetSafeHdc();
   //Gdiplus::Graphics g(hdc);
   //Gdiplus::Pen pen(Gdiplus::Color::Black, 1.0f);
   //g.DrawEllipse(&pen, rc);

}


bool CEllipticSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
	CSketcher::OnCompleteSketch(pDC, pt);
	if(m_ptFrom!=m_ptTo)
	{
		m_pView->SaveUndo(IDS_CREATION);

		CMDEllipse* pShape;
		LongRect lrc;
		if(m_nMode==kRectCenter)
		{
			pShape=new CMDEllipse(CMDRectangle::kRectCenter);
			lrc.SetToBound((m_ptFrom<<1)-m_ptTo,m_ptTo);
		}
		else
		{
			pShape=new CMDEllipse(CMDRectangle::kRectDiagonal);
			lrc.SetToBound(m_ptFrom,m_ptTo);
		}
		lrc.Normalize();
		for(int i=0;i<4;i++)
			pShape->SetShapePoint(i,lrc.GetCorner(i));
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
