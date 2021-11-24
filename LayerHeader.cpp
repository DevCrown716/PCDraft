// Layer - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "PCDraftDoc.h"
#include "Layer.h"
#include "LayerHeader.h"
#include "LayerFooter.h"
#include "LayerFrame.h"
#include "LayerListView.h"
#include "LayerDoc.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CLayerHeader, CFormView)


CLayerHeader::CLayerHeader()
	: CFormView(CLayerHeader::IDD)
{
   m_size.cx = 0;
   m_size.cy = 0;
   m_stMerge.m_bitmap.LoadBitmap(IDB_LAYER_MERGE);
   m_stMerge.m_bitmapSel.LoadBitmap(IDB_LAYER_MERGE_SEL);
   m_stMerge.m_bitmapDisabled.LoadBitmap(IDB_LAYER_MERGE_DISABLED);
   m_stAdd.m_bitmap.LoadBitmap(IDB_LAYER_ADD);
   m_stAdd.m_bitmapSel.LoadBitmap(IDB_LAYER_ADD_SEL);
   m_stDelete.m_bitmap.LoadBitmap(IDB_LAYER_DELETE);
   m_stDelete.m_bitmapSel.LoadBitmap(IDB_LAYER_DELETE_SEL);
   m_stDelete.m_bitmapDisabled.LoadBitmap(IDB_LAYER_DELETE_DISABLED);
   m_bitmapMenu.LoadBitmap(IDB_LAYER_MENU);
   m_bitmapMenuSel.LoadBitmap(IDB_LAYER_MENU_SEL);

}

CLayerHeader::~CLayerHeader()
{
}

void CLayerHeader::DoDataExchange(CDataExchange* pDX)
{
   CFormView::DoDataExchange(pDX);
   DDX_Control(pDX,ID_LAYER_MERGE,m_stMerge);
   DDX_Control(pDX,ID_LAYER_ADD,m_stAdd);
   DDX_Control(pDX,ID_LAYER_DELETE,m_stDelete);
   DDX_Control(pDX,ID_LAYER_MENU,m_stMenu);
}

BEGIN_MESSAGE_MAP(CLayerHeader, CFormView)
   ON_WM_SIZE()
   ON_STN_CLICKED(ID_LAYER_MERGE,&CLayerHeader::OnStnClickedLayerMerge)
   ON_STN_CLICKED(ID_LAYER_ADD,&CLayerHeader::OnStnClickedLayerAdd)
   ON_STN_CLICKED(ID_LAYER_DELETE,&CLayerHeader::OnStnClickedLayerDelete)
   ON_STN_CLICKED(ID_LAYER_MENU,&CLayerHeader::OnStnClickedLayerMenu)
   ON_WM_TIMER()
   ON_WM_CREATE()
   ON_WM_UNINITMENUPOPUP()
   ON_WM_HSCROLL()
   ON_EN_CHANGE(IDC_SEARCH,&CLayerHeader::OnChangeSearch)
END_MESSAGE_MAP()


// CLayerHeader diagnostics

#ifdef _DEBUG
void CLayerHeader::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLayerHeader::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLayerHeader message handlers


void CLayerHeader::OnSize(UINT nType,int cx,int cy)
{

   if(cx <= 0 || cy <= 0)
   {

      return;

   }
   
   //CFormView::OnSize(nType,cx,cy);

   if(m_size.cx <= 0 && m_size.cy <= 0)
   {

      m_size.cx = cx;

      m_size.cy = cy;

   }

   if(!IsWindow(m_stMenu.GetSafeHwnd()))
   {

      return;

   }

   CRect rectClient;

   GetClientRect(rectClient);

   CRect rectMenu;

   m_stMenu.GetWindowRect(rectMenu);

   ScreenToClient(rectMenu);

   int w = rectMenu.Width();

   rectMenu.right = rectClient.right - 10;
   rectMenu.left = rectMenu.right - w;

   m_stMenu.SetWindowPos(&CWnd::wndTop,rectMenu.left,rectMenu.top,rectMenu.Width(),rectMenu.Height(),SWP_NOSIZE);
}

CSize CLayerHeader::GetTotalSize() const
{

   return m_size;

}


void CLayerHeader::OnInitialUpdate()
{

   CFormView::OnInitialUpdate();

   EnableScrollBarCtrl(SB_BOTH,FALSE);

}


BOOL CLayerHeader::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.dwExStyle = 0;
   cs.style = WS_CHILD | WS_VISIBLE;

   return CFormView::PreCreateWindow(cs);
}

void CLayerHeader::OnStnClickedLayerMerge()
{

   GetDocument()->OnLayerMerge();

}


void CLayerHeader::OnStnClickedLayerAdd()
{
   
   GetDocument()->OnLayerAdd();

}


void CLayerHeader::OnStnClickedLayerDelete()
{

   GetDocument()->OnLayerDelete();

}

void CLayerHeader::OnStnClickedLayerMenu()
{
   
   if(m_stMenu.GetBitmap() == (HBITMAP)m_bitmapMenuSel.GetSafeHandle()
      || GetTickCount() - m_dwMenuClose < 300)
   {
      
      m_stMenu.SetBitmap((HBITMAP)m_bitmapMenu.GetSafeHandle());

   }
   else
   {

      m_stMenu.SetBitmap((HBITMAP)m_bitmapMenuSel.GetSafeHandle());
      if(m_menu.GetSafeHmenu() == NULL)
      {
         m_menu.LoadMenu(IDR_LAYER);
      }
      m_pmenu = m_menu.GetSubMenu(0);
      CRect rectWindow;
      m_stMenu.GetWindowRect(rectWindow);
      m_pmenu->TrackPopupMenu(0, rectWindow.left,rectWindow.top + rectWindow.Height() + 9,GetParentFrame());
   }
}


void CLayerHeader::OnTimer(UINT_PTR nIDEvent)
{

   CFormView::OnTimer(nIDEvent);

}


int CLayerHeader::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CFormView::OnCreate(lpCreateStruct) == -1)
      return -1;

   GetDocument()->m_playerheader = this;
   ((CLayerFrame *)GetParentFrame())->m_playerheader = this;
   
   return 0;
}


void CLayerHeader::OnUnInitMenuPopup(CMenu *pPopupMenu,UINT nFlags)
{
   // This feature requires Windows 2000 or greater.
   // The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
   // TODO: Add your message handler code here and/or call default
   if(m_pmenu == pPopupMenu)
   {
      m_stMenu.SetBitmap((HBITMAP)m_bitmapMenu.GetSafeHandle());
      m_dwMenuClose = GetTickCount();
   }

   CFormView::OnUnInitMenuPopup(pPopupMenu,nFlags);

}


void CLayerHeader::OnFileNewlibrary()
{
   
   //CLayerFrame * pframe = (CLayerFrame *) GetParentFrame();
   //pframe->OnFileNewlibrary();

}


void CLayerHeader::OnFileOpenlibrary()
{
   
   //CLayerFrame * pframe = (CLayerFrame *)GetParentFrame();
   //pframe->OnFileOpenlibrary();

}


void CLayerHeader::OnNMCustomdrawSlider(NMHDR *pNMHDR,LRESULT *pResult)
{
   
   LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
   
   if(pNMCD->hdr.code == TB_THUMBPOSITION || pNMCD->hdr.code ==  TB_THUMBTRACK)
   {
      
   }

   *pResult = 0;

}


void CLayerHeader::OnThumbposchangingSlider(NMHDR *pNMHDR,LRESULT *pResult)
{
   
   // This feature requires Windows Vista or greater.
   // The symbol _WIN32_WINNT must be >= 0x0600.
   
   NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
   
   // TODO: Add your control notification handler code here
   
   if(pNMTPC->hdr.code == TB_THUMBPOSITION || pNMTPC->hdr.code == TB_THUMBTRACK)
   {
    
   }

   *pResult = 0;

}


void CLayerHeader::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
   
   // TODO: Add your message handler code here and/or call default
   
   if(pScrollBar->GetDlgCtrlID() == IDC_SLIDER)
   {
   
      if(nSBCode == TB_THUMBPOSITION || nSBCode == TB_THUMBTRACK)
      {
      
         m_plist->SendMessage(WM_APP + 1025,0, nPos);
         
      }

   }

   CFormView::OnHScroll(nSBCode,nPos,pScrollBar);

}




void CLayerHeader::OnChangeSearch()
{
   
   // TODO:  If this is a RICHEDIT control, the control will not
   // send this notification unless you override the CFormView::OnInitDialog()
   // function and call CRichEditCtrl().SetEventMask()
   // with the ENM_CHANGE flag ORed into the mask.

   m_plist->SendMessage(WM_APP + 1025,1);

}



CLayerDoc * CLayerHeader::GetDocument()
{

   return (CLayerDoc *)CFormView::GetDocument();

}


void CLayerHeader::Refresh()
{

   m_stAdd.EnableWindow(GetDocument()->is_add_enabled());
   
   m_stDelete.EnableWindow(GetDocument()->is_delete_enabled());

   m_stMerge.EnableWindow(GetDocument()->is_merge_enabled());

}




CBtnStatic::CBtnStatic()
{

   m_bPressed = false;

}


CBtnStatic::~CBtnStatic()
{

}


BEGIN_MESSAGE_MAP(CBtnStatic,CStatic)
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_ENABLE()
END_MESSAGE_MAP()


void CBtnStatic::OnLButtonDown(UINT nFlags,CPoint point)
{
   // TODO: Add your message handler code here and/or call default
   SetCapture();
   m_bPressed = true;
   OnUpdateState();
   //CStatic::OnLButtonDown(nFlags,point);
}


void CBtnStatic::OnLButtonUp(UINT nFlags,CPoint point)
{
   // TODO: Add your message handler code here and/or call default
   CRect r;
   GetWindowRect(r);
   ScreenToClient(r);
   m_bPressed = false;
   OnUpdateState();
   if(r.PtInRect(point))
   {
      GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), BN_CLICKED),(LPARAM) GetSafeHwnd());
   }
   ReleaseCapture();
   CStatic::OnLButtonUp(nFlags,point);
}


void CBtnStatic::OnEnable(BOOL bEnable)
{
   CStatic::OnEnable(bEnable);

   OnUpdateState();
   
}


void CBtnStatic::OnUpdateState()
{

   if(!IsWindowEnabled() && m_bitmapDisabled.GetSafeHandle() != NULL)
   {

      SetBitmap((HBITMAP) m_bitmapDisabled.GetSafeHandle());

   }
   else if(m_bPressed  && m_bitmapSel.GetSafeHandle() != NULL)
   {

      SetBitmap((HBITMAP)m_bitmapSel.GetSafeHandle());

   }
   else
   {

      SetBitmap((HBITMAP)m_bitmap.GetSafeHandle());

   }

   RedrawWindow();

}