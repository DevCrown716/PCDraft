// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "ControlBoxButton.h"
#include "GetStartedWnd.h"
#include "../resource.h"
#include "../PCDraft.h"


BEGIN_MESSAGE_MAP(ControlBoxButton, BaseControl)
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSELEAVE()
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


ControlBoxButton::ControlBoxButton()
{

   m_bSetHover = false;
   m_pfontMarlett = nullptr;

}


ControlBoxButton::~ControlBoxButton()
{

   delete m_pfontMarlett;

}


void ControlBoxButton::OnLButtonUp(UINT nFlags, CPoint point)
{

   m_pbasewnd->OnButtonPress(GetDlgCtrlID());

   BaseControl::OnLButtonUp(nFlags, point);

}


void ControlBoxButton::OnMButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnMButtonDblClk(nFlags, point);
}


void ControlBoxButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
   // TODO: Add your message handler code here and/or call default

   BaseControl::OnLButtonDblClk(nFlags, point);
}


int ControlBoxButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   
   if (BaseControl::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   return 0;

}


void ControlBoxButton::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{


   CRect rectClient;

   GetClientRect(rectClient);

   Gdiplus::SolidBrush brush(m_colorBackground);

   Gdiplus::RectF rect;

   rect.X = 0.f;
   rect.Y = 0.f;
   rect.Width = (Gdiplus::REAL) rectClient.Width();
   rect.Height = (Gdiplus::REAL) rectClient.Height();

   pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeNone);

   pgraphics->FillRectangle(&brush, rect);

   pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);





   if (m_pfontMarlett == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Marlett", &installedFontCollection);

      m_pfontMarlett = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL)  (18 * 0.9 * font_scaler()), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

   }


   Gdiplus::StringFormat format;

   format.SetAlignment(::Gdiplus::StringAlignmentCenter);

   format.SetLineAlignment(::Gdiplus::StringAlignmentCenter);


   CString str;

   GetWindowText(str);

   CStringW wstr = get_uni(str);

   Gdiplus::SolidBrush brushForeground(m_colorForeground);

   pgraphics->DrawString(wstr, wstr.GetLength(), m_pfontMarlett, rect , &format, &brushForeground);

}


void ControlBoxButton::OnMouseMove(UINT nFlags, CPoint point)
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


void ControlBoxButton::OnMouseLeave()
{
   // TODO: Add your message handler code here and/or call default
   m_bSetHover = false;


   BaseControl::OnMouseLeave();
   Invalidate();
}


BOOL ControlBoxButton::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default

   //return BaseControl::OnEraseBkgnd(pDC);

   return TRUE;
}
