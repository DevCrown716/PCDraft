// LineSketcher.cpp: implementation of the CGuideSketcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuideSketcher.h"

#include "gu.h"
#include "phmath.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MDLineShape.h"

#include "Resource.h"
#include "DraftContext.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

void CGuideSketcher::OnBeginSketch(CDC* pDC, const LongPoint& pt)
{
   
   CSketcher::OnBeginSketch(pDC, pt);

   if (m_guide.m_bVertical)
   {


     m_guide.m_coord = pt.x;

   }
   else
   {

      m_guide.m_coord = pt.y;

   }

}


void CGuideSketcher::OnUpdatePt(const LongPoint& ptUpdate)
{
   
   if (m_guide.m_bVertical)
   {

      m_guide.m_coord = ptUpdate.x;
      
      m_bInDoc = m_pView->GetDocument()->XInDocument(m_guide.m_coord);

   }
   else
   {

      m_guide.m_coord = ptUpdate.y;

      m_bInDoc = m_pView->GetDocument()->YInDocument(m_guide.m_coord);

   }




}


void CGuideSketcher::OnDrawSketcher(CDC * pDC)
{

   auto ptOrg = m_pView->GetDeviceScrollPosition();

   auto pdocument = m_pView->GetDocument();

   CRect rectClient;

   m_pView->GetClientRect(rectClient);

   //Gdiplus::Graphics g(pDC->GetSafeHdc());

   //g.SetPageUnit(Gdiplus::UnitPixel);

   //g.ResetClip();

   //Gdiplus::Color color(180, 255, 0, 0);

   //Gdiplus::Pen pen(color, 1);

   CPen pen(PS_SOLID, 1 ,RGB(255, 0, 0));

   pDC->SelectObject(&pen);

   CPoint ptOffset;
   m_pView->GetClientCorner(ptOffset);

   bool bHorizontal = !m_guide.m_bVertical;

   long l = m_pView->GetDocument()->LPtoDP(m_guide.m_coord);

   if (bHorizontal)
   {

      l += ptOffset.y;
      pDC->MoveTo(rectClient.left + ptOrg.x + kLRulerWid, l);
      pDC->LineTo(rectClient.right + ptOrg.y, l);

   }
   else
   {

      l += ptOffset.x;
      pDC->MoveTo(l, rectClient.top + ptOrg.y + kLRulerWid);
      pDC->LineTo(l, rectClient.bottom + ptOrg.y);

   }

   if (m_guide.m_bVertical)
   {

      SetCursor(AfxGetApp()->LoadCursor(IDC_SPLIT));

   }
   else
   {

      SetCursor(AfxGetApp()->LoadCursor(IDC_SPLITH));

   }

}

CSketcher::ContinueMode CGuideSketcher::QueryContinue()
{
   switch (m_msg.message)
   {
   case WM_LBUTTONDOWN:
      return Complete;

   case WM_LBUTTONUP:
      return  Complete;

   case WM_KEYDOWN:
      if (m_msg.wParam != VK_ESCAPE) return ContinueUpdatePt;
      return Cancel;
   }
   return CSketcher::QueryContinue();
}

bool CGuideSketcher::OnCompleteSketch(CDC* pDC, const LongPoint& pt)
{
   CSketcher::OnCompleteSketch(pDC, pt);
   if (m_guide.m_bVertical)
   {

      m_guide.m_coord = pt.x;

      m_bInDoc = m_pView->GetDocument()->XInDocument(m_guide.m_coord);

   }
   else
   {

      m_guide.m_coord = pt.y;

      m_bInDoc = m_pView->GetDocument()->YInDocument(m_guide.m_coord);

   }


   if(m_bInDoc)
   {

      m_pView->GetDocument()->m_guidea.Add(m_guide);
      m_pView->RedrawWindow();
      return true;
   }
   return false;
}
