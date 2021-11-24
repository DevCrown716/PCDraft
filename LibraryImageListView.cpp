// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LibraryImageListView.h"
#include "LibraryDoc.h"
#include "LibraryPhoto.h"
#include "LibraryObject.h"
#include "LibraryFrame.h"
#include "LibraryFooter.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CLibraryImageListView

IMPLEMENT_DYNCREATE(CLibraryImageListView, CScrollView)

CLibraryImageListView::CLibraryImageListView()
{
   m_iSel = -1;

   m_plibrary = NULL;
   m_pfooter = NULL;
   
   
}

CLibraryImageListView::~CLibraryImageListView()
{
   if(m_plibrary != NULL)
   {
      delete m_plibrary;
   }
}

BEGIN_MESSAGE_MAP(CLibraryImageListView, CScrollView)
   ON_WM_CREATE()
   ON_WM_SIZE()
   ON_WM_LBUTTONDOWN()
   ON_MESSAGE(WM_APP + 1025,CLibraryImageListView::OnSetSizeRate)
END_MESSAGE_MAP()

void CLibraryImageListView::set_size_rate(double dRate)
{

   if(!IsWindow(GetSafeHwnd()))
   {

      return;

   }

   m_size.cx = m_sizeMin.cx + dRate * (m_sizeMax.cx - m_sizeMin.cx);
   m_size.cy = m_sizeMin.cy + dRate * (m_sizeMax.cy - m_sizeMin.cy);

   layout();

   RedrawWindow();

}

void CLibraryImageListView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();

   Gdiplus::Font f(L"Arial", 12.0f, 0, Gdiplus::UnitPixel);

   Gdiplus::Color colorBlack(0,0,0);

   Gdiplus::SolidBrush br(colorBlack);

   Gdiplus::Graphics g(pDC->GetSafeHdc());

   Gdiplus::Rect r;

   Gdiplus::RectF fr;
   Gdiplus::StringFormat format;

   format.SetAlignment(Gdiplus::StringAlignmentCenter);
   format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
   format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsNoWrap);

   for(int i = 0; i < m_plibrary->get_count(); i++)
   {

      CLibraryPhoto * pitem = m_plibrary->get_item(i);

      r.X = pitem->m_rectDraw.left;

      r.Y = pitem->m_rectDraw.top;

      r.Width = pitem->m_rectDraw.Width();

      r.Height = pitem->m_rectDraw.Height();

      g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

      g.DrawImage(pitem->m_pbitmapThumbnail,r);

      if(i == m_iSel)
      {

         Gdiplus::Rect rSel;

         rSel.X = pitem->m_rectDraw.left;

         rSel.Y = pitem->m_rectDraw.top;

         rSel.Width = pitem->m_rectDraw.Width();

         rSel.Height = pitem->m_rectDraw.Height();

         COLORREF cr = GetSysColor(COLOR_HIGHLIGHT);

         Gdiplus::Color c(GetRValue(cr),GetGValue(cr),GetBValue(cr));

         Gdiplus::Pen pen(c,3.0);

         g.DrawRectangle(&pen,rSel);

      }

      CString strName = pitem->m_strName;

      CStringW wstrName = get_uni(strName);

      fr.X = pitem->m_rect.left;

      fr.Y = pitem->m_rect.bottom;

      fr.Width = pitem->m_rect.Width();

      fr.Height = 20;

      g.DrawString(wstrName,wstrName.GetLength(),&f,fr,&format,&br);

   }

}


// CLibraryImageListView diagnostics

#ifdef _DEBUG
void CLibraryImageListView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CLibraryImageListView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLibraryImageListView message handlers


int CLibraryImageListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CView::OnCreate(lpCreateStruct) == -1)
      return -1;

   m_sizeMax.cx = 267;
   m_sizeMax.cy = 130;
   m_sizeMin.cx = m_sizeMax.cx / 3;
   m_sizeMin.cy = m_sizeMax.cy / 3;

   set_size_rate(0.0);

   CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

   pframe->m_pphotolibrarylistview = this;

   CLibraryDoc * pdoc = (CLibraryDoc *)  GetDocument();

   CString strPath = pdoc->GetDefaultPhotoLibraryPath();

   if(!PathIsDirectory(strPath))
   {

      CreateDirectory(strPath,NULL);

   }

   library_load(strPath,true);
   return 0;
}


void CLibraryImageListView::OnSize(UINT nType,int cx,int cy)
{
   CView::OnSize(nType,cx,cy);

   layout();
}

LRESULT CLibraryImageListView::OnSetSizeRate(WPARAM wparam,LPARAM lparam)
{

   if(wparam == 0)
   {
    
      set_size_rate(lparam / 100.0);

   }
   else
   {
      CString str;
      m_pfooter->m_search.GetWindowText(str);
      m_plibrary->update_filter(str,m_iSel);

      layout();
      RedrawWindow();

   }


   return 0;

}


void CLibraryImageListView::layout()
{
   if(!IsWindow(GetSafeHwnd()))
   {

      return;

   }

   if(m_plibrary == NULL)
   {

      return;

   }

   CRect rC;

   GetClientRect(rC);

   int iW = rC.Width();

   int iH = rC.Height();

   CLibraryDoc * pDoc = (CLibraryDoc *) GetDocument();

   int x = 3;

   int y = 3;

   int iColCount = (iW - 6) / m_size.cx;

   if(iColCount <= 0)
   {

      return;

   }

   if(iColCount > m_plibrary->get_count())
   {

      iColCount = m_plibrary->get_count();

   }

   int iSpacing;

   if(iColCount <= 1)
   {

      iSpacing = 0;

   }
   else
   {

      iSpacing = ((iW - 6) - iColCount * m_size.cx) / (iColCount - 1);

   }

   int iCol = 0;

   for(int i = 0; i < m_plibrary->get_count(); i++)
   {

      CLibraryPhoto * pitem = m_plibrary->get_item(i);

      pitem->m_rect.left = x;

      pitem->m_rect.top = y;

      pitem->m_rect.right = x + m_size.cx;

      pitem->m_rect.bottom = y + m_size.cy;

      int w = pitem->m_pbitmap->GetWidth();

      int h = pitem->m_pbitmap->GetHeight();

      double dRate = min((double)m_size.cx / (double)w,(double)m_size.cy / (double)h);

      int wDraw = w * dRate;

      int hDraw = h * dRate;

      pitem->m_rectDraw.left = x + (m_size.cx - wDraw) / 2;

      pitem->m_rectDraw.top = y + (m_size.cy - hDraw) / 2;

      pitem->m_rectDraw.right = pitem->m_rectDraw.left + wDraw;

      pitem->m_rectDraw.bottom = pitem->m_rectDraw.top + hDraw;

      x += m_size.cx;

      x += iSpacing;

      iCol++;

      if(iCol >= iColCount)
      {

         iCol = 0;

         x = 3;

         y += m_size.cy + 20;

      }

   }

   CSize sizeTotal;

   CSize sizePage;

   CSize sizeLine;

   sizeTotal.cx = iW;

   sizeTotal.cy = y;

   sizePage = rC.Size();

   sizeLine = sizePage;

   sizeLine.cy /= 3;

   SetScrollSizes(MM_TEXT,sizeTotal,sizePage,sizeLine);


   if(m_pfooter != NULL && ::IsWindow(m_pfooter->m_stCount.GetSafeHwnd()))
   {

      CString strItemCount;

      strItemCount.LoadStringA(IDS_LIBRARY_ITEM_COUNT);

      CString str;

      str.Format(strItemCount,m_plibrary->get_count());

      m_pfooter->m_stCount.SetWindowText(str);

   }
}


void CLibraryImageListView::library_load(CString strPath,bool bRedraw)
{

   if(m_plibrary != NULL)
   {

      if(m_plibrary->m_strPath.CompareNoCase(strPath) == 0)
      {

         return;

      }

///      m_plibrary->defer_delete();

   }

   m_plibrary = new CPhotoLibrary();

   m_plibrary->m_strPath = strPath;

   m_plibrary->load();

   if(bRedraw)
   {

      layout();

      RedrawWindow();

   }

}


//CLibraryItem * CLibraryImageListView::LibraryItemFromSelection()
//{
//
//   if(m_iSel < 0 || m_iSel >= m_plibrary->get_count())
//   {
//
//      return NULL;
//
//   }
//
//   CLibraryItem * pitemNew = new CLibraryItem(m_plibrary->get_item(m_iSel));
//
//   return pitemNew;
//
//}


void CLibraryImageListView::OnLButtonDown(UINT nFlags,CPoint point)
{

   int iSel = -1;

   CPoint ptHitTest(point);

   ptHitTest += GetDeviceScrollPosition();

   for(int i = 0; i < m_plibrary->get_count(); i++)
   {

      CLibraryPhoto * pitem = m_plibrary->get_item(i);

      if(pitem->m_rect.PtInRect(ptHitTest))
      {

         iSel = i;

         break;

      }

   }

   m_iSel = iSel;

   //CLibraryItem * pitemSel = LibraryItemFromSelection();

   //if(pitemSel != NULL)
   //{

   //   m_drag.StartDragging(this, pitemSel,&point);

   //}

   RedrawWindow();

   CScrollView::OnLButtonDown(nFlags,point);

}