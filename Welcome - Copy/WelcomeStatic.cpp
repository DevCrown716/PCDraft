// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "WelcomeStatic.h"
#include "../resource.h"


WelcomeStatic::WelcomeStatic()
{

   m_pfont = nullptr;

}


WelcomeStatic::~WelcomeStatic()
{
   delete m_pfont;

}


BEGIN_MESSAGE_MAP(WelcomeStatic, BaseControl)
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()


void WelcomeStatic::OnLButtonUp(UINT nFlags, CPoint point)
{

   BaseControl::OnLButtonUp(nFlags, point);

}


void WelcomeStatic::OnMButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnMButtonDblClk(nFlags, point);

}


void WelcomeStatic::OnLButtonDblClk(UINT nFlags, CPoint point)
{
   
   BaseControl::OnLButtonDblClk(nFlags, point);

}


int WelcomeStatic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseControl::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   return 0;

}


void WelcomeStatic::OnSize(UINT nType, int cx, int cy)
{

   BaseControl::OnSize(nType, cx, cy);

}


void WelcomeStatic::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   Gdiplus::RectF rect;

   CRect rectClient;

   GetClientRect(rectClient);

   rect.X = 0;

   rect.Y = 0;

   rect.Width = rectClient.Width();

   rect.Height = rectClient.Height();

   if (m_pfont == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI", &installedFontCollection);

      m_pfont = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (18.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }

   // Message
   {

      CString str;

      GetWindowText(str);

      CStringW wstr = get_uni(str);

      Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0));

      Gdiplus::StringFormat format;

      format.SetAlignment(::Gdiplus::StringAlignmentNear);

      format.SetLineAlignment(::Gdiplus::StringAlignmentCenter);

      pgraphics->DrawString(wstr, wstr.GetLength(), m_pfont, rect, &format, &brush);

   }

}



