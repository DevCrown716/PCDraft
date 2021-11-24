// ArrowButton.cpp : implementation file
//

#include "stdafx.h"
#include "PCDraft.h"
#include "ArrowButton.h"


// CArrowButton

IMPLEMENT_DYNAMIC(CArrowButton, CButton)

CArrowButton::CArrowButton()
{
   m_bPressed = false;
   m_iHover = -1;
   m_bTimer = true;
}

CArrowButton::~CArrowButton()
{
}


BEGIN_MESSAGE_MAP(CArrowButton, CButton)
   ON_WM_PAINT()
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSELEAVE()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_TIMER()
END_MESSAGE_MAP()



// CArrowButton message handlers




void CArrowButton::OnPaint()
{
   CPaintDC dc(this); // device context for painting
   CRect rectClient;
   GetClientRect(rectClient);
   if(m_bPressed)
   {
      dc.FillSolidRect(rectClient,GetSysColor(COLOR_3DLIGHT));
   }
   else
   {
      dc.FillSolidRect(rectClient,GetSysColor(COLOR_3DFACE));
   }
   if(m_iHover >= 0)
   {
      dc.Draw3dRect(rectClient,GetSysColor(COLOR_3DDKSHADOW),GetSysColor(COLOR_3DDKSHADOW));
   }
   else
   {
      dc.Draw3dRect(rectClient,GetSysColor(COLOR_BTNSHADOW),GetSysColor(COLOR_BTNSHADOW));
   }
   int x = rectClient.CenterPoint().x - 1;
   int y = rectClient.CenterPoint().y;

   if(m_etype == type_left)
   {

      dc.SetPixel(x,y,0);

      x++;
      dc.SetPixel(x,y - 1,0);
      dc.SetPixel(x,y,0);
      dc.SetPixel(x,y + 1,0);

      x++;
      dc.SetPixel(x,y - 2,0);
      dc.SetPixel(x,y - 1,0);
      dc.SetPixel(x,y,0);
      dc.SetPixel(x,y + 1,0);
      dc.SetPixel(x,y + 2,0);

   }
   else
   {
      dc.SetPixel(x,y - 2,0);
      dc.SetPixel(x,y - 1,0);
      dc.SetPixel(x,y,0);
      dc.SetPixel(x,y + 1,0);
      dc.SetPixel(x,y + 2,0);

      x++;
      dc.SetPixel(x,y - 1,0);
      dc.SetPixel(x,y,0);
      dc.SetPixel(x,y + 1,0);

      x++;
      dc.SetPixel(x,y,0);

   }
   
}


void CArrowButton::OnMouseMove(UINT nFlags,CPoint point)
{
   // TODO: Add your message handler code here and/or call default
   if(GetCapture() != this)
   {
      m_iHover = 1;
      SetCapture();
      Invalidate();
   }
   CRect rectClient;
   GetClientRect(rectClient);
   if(!rectClient.PtInRect(point) && !m_bPressed)
   {
      ReleaseCapture();
      m_iHover =-1;
      Invalidate();

   }
   CButton::OnMouseMove(nFlags,point);
}


void CArrowButton::OnMouseLeave()
{
   // TODO: Add your message handler code here and/or call default

   CButton::OnMouseLeave();
}


void CArrowButton::OnLButtonDown(UINT nFlags,CPoint point)
{
   SetCapture();

   m_bPressed = true;

   CRect rectClient;
   GetClientRect(rectClient);
   if(rectClient.PtInRect(point))
   {
      GetParent()->SendMessage(WM_COMMAND,GetDlgCtrlID());
   }
   if(m_bTimer)
   SetTimer(123,20, NULL);
   //CButton::OnLButtonDown(nFlags,point);
   Invalidate();
}


void CArrowButton::OnLButtonUp(UINT nFlags,CPoint point)
{
   KillTimer(123);
   ReleaseCapture();
   m_bPressed = false;
   CButton::OnLButtonUp(nFlags,point);
   Invalidate();
}


void CArrowButton::DrawItem(LPDRAWITEMSTRUCT)
{
   
}

void CArrowButton::OnTimer(UINT_PTR nIDEvent)
{
   // TODO: Add your message handler code here and/or call default
   if(nIDEvent == 123)
   {
      if(IsWindowVisible())
      {
         GetParent()->SendMessage(WM_COMMAND,GetDlgCtrlID());
      }
      else
      {
         KillTimer(123);
      }
   }
   CButton::OnTimer(nIDEvent);
}
