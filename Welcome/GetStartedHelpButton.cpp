// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "GetStartedHelpButton.h"
#include "GetStartedWnd.h"
#include "../resource.h"
#include "../PCDraft.h"


Gdiplus::Bitmap * LoadPNG(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);


BEGIN_MESSAGE_MAP(GetStartedHelpButton, BaseControl)
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


GetStartedHelpButton::GetStartedHelpButton()
{

   m_pimage44 = nullptr;
   m_bDefaultClick = true;
   m_bHoverNotify = true;
   m_pfontLine1 = nullptr;
   m_bSetHover = false;
   m_crBackground = RGB(242, 242, 242);

}


GetStartedHelpButton::~GetStartedHelpButton()
{

   delete m_pfontLine1;

}


int GetStartedHelpButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseControl::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   return 0;
}


void GetStartedHelpButton::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect rectClient;

   GetClientRect(rectClient);


   if (m_pfontLine1 == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI", &installedFontCollection);

      m_pfontLine1 = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (15.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }

   Gdiplus::REAL spacing;

   Gdiplus::Rect rIcon;


   {

      Gdiplus::Pen pen(Gdiplus::Color(255, 192, 192, 192), 1.0);

      rIcon.Width = 24 * size_scaler();

      rIcon.Height = 24 * size_scaler();

      rIcon.Y = (rectClient.Height() - rIcon.Width) / 2.0f;

      spacing = rIcon.Y * 1.25f;

      rIcon.X = rectClient.Width() - rIcon.Width;

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


   int iLeft = rectClient.left + 2;

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

   Gdiplus::RectF        rectF1(iLeft, 0, rectClient.Width() - rIcon.Width - 8.0 * size_scaler(), rectClient.Height());
   Gdiplus::RectF        rectF2(iLeft, 0, rectClient.Width(), rectClient.Height());

   CStringW wstrLine1 = get_uni(strLine1);
   CStringW wstrLine2 = get_uni(strLine2);

   Gdiplus::StringFormat format;

   format.SetAlignment(::Gdiplus::StringAlignmentFar);

   format.SetLineAlignment(::Gdiplus::StringAlignmentNear);

   if (!strLine1.IsEmpty())
   { 

      Gdiplus::RectF        rectOut;

      pgraphics->MeasureString(wstrLine1, wstrLine1.GetLength(), m_pfontLine1, rectF1, &format, &rectOut);

      iLineHeight1 = rectOut.Height;

   }

   rectF1.Y = (rectClient.Height() - (iLineHeight1 + iLineHeight2)) / 2;

   if(!strLine1.IsEmpty())
   {


      Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));

      pgraphics->DrawString(wstrLine1, wstrLine1.GetLength(), m_pfontLine1, rectF1, &format, &brush);

   }


}


void GetStartedHelpButton::OnMouseMove(UINT nFlags, CPoint point)
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


void GetStartedHelpButton::OnMouseLeave()
{
   // TODO: Add your message handler code here and/or call default
   m_bSetHover = false;


   BaseControl::OnMouseLeave();
   Invalidate();
}


BOOL GetStartedHelpButton::OnEraseBkgnd(CDC* pDC)
{

   return TRUE;

}


void GetStartedHelpButton::OnLButtonDown(UINT nFlags, CPoint point)
{
   
   SetFocus();

   Invalidate();

   BaseControl::OnLButtonDown(nFlags, point);

}
