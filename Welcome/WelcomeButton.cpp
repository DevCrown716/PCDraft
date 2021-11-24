// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "WelcomeButton.h"
#include "GetStartedWnd.h"
#include "../resource.h"
#include "../PCDraft.h"


BEGIN_MESSAGE_MAP(WelcomeButton, BaseControl)
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
   ON_WM_ENABLE()
END_MESSAGE_MAP()


WelcomeButton::WelcomeButton()
{

   m_bDefaultClick = true;
   m_bHoverNotify = true;
   m_pfont = nullptr;
   m_bSetHover = false;

}


WelcomeButton::~WelcomeButton()
{

   delete m_pfont;

}


//void WelcomeButton::OnLButtonUp(UINT nFlags, CPoint point)
//{
//
//   OnClick();
//
//   BaseControl::OnLButtonUp(nFlags, point);
//
//}


void WelcomeButton::OnMButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnMButtonDblClk(nFlags, point);

}


void WelcomeButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnLButtonDblClk(nFlags, point);

}


int WelcomeButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseControl::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   return 0;

}


void WelcomeButton::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect rectClient;

   GetClientRect(rectClient);



   pdc->FillSolidRect(rectClient, RGB(242, 242, 242));

   {

      Gdiplus::Color colorBorder;

      if (!IsWindowEnabled())
      {

         colorBorder = Gdiplus::Color(255, 182, 182, 182);

      }
      else if (GetFocus() == this)
      {

         colorBorder = Gdiplus::Color(255, 200, 80, 52);

      }
      else
      {

         colorBorder = Gdiplus::Color(255, 192, 192, 192);

      }

      Gdiplus::Pen pen(colorBorder, 1.0);

      Gdiplus::Color colorFill;

      if (!IsWindowEnabled())
      {

         colorFill = Gdiplus::Color(255, 202, 202, 202);

      }
      else if (GetFocus() == this)
      {

         colorFill = Gdiplus::Color(255, 255, 200, 182);

      }
      else if (m_bSetHover)
      {

         colorFill = Gdiplus::Color(255, 242, 242, 242);

      }
      else
      {

         colorFill = Gdiplus::Color(255, 255, 255, 255);

      }

      int iRadius = 11 * size_scaler();

      Gdiplus::SolidBrush brush(colorFill);

      Gdiplus::GraphicsPath path;

      Gdiplus::RectF rect1(rectClient.Width() - iRadius - 2, 1, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect1, -90, 90);

      Gdiplus::RectF rect2(rectClient.Width() - iRadius - 2, rectClient.Height() - iRadius - 2, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect2, 0, 90);

      Gdiplus::RectF rect3(1, rectClient.Height() - iRadius - 2, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect3, 90, 90);

      Gdiplus::RectF rect4(1, 1, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect4, 180, 90);

      path.CloseFigure();

      pgraphics->FillPath(&brush, &path);

      pgraphics->DrawPath(&pen, &path);

   }

   if (m_pfont == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI", &installedFontCollection);

      m_pfont = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (14.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }

   CString str;

   GetWindowText(str);

   Gdiplus::RectF rectF(0, 0, rectClient.Width(), rectClient.Height());

   CStringW wstr = get_uni(str);

   Gdiplus::StringFormat format;

   format.SetAlignment(::Gdiplus::StringAlignmentCenter);

   format.SetLineAlignment(::Gdiplus::StringAlignmentCenter);

   Gdiplus::Color colorText;

   if (!IsWindowEnabled())
   {

      colorText = Gdiplus::Color(255,129, 128, 128);

   }
   else
   {

      colorText = Gdiplus::Color(0, 0, 0);

   }

   Gdiplus::SolidBrush brush(colorText);

   pgraphics->DrawString(wstr, wstr.GetLength(), m_pfont, rectF, &format, &brush);

}


void WelcomeButton::OnMouseMove(UINT nFlags, CPoint point)
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


void WelcomeButton::OnMouseLeave()
{
   // TODO: Add your message handler code here and/or call default
   m_bSetHover = false;


   BaseControl::OnMouseLeave();
   Invalidate();
}


BOOL WelcomeButton::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default

   //return BaseControl::OnEraseBkgnd(pDC);

   return TRUE;
}


//void WelcomeButton::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//
//   if (nChar == VK_RETURN || nChar == VK_SPACE)
//   {
//
//      OnClick();
//
//   }
//
//   BaseControl::OnChar(nChar, nRepCnt, nFlags);
//}


void WelcomeButton::OnLButtonDown(UINT nFlags, CPoint point)
{

   SetFocus();

   BaseControl::OnLButtonDown(nFlags, point);

}


void WelcomeButton::OnEnable(BOOL bEnable)
{
   BaseControl::OnEnable(bEnable);
   Invalidate();
   // TODO: Add your message handler code here
}
