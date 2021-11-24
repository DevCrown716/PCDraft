// Layer - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LayerListView.h"
#include "PCDraftDoc.h"
#include "LayerFrame.h"
#include "LayerDoc.h"
#include "LayerFooter.h"
#include "LayerHeader.h"
#include "Layer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void copy(Gdiplus::Rect & r,LPCRECT lpcrect)
{
   r.X = lpcrect->left;
   r.Y = lpcrect->top;
   r.Width = lpcrect->right - lpcrect->left;
   r.Height = lpcrect->bottom - lpcrect->top;
}


void copy(Gdiplus::RectF & r,LPCRECT lpcrect)
{

   r.X = (Gdiplus::REAL) lpcrect->left;
   r.Y = (Gdiplus::REAL) lpcrect->top;
   r.Width = (Gdiplus::REAL) ( lpcrect->right - lpcrect->left);
   r.Height = (Gdiplus::REAL) (lpcrect->bottom - lpcrect->top);

}


Gdiplus::Bitmap * LoadPNG(LPCTSTR pName,LPCTSTR pType,HMODULE hInst);


IMPLEMENT_DYNCREATE(CLayerListView,CScrollView)


CString new_uuid();


#define ID_EDITLABEL 50000


CLayerListView::CLayerListView()
{
   m_iEdit = -1;
   m_iCombo = -1;
   m_bDrag = false;
   m_iSelPress = -1;
   m_iDrag = -1;
   m_iSizeLast = -1;
   
   m_iItemHeight = 18;

   m_iLastSel = -1;
   m_pfooter = NULL;

   m_bitmapaState[0] = LoadPNG(MAKEINTRESOURCE(IDB_PNG_LAYER_ACTIVE), "PNG", NULL);
   m_bitmapaState[1] = LoadPNG(MAKEINTRESOURCE(IDB_PNG_LAYER_LOCKED),"PNG",NULL);
   m_bitmapaShow[0] = LoadPNG(MAKEINTRESOURCE(IDB_PNG_LAYER_VISIBLE),"PNG",NULL);
   m_bitmapaShow[1] = LoadPNG(MAKEINTRESOURCE(IDB_PNG_LAYER_GRAYED),"PNG",NULL);
   m_bitmapaShow[2] = LoadPNG(MAKEINTRESOURCE(IDB_PNG_LAYER_HIDDEN),"PNG",NULL);

   m_pbitmapDropDown = LoadPNG(MAKEINTRESOURCE(IDB_PNG_LAYER_DROPDOWN),"PNG",NULL);


}


CLayerListView::~CLayerListView()
{


}


BEGIN_MESSAGE_MAP(CLayerListView, CScrollView)
   ON_WM_CREATE()
   ON_WM_QUERYDRAGICON()
   ON_MESSAGE(WM_APP + 1024,CLayerListView::OnDropLayerItem)
   ON_MESSAGE(WM_APP + 1025,CLayerListView::OnSetSizeRate)
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_MOUSEMOVE()
   ON_WM_SIZE()
   ON_WM_DESTROY()
   ON_MESSAGE(WM_USER + 384,&CLayerListView::OnEditKillFocus)
   ON_COMMAND(ID_FILE_RENAMEITEM,&CLayerListView::OnFileRenameitem)
   ON_UPDATE_COMMAND_UI(ID_FILE_RENAMEITEM,&CLayerListView::OnUpdateFileRenameitem)
   ON_WM_KEYDOWN()
   ON_NOTIFY_REFLECT(LVN_ITEMCHANGED,&CLayerListView::OnLvnItemchanged)
//   ON_WM_NCPAINT()
ON_WM_ERASEBKGND()
ON_WM_TIMER()
END_MESSAGE_MAP()



void CLayerListView::set_size_rate(double dRate)
{

   if(!IsWindow(GetSafeHwnd()))
   {

      return;

   }

   m_size.cx = (LONG) (m_sizeMin.cx + dRate * (m_sizeMax.cx - m_sizeMin.cx));
   m_size.cy = (LONG) (m_sizeMin.cy + dRate * (m_sizeMax.cy - m_sizeMin.cy));

   layout();

   RedrawWindow();

}


void CLayerListView::OnDraw(CDC* pDC)
{



   Gdiplus::Color colorBlack(0,0,0);

   Gdiplus::SolidBrush br(colorBlack);

   Gdiplus::Color colorState;

   m_bitmapaState[0]->GetPixel(0,0,&colorState);
   
   Gdiplus::SolidBrush brState(colorState);

   Gdiplus::Color colorShow;

   m_bitmapaShow[0]->GetPixel(0,0,&colorShow);

   Gdiplus::SolidBrush brShow(colorShow);

   COLORREF crSelBk = GetSysColor(COLOR_HIGHLIGHT);

   Gdiplus::Color colorSelBk(GetRValue(crSelBk),GetGValue(crSelBk),GetBValue(crSelBk));

   Gdiplus::SolidBrush brSelBk(colorSelBk);

   COLORREF crSel = GetSysColor(COLOR_HIGHLIGHTTEXT);

   Gdiplus::Color colorSel(GetRValue(crSel),GetGValue(crSel),GetBValue(crSel));

   Gdiplus::SolidBrush brSel(colorSel);

   COLORREF crItemBk = GetSysColor(COLOR_WINDOW);

   Gdiplus::Color colorItemBk(GetRValue(crItemBk),GetGValue(crItemBk),GetBValue(crItemBk));

   Gdiplus::SolidBrush brItemBk(colorItemBk);

   COLORREF crBk = GetSysColor(COLOR_BTNFACE);

   Gdiplus::Color colorBk(GetRValue(crBk),GetGValue(crBk),GetBValue(crBk));

   Gdiplus::SolidBrush brBk(colorBk);

   Gdiplus::Graphics g(pDC->GetSafeHdc());

   COLORREF crDk = GetSysColor(COLOR_3DLIGHT);

   Gdiplus::Color colorDk(GetRValue(crDk),GetGValue(crDk),GetBValue(crDk));

   Gdiplus::Pen penDk(colorDk);

   Gdiplus::Rect r;

   Gdiplus::RectF fr;

   Gdiplus::StringFormat format;

   format.SetAlignment(Gdiplus::StringAlignmentNear);

   format.SetLineAlignment(Gdiplus::StringAlignmentFar);

   format.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);

   format.SetFormatFlags(format.GetFormatFlags() | Gdiplus::StringFormatFlagsNoWrap);

   Gdiplus::StringFormat formatNumber;

   formatNumber.SetAlignment(Gdiplus::StringAlignmentFar);

   formatNumber.SetLineAlignment(Gdiplus::StringAlignmentFar);

   formatNumber.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);

   formatNumber.SetFormatFlags(formatNumber.GetFormatFlags() | Gdiplus::StringFormatFlagsNoWrap);

   m_iItemHeight = 0;
   m_iItemHeight = max(m_iItemHeight, (int) ( m_bitmapaState[0]->GetHeight() + 2));
   m_iItemHeight = max(m_iItemHeight, (int) (m_bitmapaShow[0]->GetHeight() + 2));

   Gdiplus::Font f(L"Segoe UI",(Gdiplus::REAL) (m_iItemHeight-8),0,Gdiplus::UnitPixel);

   CRect rA;

   Gdiplus::Rect rB;


   CRect ra[layer_subitem_len];

   Gdiplus::Rect rb[layer_subitem_len];

   int iLastY = 0;

   for(int i = 0; i < GetDocument()->get_item_count(); i++)
   {

      get_item_rect(rA,i);

      iLastY = rA.bottom;

      rA.top++;

      copy(rB,rA);

      Gdiplus::Brush * pbrText;

      if(is_selected(i))
      {
         
         g.FillRectangle(&brSelBk,rB);

         pbrText = &brSel;

      }
      else
      {

         g.FillRectangle(&brItemBk,rB);
         
         pbrText = &br;

      }

      {
       
         get_subitem_rect(ra[layer_subitem_state],i,layer_subitem_state,true);

         ra[layer_subitem_state].top++;

         copy(rb[layer_subitem_state],ra[layer_subitem_state]);

         g.FillRectangle(&brState,rb[layer_subitem_state]);

      }

      {

         get_subitem_rect(rA,i,layer_subitem_state);

         copy(rB,rA);

         g.DrawImage(m_bitmapaState[GetDocument()->get_layer_state(i)],rB);

      }

      {

         get_subitem_rect(ra[layer_subitem_show],i,layer_subitem_show,true);

         ra[layer_subitem_show].top++;

         copy(rb[layer_subitem_show],ra[layer_subitem_show]);

         g.FillRectangle(&brShow,rb[layer_subitem_show]);

      }

      {

         get_subitem_rect(rA,i,layer_subitem_show);

         copy(rB,rA);

         g.DrawImage(m_bitmapaShow[GetDocument()->get_layer_show(i)],rB);

      }


      CStringW wstr = get_uni(GetDocument()->get_layer_name(i));

      get_subitem_rect(rA,i,layer_subitem_name);

      copy(fr,rA);

      g.DrawString(wstr,wstr.GetLength(),&f,fr,&format,pbrText);



      wstr = get_uni(GetDocument()->get_layer_scale_text(i));

      get_subitem_rect(rA,i,layer_subitem_scale);

      copy(fr,rA);

      g.DrawString(wstr,wstr.GetLength(),&f,fr,&format,pbrText);


      fr.X = fr.GetRight() - m_pbitmapDropDown->GetWidth();
      fr.Y = fr.GetBottom() - m_pbitmapDropDown->GetHeight();
      fr.Width = (Gdiplus::REAL) m_pbitmapDropDown->GetWidth();
      fr.Height = (Gdiplus::REAL) (m_pbitmapDropDown->GetHeight());
      g.DrawImage(m_pbitmapDropDown,fr);
      

      wstr = get_uni(GetDocument()->get_layer_shape_count_text(i));

      get_subitem_rect(rA,i,layer_subitem_count);

      copy(fr,rA);

      g.DrawString(wstr,wstr.GetLength(),&f,fr,&formatNumber,pbrText);

   }


   

   GetClientRect(rA);

   if(iLastY < rA.bottom)
   {

      rA.top = iLastY;

      copy(rB,rA);

      g.FillRectangle(&brBk,rB);

   }

   for(int i = 0; i <=GetDocument()-> get_item_count(); i++)
   {
      
      if(i != GetDocument()-> get_item_count())
      {
         for(int j = 0; j < layer_subitem_count; j++)
         {

            get_subitem_rect(rA,i,(e_layer_subitem)j,true);

            copy(rB,rA);

            g.DrawLine(&penDk,rB.GetRight(),rB.GetTop(),rB.GetRight(),rB.GetBottom());

         }

      }

      get_item_rect(rA,i);

      copy(rB,rA);

      if(i != GetDocument()-> get_item_count())
      {
         g.DrawLine(&penDk,rB.GetLeft(),rB.GetBottom(),rB.GetRight(),rB.GetBottom());

      }

      if(m_bDrag)
      {

         if(i == m_iDrag)
         {

            rA.bottom = rA.top + 1;
            
            copy(rB,rA);

            g.FillRectangle(&brSelBk,rB);

            rA.top = rA.bottom + 1;
            rA.bottom= rA.top;

            copy(rB,rA);

            g.FillRectangle(&brBk,rB);

         }

      }


   }

}


#ifdef _DEBUG

void CLayerListView::AssertValid() const
{
	
   CScrollView::AssertValid();

}

void CLayerListView::Dump(CDumpContext& dc) const
{
	
   CScrollView::Dump(dc);

}

#endif //_DEBUG


int CLayerListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   
   if(CScrollView::OnCreate(lpCreateStruct) == -1)
      return -1;

   CLayerFrame * playerframe = (CLayerFrame *)GetParentFrame();

   playerframe->m_playerlistview = this;

   m_edit.m_pwnd = this;
   m_combo.m_pwnd = this;

   m_edit.Create(WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,CRect(0,0,0,0),this,layer_subitem_name);
   m_combo.Create(WS_CHILD | CBS_DROPDOWNLIST,CRect(0,0,0,0),this,layer_subitem_scale);

   SetScrollSizes(MM_TEXT,CSize(0,0));


   ModifyStyleEx(WS_EX_CLIENTEDGE,0);

   ModifyStyle(WS_BORDER, 0);

   m_font.CreatePointFont(110,"Segoe UI");
   m_edit.SetFont(&m_font);

   m_combo.SetFont(&m_font);

   m_sizeMax.cx = 267;
   m_sizeMax.cy = 130;
   m_sizeMin.cx = m_sizeMax.cx / 3;
   m_sizeMin.cy = m_sizeMax.cy / 3;
   set_size_rate(0.0);

   GetDocument()->m_playerframe = playerframe;

   GetDocument()->m_playerlistview = this;

   //m_dragdrop.Initialize(this);

   //CLayerDoc * pdoc = GetDocument();

   //CString strPath = pdoc->GetDefaultLayerPath();

   //if(!PathIsDirectory(strPath))
   //{

   //   CreateDirectory(strPath, NULL);

   //}

   //library_load(strPath, true, true);

   layout();

   return 0;

}


HCURSOR CLayerListView::OnQueryDragIcon()
{

   return CScrollView::OnQueryDragIcon();

}



LRESULT CLayerListView::OnDropLayerItem(WPARAM wparam,LPARAM lparam)
{

   //CLayerItem * pitem = (CLayerItem *) lparam;

   //m_plibrary->library_add(pitem);

   CLayerDoc * pDoc = GetDocument();

   //m_plibrary->save();

   layout();

   RedrawWindow();

   return 0;

}



LRESULT CLayerListView::OnSetSizeRate(WPARAM wparam,LPARAM lparam)
{

   if(wparam == 0)
   {
      set_size_rate(lparam / 100.0);

   }
   else
   {
      CString str;
      m_pfooter->m_search.GetWindowText(str);
      m_iaSel.RemoveAll();
      //m_plibrary->update_filter(str);
      layout();
      RedrawWindow();
   }

   return 0;

}


void CLayerListView::layout()
{

   if(!IsWindow(GetSafeHwnd()))
   {
      
      return;

   }

   //if(m_plibrary == NULL)
   //{

   //   return;

   //}

   CRect rC;

   GetClientRect(rC);

   int iW = rC.Width();

   int iH = rC.Height();

   CSize sizeTotal;

   CSize sizePage;

   CSize sizeLine;

   sizeTotal.cx = iW;

   sizeTotal.cy = m_iItemHeight * GetDocument()->get_item_count();

   sizePage = rC.Size();

   sizeLine = sizePage;

   sizeLine.cy /= 3;

   SetScrollSizes(MM_TEXT,sizeTotal,sizePage,sizeLine);

   //if(m_pfooter != NULL && ::IsWindow(m_pfooter->m_stCount.GetSafeHwnd()))
   //{

   //   CString strItemCount;

   //   strItemCount.LoadStringA(IDS_LIBRARY_ITEM_COUNT);

   //   strItemCount.Format(strItemCount,m_plibrary->get_count());

   //   m_pfooter->m_stCount.SetWindowText(strItemCount);

   //}

}


void CLayerListView::OnUpdateItemCount()
{

   

}


CLayerDoc * CLayerListView::GetDocument()
{

   return (CLayerDoc *)CScrollView::GetDocument();

}


void CLayerListView::OnMouseMove(UINT nFlags,CPoint point)
{

   if(m_bDrag)
   {

      int iDrag = -1;

      CRect rectClient;

      GetClientRect(rectClient);

      int iLineSel = get_selected_item();

      CPoint ptHitTest(point);

      ptHitTest += GetDeviceScrollPosition();

      CPoint ptStart = GetDeviceScrollPosition();

      CPoint ptEnd = GetDeviceScrollPosition();
      
      ptEnd.Offset(0,rectClient.Height());

      int iMin = -1;

      int iMax = -1;

      CRect r;

      for(int i = 0; i <= GetDocument()->get_item_count(); i++)
      {

         get_item_rect(r,i);

         if(r.PtInRect(ptStart))
         {

            iMin = i;

         }

         if(r.PtInRect(ptHitTest))
         {

            iDrag = i;

         }

         if(r.PtInRect(ptEnd))
         {

            iMax = i;

         }

      }

      if(iDrag == iMin && iMin > 0)
      {

         if(m_iDrag != iDrag)
         {

            m_iDrag = iDrag;

            m_dwMinLast = GetTickCount();

            SetTimer(1024,300, NULL);

            RedrawWindow();

         }
         else if(GetTickCount() - m_dwMinLast > 250)
         {

            SetScrollPos(SB_VERT,max(0,GetDeviceScrollPosition().y - m_iItemHeight));

            m_dwMinLast = GetTickCount();

            return;

         }

      }
      else if(iDrag == iMax && iMax > 0)
      {

         if(m_iDrag != iDrag)
         {



            m_iDrag = iDrag;

            m_dwMinLast = GetTickCount();

            SetTimer(1024,300,NULL);

            RedrawWindow();

         }
         else if(GetTickCount() - m_dwMinLast > 250)
         {

            SetScrollPos(SB_VERT,max(0,GetDeviceScrollPosition().y + m_iItemHeight));

            m_dwMinLast = GetTickCount();

            return;

         }

      }
      else if(m_iDrag != iDrag)
      {

         KillTimer(1024);

         m_iDrag = iDrag;

         RedrawWindow();

      }

   }


}

void CLayerListView::OnLButtonDown(UINT nFlags,CPoint point)
{

   m_iSelPress = -1;

   int iLineSel = get_selected_item();

   CPoint ptHitTest(point);

   ptHitTest += GetDeviceScrollPosition();

   CRect r;

   for(int i = 0; i < GetDocument()->get_item_count(); i++)
   {

      get_subitem_rect(r,i,layer_subitem_state,true);

      if(r.PtInRect(ptHitTest))
      {

         OnActivateLayer(i);

         return;

      }

      get_subitem_rect(r,i,layer_subitem_show,true);

      if(r.PtInRect(ptHitTest))
      {

         OnLayerNextShow(i);

         return;

      }

      get_item_rect(r,i);

      if(r.PtInRect(ptHitTest))
      {

         m_iSelPress = i;

         break;

      }

   }

   finish_rename_item();

   if(is_selected(m_iSelPress))
   {

      m_iDrag = m_iSelPress;

      m_iSelPress = -1;

      m_bDrag = true;

      m_dwLastPress = GetTickCount();

   }

   SetCapture();

   CScrollView::OnLButtonDown(nFlags,point);

}


void CLayerListView::OnLButtonUp(UINT nFlags,CPoint point)
{

   ReleaseCapture();

   KillTimer(1024);

   if(m_bDrag && GetTickCount() - m_dwLastPress < 200)
   {

      CPoint ptHitTest(point);

      ptHitTest += GetDeviceScrollPosition();

      CRect r;

      get_subitem_rect(r,m_iDrag,layer_subitem_name,true);

      if(r.PtInRect(ptHitTest))
      {

         m_bDrag = false;

         int iDrag = m_iDrag;

         m_iDrag = -1;

         OnEditLayerName(iDrag);

         return;

      }

      get_subitem_rect(r,m_iDrag,layer_subitem_scale,true);

      if(r.PtInRect(ptHitTest))
      {

         m_bDrag = false;

         int iDrag = m_iDrag;

         m_iDrag = -1;

         OnDropDownLayerScale(iDrag);

         return;

      }

   }

   if(m_bDrag)
   {

      m_bDrag = false;

      if(m_iDrag >= 0)
      {

         m_iaSel.Sort();

         int iDrag = m_iDrag;

         m_iDrag = -1;

         GetDocument()->m_pDoc->MoveLayer(m_iaSel,iDrag);


      }


   }
   else
   {

      if(m_iSelPress >= 0)
      {


         if(!(nFlags & MK_CONTROL))
         {

            m_iaSel.RemoveAll();

         }

         if(m_iSelPress >= 0)
         {


            if(nFlags & MK_SHIFT)
            {

               if(m_iLastSel >= 0)
               {

                  int iMin = min(m_iSelPress,m_iLastSel);

                  int iMax = max(m_iSelPress,m_iLastSel);

                  for(int i = iMin; i <= iMax; i++)
                  {

                     m_iaSel.Add(i);

                  }

               }
            }
            else
            {

               m_iaSel.Add(m_iSelPress);

            }

         }

      }

   }

   m_iLastSel = m_iSelPress;
   
   GetDocument()->m_playerheader->Refresh();

   RedrawWindow();

}


void CLayerListView::OnSize(UINT nType,int cx,int cy)
{
   
   CScrollView::OnSize(nType,cx,cy);

   layout();

}


//CLayerItem * CLayerListView::LayerItemFromSelection()
//{
//
//   if(m_iaSel.GetItemCount() <= 0)
//   {
//
//      return NULL;
//
//   }
// 
//   if(m_iaSel[0] < 0 || m_iaSel[0] >= m_plibrary->get_count())
//   {
//
//      return NULL;
//
//   }
//
//   CLayerItem * pitemNew = m_plibrary->get_item(m_iaSel[0]);
//
//   m_plibrary->m_itema.ElementAt(m_iaSel[0]) = new CLayerItem();
//
//   m_plibrary->m_itema.ElementAt(m_iaSel[0])->m_strPath = pitemNew->m_strPath;
//
//   m_plibrary->m_itema.ElementAt(m_iaSel[0])->m_strUuid = pitemNew->m_strUuid;
//
//   m_plibrary->m_itema.ElementAt(m_iaSel[0])->load();
//
//   m_plibrary->load(true);
//
//   layout();
//
//   return pitemNew;
//
//}


void CLayerListView::library_load(CString strPath, bool bAsync, bool bRedraw)
{

   //if(m_plibrary != NULL)
   //{

   //   if(m_plibrary->m_strPath.CompareNoCase(strPath) == 0)
   //   {

   //      return;

   //   }

   //   m_plibrary->defer_delete();

   //}
   
   m_iaSel.RemoveAll();

   //m_plibrary = new CLayer();

   //m_plibrary->m_strPath = strPath;

   //m_plibrary->load(bAsync);

   //if(bRedraw)
   //{

   //   layout();

   //   RedrawWindow();

   //}

}


void CLayerListView::OnDestroy()
{

   //m_dragdrop.Revoke();

   CScrollView::OnDestroy();

}

void CLayerListView::Refresh()
{
   
   if(m_iSizeLast != GetDocument()->get_item_count())
   {

      m_iSizeLast = GetDocument()->get_item_count();

      layout();

      RedrawWindow();

   }

}
bool CLayerListView::is_selected(int iItem)
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

int CLayerListView::get_sel()
{

   if(m_iaSel.GetCount() != 1)
   {
      
      return -1;

   }

   //if(m_iaSel[0] < 0 || m_iaSel[0] >= m_plibrary->get_count())
   //{

   //   return -1;

   //}

   return m_iaSel[0];

}


void CLayerListView::rename_item()
{

   if(get_sel() < 0)
   {

      return;

   }

   CRect rect;

   //rect = m_plibrary->get_item(get_sel())->m_rectLabel;

   //rect -= GetDeviceScrollPosition();

   //CStringW wstrText = get_uni(m_plibrary->get_item(get_sel())->m_strName);

   //::SetWindowTextW(m_edit.GetSafeHwnd(),wstrText);
   //
   //m_edit.SetWindowPos(&CWnd::wndTop,rect.left,rect.top,rect.Width(),rect.Height(),SWP_SHOWWINDOW);
   //m_edit.ShowWindow(SW_SHOW);
   //m_edit.SetFocus();
   //m_edit.SetSel(0,-1);

}

LRESULT CLayerListView::OnEditKillFocus(WPARAM wparam,LPARAM lparam)
{
   if(lparam == (LPARAM)m_edit.GetSafeHwnd())
   {
      finish_rename_item();
   }
   else if(lparam == (LPARAM)m_combo.GetSafeHwnd())
   {
      finish_scale_item();
   }

   return 0;
}

void CLayerListView::OnFileRenameitem()
{
   
   //CLayerFrame * pframe = (CLayerFrame *) GetParentFrame();

   //pframe->OnFileRenameitem();

}


void CLayerListView::OnUpdateFileRenameitem(CCmdUI *pCmdUI)
{
   //CLayerFrame * pframe = (CLayerFrame *)GetParentFrame();

   //pframe->OnUpdateFileRenameitem(pCmdUI);
}


BOOL CLayerListView::PreTranslateMessage(MSG* pMsg)
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

void CLayerListView::finish_rename_item()
{

   if(m_iEdit < 0)
   {

      return;

   }

   if(!m_edit.IsWindowVisible())
   {

      return;

   }

   //// handle return pressed in edit control
   CHAR psz[1024];
   ::GetWindowText(m_edit.GetSafeHwnd(),psz,sizeof(psz) / sizeof(psz[0]));
   CStringW wstr = get_uni(psz,CP_ACP);

   CLayer* pLayer=GetDocument()->m_pDoc->GetLayer(m_iEdit);
   m_edit.ShowWindow(SW_HIDE);
   CString sName = get_utf8(wstr);
   if(sName != pLayer->m_strName)
   {
      if(GetDocument()->m_pDoc->CheckLayerName(sName))
      {
         pLayer->m_strName=sName;
         GetDocument()->m_pDoc->LayersChanged();
         RedrawWindow();
      }
      else
      {
         AfxMessageBox(IDS_ALREADY_TAKEN);
            OnEditLayerName(m_iEdit);
      }
   }

}

void CLayerListView::finish_scale_item()
{

   if(m_iCombo < 0)
   {

      return;

   }

   if(!m_combo.IsWindowVisible())
   {

      return;

   }


   CLayer* pLayer=GetDocument()->m_pDoc->GetLayer(m_iCombo);
   pLayer->m_nScale = m_combo.GetCurSel() + 1;
   GetDocument()->m_pDoc->LayersChanged();
   m_combo.ShowWindow(SW_HIDE);

}



void CLayerListView::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
{
   if(nChar == VK_DELETE)
   {

      if(m_iaSel.GetCount() > 0)
      {

         if(AfxMessageBox(IDS_LIBRARY_DELETE_SEL_WARNING,MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
         {

            CStringArray straDel;
            
            //m_plibrary->get_uuida(straDel, m_iaSel);

            //m_plibrary->del(straDel);

            layout();

            RedrawWindow();

         }

      }

   }

   CScrollView::OnKeyDown(nChar,nRepCnt,nFlags);

}


int CLayerListView::get_selected_item()
{

   if(get_selected_count() != 1)
   {

      return -1;

   }

   return m_iaSel[0];

}


bool CLayerListView::is_add_enabled()
{

   return true;

}


bool CLayerListView::is_delete_enabled()
{

   return get_selected_count() >= 1 && GetDocument()->get_item_count() >= 2;

}


bool CLayerListView::is_rename_enabled()
{

   return get_selected_count() == 1;

}


bool CLayerListView::is_show_enabled()
{

   return get_selected_not_show(layer_show_visible) >= 1;

}

bool CLayerListView::is_gray_enabled()
{

   return get_selected_not_show(layer_show_grayed) >= 1;

}

bool CLayerListView::is_hide_enabled()
{

   return get_selected_not_show(layer_show_hidden) >= 1;

}



bool CLayerListView::is_merge_enabled()
{

   return get_selected_count() >= 2;

}


void CLayerListView::get_item_rect(LPRECT lprect,int iItem)
{

   GetClientRect(lprect);

   lprect->top += iItem * m_iItemHeight;
   lprect->bottom = lprect->top + m_iItemHeight;

}


void CLayerListView::get_subitem_rect(LPRECT lprect,int iItem,e_layer_subitem esubitem, bool bExternal)
{
   
   get_item_rect(lprect,iItem);

   for(int i = 0; i < esubitem; i++)
   {
      
      lprect->left += get_subitem_width((e_layer_subitem)i);

   }

   lprect->right = lprect->left + get_subitem_width(esubitem);

   if(!bExternal)
   {

      if(esubitem == layer_subitem_state)
      {

         lprect->left = ((lprect->right - lprect->left) - m_bitmapaState[0]->GetWidth()) / 2 + lprect->left;
         lprect->top = ((lprect->bottom - lprect->top) - m_bitmapaState[0]->GetHeight()) / 2 + lprect->top;
         lprect->right = lprect->left + m_bitmapaState[0]->GetWidth();
         lprect->bottom = lprect->top + m_bitmapaState[0]->GetHeight();

      }
      else if(esubitem == layer_subitem_show)
      {

         lprect->left = ((lprect->right - lprect->left) - m_bitmapaShow[0]->GetWidth()) / 2 + lprect->left;
         lprect->top = ((lprect->bottom - lprect->top) - m_bitmapaShow[0]->GetHeight()) / 2 + lprect->top;
         lprect->right = lprect->left + m_bitmapaShow[0]->GetWidth();
         lprect->bottom = lprect->top + m_bitmapaShow[0]->GetHeight();

      }

   }

}


int CLayerListView::get_subitem_width(e_layer_subitem esubitem)
{

   if(esubitem == layer_subitem_state)
   {

      return m_bitmapaState[0]->GetWidth() + 4;

   }
   else if(esubitem == layer_subitem_show)
   {

      return m_bitmapaShow[0]->GetWidth() + 4;

   }
   else if(esubitem == layer_subitem_scale)
   {

      return 80;

   }
   else if(esubitem == layer_subitem_count)
   {

      return 45;

   }
   else
   {

      CRect rect;

      GetClientRect(rect);

      return rect.Width()
         - get_subitem_width(layer_subitem_state)
         - get_subitem_width(layer_subitem_show)
         - get_subitem_width(layer_subitem_scale)
         - get_subitem_width(layer_subitem_count);

   }


}

void CLayerListView::OnSelchangeLayerList()
{
   
   //if(GetListCtrl().GetSelectedCount()>1)
   {
      //GetDlgItem(IDC_MERGE)->EnableWindow(TRUE);
      //GetDlgItem(IDC_RENAME)->EnableWindow(FALSE);
      //GetDlgItem(IDC_EDIT_LAYER)->EnableWindow(FALSE);
      //BOOL bEnabled;
      //int nGrayed=-1,nHidden=-1;
      //for(int i=0;i<GetListCtrl().GetItemCount();i++)
      //   if(GetListCtrl().GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
      //   {
      //      CLayer* pLayer=GetDocument()->m_pDoc->GetLayer(i);
      //      if(nGrayed<0)
      //         nGrayed=pLayer->m_bGrayed?1:0;
      //      else if(nGrayed != 2 && nGrayed != (pLayer->m_bGrayed?1:0))
      //         nGrayed=2;
      //      if(nHidden<0)
      //         nHidden=pLayer->m_bHidden?1:0;
      //      else if(nHidden != 2 && nHidden != (pLayer->m_bHidden?1:0))
      //         nHidden=2;
      //      bEnabled=pLayer != GetDocument()->m_pDoc->GetActiveLayer();
      //      if(!bEnabled)
      //      {
      //         nGrayed=0;
      //         nHidden=0;
      //         break;
      //      }
      //   }
      //CheckDlgButton(IDC_HIDDEN,nHidden);
      //CheckDlgButton(IDC_GRAYED,nGrayed);
      //GetDlgItem(IDC_HIDDEN)->EnableWindow(bEnabled);
      //GetDlgItem(IDC_GRAYED)->EnableWindow(bEnabled);
      //AfxSetWindowText(m_LayerName.GetSafeHwnd(),"");
      //m_LayerName.EnableWindow(FALSE);
      //m_Scale.SetCurSel(-1);
      //m_Scale.EnableWindow(FALSE);
   }
   //else
   {
      //GetListCtrl().EditLabel(

      //GetDlgItem(IDC_MERGE)->EnableWindow(FALSE);
      //GetDlgItem(IDC_RENAME)->EnableWindow(TRUE);
      //GetDlgItem(IDC_EDIT_LAYER)->EnableWindow(TRUE);
      //m_LayerName.EnableWindow(TRUE);
      //CLayer* pLayer=GetDocument()->m_pDoc->GetLayer(GetListCtrl().GetCaretIndex());
      //BOOL bEnabled=pLayer != GetDocument()->m_pDoc->GetActiveLayer();
      //GetDlgItem(IDC_HIDDEN)->EnableWindow(bEnabled);
      //GetDlgItem(IDC_GRAYED)->EnableWindow(bEnabled);
      //CheckDlgButton(IDC_HIDDEN,(bEnabled && pLayer->m_bHidden)?1:0);
      //CheckDlgButton(IDC_GRAYED,(bEnabled && pLayer->m_bGrayed)?1:0);
      //AfxSetWindowText(m_LayerName.GetSafeHwnd(),pLayer->m_strName);
      //m_Scale.EnableWindow();
      //if(m_Scale.GetCurSel() != pLayer->m_nScale - 1)	m_Scale.SetCurSel(pLayer->m_nScale - 1);
      //m_LayerName.SetSel(0,-1);
      //if(GetActiveWindow() == this)
      //   GotoDlgCtrl(&m_LayerName);
   }

}


void CLayerListView::OnLvnItemchanged(NMHDR *pNMHDR,LRESULT *pResult)
{
   LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
   // TODO: Add your control notification handler code here

   if(pNMLV->uChanged & LVIS_SELECTED)
   {


      OnSelchangeLayerList();

   }

   *pResult = 0;
}


int CLayerListView::get_selected_count()
{

   return (int) m_iaSel.GetCount();

}


CLayer * CLayerListView::GetActiveLayer()
{
   
   return GetDocument()->m_pDoc->GetActiveLayer();

}





void CLayerListView::set_selected(int iSel,bool bSel)
{

   if(iSel < 0)
   {

      return;

   }

   if(bSel)
   {
      
      m_iaSel.AddUnique(iSel);

   }
   else
   {
      
      m_iaSel.RemoveRenumber(iSel);

   }

}


void CLayerListView::unselect_all()
{

   m_iaSel.RemoveAll();

}


BOOL CLayerListView::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.dwExStyle = cs.dwExStyle & ~WS_EX_CLIENTEDGE;
   cs.style |= WS_VSCROLL;
   cs.style &= ~WS_HSCROLL;

   return CScrollView::PreCreateWindow(cs);
}


void CLayerListView::OnNcPaint()
{
   // TODO: Add your message handler code here
   // Do not call CScrollView::OnNcPaint() for painting messages
   CWindowDC dc(this);

   CRect r;
   GetWindowRect(&r);

   r.OffsetRect(-r.TopLeft());

   CPen penGray;
   penGray.CreatePen(PS_SOLID,1,GetSysColor(COLOR_BTNSHADOW));
   CPen penWhite;
   penWhite.CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOW));

   dc.MoveTo(r.left,r.top);
   dc.SelectObject(&penGray);
   dc.LineTo(r.right+1,r.top);
   dc.MoveTo(r.right - 1,r.top + 1);
   dc.SelectObject(&penWhite);
   dc.LineTo(r.right-1,r.bottom-2);
   dc.MoveTo(r.right - 1,r.bottom - 1);
   dc.SelectObject(&penGray);
   dc.LineTo(r.left-1,r.bottom-1);
   dc.SelectObject(&penWhite);
   dc.LineTo(r.left,r.bottom - 2);
   dc.LineTo(r.left,r.top);

}


void CLayerListView::OnActivateLayer(int iIndex)
{
 
   GetDocument()->m_pDoc->SetActiveLayer(iIndex);

   GetDocument()->m_pDoc->GetActiveLayer()->m_bHidden=false;

   GetDocument()->m_pDoc->GetActiveLayer()->m_bGrayed=false;

   GetDocument()->m_pDoc->LayersChanged();
   
   RedrawWindow();

}

void CLayerListView::OnLayerNextShow(int i)
{

   if(GetDocument()->m_pDoc->m_Layers[i] == GetDocument()->m_pDoc->GetActiveLayer())
   {

      GetDocument()->m_pDoc->m_Layers[i]->m_bHidden = false;

      GetDocument()->m_pDoc->m_Layers[i]->m_bGrayed = false;

   }
   else if(GetDocument()->m_pDoc->m_Layers[i]->m_bHidden)
   {

      GetDocument()->m_pDoc->m_Layers[i]->m_bHidden = false;

      GetDocument()->m_pDoc->m_Layers[i]->m_bGrayed = false;

   }
   else if(GetDocument()->m_pDoc->m_Layers[i]->m_bGrayed)
   {

      GetDocument()->m_pDoc->m_Layers[i]->m_bHidden = true;

      GetDocument()->m_pDoc->m_Layers[i]->m_bGrayed = false;

   }
   else
   {

      GetDocument()->m_pDoc->m_Layers[i]->m_bHidden = false;

      GetDocument()->m_pDoc->m_Layers[i]->m_bGrayed = true;

   }

   GetDocument()->m_pDoc->LayersChanged();

   RedrawWindow();

}

BOOL CLayerListView::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default
   return TRUE;
   return CScrollView::OnEraseBkgnd(pDC);
}



int CLayerListView::get_selected_not_show(e_layer_show eshow)
{

   int iCount = 0;

   for(int i = 0; i < m_iaSel.GetCount(); i++)
   {
      if(GetDocument()->m_pDoc->m_Layers[m_iaSel[i]] != GetDocument()->m_pDoc->GetActiveLayer())
      {
         
         if(GetDocument()->get_layer_show(m_iaSel[i]) != eshow)
         {

            iCount++;

         }

      }
   }

   return iCount;

}


void CLayerListView::OnTimer(UINT_PTR nIDEvent)
{
   // TODO: Add your message handler code here and/or call default
   if(nIDEvent == 1024)
   {
      CPoint pt;
      ::GetCursorPos(&pt);
      ScreenToClient(&pt);
      OnMouseMove(0,pt);
   }
   CScrollView::OnTimer(nIDEvent);
}


void CLayerListView::OnEditLayerName(int iItem)
{
   m_iEdit = iItem;
   CRect r;
   get_subitem_rect(r,iItem,layer_subitem_name, true);
   r -= GetDeviceScrollPosition();
   m_edit.SetWindowText(get_cp(get_uni(GetDocument()->get_layer_name(iItem)),CP_ACP));
   m_edit.SetWindowPos(&CWnd::wndTop,r.left,r.top,r.Width(),r.Height(),SWP_SHOWWINDOW);
   m_edit.ShowWindow(SW_SHOW);
   m_edit.SetFocus();
   m_edit.SetSel(0,-1);

}


void CLayerListView::OnDropDownLayerScale(int iItem)
{

   m_iCombo = iItem;

   CRect r;

   get_subitem_rect(r,iItem,layer_subitem_scale,true);

   r -= GetDeviceScrollPosition();

   UINT nScaleFirst,nScaleLast;

   if(GetDocument()->m_pDoc->m_bEnglishUnits)
   {
      nScaleFirst=IDS_ENGLISH_SCALE_FIRST;
      nScaleLast=IDS_ENGLISH_SCALE_LAST;
   }
   else
   {
      nScaleFirst=IDS_METRIC_SCALE_FIRST;
      nScaleLast=IDS_METRIC_SCALE_LAST;
   }

   m_combo.ResetContent();
   for(UINT i=nScaleFirst;i <= nScaleLast;i++)
   {
      CString str;
      VERIFY(str.LoadString(i));
      m_combo.AddString(str);
   }

   m_combo.SetWindowPos(&CWnd::wndTop,r.left,r.top,r.Width(),r.Height() * (nScaleLast-nScaleFirst+1),SWP_SHOWWINDOW);
   m_combo.ShowWindow(SW_SHOW);
   m_combo.SetFocus();
   m_combo.SetCurSel(GetDocument()->get_layer_scale(iItem));
   m_combo.ShowDropDown();
   m_combo.SetDroppedWidth(r.Height() * 3);

}
