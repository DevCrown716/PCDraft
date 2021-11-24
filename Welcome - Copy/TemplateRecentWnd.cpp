// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "TemplateRecentWnd.h"
#include "TemplateWnd.h"
#include "GetStartedWnd.h"
#include "../resource.h"
#include "../PCDraft.h"
#include "ScrollHelper.h"
#include "recent_file_list.h"
#include "MirrorFile.h"


bool Contains(const CRect & rectBigger, const CRect & rectSmaller);

Gdiplus::Bitmap * LoadPNG(LPCTSTR pName, LPCTSTR pType, HMODULE hInst);

BEGIN_MESSAGE_MAP(TemplateRecentWnd, BaseControl)
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_ERASEBKGND()
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSELEAVE()
   ON_WM_KEYDOWN()
   ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

TemplateRecentWnd::TemplateRecentWnd()
{

   m_bBaseControlArrowNavigation = false;

   m_iSelGroup = 0;

   m_iSelItem = 0;

   m_iHoverGroup = -1;

   m_iHoverItem = -1;

   m_precentfilelist = new recent_file_list(0, L"Recent Documents", L"File%d", L"Time%d", 50);

   m_precentfilelist->ReadList();
   
   m_crBackground = RGB(242, 242, 242);
   m_pfont = nullptr;
   m_pfontLine1 = nullptr;
   m_pfontLine2 = nullptr;

   if(false)
   {

      {

         Group group;

         group.m_title.m_strTitle = "Today";

         {

            Item item;

            item.m_wstrPath = L"C:\\Users\\Phil\\Desktop\\Document1.md70";

            group.m_itema.Add(item);

         }

         {

            Item item;

            item.m_wstrPath = L"C:\\Users\\Phil\\Desktop\\Document2.md70";

            group.m_itema.Add(item);

         }


         {

            Item item;

            item.m_wstrPath = L"C:\\Users\\Phil\\Desktop\\MyFolder\\MySubFolder\\Document3.md70";

            group.m_itema.Add(item);

         }

         {

            Item item;

            item.m_wstrPath = L"C:\\Users\\Phil\\Desktop\\MyFolder\\MySubFolder\\Document4.md70";

            group.m_itema.Add(item);

         }

         {

            Item item;

            item.m_wstrPath = L"C:\\Users\\Phil\\Desktop\\MyFolder\\MySubFolder\\Document5.md70";

            group.m_itema.Add(item);

         }


         m_groupa.Add(group);

      }

      {

         Group group;

         group.m_title.m_strTitle = "Yesterday";

         {

            Item item;

            item.m_wstrPath = L"C:\\Users\\Phil\\Desktop\\Document1.md70";

            group.m_itema.Add(item);

         }

         {

            Item item;

            item.m_wstrPath = L"C:\\Users\\Phil\\Desktop\\Document2.md70";

            group.m_itema.Add(item);

         }


         {

            Item item;

            item.m_wstrPath = L"C:\\Users\\Phil\\Desktop\\MyFolder\\MySubFolder\\Document3.md70";

            group.m_itema.Add(item);

         }

         {

            Item item;

            item.m_wstrPath = L"C:\\Users\\Phil\\Desktop\\MyFolder\\MySubFolder\\Document4.md70";

            group.m_itema.Add(item);

         }

         {

            Item item;

            item.m_wstrPath = L"C:\\Users\\Phil\\Desktop\\MyFolder\\MySubFolder\\Document5.md70";

            group.m_itema.Add(item);

         }


         m_groupa.Add(group);

      }

   }

   m_pbitmap = LoadPNG(MAKEINTRESOURCEA(IDB_PNG_PCDRAFT_DOC48), "PNG", NULL);

}


TemplateRecentWnd::~TemplateRecentWnd()
{
   delete m_precentfilelist;
   delete m_pbitmap;

}


void TemplateRecentWnd::UpdateList(const CString & strFilter)
{

   m_groupa.RemoveAll();

   Group group;

   CTime timeNow = CTime::GetCurrentTime();

   int iSize = m_precentfilelist->GetSize();

   for (int i = 0; i < iSize; i++)
   {

      CStringW wstrPath = m_precentfilelist->operator[](i);

      CString strPath = get_utf8(wstrPath);

      if (MatchDocument(strPath, strFilter))
      {

         CMirrorFileW  file;

         file.Open(wstrPath, CFile::modeRead, nullptr);

         if (file.operator HANDLE() != INVALID_HANDLE_VALUE
            && file.operator HANDLE() != nullptr)
         {

            Item item;

            item.m_wstrPath = wstrPath;

            CTime time= m_precentfilelist->get_time(i);

            CTimeSpan span = timeNow - time;

            CString strGroup;

            int iHours = span.GetTotalHours();

            if (iHours < 24)
            {

               strGroup = "Today";

            }
            else if (iHours < 48)
            {

               strGroup = "Yesteday";

            }
            else if (iHours < 24 * 7)
            {

               strGroup = "Last Week";

            }
            else if (iHours < 24 * 30)
            {

               strGroup = "Last Month";

            }
            else
            {

               strGroup = "Old Files";

            }

            if (group.m_title.m_strTitle != strGroup)
            {

               if (group.m_itema.GetCount() > 0)
               {

                  m_groupa.Add(group);

                  group.m_itema.RemoveAll();

               }

               group.m_title.m_strTitle = strGroup;

            }

            group.m_itema.Add(item);

         }

      }

   }


   if (group.m_itema.GetCount() > 0)
   {

      m_groupa.Add(group);

   }



   m_iSelGroup = -1;

   m_iSelItem = -1;


   layout();

   Invalidate();

}


bool TemplateRecentWnd::MatchDocument(const CString & strPath, const CString & strFilter)
{

   if (strFilter.IsEmpty())
   {

      return true;

   }


   int iFind = strPath.ReverseFind('\\');

   if (iFind < 0)
   {


      return false;
   }

   CString strTitle = strPath.Mid(iFind + 1);

   strTitle.MakeLower();

   CString strFilterLower(strFilter);

   strFilterLower.MakeLower();

   if (strTitle.Find(strFilterLower) < 0)
   {

      return false;

   }

   return true;

}


BOOL TemplateRecentWnd::OnEraseBkgnd(CDC* pDC)
{

   return TRUE;

}

bool TemplateRecentWnd::hit_test(int & iGroup, int & iItem, CPoint point)
{
   
   point.y += m_scrollHelper->GetScrollPos().cy;
   point.x += m_scrollHelper->GetScrollPos().cx;

   for (iGroup = 0; iGroup < m_groupa.GetCount(); iGroup++)
   {

      Group & group = m_groupa[iGroup];

      for (iItem = 0; iItem < group.m_itema.GetCount(); iItem++)
      {

         Item & item = group.m_itema[iItem];

         if (item.m_rect.PtInRect(point))
         {

            return true;

         }

      }

   }

   iGroup = -1;

   iItem = -1;

   return false;

}


void TemplateRecentWnd::OnLButtonDown(UINT nFlags, CPoint point)
{

   int iSelGroup;

   int iSelItem;

   if (hit_test(iSelGroup, iSelItem, point))
   {

      if (iSelGroup >= 0 && iSelItem >= 0)
      {

         m_iHoverGroup = iSelGroup;

         m_iHoverItem = iSelItem;

         m_iSelGroup = iSelGroup;

         m_iSelItem = iSelItem;

         Invalidate();

      }

   }

   BaseControl::OnLButtonUp(nFlags, point);

}


void TemplateRecentWnd::OnLButtonUp(UINT nFlags, CPoint point)
{

   int iGroup;

   int iItem;

   if(hit_test(iGroup, iItem, point))
   {

      OnClick(iGroup, iItem);

      return;

   }

   BaseControl::OnLButtonUp(nFlags, point);

}


void TemplateRecentWnd::OnMButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnMButtonDblClk(nFlags, point);

}


void TemplateRecentWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{

   BaseControl::OnLButtonDblClk(nFlags, point);

}


int TemplateRecentWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   
   if (BaseControl::OnCreate(lpCreateStruct) == -1)
      return -1;

   enable_scroll_helper();

   //m_buttonNew.Create(NULL, "New", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1000);
   //m_buttonRecent.Create(NULL, "Recent", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1001);
   //m_buttonOpen.Create(NULL, "Open", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_FILE_OPEN);

   //m_buttonWhatsNew.Create(NULL, "What's New", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1000);
   //m_buttonHelp.Create(NULL, "Help & Support", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1001);
   //m_buttonCommunity.Create(NULL, "Community", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1002);

   // TODO:  Add your specialized creation code here

   UpdateList();

   return 0;
}


void TemplateRecentWnd::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   for (int iGroup = 0; iGroup < m_groupa.GetCount(); iGroup++)
   {

      Group & group = m_groupa[iGroup];

      OnDrawGroup(iGroup, pdc, pgraphics, group);

   }


   //CRect rectClient;

   //GetClientRect(rectClient);

   //pdc->FillSolidRect(rectClient, RGB(242, 242, 242));

   //Gdiplus::Graphics g(pdc->GetSafeHdc());

   //Gdiplus::RectF rectSrc(0, 0,
   //   m_pbitmap->GetWidth(),
   //   m_pbitmap->GetHeight());

   //Gdiplus::RectF rectDst(
   //   (rectClient.Width() - rectSrc.Width * size_scaler()) / 2,
   //   40 * size_scaler(),
   //   rectSrc.Width * size_scaler(),
   //   rectSrc.Height * size_scaler());

   //pgraphics->DrawImage(m_pbitmap, rectDst, rectSrc, Gdiplus::UnitPixel);

   //int left = 25;

   //Gdiplus::Pen pen(Gdiplus::Color(255, 190, 70, 42), 4.0);

   //pgraphics->DrawLine(&pen, left, m_xSeparator, rectClient.Width() - left, m_xSeparator);
}



void TemplateRecentWnd::OnSize(UINT nType, int cx, int cy)
{

   BaseControl::OnSize(nType, cx, cy);

   layout();

}


void TemplateRecentWnd::layout()
{
   // TODO: Add your message handler code here

   CRect rectClient;

   GetClientRect(rectClient);

   int cx = rectClient.Width();

   int cy = rectClient.Height();

   int iButtonHeight = 60;

   int y = 0;

   CRect rectTotal;

   for (int i = 0; i < m_groupa.GetCount(); i++)
   {

      Group & group = m_groupa[i];

      group.m_title.m_rect.left = 20;
      group.m_title.m_rect.right = cx - group.m_title.m_rect.left;
      group.m_title.m_rect.top = y;
      y += iButtonHeight;
      group.m_title.m_rect.bottom = y;

      if (i == 0)
      {

         rectTotal = group.m_title.m_rect;

      }
      else
      {

         rectTotal.UnionRect(rectTotal, group.m_title.m_rect);

      }



      for (int j = 0; j < group.m_itema.GetCount(); j++)
      {

         Item & item = group.m_itema[j];

         item.m_rect.left = 20;
         item.m_rect.right = cx - item.m_rect.left;
         item.m_rect.top = y;
         y += iButtonHeight;
         item.m_rect.bottom = y;

         rectTotal.UnionRect(rectTotal, item.m_rect);

      }

   }

}

void TemplateRecentWnd::OnDrawGroup(int iGroup, CDC * pdc, Gdiplus::Graphics * pgraphics, Group & group)
{

   OnDrawTitle(iGroup, pdc, pgraphics, group.m_title);

   for (int iItem = 0; iItem < group.m_itema.GetCount(); iItem++)
   {

      Item & item = group.m_itema[iItem];

      OnDrawItem(iGroup, iItem, pdc, pgraphics, item);

   }

}
void TemplateRecentWnd::OnDrawTitle(int iGroup, CDC * pdc, Gdiplus::Graphics * pgraphics, Title & title)
{

   Gdiplus::RectF rect;

   CRect rectClient(title.m_rect);


   rect.X = rectClient.left;

   rect.Y = rectClient.top;

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

      CString str = title.m_strTitle;

      CStringW wstr = get_uni(str);

      Gdiplus::SolidBrush brush(Gdiplus::Color(255, 0, 0, 0));

      Gdiplus::StringFormat format;

      format.SetAlignment(::Gdiplus::StringAlignmentNear);

      format.SetLineAlignment(::Gdiplus::StringAlignmentCenter);

      pgraphics->DrawString(wstr, wstr.GetLength(), m_pfont, rect, &format, &brush);

   }


}





void TemplateRecentWnd::OnDrawItem(int iGroup, int iItem, CDC * pdc, Gdiplus::Graphics * pgraphics, Item & item)
{

   CRect rectClient(item.m_rect);

   //{

   //   Gdiplus::Color colorBorder;

   //   if (GetFocus() == this)
   //   {

   //      colorBorder = Gdiplus::Color(255, 200, 80, 52);

   //   }
   //   else
   //   {

   //      colorBorder = Gdiplus::Color(255, 192, 192, 192);

   //   }

   //   Gdiplus::Pen pen(colorBorder, 1.0);

   //   Gdiplus::Color colorFill;

   //   if (GetFocus() == this)
   //   {

   //      colorFill = Gdiplus::Color(255, 255, 200, 182);

   //   }
   //   else if (m_bSetHover)
   //   {

   //      colorFill = Gdiplus::Color(255, 242, 242, 242);

   //   }
   //   else
   //   {

   //      colorFill = Gdiplus::Color(255, 255, 255, 255);

   //   }

   //   int iRadius = 11 * size_scaler();

   //   Gdiplus::SolidBrush brush(colorFill);

   //   Gdiplus::GraphicsPath path;

   //   Gdiplus::RectF rect1(item.m_rect.left+(Gdiplus::REAL)(rectClient.Width() - iRadius) - 2.0f, item.m_rect.top+ 1.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

   //   path.AddArc(rect1, -90.0f, 90.0f);

   //   Gdiplus::RectF rect2(item.m_rect.left + (Gdiplus::REAL)(rectClient.Width() - iRadius) - 2.0f, item.m_rect.top + (Gdiplus::REAL)(rectClient.Height() - iRadius) - 2.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

   //   path.AddArc(rect2, 0.0f, 90.0f);

   //   Gdiplus::RectF rect3(item.m_rect.left + 1.0f, item.m_rect.top + (Gdiplus::REAL)(rectClient.Height() - iRadius) - 2.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

   //   path.AddArc(rect3, 90.0f, 90.0f);

   //   Gdiplus::RectF        rect4(item.m_rect.left + 1.0f, item.m_rect.top + 1.0f, (Gdiplus::REAL) (iRadius), (Gdiplus::REAL) (iRadius));

   //   path.AddArc(rect4, 180.0f, 90.0f);

   //   path.CloseFigure();

   //   pgraphics->FillPath(&brush, &path);

   //   pgraphics->DrawPath(&pen, &path);

   //}



   if (m_pfontLine1 == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI Semibold", &installedFontCollection);

      m_pfontLine1 = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (15.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }

   if (m_pfontLine2 == nullptr)
   {

      Gdiplus::InstalledFontCollection installedFontCollection;

      auto pFontFamily = new Gdiplus::FontFamily(L"Segoe UI Semibold", &installedFontCollection);

      m_pfontLine2 = new Gdiplus::Font(pFontFamily, (Gdiplus::REAL) (12.0f * font_scaler()), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

   }

   Gdiplus::Rect rect1;

   rect1.X = rectClient.left;
   rect1.Y = rectClient.top;
   rect1.Width = rectClient.Width();
   rect1.Height = rectClient.Height();

   if ((iItem == m_iHoverItem && iGroup == m_iHoverGroup)
      || 
      (iItem == m_iSelItem && iGroup == m_iSelGroup))
   {


      if (iItem == m_iSelItem && iGroup == m_iSelGroup)
      {

         Gdiplus::SolidBrush bBackground(Gdiplus::Color(255, 255, 200, 182));

         pgraphics->FillRectangle(&bBackground, rect1);

      }
      else
      {

         Gdiplus::SolidBrush bBackground(Gdiplus::Color(255, 182, 182, 182));

         pgraphics->FillRectangle(&bBackground, rect1);

      }

   }



   Gdiplus::REAL spacing;

   Gdiplus::Rect rIcon;


   {

      Gdiplus::Pen pen(Gdiplus::Color(255, 192, 192, 192), 1.0);

      rIcon.Width = 48 * size_scaler();

      rIcon.Height = 48 * size_scaler();

      rIcon.Y = (rectClient.Height() - rIcon.Width) / 2.0f;

      spacing = rIcon.Y * 1.25f;

      rIcon.X = spacing;

      rIcon.X += item.m_rect.left;
      rIcon.Y += item.m_rect.top;

      if (m_pbitmap)
      {

         pgraphics->DrawImage(m_pbitmap, rIcon);

      }
      else
      {

         pgraphics->DrawRectangle(&pen, rIcon);
      }


   }

   CStringW str(item.m_wstrPath);

   CStringW strLine1;
   CStringW strLine2;


   int iLeft = rIcon.X + rIcon.Width + spacing;

   auto iFind = str.ReverseFind('\\');

   if (iFind >= 0)
   {

      strLine2 = str.Left(iFind);
      strLine1 = str.Mid(iFind + 1);

   }
   else
   {

      strLine1 = str;

   }

   int iLineHeight1 = 0;
   int iLineHeight2 = 0;

   Gdiplus::RectF        rectF1(iLeft, 0, rectClient.Width(), rectClient.Height());
   Gdiplus::RectF        rectF2(iLeft, 0, rectClient.Width(), rectClient.Height());

   CStringW wstrLine1 = strLine1;
   CStringW wstrLine2 = strLine2;

   wstrLine2.Replace(L"\\", L"  »  ");

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

   if (!strLine1.IsEmpty())
   {


      Gdiplus::SolidBrush brush(Gdiplus::Color(0, 0, 0));
      rectF1.X += item.m_rect.left;
      rectF1.Y += item.m_rect.top;



      pgraphics->DrawString(wstrLine1, wstrLine1.GetLength(), m_pfontLine1, rectF1, &format, &brush);

   }

   if (!strLine2.IsEmpty())
   {

      Gdiplus::SolidBrush brush(Gdiplus::Color(143, 143, 143));
      rectF2.X += item.m_rect.left;
      rectF2.Y += item.m_rect.top;

      pgraphics->DrawString(wstrLine2, wstrLine2.GetLength(), m_pfontLine2, rectF2, &format, &brush);

   }


   if ((iItem == m_iSelItem && iGroup == m_iSelGroup))
   {

      Gdiplus::Pen pen(Gdiplus::Color(255, 255, 180, 122), 2.0);

      pgraphics->DrawRectangle(&pen, rect1);

   }




}





void TemplateRecentWnd::OnMouseMove(UINT nFlags, CPoint point)
{

   int iHoverGroup;

   int iHoverItem;

   if (hit_test(iHoverGroup, iHoverItem, point))
   {

      int iHoverGroupOld = m_iHoverGroup;

      int iHoverItemOld = m_iHoverItem;

      if (iHoverItem != m_iHoverItem || iHoverGroup != m_iHoverGroup)
      {

         if ((m_iHoverGroup < 0 || m_iHoverItem < 0) &&
            ((iHoverItem >= 0) && (iHoverGroup >= 0)))
         {

            TRACKMOUSEEVENT tme = {};
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            TrackMouseEvent(&tme);


         }

         m_iHoverGroup = iHoverGroup;

         m_iHoverItem = iHoverItem;

         if (iHoverGroupOld >= 0 && iHoverGroupOld < m_groupa.GetCount()
            && iHoverItemOld >= 0 && iHoverItemOld < m_groupa[iHoverGroupOld].m_itema.GetCount())
         {

            InvalidateRect(m_groupa[iHoverGroupOld].m_itema[iHoverItemOld].m_rect - m_scrollHelper->GetScrollPos());

         }

         if (iHoverGroup >= 0 && iHoverGroup < m_groupa.GetCount()
            && iHoverItem >= 0 && iHoverItem < m_groupa[iHoverGroup].m_itema.GetCount())
         {

            InvalidateRect(m_groupa[iHoverGroup].m_itema[iHoverItem].m_rect - m_scrollHelper->GetScrollPos());

         }

      }

   }

   BaseControl::OnMouseMove(nFlags, point);
}


void TemplateRecentWnd::OnMouseLeave()
{
   // TODO: Add your message handler code here and/or call default

   BaseControl::OnMouseLeave();

   m_iHoverGroup = -1;

   m_iHoverItem = -1;

   Invalidate();

}


void TemplateRecentWnd::ensure_item_visible(int iGroup, int iItem)
{

   CRect rect;

   GetClientRect(rect);

   rect.OffsetRect(m_scrollHelper->m_scrollPos);

   if (!Contains(rect, m_groupa[iGroup].m_itema[iItem].m_rect))
   {

      m_scrollHelper->m_scrollPos.cx = 0;
      m_scrollHelper->m_scrollPos.cy = m_groupa[iGroup].m_itema[iItem].m_rect.top;

   }



}

bool TemplateRecentWnd::next_item(int & iGroup, int & iItem)
{

   if (iGroup < 0 || iItem < 0)
   {

      if (m_groupa.IsEmpty())
      {

         return false;

      }

      iGroup = 0;

      iItem = 0;
      

   }

   if (iItem < m_groupa[iGroup].m_itema.GetUpperBound())
   {

      iItem++;

      return true;

   }

   if (iGroup < m_groupa.GetUpperBound())
   {

      iGroup++;

      iItem = 0;

      return true;

   }

   return false;

}

bool TemplateRecentWnd::previous_item(int & iGroup, int & iItem)
{

   if (iGroup < 0 || iItem < 0)
   {

      if (m_groupa.IsEmpty())
      {

         return false;

      }

      iGroup = 0;

      iItem = 0;


   }

   if (iItem > 0)
   {

      iItem--;

      return true;

   }

   if (iGroup > 0)
   {

      iGroup--;

      iItem = m_groupa[iGroup].m_itema.GetUpperBound();

      return true;

   }

   return false;


}



void TemplateRecentWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   // TODO: Add your message handler code here and/or call default

   BaseControl::OnKeyDown(nChar, nRepCnt, nFlags);


   if (nChar == VK_RETURN || nChar == VK_SPACE)
   {

      OnClick(m_iSelGroup, m_iSelItem);

   }
   else if (nChar == VK_LEFT || nChar == VK_UP)
   {

      if (previous_item(m_iSelGroup, m_iSelItem))
      {

         ensure_item_visible(m_iSelGroup, m_iSelItem);

         Invalidate();

      }

   }
   else if (nChar == VK_RIGHT || nChar == VK_DOWN)
   {

      if (next_item(m_iSelGroup, m_iSelItem))
      {

         ensure_item_visible(m_iSelGroup, m_iSelItem);

         Invalidate();

      }

   }

}


void TemplateRecentWnd::OnClick(int iGroup, int iItem)
{

   if (iGroup < 0)
   {

      return;

   }

   if (iGroup >= m_groupa.GetCount())
   {

      return;

   }

   auto & group = m_groupa[iGroup];

   if (iItem < 0)
   {

      return;

   }

   if (iItem >= group.m_itema.GetCount())
   {

      return;

   }

   Item & item = group.m_itema[iItem];

   if (((CPCDraftApp*)AfxGetApp())->m_pTemplateWnd)
   {

      ((CPCDraftApp*)AfxGetApp())->m_pTemplateWnd->ShowWindow(SW_HIDE);

   }

   if (((CPCDraftApp*)AfxGetApp())->m_pGetStartedWnd)
   {

      ((CPCDraftApp*)AfxGetApp())->m_pGetStartedWnd->ShowWindow(SW_HIDE);

   }

   ((CPCDraftApp*)AfxGetApp())->ShowMainFrame();

   ((CPCDraftApp*)AfxGetApp())->m_bWorkaroundUTF8 = true;

   ((CPCDraftApp*)AfxGetApp())->OpenDocumentFile(get_utf8(item.m_wstrPath));

   ((CPCDraftApp*)AfxGetApp())->m_bWorkaroundUTF8 = false;


}


void TemplateRecentWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{

   BaseControl::OnShowWindow(bShow, nStatus);

}



