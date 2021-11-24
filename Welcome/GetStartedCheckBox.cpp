// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "GetStartedCheckBox.h"
#include "TemplateWnd.h"
#include "../resource.h"
#include "../PCDraft.h"


const	LPCTSTR	gApplicationDefaults = "ApplicationDefaults";
const	LPCTSTR	gWindowAtStartUp = "WindowAtStartUp";


Gdiplus::Bitmap * LoadPNG(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);


BEGIN_MESSAGE_MAP(GetStartedCheckBox, BaseControl)
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSELEAVE()
   ON_WM_ERASEBKGND()
   ON_WM_CHAR()
END_MESSAGE_MAP()


GetStartedCheckBox::GetStartedCheckBox()
{

   m_pfont = nullptr;

   m_bSetHover = false;

   m_bChecked = false;

   m_pbitmapChecked = LoadPNG(MAKEINTRESOURCEA(IDB_PNG_WELCOME_CHECKED), "PNG", NULL);

   m_pbitmapUnchecked = LoadPNG(MAKEINTRESOURCEA(IDB_PNG_WELCOME_UNCHECKED), "PNG", NULL);

}


GetStartedCheckBox::~GetStartedCheckBox()
{

   delete m_pbitmapChecked;
   delete m_pbitmapUnchecked;

}


void GetStartedCheckBox::OnLButtonUp(UINT nFlags, CPoint point)
{

   OnClick();

   BaseControl::OnLButtonUp(nFlags, point);

   Invalidate();

}


void GetStartedCheckBox::OnMButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnMButtonDblClk(nFlags, point);

}


void GetStartedCheckBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnLButtonDblClk(nFlags, point);

}


int GetStartedCheckBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseControl::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   return 0;

}


void GetStartedCheckBox::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect rectClient;

   GetClientRect(rectClient);

   pdc->FillSolidRect(rectClient, RGB(242, 242, 242));

   Gdiplus::Graphics g(pdc->GetSafeHdc());

   pgraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

   if (m_bChecked)
   {

      pgraphics->DrawImage(m_pbitmapChecked, 0, 0, m_pbitmapChecked->GetWidth() * size_scaler() *0.8, m_pbitmapChecked->GetHeight() * size_scaler()*0.8);

   }
   else
   {

      pgraphics->DrawImage(m_pbitmapUnchecked, 0, 0, m_pbitmapChecked->GetWidth() * size_scaler()*0.8, m_pbitmapChecked->GetHeight() * size_scaler()*0.8);

   }

   if (m_pfont == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI", &installedFontCollection);

      m_pfont = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (15.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }

   CString str;

   GetWindowText(str);

   CStringW wstr = get_uni(str);

   Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));

   Gdiplus::RectF rectF(m_pbitmapChecked->GetWidth() + 8.0 * size_scaler(), 0, rectClient.Width(), rectClient.Height()*0.7);

   Gdiplus::StringFormat format;

   format.SetLineAlignment(::Gdiplus::StringAlignmentCenter);

   Gdiplus::RectF r;

   pgraphics->DrawString(wstr, wstr.GetLength(), m_pfont, rectF, &format, &brush);

   pgraphics->MeasureString(wstr, wstr.GetLength(), m_pfont, rectF, &format, &r);

   if (GetFocus() == this)
   {

      Gdiplus::Pen pen(Gdiplus::Color(255, 200, 80, 52), 1.0);

      pen.SetDashStyle(Gdiplus::DashStyleDash);

      Gdiplus::REAL spacea[2];

      spacea[0] = 3.0;

      spacea[1] = 3.0;

      pen.SetDashPattern(spacea, 2);

      pgraphics->DrawRectangle(&pen, r);

   }

}


void GetStartedCheckBox::OnMouseMove(UINT nFlags, CPoint point)
{

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


void GetStartedCheckBox::OnMouseLeave()
{

   m_bSetHover = false;

   BaseControl::OnMouseLeave();
   
   Invalidate();

}


BOOL GetStartedCheckBox::OnEraseBkgnd(CDC* pDC)
{

   return TRUE;

}





void GetStartedCheckBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (nChar == VK_RETURN || nChar == VK_SPACE)
   {

      OnClick();

   }

   BaseControl::OnChar(nChar, nRepCnt, nFlags);
}

void GetStartedCheckBox::OnClick()
{


   m_bChecked = !m_bChecked;

   Invalidate();

   BaseControl::OnClick();

}