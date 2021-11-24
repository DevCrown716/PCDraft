// RectSketcher.cpp: implementation of the CRectSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RectSketcher.h"

#include "gu.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDRectangle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////

void CRectSketcher::MakeRect(Gdiplus::RectF &rc)
{
   LongPoint lpt=m_ptFrom;
   if(m_nMode==kRectCenter)
   {
      lpt.x=(lpt.x<<1)-m_ptTo.x;
      lpt.y=(lpt.y<<1)-m_ptTo.y;
   }
   auto p1 = m_pView->m_Mapper.LPtoDPF(lpt);
   rc.X = p1.x;
   rc.Y = p1.y;
   auto p2 = m_pView->m_Mapper.LPtoDPF(m_ptTo);
   //rc.BottomRight()=m_pView->m_Mapper.LPtoDP(m_ptTo);
   rc.Width = p2.x - p1.x;
   rc.Height = p2.y - p1.y;
}
//void CRectSketcher::MakeRect(CRect &rc)
//{
//   LongPoint lpt = m_ptFrom;
//   if (m_nMode == kRectCenter)
//   {
//      lpt.x = (lpt.x << 1) - m_ptTo.x;
//      lpt.y = (lpt.y << 1) - m_ptTo.y;
//   }
//   rc.TopLeft() = m_pView->m_Mapper.LPtoDP(lpt);
//   rc.BottomRight() = m_pView->m_Mapper.LPtoDP(m_ptTo);
//}

void CRectSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
   CSketcher::OnBeginSketch(pDC, pt);
   m_ptFrom=m_ptTo=pt;
}

LongPoint CRectSketcher::AdjustPosition(LongPoint pt)
{
   /*	if(GetKeyState(VK_SHIFT)<0)
   	{
   		// rectangle sketcher draws squares
   		Coord a=pt.x-m_ptFrom.x, b=pt.y-m_ptFrom.y;
   		if(Abs(a)>Abs(b)) a=Sign(a)*Abs(b); else b=Sign(b)*Abs(a);
   		pt.x=m_ptFrom.x+a; pt.y=m_ptFrom.y+b;
   	}
   */	return pt;
}

void CRectSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
   m_ptTo=ptUpdate;
   if(GetKeyState(VK_SHIFT)<0)
   {
      // rectangle sketcher draws squares
      Coord a=m_ptTo.x-m_ptFrom.x;
      Coord b=m_ptTo.y-m_ptFrom.y;
      if(Abs(a)>Abs(b)) a=Sign(a)*Abs(b);
      else b=Sign(b)*Abs(a);
      m_ptTo.x=m_ptFrom.x+a;
      m_ptTo.y=m_ptFrom.y+b;
   }

   if(m_nMode==kRectCenter)
   {
      m_pView->SetShowSize(0,1,Abs(m_ptTo.x-m_ptFrom.x)<<1);
      m_pView->SetShowSize(1,2,Abs(m_ptTo.y-m_ptFrom.y)<<1);
   }
   else
   {
      m_pView->SetShowSize(0,1,Abs(m_ptTo.x-m_ptFrom.x));
      m_pView->SetShowSize(1,2,Abs(m_ptTo.y-m_ptFrom.y));
   }
}

void CRectSketcher::OnDrawSketcher(CDC * pDC)
{
   ::Gdiplus::RectF rc;
   MakeRect(rc);

   auto pointViewport = pDC->GetViewportOrg();

   if (pointViewport.x < 0)
   {

      rc.X -= 1.0;

   }

   if (pointViewport.y < 0)
   {

       rc.Y -= 1.0;

   }

   //rc.right++;
   //rc.bottom++;
   //rc.NormalizeRect();
   //rc.top--;
   //rc.right++;
   //rc.bottom--;

   //Gdiplus::RectF rc;
   //MakeRect(rc);
//   HDC hdc = pDC->GetSafeHdc();
//   Gdiplus::Graphics g(hdc);
//   Gdiplus::Pen pen(Gdiplus::Color::Black, 1.0f);
//   g.DrawRectangle(&pen, rc);

   CRect r;
   r.left = lround(rc.X);
   r.top = lround(rc.Y);
   r.right = lround(rc.X + rc.Width) + 1;
   r.bottom = lround(rc.Y + rc.Height) + 1;
   pDC->Rectangle(&r);

   //   HDC hdc = pDC->GetSafeHdc();
   //Gdiplus::Graphics g(hdc);
   //Gdiplus::Pen pen(Gdiplus::Color::Black, 1.0f);
   //g.DrawRectangle(&pen, rc);

}

bool CRectSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
   CSketcher::OnCompleteSketch(pDC, pt);
   if(m_ptFrom!=m_ptTo)
   {
      m_pView->SaveUndo(IDS_CREATION);

      CMDRectangle* pShape;
      LongRect lrc;
      if(m_nMode==kRectCenter)
      {
         pShape=new CMDRectangle(CMDRectangle::kRectCenter);
         lrc.SetToBound((m_ptFrom<<1)-m_ptTo,m_ptTo);
      }
      else
      {
         pShape=new CMDRectangle(CMDRectangle::kRectDiagonal);
         lrc.SetToBound(m_ptFrom,m_ptTo);
      }
      lrc.Normalize();
      for(int i=0; i<4; i++)
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
