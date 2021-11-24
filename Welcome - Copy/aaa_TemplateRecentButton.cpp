// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "TemplateRecentButton.h"
#include "GetStartedWnd.h"
#include "../resource.h"
#include "../PCDraft.h"


Gdiplus::Bitmap * LoadPNG(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);


BEGIN_MESSAGE_MAP(TemplateRecentButton, BaseControl)
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSELEAVE()
   ON_WM_ERASEBKGND()
   ON_WM_CHAR()
   ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


TemplateRecentButton::TemplateRecentButton()
{

   m_pimage44 = nullptr;
   m_bDefaultClick = true;
   m_bHoverNotify = true;
   m_pfontLine1 = nullptr;
   m_pfontLine2 = nullptr;
   m_bSetHover = false;

}


TemplateRecentButton::~TemplateRecentButton()
{

   delete m_pfontLine1;
   delete m_pfontLine2;

}




void TemplateRecentButton::OnMButtonDblClk(UINT nFlags, CPoint point)
{
   // TODO: Add your message handler code here and/or call default

   BaseControl::OnMButtonDblClk(nFlags, point);
}


void TemplateRecentButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
   // TODO: Add your message handler code here and/or call default

   BaseControl::OnLButtonDblClk(nFlags, point);
}


int TemplateRecentButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseControl::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   return 0;
}




void TemplateRecentButton::OnMouseMove(UINT nFlags, CPoint point)
{
   // TODO: Add your message handler code here and/or call default
   if (!m_bSetHover)
   {
      m_bSetHover = true;
      TRACKMOUSEEVENT tme = { sizeof(tme) };
      tme.dwFlags = TME_LEAVE;
      tme.hwndTrack = GetSafeHwnd();

      TrackMouseEvent(&tme);
      Invalidate();

   }
   BaseControl::OnMouseMove(nFlags, point);
}


void TemplateRecentButton::OnMouseLeave()
{
   // TODO: Add your message handler code here and/or call default
   m_bSetHover = false;


   BaseControl::OnMouseLeave();
   Invalidate();
}


BOOL TemplateRecentButton::OnEraseBkgnd(CDC* pDC)
{

   return TRUE;

}




void TemplateRecentButton::OnLButtonDown(UINT nFlags, CPoint point)
{

   SetFocus();

   BaseControl::OnLButtonDown(nFlags, point);
}
