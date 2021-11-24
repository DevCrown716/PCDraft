#include "stdafx.h"
#include "TemplateDropList.h"
#include "TemplateDropButton.h"


TemplateDropList::TemplateDropList()
{

   m_bDefaultDrawBackground = false;

   m_bResizable = false;
   m_bResizeGrip = false;
   m_bControlBox = false;
   m_pfontDropList = nullptr;
   m_iCurSel = -1;
   m_bSetHover = false;
   m_pHeaderFont = NULL;
   m_pTextFont = NULL;
   m_pBk = NULL;
   m_pBkHover = NULL;
   m_pBkSelected = NULL;
   m_pBkHoverSelected = NULL;
   m_pFr = NULL;
   m_pFrHover = NULL;
   m_pFrSelected = NULL;
   m_pFrHoverSelected = NULL;

}


TemplateDropList::~TemplateDropList()
{

   if (m_pHeaderFont) delete m_pHeaderFont;
   if (m_pBk) delete m_pBk;
   if (m_pBkHover) delete m_pBkHover;
   if (m_pBkSelected) delete m_pBkSelected;
   if (m_pBkHoverSelected) delete m_pBkHoverSelected;
   if (m_pFr) delete m_pFr;
   if (m_pFrHover) delete m_pFrHover;
   if (m_pFrSelected) delete m_pFrSelected;
   if (m_pFrHoverSelected) delete m_pFrHoverSelected;
   if (m_pBkPen) delete m_pBkPen;

}


BEGIN_MESSAGE_MAP(TemplateDropList, BaseWnd)
   ON_WM_CREATE()
   ON_WM_SIZE()
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSELEAVE()
   ON_WM_KILLFOCUS()
   ON_WM_LBUTTONUP()
   ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


int TemplateDropList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (BaseWnd::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   ModifyStyle(WS_CAPTION 
      | WS_MINIMIZE
      | WS_MINIMIZEBOX 
      |WS_MAXIMIZE
      | WS_MAXIMIZEBOX
      | WS_SYSMENU
      | WS_THICKFRAME, 0, SWP_FRAMECHANGED);

   m_iItemHeight = size_scaler() * 20;
   
   m_pBk = new Gdiplus::SolidBrush(Gdiplus::Color(255, 242, 242, 242));
   m_pBkHover = new Gdiplus::SolidBrush(Gdiplus::Color(255, 232, 232, 232));
   m_pBkSelected = new Gdiplus::SolidBrush(Gdiplus::Color(255, 180, 60, 32));
   m_pBkHoverSelected = new Gdiplus::SolidBrush(Gdiplus::Color(255, 190, 70, 42));
   m_pFr = new Gdiplus::SolidBrush(Gdiplus::Color(255, 0, 0, 0));
   m_pFrHover = new Gdiplus::SolidBrush(Gdiplus::Color(255, 0, 0, 0));
   m_pFrSelected = new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 255));
   m_pFrHoverSelected = new Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 255));

   return 0;

}


void TemplateDropList::OnSize(UINT nType, int cx, int cy)
{
   
   BaseWnd::OnSize(nType, cx, cy);
   
}


int TemplateDropList::hit_test(CPoint point)
{

   CRect rectItem;

   for (int i = 0; i < m_straItem.GetCount(); i++)
   {

      GetItemRect(rectItem, i);

      if (rectItem.PtInRect(point))
      {

         return i;

      }

   }

   return -1;

}


void TemplateDropList::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   CRect rectClient;

   GetClientRect(rectClient);

   CRect rectItem;

   rectItem.top = rectClient.top;

   for (int i = 0; i < m_straItem.GetCount(); i++)
   {

      CStringW strItem = m_straItem[i];

      rectItem.bottom = rectItem.top + m_iItemHeight;

      bool bSelected = m_iCurSel == i;

      bool bHover = m_iHover == i;

      GetItemRect(rectItem, i);

      DrawItem(pgraphics, strItem, rectItem, bSelected, bHover);

   }

}


void TemplateDropList::GetItemRect(LPRECT lprect, int iItem)
{

   GetClientRect(lprect);

   lprect->top = iItem * m_iItemHeight;
   lprect->bottom = lprect->top + m_iItemHeight;

}


void TemplateDropList::DrawItem(Gdiplus::Graphics * pgraphics, CStringW str, CRect rectItem, BOOL bSelected, bool bHover)
{

   Gdiplus::Brush * pbrushBackground = nullptr;
   Gdiplus::Brush * pbrushForeground = nullptr;

   if (bHover)
   {

      if (bSelected)
      {

         pbrushBackground = m_pBkHoverSelected;
         pbrushForeground = m_pFrHoverSelected;

      }
      else
      {

         pbrushBackground = m_pBkHover;
         pbrushForeground = m_pFrHover;

      }

   }
   else
   {

      if (bSelected)
      {

         pbrushBackground = m_pBkSelected;
         pbrushForeground = m_pFrSelected;

      }
      else
      {

         pbrushBackground = m_pBk;
         pbrushForeground = m_pFr;

      }

   }

   Gdiplus::RectF rItem;

   rItem.X = rectItem.left;
   rItem.Y = rectItem.top;
   rItem.Width = rectItem.Width();
   rItem.Height = rectItem.Height();

   pgraphics->FillRectangle(pbrushBackground, rItem);

   if (m_pfontDropList == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI", &installedFontCollection);

      m_pfontDropList = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (12.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }

   Gdiplus::RectF rectF(5, rectItem.top, rectItem.Width(), rectItem.Height());

   //CStringW wstr = get_uni(str);

   CStringW wstr = str;

   Gdiplus::StringFormat format;

   format.SetAlignment(::Gdiplus::StringAlignmentNear);

   format.SetLineAlignment(::Gdiplus::StringAlignmentCenter);

   pgraphics->DrawString(wstr, wstr.GetLength(), m_pfontDropList, rectF, &format, pbrushForeground);

}



void TemplateDropList::OnMouseMove(UINT nFlags, CPoint point)
{

   int iHover = hit_test(point);

   if (m_iHover != iHover)
   {

      m_iHover = iHover;

      Invalidate();

   }

   BaseWnd::OnMouseMove(nFlags, point);

}


void TemplateDropList::OnMouseLeave()
{

   m_iHover = -1;

   BaseWnd::OnMouseLeave();

   Invalidate();

}





void TemplateDropList::OnKillFocus(CWnd* pNewWnd)
{
   BaseWnd::OnKillFocus(pNewWnd);

   ShowWindow(SW_HIDE);
}


void TemplateDropList::OnLButtonUp(UINT nFlags, CPoint point)
{
   // TODO: Add your message handler code here and/or call default

   BaseWnd::OnLButtonUp(nFlags, point);
   m_iCurSel = hit_test(point);

   ShowWindow(SW_HIDE);

   m_pbutton->OnChangeSel();

}


void TemplateDropList::OnLButtonDown(UINT nFlags, CPoint point)
{
   // TODO: Add your message handler code here and/or call default




   BaseWnd::OnLButtonDown(nFlags, point);
}
