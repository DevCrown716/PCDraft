// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "TemplateComboBox.h"
#include "WelcomeEdit.h"
#include "BaseWnd.h"
#include "BaseControl.h"
#include "../resource.h"


TemplateDropButton::TemplateDropButton()
{
   m_pTextFont = nullptr;
   m_pDropList = nullptr;
   m_bOver = false;
   m_bDeleteFont = false;
   m_bChangeTitle = true;
   m_pBkBrush = NULL;
   m_pBkOverBrush = NULL;
   m_pBorderPen = NULL;
   m_pTriBrush = NULL;
   m_pTriPen = NULL;
   m_nListHeight = 100;
   m_pfontDropButton = nullptr;
}

TemplateDropButton::~TemplateDropButton()
{
   delete	m_pBkBrush;
   delete	m_pBkOverBrush;
   delete	m_pBorderPen;
   delete	m_pTriBrush;
   delete	m_pTriPen;
   delete	m_pDropList;
   if (m_bDeleteFont) delete m_pTextFont;
}

BEGIN_MESSAGE_MAP(TemplateDropButton, BaseControl)
   ON_WM_LBUTTONDOWN()
   ON_WM_SIZE()
   ON_WM_CREATE()
END_MESSAGE_MAP()


int TemplateDropButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseControl::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

 
   m_crBackground = RGB(242, 242, 242);
   m_pBkBrush = new CBrush(RGB(242, 242, 242));
   m_pBkOverBrush = new CBrush(RGB(220, 80, 40));

   // Create the Down highlight pen
   m_pBorderPen = new CPen(PS_SOLID, 1, RGB(0, 0, 0));


   m_crTxtColor = RGB(0, 0, 0);

   m_crTxtOverColor = RGB(0, 0, 0);

   m_pTriBrush = new CBrush(m_crTxtColor);
   m_pTriPen = new CPen(PS_SOLID, 1, m_crTxtColor);


   if (!m_pTextFont)
   {
      m_bDeleteFont = true;
      m_pTextFont = new CFont();
      m_pTextFont->CreateFont(20 * font_scaler(), 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_CHARACTER_PRECIS,
         CLIP_LH_ANGLES, 5, VARIABLE_PITCH | FF_SWISS, "Segoe UI");
   }



   defer_create_list();



   return 0;

}


bool TemplateDropButton::defer_create_list()
{

   if (m_pDropList == nullptr)
   {

      DWORD dwDefaultStyle = 0;

      UINT nIDResource = IDR_WELCOME;

      LPCTSTR lpszClass = GetIconWndClass(dwDefaultStyle, nIDResource);
      
      DWORD dwStyle = 0;
      
      m_pDropList = new TemplateDropList();

      m_pDropList->m_pbutton = this;
      
      if (!m_pDropList->CreateEx(
         WS_EX_TOOLWINDOW
         | WS_EX_TOPMOST, lpszClass, "", dwStyle, CRect(0, 0, 0, 0), nullptr, 0))
      {

         return FALSE;

      }

      m_pDropList->SetFont(m_pTextFont);

   }

   return true;

}


void TemplateDropButton::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect 	rectClient;

   GetClientRect(rectClient);

   if (m_nListWidth <= 5)
   {

      m_nListWidth = rectClient.Width();

   }

   int h = rectClient.Height();

   pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   pgraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
   pgraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
   pgraphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
   pgraphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);

   int iTriangleHeight = 6;

   int iTriangleSpacing = 4;

   int iTriangleOutside = (h - (iTriangleHeight * 2) - iTriangleSpacing) / 2;

   int iTriangleWidth = 7;

   int iTriangleRightMargin = 8;

   CRect rectUp(
      rectClient.right - iTriangleWidth - iTriangleRightMargin,
      iTriangleOutside,
      rectClient.right - iTriangleRightMargin,
      iTriangleOutside + iTriangleHeight);
   CRect rectDown(
      rectClient.right - iTriangleWidth - iTriangleRightMargin,
      h - iTriangleOutside - iTriangleHeight,
      rectClient.right - iTriangleRightMargin,
      h - iTriangleOutside);

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

      int iRadius = 5 * size_scaler();

      Gdiplus::Point point1;
      Gdiplus::Point point2;
      point1.X = 0;
      point2.X = 0;
      point1.Y = 0;
      point2.Y = rectClient.bottom;


      Gdiplus::LinearGradientBrush brush(point1, point2, Gdiplus::Color(242, 242, 242), Gdiplus::Color(236, 236, 236));


      Gdiplus::GraphicsPath path;

      Gdiplus::RectF rect1((Gdiplus::REAL)(rectClient.Width() - iRadius) - 2.0f, 1.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect1, -90.0f, 90.0f);

      Gdiplus::RectF rect2((Gdiplus::REAL)(rectClient.Width() - iRadius) - 2.0f, (Gdiplus::REAL)(rectClient.Height() - iRadius) - 2.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect2, 0.0f, 90.0f);

      Gdiplus::RectF rect3(1.0f, (Gdiplus::REAL)(rectClient.Height() - iRadius) - 2.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect3, 90.0f, 90.0f);

      Gdiplus::RectF        rect4(1.0f, 1.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

      path.AddArc(rect4, 180.0f, 90.0f);

      path.CloseFigure();

      pgraphics->FillPath(&brush, &path);

      pgraphics->DrawPath(&pen, &path);

   }




   DrawTriangleUp(pgraphics, rectUp);
   DrawTriangleDown(pgraphics, rectDown);

   if (m_pfontDropButton == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI", &installedFontCollection);

      m_pfontDropButton = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (12.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }




   int iSel = GetList()->m_iCurSel;

   if (iSel >= 0)
   {
      CStringW str;

      str = GetList()->m_straItem[iSel];

      Gdiplus::RectF rectF(5, 0, rectClient.Width(), rectClient.Height());

      CStringW wstr = str;

      Gdiplus::StringFormat format;

      format.SetAlignment(::Gdiplus::StringAlignmentNear);

      format.SetLineAlignment(::Gdiplus::StringAlignmentCenter);

      Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));

      pgraphics->DrawString(wstr, wstr.GetLength(), m_pfontDropButton, rectF, &format, &brush);


   }

}


void TemplateDropButton::DrawTriangleUp(Gdiplus::Graphics * pgraphics, LPCRECT lpcrect)
{

   Gdiplus::PointF pointa[3];

   Gdiplus::SolidBrush brush(Gdiplus::Color(128, 128, 128));

   pointa[0].X = (double)(lpcrect->right + lpcrect->left) / 2.0f;
   pointa[0].Y = lpcrect->top;
   pointa[1].X = lpcrect->left;
   pointa[1].Y = lpcrect->bottom;
   pointa[2].X = lpcrect->right;
   pointa[2].Y = lpcrect->bottom;

   pgraphics->FillPolygon(&brush, pointa, 3);

}

void TemplateDropButton::DrawTriangleDown(Gdiplus::Graphics * pgraphics, LPCRECT lpcrect)
{

   Gdiplus::PointF pointa[3];

   Gdiplus::SolidBrush brush(Gdiplus::Color(128, 128, 128));

   pointa[0].X = (double)(lpcrect->right + lpcrect->left) / 2.0f;
   pointa[0].Y = lpcrect->bottom;
   pointa[1].X = lpcrect->left;
   pointa[1].Y = lpcrect->top;
   pointa[2].X = lpcrect->right;
   pointa[2].Y = lpcrect->top;

   pgraphics->FillPolygon(&brush, pointa, 3);

}

void TemplateDropButton::OnSize(UINT nType, int cx, int cy)
{

   BaseControl::OnSize(nType, cx, cy);

   m_nListWidth = cx;


}



void TemplateDropButton::DisplayList()
{

   CRect ButtonRect;

   GetClientRect(ButtonRect);

   MapWindowPoints(GetDesktopWindow(), ButtonRect);

   int nTemp = m_pDropList->m_iItemHeight;

   int nCount = m_pDropList->m_straItem.GetCount();

   m_nListHeight = nCount * nTemp;

   int nHeight = m_nListHeight;

   CRect NewRect(ButtonRect.left, ButtonRect.bottom - 1, ButtonRect.left + m_nListWidth, ButtonRect.bottom + nHeight);

   m_ListRect = NewRect;

   m_pDropList->SetWindowPos(nullptr,
      m_ListRect.left,
      m_ListRect.top,
      m_ListRect.Width(),
      m_ListRect.Height(),
      SWP_SHOWWINDOW);

}

void TemplateDropButton::OnChangeSel()
{

   Invalidate();

   GetParent()->PostMessage(WM_USER + 121, GetDlgCtrlID());

}


void TemplateDropButton::OnLButtonDown(UINT nFlags, CPoint point)
{

   if (m_pDropList->IsWindowVisible())
   {
     
      m_pDropList->ShowWindow(SW_HIDE);

   }
   else
   {

      DisplayList();

   }

   BaseControl::OnLButtonDown(nFlags, point);

}


