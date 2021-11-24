// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LibraryListView.h"
#include "LibraryObject.h"
#include "LibraryFrame.h"
#include "LibraryDoc.h"
#include "LibraryFooter.h"
#include "LibraryTreeView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
HBITMAP CreateDIB(int x,int y,COLORREF * pdata,int stride,COLORREF ** p = NULL); // Interface Update - Infinisys Ltd.
IMPLEMENT_DYNCREATE(CLibraryListView, CScrollView)
bool is_updating_library_tree();
CString new_uuid();
CMutex * library_mutex();
bool is_spotlibrary_path(CString strPath);

CString acp_to_utf8(CString str)
{
   return get_cp(get_uni(str,CP_ACP),CP_UTF8);
}

CString utf8_to_acp(CString str)
{
   return get_cp(get_uni(str,CP_UTF8),CP_ACP);
}


CString oem_to_mac(CString str)
{
   return get_cp(get_uni(str,CP_MACCP),CP_UTF8);
}

CString mac_to_oem(CString str)
{
   return get_cp(get_uni(str,CP_UTF8),CP_MACCP);
}

#define ID_EDITLABEL 50000


CLibraryListView::CLibraryListView()
{
   
   m_fontProgress.CreatePointFont(92, "Segoe UI");
   m_iLastSel = -1;
   m_plibrary = NULL;
   m_pfooter = NULL;
   m_bLoadingLibrary = false;
   m_bProgressBarVisible = false;
   m_plibraryProgress = nullptr;

}

CLibraryListView::~CLibraryListView()
{

   if(m_plibrary != NULL)
   {

      try
      {

         delete m_plibrary;

      }
      catch(...)
      {

      }



   }



}

BEGIN_MESSAGE_MAP(CLibraryListView, CScrollView)
   ON_WM_CREATE()
   ON_WM_QUERYDRAGICON()
   ON_MESSAGE(WM_APP + 1024,CLibraryListView::OnDropLibraryItem)
   ON_MESSAGE(WM_APP + 1025,CLibraryListView::OnSetSizeRate)
   ON_MESSAGE(WM_APP + 1026,CLibraryListView::OnFinishLoading)
   ON_WM_LBUTTONDOWN()
   ON_WM_SIZE()
   ON_WM_DESTROY()
   ON_MESSAGE(WM_USER+384,&CLibraryListView::OnEditKillFocus)
   ON_COMMAND(ID_FILE_RENAMEITEM,&CLibraryListView::OnFileRenameitem)
   ON_UPDATE_COMMAND_UI(ID_FILE_RENAMEITEM,&CLibraryListView::OnUpdateFileRenameitem)
   ON_WM_KEYDOWN()
   ON_WM_ERASEBKGND()
   ON_WM_PAINT()
   ON_WM_SHOWWINDOW()
   ON_WM_TIMER()
END_MESSAGE_MAP()



void CLibraryListView::set_size_rate(double dRate,bool bLayoutAndRedraw)
{

   if(!IsWindow(GetSafeHwnd()))
   {

      return;

   }

   m_size.cx = m_sizeMin.cx + dRate * (m_sizeMax.cx - m_sizeMin.cx);
   m_size.cy = m_sizeMin.cy + dRate * (m_sizeMax.cy - m_sizeMin.cy);

   if(bLayoutAndRedraw)
   {

      layout();

      Invalidate(FALSE);

   }

}


bool CLibraryListView::is_drop_enabled()
{

   if(m_plibrary == NULL)
   {

      return false;

   }

   return true;

}


void CLibraryListView::OnDraw(CDC* pdcWindow)
{

   CSingleLock sl1(library_mutex(), TRUE);

   if (m_plibrary == NULL)
   {

      return;

   }

//   CSingleLock sl(&m_plibrary->m_mutex, TRUE);

   CRect rectClient;

   GetClientRect(rectClient);

   CBitmap bmpMem;

   bmpMem.CreateCompatibleBitmap(pdcWindow,rectClient.Width(),rectClient.Height());

   CDC dcMem;

   dcMem.CreateCompatibleDC(pdcWindow);

   dcMem.SetMapMode(MM_TEXT);

   CBitmap * pbmpOld = dcMem.SelectObject(&bmpMem);

   dcMem.FillSolidRect(rectClient,RGB(255,255,255));

   dcMem.SetViewportOrg(pdcWindow->GetViewportOrg());

   Gdiplus::Font f(L"Segoe UI",12.0f,0,Gdiplus::UnitPixel);

   Gdiplus::Color colorBlack(0,0,0);

   Gdiplus::SolidBrush br(colorBlack);

   {

      Gdiplus::Graphics g(dcMem.GetSafeHdc());

      Gdiplus::Rect r;

      Gdiplus::RectF fr;

      Gdiplus::StringFormat format;

      format.SetAlignment(Gdiplus::StringAlignmentCenter);

      format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);

      format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsNoWrap);

      if (m_plibrary == NULL)
      {

         return;

      }

      for(int i = 0; i < m_plibrary->get_count(); i++)
      {

         if (m_plibrary == NULL)
         {

            return;

         }

         CLibraryItem * pitem = m_plibrary->get_item(i);

         if(pitem->m_rectDraw.bottom < -pdcWindow->GetViewportOrg().y)
         {

            continue;

         }

         if(pitem->m_rectDraw.top > -pdcWindow->GetViewportOrg().y + rectClient.Height())
         {

            break;

         }

         r.X = pitem->m_rectDraw.left;

         r.Y = pitem->m_rectDraw.top;

         r.Width = pitem->m_rectDraw.Width();

         r.Height = pitem->m_rectDraw.Height();

         g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

         g.DrawImage(pitem->get_thumbnail2(),r);

         if(is_selected(i))
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

         if (m_plibrary == NULL)
         {

            return;

         }

      }

   }

   pdcWindow->BitBlt(
      -pdcWindow->GetViewportOrg().x,-pdcWindow->GetViewportOrg().y,
      rectClient.Width(),rectClient.Height(),
      &dcMem,
      -pdcWindow->GetViewportOrg().x,-pdcWindow->GetViewportOrg().y,
      SRCCOPY);


   dcMem.SelectObject(pbmpOld);



}


// CLibraryListView diagnostics

#ifdef _DEBUG
void CLibraryListView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CLibraryListView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


UINT AFX_CDECL library_library_load(LPVOID p)
{

   CLibraryListView * pview = (CLibraryListView *)p;

   CSingleLock sl(&pview->m_mutexLoad);

   if (pview->GetDocument()->m_ptreeview->m_bFirstLoad)
   {

      pview->EnableWindow(FALSE);

      pview->RedrawWindow();

   }

   pview->SetTimer(123, 20, NULL);

   pview->library_load(pview->m_strLoadPath,true,false);

   if (GetTickCount() - pview->m_dwStartProgress > 200)
   {

      if (pview)
      {

         if (pview->m_plibrary)
         {

            pview->m_plibrary->m_dProgress = 1.0;

            pview->Invalidate(FALSE);

         }

      }

      Sleep(40);

      pview->KillTimer(123);

   }

   pview->PostMessage(WM_APP + 1026);

   return 0;

}


void CLibraryListView::start_load(CString strLoadPath)
{

   {

      CSingleLock sl(&m_mutexLoad);

      if(m_plibrary != NULL)
      {

         m_plibrary->save_cache();

         if(m_plibrary->m_strPath2.CompareNoCase(strLoadPath) == 0)
         {

            return;

         }

         m_plibrary->defer_delete();

      }

      m_plibrary = NULL;

      m_iaSel.RemoveAll();

      Invalidate(FALSE);

      m_strLoadPath = strLoadPath;

   }

   if(m_bLoadingLibrary)
   {

      return;

   }

   m_dwStartProgress = GetTickCount();

   m_bLoadingLibrary = true;

   CLibraryDoc * pdoc = (CLibraryDoc *)GetDocument();

   pdoc->m_ptreeview->EnableWindow(FALSE);

   AfxBeginThread(&library_library_load,this);

}


int CLibraryListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   
   if(CView::OnCreate(lpCreateStruct) == -1)
      return -1;

   m_strProgress.LoadString(IDS_LOADING_LIBRARIES);

   CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

   pframe->m_plibrarylistview = this;
   if (pframe->m_plibrarytreeview)
   {
      pframe->m_plibrarytreeview->m_plibrarylistview = this;
   }

   //VERIFY(m_wndProgBar.Create(WS_CHILD, CRect(), this, 1));
   //m_wndProgBar.SetRange(0, 100);

   m_edit.m_pwnd = this;

   m_font.CreatePointFont(100,"Segoe UI");
   m_sizeMax.cx = 267;
   m_sizeMax.cy = 130;
   m_sizeMin.cx = m_sizeMax.cx / 3;
   m_sizeMin.cy = m_sizeMax.cy / 3;
   m_edit.Create(WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,CRect(0,0,0,0),this,ID_EDITLABEL);
   m_edit.SetFont(&m_font);
   set_size_rate(0.0, false);

   m_dragdrop.Initialize(this);

   CLibraryDoc * pdoc = GetDocument();

   //CString strPath = pdoc->GetDefaultLibraryPath();

   //if(!PathIsDirectory(strPath))
   //{

   //   CreateDirectory(strPath,NULL);

   //}

   //start_load(strPath);

   return 0;

}


HCURSOR CLibraryListView::OnQueryDragIcon()
{

   return CView::OnQueryDragIcon();

}


LRESULT CLibraryListView::OnDropLibraryItem(WPARAM wparam,LPARAM lparam)
{

   CLibraryItem * pitem = (CLibraryItem *) lparam;

   if(!is_spotlibrary_path(m_plibrary->m_strPath2))
   {

      return 0;

   }

   m_plibrary->library_add(pitem);

   CLibraryDoc * pDoc = GetDocument();

   m_plibrary->save();

   layout();

   Invalidate(FALSE);

   return 0;

}


LRESULT CLibraryListView::OnSetSizeRate(WPARAM wparam,LPARAM lparam)
{

   if(wparam == 0)
   {
      set_size_rate(lparam / 100.0, true);

   }
   else
   {
      CString str;
      m_pfooter->m_search.GetWindowText(str);
      m_iaSel.RemoveAll();
      m_plibrary->update_filter(str);
      layout();
      Invalidate(FALSE);
   }

   return 0;

}


void CLibraryListView::layout()
{

   if(!IsWindow(GetSafeHwnd()))
   {
      
      return;

   }

   if(m_plibrary == NULL)
   {

      return;

   }

   if(!m_plibrary->m_bLibraryOk)
   {

      return;

   }

   CRect rC;

   GetClientRect(rC);

   int iW = rC.Width();

   int iH = rC.Height();

   CLibraryDoc * pDoc = GetDocument();

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

   int maxy = 0;

   for(int i = 0; i < m_plibrary->get_count(); i++)
   {
      
      CLibraryItem * pitem = m_plibrary->get_item(i);
      
      pitem->m_rect.left = x;

      pitem->m_rect.top = y;

      pitem->m_rect.right = x + m_size.cx;

      pitem->m_rect.bottom = y + m_size.cy;

      pitem->m_rectLabel.left = x;

      pitem->m_rectLabel.top = pitem->m_rect.bottom;

      pitem->m_rectLabel.right = x + m_size.cx;

      pitem->m_rectLabel.bottom = pitem->m_rectLabel.top + 20;

      maxy = max(maxy, pitem->m_rectLabel.bottom);

      int w = pitem->get_thumbnail_size().cx;

      int h = pitem->get_thumbnail_size().cy;

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

   sizeTotal.cy = maxy;

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

   CRect rectStatusBar;

   rectStatusBar.top = ((rC.top + rC.bottom) / 2) - 8;
   rectStatusBar.bottom = ((rC.top + rC.bottom) / 2) + 8;

   rectStatusBar.left = (rC.Width()) / 5;
   rectStatusBar.right = (rC.Width()) * 4 / 5;

   m_rectProgress = rectStatusBar;

}


CLibraryDoc * CLibraryListView::GetDocument()
{

   return (CLibraryDoc *)CView::GetDocument();

}


void CLibraryListView::OnLButtonDown(UINT nFlags,CPoint point)
{

   int iSel = -1;

   CPoint ptHitTest(point);

   ptHitTest += GetDeviceScrollPosition();

   for(int i = 0; i < m_plibrary->get_count(); i++)
   {

      CLibraryItem * pitem = m_plibrary->get_item(i);

      if(pitem->m_rect.PtInRect(ptHitTest))
      {

         iSel = i;

         break;

      }
      if(pitem->m_rectLabel.PtInRect(ptHitTest))
      {
         if(m_edit.IsWindowVisible() && get_sel() == i)
         {
            return;
         }

         finish_rename_item();
         m_iaSel.RemoveAll();
         m_iaSel.Add(i);
         rename_item();
         CView::OnLButtonDown(nFlags,point);
         return;

      }

   }

   finish_rename_item();

   if(!(nFlags & MK_CONTROL))
   {

      m_iaSel.RemoveAll();

   }

   if(nFlags & MK_SHIFT)
   {

      if(m_iLastSel >= 0)
      {

         int iMin = min(iSel,m_iLastSel);

         int iMax = max(iSel,m_iLastSel);

         for(int i = iMin; i <= iMax; i++)
         {

            m_iaSel.Add(i);

         }

      }
   }
   else
   {

      m_iaSel.Add(iSel);

   }

   m_iLastSel = iSel;

   CLibraryItem * pitemSel = LibraryItemFromSelection();

   if(pitemSel != NULL)
   {

      m_dragdrop.StartDragging(pitemSel,&point);

   }

   Invalidate(FALSE);

   CView::OnLButtonDown(nFlags,point);

}


void CLibraryListView::OnSize(UINT nType,int cx,int cy)
{
   
   CView::OnSize(nType,cx,cy);

   layout();

}


CLibraryItem * CLibraryListView::LibraryItemFromSelection()
{

   if(m_iaSel.GetCount() <= 0)
   {

      return NULL;

   }
 
   if(m_iaSel[0] < 0 || m_iaSel[0] >= m_plibrary->get_count())
   {

      return NULL;

   }

   CLibraryItem * pitemNew = m_plibrary->get_item(m_iaSel[0]);

   m_plibrary->m_itema.ElementAt(m_iaSel[0]) = new CLibraryItem();

   m_plibrary->m_itema.ElementAt(m_iaSel[0])->m_plibrary = m_plibrary;

   m_plibrary->m_itema.ElementAt(m_iaSel[0])->m_strPath = pitemNew->m_strPath;

   m_plibrary->m_itema.ElementAt(m_iaSel[0])->m_strUuid = pitemNew->m_strUuid;

   m_plibrary->m_itema.ElementAt(m_iaSel[0])->load();

   m_plibrary->m_itema.ElementAt(m_iaSel[0])->load_bk();

   //m_plibrary->load(m_plibrary->m_strPath2, true);

   layout();

   return pitemNew;

}



void CLibraryListView::library_load(CString strPath, bool bAsync, bool bRedraw)
{

   if(m_plibrary != NULL)
   {

      if(m_plibrary->m_strPath2.CompareNoCase(strPath) == 0)
      {

         return;

      }

      m_plibrary->defer_delete();

   }
   
   m_iaSel.RemoveAll();

   auto plibrary = new CLibrary();

   m_plibraryProgress = plibrary;

   plibrary->m_strPath2 = strPath;

   plibrary->load(strPath, bAsync, 0);

   if (plibrary)
   {

      plibrary->sort_by_name();

   }

   m_plibrary = plibrary;

   m_plibraryProgress = nullptr;

   if(bRedraw)
   {

      layout();

      Invalidate(FALSE);

   }


}


void CLibraryListView::OnDestroy()
{

   m_dragdrop.Revoke();

   CView::OnDestroy();

}


bool CLibraryListView::is_selected(int iItem)
{

   for(int i = 0; i < m_iaSel.GetCount(); i++)
   {

      if(m_iaSel[i] == iItem)
      {

         return true;

      }

   }

   return false;

}

int CLibraryListView::get_sel()
{

   if(m_iaSel.GetCount() != 1)
   {
      
      return -1;

   }

   if(m_iaSel[0] < 0 || m_iaSel[0] >= m_plibrary->get_count())
   {

      return -1;

   }

   return m_iaSel[0];

}


void CLibraryListView::rename_item()
{

   if(get_sel() < 0)
   {

      return;

   }

   CRect rect;

   rect = m_plibrary->get_item(get_sel())->m_rectLabel;

   rect -= GetDeviceScrollPosition();

   CStringW wstrText = get_uni(m_plibrary->get_item(get_sel())->m_strName);

   ::SetWindowTextW(m_edit.GetSafeHwnd(),wstrText);
   
   m_edit.SetWindowPos(&CWnd::wndTop,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW);
   m_edit.ShowWindow(SW_SHOW);
   m_edit.SetFocus();
   m_edit.SetSel(0,-1);

}

LRESULT CLibraryListView::OnEditKillFocus(WPARAM wparam, LPARAM lparam)
{
   finish_rename_item();

   return 0;

}

void CLibraryListView::OnFileRenameitem()
{
   
   CLibraryFrame * pframe = (CLibraryFrame *) GetParentFrame();

   pframe->OnFileRenameitem();

}


void CLibraryListView::OnUpdateFileRenameitem(CCmdUI *pCmdUI)
{
   CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

   pframe->OnUpdateFileRenameitem(pCmdUI);
}


BOOL CLibraryListView::PreTranslateMessage(MSG* pMsg)
{
   if(pMsg->message == WM_KEYDOWN &&
      pMsg->wParam == VK_RETURN &&
      GetFocus() == &m_edit)
   {
      finish_rename_item();
      return TRUE; // this doesn't need processing anymore
   }
   return CScrollView::PreTranslateMessage(pMsg);
}

void CLibraryListView::finish_rename_item()
{

   if(get_sel() < 0)
   {

      return;

   }

   if(!m_edit.IsWindowVisible())
   {

      return;

   }

   // handle return pressed in edit control
   WCHAR pwsz[1024];
   ::GetWindowTextW(m_edit.GetSafeHwnd(),pwsz,sizeof(pwsz) / sizeof(pwsz[0]));
   auto pitem = m_plibrary->get_item(get_sel());
   pitem->m_strName = get_utf8(pwsz);
   pitem->save();
   auto & cacheitem = m_plibrary->m_mapCache[pitem->m_strUuid];

   cacheitem.m_strName = pitem->m_strName;

   m_plibrary->m_bCacheModified = true;
   layout();
   Invalidate(FALSE);
   m_edit.ShowWindow(SW_HIDE);

}

void CLibraryListView::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
{
   if(nChar == VK_DELETE)
   {

      if(m_iaSel.GetCount() > 0)
      {

         if(AfxMessageBox(IDS_LIBRARY_DELETE_SEL_WARNING,MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
         {

            CStringArray straDel;
            
            m_plibrary->get_uuida(straDel, m_iaSel);

            m_plibrary->del(straDel);

            layout();

            Invalidate(FALSE);

         }

      }

   }

   CScrollView::OnKeyDown(nChar,nRepCnt,nFlags);
}



DROPEFFECT CLibraryListView::OnDragOver(CWnd * pWnd,COleDataObject* pDataObject,DWORD dwKeyState,CPoint point)
{

   if(pDataObject->IsDataAvailable(CF_OWNERDISPLAY))
   {

      if(!is_spotlibrary_path(m_plibrary->m_strPath2))
      {

         return DROPEFFECT_NONE;

      }

   }

   return DROPEFFECT_COPY;

}


DROPEFFECT CLibraryListView::OnDragEnter(CWnd * pWnd,COleDataObject* pDataObject,DWORD dwKeyState,CPoint point)
{

   return OnDragOver(pWnd,pDataObject,dwKeyState,point);

}

BOOL CLibraryListView::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default

   //return CScrollView::OnEraseBkgnd(pDC);

   return TRUE;
}


void CLibraryListView::OnPaint()
{

   CPaintDC dc(this); // device context for painting

   if(is_updating_library_tree() || m_bLoadingLibrary || m_plibrary == NULL || !m_plibrary->m_bLibraryOk || m_nMapMode == 0)
   {

      if (!m_bProgressBarVisible)
      {

         m_bProgressBarVisible = true;

      }


      CRect rC;

      GetClientRect(rC);

      dc.SetMapMode(MM_TEXT);

      double dProgress = 0.0;

      if (m_plibraryProgress)
      {

         dProgress = m_plibraryProgress->m_dProgress;

      }
      else if (m_plibrary)
      {

         dProgress = m_plibrary->m_dProgress;

      }


      if (IsWindowEnabled())
      {

         dc.FillSolidRect(rC, RGB(255, 255, 255));

      }
      else
      {

         dc.FillSolidRect(rC, ::GetSysColor(COLOR_3DLIGHT));

      }

      if (GetTickCount() - m_dwStartProgress > 200)
      {

         dc.FillSolidRect(m_rectProgress, RGB(230, 230, 230));

         CRect rectProgress(m_rectProgress);

         rectProgress.right = rectProgress.Width() * dProgress + rectProgress.left;

         dc.FillSolidRect(rectProgress, RGB(100, 200, 120));

         dc.Draw3dRect(m_rectProgress, RGB(192, 192, 192), RGB(192, 192, 192));

         dc.SetBkMode(TRANSPARENT);

         dc.SetTextColor(RGB(50, 50, 50));

         dc.SelectObject(&m_fontProgress);

         CRect rectText(m_rectProgress);

         rectText.OffsetRect(0, 16);

         rectText.bottom += 16;

         dc.DrawText(m_strProgress + CString('.', (GetTickCount() / 500 % 4)), rectText, DT_LEFT | DT_VCENTER);


      }

      return;

   }
   else
   {

      if (m_bProgressBarVisible)
      {

//         m_wndProgBar.ModifyStyle(WS_VISIBLE, 0, RDW_INVALIDATE | RDW_UPDATENOW);

         m_bProgressBarVisible = false;

      }


   }

   OnPrepareDC(&dc);

   OnDraw(&dc);

}


void CLibraryListView::OnPrint(CDC* pDC,CPrintInfo* pInfo)
{
   // TODO: Add your specialized code here and/or call the base class

   CScrollView::OnPrint(pDC,pInfo);
}


void CLibraryListView::OnShowWindow(BOOL bShow,UINT nStatus)
{
   CScrollView::OnShowWindow(bShow,nStatus);

}


LRESULT CLibraryListView::OnFinishLoading(WPARAM wparam,LPARAM lparam)
{

   CLibraryDoc * pdoc = (CLibraryDoc *)GetDocument();



   if (m_plibrary)
   {

      m_plibrary->m_bLibraryOk = true;

   }

   m_bLoadingLibrary = false;

   if (GetDocument()->m_ptreeview->m_bFirstLoad)
   {

      EnableWindow(TRUE);

      RedrawWindow();

      GetDocument()->m_ptreeview->m_bFirstLoad = false;

   }

   pdoc->m_ptreeview->EnableWindow(TRUE);

   Invalidate(FALSE);

   CRect rClient;

   GetClientRect(rClient);

   SendMessage(WM_SIZE,0,MAKELPARAM(rClient.Width(),rClient.Height()));

   if (m_plibrary)
   {

      m_plibrary->m_bLibraryOk = TRUE;

      RedrawWindow();

   }

   return 0;

}


void CLibraryListView::OnTimer(UINT_PTR nIDEvent)
{

   CScrollView::OnTimer(nIDEvent);

   if(nIDEvent == 123)
   {

      Invalidate(FALSE);

   }

}


//
//CLibraryItem * pitemCopy = m_plibrary->get_item(m_iaSel[0]);
//
//auto pitemNew = new CLibraryItem();
//
//pitemNew->m_plibrary = m_plibrary;
//
//pitemNew->m_strPath = pitemCopy->m_strPath;
//
//pitemNew->m_strUuid = pitemCopy->m_strUuid;
//
//pitemNew->load();
//
//pitemNew->load_bk();
//
//pitemNew->get_thumbnail_size();
//
//pitemNew->get_thumbnail2();
//
//


//CLibraryItem * pitemNew = m_plibrary->get_item(m_iaSel[0]);
//
//m_plibrary->m_itema.ElementAt(m_iaSel[0]) = new CLibraryItem();
//
//m_plibrary->m_itema.ElementAt(m_iaSel[0])->m_plibrary = m_plibrary;
//
//m_plibrary->m_itema.ElementAt(m_iaSel[0])->m_strPath = pitemNew->m_strPath;
//
//m_plibrary->m_itema.ElementAt(m_iaSel[0])->m_strUuid = pitemNew->m_strUuid;
//
//m_plibrary->m_itema.ElementAt(m_iaSel[0])->load();
//
//m_plibrary->m_itema.ElementAt(m_iaSel[0])->load_bk();
