// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "TemplateLeftButton.h"
#include "TemplateWnd.h"
#include "../resource.h"
#include "../PCDraft.h"


BEGIN_MESSAGE_MAP(TemplateLeftButton, BaseControl)
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSELEAVE()
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


TemplateLeftButton::TemplateLeftButton()
{

   m_bSel = false;
   m_bDefaultClick = true;
   m_bHoverNotify = true;
   m_pfont = nullptr;
   m_bSetHover = false;

}


TemplateLeftButton::~TemplateLeftButton()
{

   delete m_pfont;

}


//void TemplateLeftButton::OnLButtonUp(UINT nFlags, CPoint point)
//{
//
//   BaseControl::OnLButtonUp(nFlags, point);
//
//}


void TemplateLeftButton::OnMButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnMButtonDblClk(nFlags, point);

}


void TemplateLeftButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnLButtonDblClk(nFlags, point);

}


int TemplateLeftButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseControl::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   return 0;

}


void TemplateLeftButton::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect rectClient;

   GetClientRect(rectClient);

   if (m_bSetHover || m_bSel)
   {
      pdc->FillSolidRect(rectClient, RGB(200, 80, 52));
   }
   else
   {
      pdc->FillSolidRect(rectClient, RGB(180, 60, 32));

   }

   Gdiplus::Graphics g(pdc->GetSafeHdc());

   pgraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

   if (m_pfont == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI Semibold", &installedFontCollection);

      m_pfont = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (15.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }

   CString str;

   GetWindowText(str);

   CStringW wstr = get_uni(str);

   Gdiplus::PointF point(50, 20);

   Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 255));

   Gdiplus::RectF        rectF(30 * size_scaler(), 0, rectClient.Width(), rectClient.Height());

   Gdiplus::StringFormat format;

   format.SetLineAlignment(::Gdiplus::StringAlignmentCenter);


   pgraphics->DrawString(wstr,wstr.GetLength(), m_pfont, rectF, &format, &brush);

}


BOOL TemplateLeftButton::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default

   //return BaseControl::OnEraseBkgnd(pDC);

   return TRUE;
}
