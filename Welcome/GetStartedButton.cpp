// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "GetStartedButton.h"
#include "GetStartedWnd.h"
#include "../resource.h"
#include "../PCDraft.h"


Gdiplus::Bitmap * LoadPNG(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);


BEGIN_MESSAGE_MAP(GetStartedButton, BaseControl)
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


GetStartedButton::GetStartedButton()
{

   m_pimage44 = nullptr;
   m_bDefaultClick = true;
   m_bHoverNotify = true;
   m_pfontLine1 = nullptr;
   m_pfontLine2 = nullptr;
   m_bSetHover = false;

}


GetStartedButton::~GetStartedButton()
{

   delete m_pfontLine1;
   delete m_pfontLine2;

}


int GetStartedButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseControl::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   return 0;
}


void GetStartedButton::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect rectClient;

   GetClientRect(rectClient);

   {

      Gdiplus::Color colorBorder;

      if (GetFocus() == this)
      {

         colorBorder = Gdiplus::Color(255, 200, 80, 52);

      }
      else
      {

         colorBorder = Gdiplus::Color(255, 192, 192, 192);

      }

      Gdiplus::Pen pen(colorBorder, 1.0);

      Gdiplus::Color colorFill;

      if (GetFocus() == this)
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

      Gdiplus::RectF rect1((Gdiplus::REAL)(rectClient.Width() - iRadius)-2.0f, 1.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect1, -90.0f,90.0f);

      Gdiplus::RectF rect2((Gdiplus::REAL)(rectClient.Width() - iRadius) - 2.0f , (Gdiplus::REAL)(rectClient.Height()- iRadius)-2.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect2, 0.0f, 90.0f);

      Gdiplus::RectF rect3(1.0f, (Gdiplus::REAL)(rectClient.Height() - iRadius) -2.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect3, 90.0f, 90.0f);

      Gdiplus::RectF        rect4(1.0f, 1.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect4, 180.0f, 90.0f);

      path.CloseFigure();

       pgraphics->FillPath(&brush, &path);

       pgraphics->DrawPath( &pen, &path);

   }



   if (m_pfontLine1 == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI", &installedFontCollection);

      m_pfontLine1 = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (18.0f * font_scaler()), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);

   }

   if (m_pfontLine2 == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI", &installedFontCollection);

      m_pfontLine2 = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (18.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }


   Gdiplus::REAL spacing;

   Gdiplus::Rect rIcon;


   {

      Gdiplus::Pen pen(Gdiplus::Color(255, 192, 192, 192), 1.0);

      rIcon.Width = 32 * size_scaler();

      rIcon.Height = 32 * size_scaler();

      rIcon.Y = (rectClient.Height() - rIcon.Width) / 2.0f;

      spacing = rIcon.Y * 1.25f;

      rIcon.X = spacing;

      if (m_pimage44)
      {

         pgraphics->DrawImage(m_pimage44, rIcon);

      }
      else
      {

         pgraphics->DrawRectangle(&pen, rIcon);
      }


   }

   CString str;

   GetWindowText(str);

   CString strLine1;
   CString strLine2;


   int iLeft = rIcon.X + rIcon.Width + spacing;

   auto iFind = str.Find('\n');

   if (iFind >= 0)
   {

      strLine1 = str.Left(iFind);
      strLine2 = str.Mid(iFind + 1);

   }
   else
   {

      strLine1 = str;

   }

   int iLineHeight1 = 0;
   int iLineHeight2 = 0;

   Gdiplus::RectF        rectF1(iLeft, 0, rectClient.Width(), rectClient.Height());
   Gdiplus::RectF        rectF2(iLeft, 0, rectClient.Width(), rectClient.Height());

   CStringW wstrLine1 = get_uni(strLine1);
   CStringW wstrLine2 = get_uni(strLine2);

   Gdiplus::StringFormat format;

   format.SetLineAlignment(::Gdiplus::StringAlignmentNear);

   if (!strLine1.IsEmpty())
   { 

      Gdiplus::RectF        rectOut;

      pgraphics->MeasureString(wstrLine1, wstrLine1.GetLength(), m_pfontLine1, rectF1, &format, &rectOut);

      iLineHeight1 = rectOut.Height;

   }

   if (!strLine2.IsEmpty())
   {

      Gdiplus::RectF        rectOut;

      pgraphics->MeasureString(wstrLine2, wstrLine2.GetLength(), m_pfontLine2, rectF2, &format, &rectOut);

      iLineHeight2 = rectOut.Height;

   }

   rectF1.Y = (rectClient.Height() - (iLineHeight1 + iLineHeight2)) / 2;
   rectF2.Y = rectF1.Y + iLineHeight1;

   if(!strLine1.IsEmpty())
   {


      Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));

      pgraphics->DrawString(wstrLine1, wstrLine1.GetLength(), m_pfontLine1, rectF1, &format, &brush);

   }

   if (!strLine2.IsEmpty())
   {

      Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));

      pgraphics->DrawString(wstrLine2, wstrLine2.GetLength(), m_pfontLine2, rectF2, &format, &brush);

   }


}


void GetStartedButton::OnMouseMove(UINT nFlags, CPoint point)
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


void GetStartedButton::OnMouseLeave()
{
   // TODO: Add your message handler code here and/or call default
   m_bSetHover = false;


   BaseControl::OnMouseLeave();
   Invalidate();
}


BOOL GetStartedButton::OnEraseBkgnd(CDC* pDC)
{

   return TRUE;

}


void GetStartedButton::OnLButtonDown(UINT nFlags, CPoint point)
{
   
   SetFocus();

   Invalidate();

   BaseControl::OnLButtonDown(nFlags, point);

}
