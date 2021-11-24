// Layer - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LayerFooter.h"
#include "LayerFrame.h"
#include "LayerListView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CLayerFooter


IMPLEMENT_DYNCREATE(CLayerFooter, CFormView)


CLayerFooter::CLayerFooter()
	: CFormView(CLayerFooter::IDD)
{
   
   m_bitmapOpen.LoadBitmap(IDB_LIBRARY_OPEN);
   m_bitmapOpenSel.LoadBitmap(IDB_LIBRARY_OPEN_SEL);

}


CLayerFooter::~CLayerFooter()
{

}


void CLayerFooter::DoDataExchange(CDataExchange* pDX)
{

   CFormView::DoDataExchange(pDX);

   //DDX_Control(pDX,IDC_STATIC_LESS,m_stLess);
   //DDX_Control(pDX,IDC_STATIC_MORE,m_stMore);
   //DDX_Control(pDX,IDC_SLIDER,m_slider);
   //DDX_Control(pDX,IDC_LIBRARY_OPEN,m_stLayerOpen);
   //DDX_Control(pDX,IDC_SEARCH,m_search);
   //DDX_Control(pDX,IDC_STATIC_COUNT,m_stCount);

}


BEGIN_MESSAGE_MAP(CLayerFooter, CFormView)
   ON_WM_SIZE()
   ON_STN_CLICKED(IDC_LIBRARY_OPEN,&CLayerFooter::OnStnClickedLayerOpen)
   ON_WM_TIMER()
   ON_WM_CREATE()
   ON_WM_UNINITMENUPOPUP()
   ON_COMMAND(ID_FILE_NEWLIBRARY,&CLayerFooter::OnFileNewlibrary)
   ON_COMMAND(ID_FILE_OPENLIBRARY,&CLayerFooter::OnFileOpenlibrary)
   ON_NOTIFY(TRBN_THUMBPOSCHANGING,IDC_SLIDER,&CLayerFooter::OnThumbposchangingSlider)
   ON_WM_HSCROLL()
   ON_EN_CHANGE(IDC_SEARCH,&CLayerFooter::OnChangeSearch)
   ON_COMMAND(ID_FILE_RENAMEITEM,&CLayerFooter::OnFileRenameitem)
   ON_UPDATE_COMMAND_UI(ID_FILE_RENAMEITEM,&CLayerFooter::OnUpdateFileRenameitem)
END_MESSAGE_MAP()


// CLayerFooter diagnostics

#ifdef _DEBUG
void CLayerFooter::AssertValid() const
{
	CFormView::AssertValid();
}

void CLayerFooter::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CLayerFooter message handlers


void CLayerFooter::OnSize(UINT nType,int cx,int cy)
{

   CRect rectClient;

   GetClientRect(rectClient);

   if(!IsWindow(m_stLess.GetSafeHwnd()) || !IsWindow(m_stMore.GetSafeHwnd()) || !IsWindow(m_slider.GetSafeHwnd()))
   {

      return;

   }

   CRect rectLess;
   m_stLess.SetWindowPos(&CWnd::wndTop,8,12,0,0,SWP_NOSIZE);
   m_stLess.GetWindowRect(rectLess);
   ScreenToClient(rectLess);

   CRect rectMore;
   m_stMore.GetWindowRect(rectMore);
   m_stMore.SetWindowPos(&CWnd::wndTop,rectClient.Width() - 8 - rectMore.Width(),12 - (rectMore.Height() - rectLess.Height()) /2,0,0,SWP_NOSIZE);
   m_stMore.GetWindowRect(rectMore);
   ScreenToClient(rectMore);

   m_slider.SetWindowPos(&CWnd::wndTop,rectLess.right + 8,8,rectMore.left - 8- rectLess.right - 8,20,0);

   m_search.SetWindowPos(&CWnd::wndTop,rectLess.right + 45,36, rectClient.right * 4 /7,20,0);

   int iCountWidth = 40;
   m_stCount.SetWindowPos(&CWnd::wndTop,rectClient.Width() - 8 - iCountWidth,38,iCountWidth,20,0);

}


void CLayerFooter::OnInitialUpdate()
{

   CFormView::OnInitialUpdate();

   //EnableScrollBarCtrl(SB_BOTH,FALSE);

   //m_search.SetBrowseButtonImage((HICON) ::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_SEARCH),IMAGE_ICON,16, 16,LR_LOADTRANSPARENT));

   //m_slider.SetRange(0,100);

}


BOOL CLayerFooter::PreCreateWindow(CREATESTRUCT& cs)
{

   cs.dwExStyle = 0;

   cs.style = WS_CHILD | WS_VISIBLE;

   return CFormView::PreCreateWindow(cs);

}


void CLayerFooter::OnStnClickedLayerOpen()
{
   
   if(m_stLayerOpen.GetBitmap() == (HBITMAP)m_bitmapOpenSel.GetSafeHandle()
      || GetTickCount() - m_dwMenuClose < 300)
   {
      
      m_stLayerOpen.SetBitmap((HBITMAP)m_bitmapOpen.GetSafeHandle());

   }
   else
   {

      m_stLayerOpen.SetBitmap((HBITMAP)m_bitmapOpenSel.GetSafeHandle());
      if(m_menu.GetSafeHmenu() == NULL)
      {
         m_menu.LoadMenu(IDR_LIBRARY);
      }
      m_pmenu = m_menu.GetSubMenu(0);
      CRect rectWindow;
      m_stLayerOpen.GetWindowRect(rectWindow);
      m_pmenu->TrackPopupMenu(0, rectWindow.left,rectWindow.top + rectWindow.Height() + 9,this);
   }
}


void CLayerFooter::OnTimer(UINT_PTR nIDEvent)
{

   CFormView::OnTimer(nIDEvent);

}


int CLayerFooter::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CFormView::OnCreate(lpCreateStruct) == -1)
      return -1;
   
   return 0;
}


void CLayerFooter::OnUnInitMenuPopup(CMenu *pPopupMenu,UINT nFlags)
{
   // This feature requires Windows 2000 or greater.
   // The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
   // TODO: Add your message handler code here and/or call default
   if(m_pmenu == pPopupMenu)
   {
      m_stLayerOpen.SetBitmap((HBITMAP)m_bitmapOpen.GetSafeHandle());
      m_dwMenuClose = GetTickCount();
   }

   CFormView::OnUnInitMenuPopup(pPopupMenu,nFlags);

}


void CLayerFooter::OnFileNewlibrary()
{
   
//   CLayerFrame * pframe = (CLayerFrame *) GetParentFrame();

//   pframe->OnFileNewlibrary();

}


void CLayerFooter::OnFileOpenlibrary()
{
   
//   CLayerFrame * pframe = (CLayerFrame *)GetParentFrame();

//   pframe->OnFileOpenlibrary();

}


void CLayerFooter::OnNMCustomdrawSlider(NMHDR *pNMHDR,LRESULT *pResult)
{
   LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
   if(pNMCD->hdr.code == TB_THUMBPOSITION
      || pNMCD->hdr.code ==  TB_THUMBTRACK)
   {
      
   }
   *pResult = 0;
}

void CLayerFooter::OnThumbposchangingSlider(NMHDR *pNMHDR,LRESULT *pResult)
{
   // This feature requires Windows Vista or greater.
   // The symbol _WIN32_WINNT must be >= 0x0600.
   NMTRBTHUMBPOSCHANGING *pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);
   // TODO: Add your control notification handler code here
   if(pNMTPC->hdr.code == TB_THUMBPOSITION
      || pNMTPC->hdr.code == TB_THUMBTRACK)
   {
    
   }
   *pResult = 0;
}


void CLayerFooter::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
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


void CLayerFooter::OnChangeSearch()
{
   // TODO:  If this is a RICHEDIT control, the control will not
   // send this notification unless you override the CFormView::OnInitDialog()
   // function and call CRichEditCtrl().SetEventMask()
   // with the ENM_CHANGE flag ORed into the mask.

   m_plist->SendMessage(WM_APP + 1025,1);
}



void CLayerFooter::OnFileRenameitem()
{

//   CLayerFrame * pframe = (CLayerFrame *)GetParentFrame();

//   pframe->OnFileRenameitem();

}


void CLayerFooter::OnUpdateFileRenameitem(CCmdUI *pCmdUI)
{

//   CLayerFrame * pframe = (CLayerFrame *)GetParentFrame();

//   pframe->OnUpdateFileRenameitem(pCmdUI);

}
