// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LibraryFooter.h"
#include "LibraryFrame.h"
#include "LibraryListView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CLibraryFooter

IMPLEMENT_DYNCREATE(CLibraryFooter, CFormView)

CLibraryFooter::CLibraryFooter()
	: CFormView(CLibraryFooter::IDD)
{
   m_bitmapOpen.LoadBitmap(IDB_LIBRARY_OPEN);
   m_bitmapOpenSel.LoadBitmap(IDB_LIBRARY_OPEN_SEL);
}

CLibraryFooter::~CLibraryFooter()
{
}

void CLibraryFooter::DoDataExchange(CDataExchange* pDX)
{
   CFormView::DoDataExchange(pDX);
   DDX_Control(pDX,IDC_STATIC_LESS,m_stLess);
   DDX_Control(pDX,IDC_STATIC_MORE,m_stMore);
   DDX_Control(pDX,IDC_SLIDER,m_slider);
   DDX_Control(pDX,IDC_LIBRARY_OPEN,m_stLibraryOpen);
   DDX_Control(pDX,IDC_SEARCH,m_search);
   DDX_Control(pDX,IDC_STATIC_COUNT,m_stCount);
}

BEGIN_MESSAGE_MAP(CLibraryFooter, CFormView)
   ON_WM_SIZE()
   ON_STN_CLICKED(IDC_LIBRARY_OPEN,&CLibraryFooter::OnStnClickedLibraryOpen)
   ON_WM_TIMER()
   ON_WM_CREATE()
   ON_WM_UNINITMENUPOPUP()
   ON_COMMAND(ID_FILE_NEWLIBRARY,&CLibraryFooter::OnFileNewlibrary)
   ON_COMMAND(ID_FILE_OPENLIBRARY,&CLibraryFooter::OnFileOpenlibrary)
   ON_NOTIFY(TRBN_THUMBPOSCHANGING,IDC_SLIDER,&CLibraryFooter::OnThumbposchangingSlider)
   ON_WM_HSCROLL()
   ON_EN_CHANGE(IDC_SEARCH,&CLibraryFooter::OnChangeSearch)
   ON_COMMAND(ID_FILE_RENAMEITEM,&CLibraryFooter::OnFileRenameitem)
   ON_UPDATE_COMMAND_UI(ID_FILE_RENAMEITEM,&CLibraryFooter::OnUpdateFileRenameitem)
   ON_WM_PAINT()
END_MESSAGE_MAP()


// CLibraryFooter diagnostics

#ifdef _DEBUG
void CLibraryFooter::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLibraryFooter::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLibraryFooter message handlers


void CLibraryFooter::OnSize(UINT nType,int cx,int cy)
{
   //CFormView::OnSize(nType,cx,cy);

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

   m_search.SetWindowPos(&CWnd::wndTop,rectLess.right + 45,36, rectClient.right * 4 /8,20,0);

   int iCountWidth = 60;
   m_stCount.SetWindowPos(&CWnd::wndTop,rectClient.Width() - 8 - iCountWidth,38,iCountWidth,20,0);


}


void CLibraryFooter::OnInitialUpdate()
{
   CFormView::OnInitialUpdate();

   EnableScrollBarCtrl(SB_BOTH,FALSE);

   m_search.SetBrowseButtonImage((HICON) ::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_SEARCH),IMAGE_ICON,16, 16,LR_LOADTRANSPARENT));

   m_slider.SetRange(0,100);



}


BOOL CLibraryFooter::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.dwExStyle = 0;
   cs.style = WS_CHILD | WS_VISIBLE;

   return CFormView::PreCreateWindow(cs);
}


void CLibraryFooter::OnStnClickedLibraryOpen()
{
   
   if(m_stLibraryOpen.GetBitmap() == (HBITMAP)m_bitmapOpenSel.GetSafeHandle()
      || GetTickCount() - m_dwMenuClose < 300)
   {
      
      m_stLibraryOpen.SetBitmap((HBITMAP)m_bitmapOpen.GetSafeHandle());

   }
   else
   {

      m_stLibraryOpen.SetBitmap((HBITMAP)m_bitmapOpenSel.GetSafeHandle());
      if(m_menu.GetSafeHmenu() == NULL)
      {
         m_menu.LoadMenu(IDR_LIBRARY);
      }
      m_pmenu = m_menu.GetSubMenu(0);
      CRect rectWindow;
      m_stLibraryOpen.GetWindowRect(rectWindow);
      m_pmenu->TrackPopupMenu(0, rectWindow.left,rectWindow.top + rectWindow.Height() + 9,this);
   }
}


void CLibraryFooter::OnTimer(UINT_PTR nIDEvent)
{

   CFormView::OnTimer(nIDEvent);

}


int CLibraryFooter::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CFormView::OnCreate(lpCreateStruct) == -1)
      return -1;
   
   return 0;
}


void CLibraryFooter::OnUnInitMenuPopup(CMenu *pPopupMenu,UINT nFlags)
{
   // This feature requires Windows 2000 or greater.
   // The symbols _WIN32_WINNT and WINVER must be >= 0x0500.
   // TODO: Add your message handler code here and/or call default
   if(m_pmenu == pPopupMenu)
   {
      m_stLibraryOpen.SetBitmap((HBITMAP)m_bitmapOpen.GetSafeHandle());
      m_dwMenuClose = GetTickCount();
   }

   CFormView::OnUnInitMenuPopup(pPopupMenu,nFlags);

}


void CLibraryFooter::OnFileNewlibrary()
{
   
   CLibraryFrame * pframe = (CLibraryFrame *) GetParentFrame();

   pframe->OnFileNewlibrary();

}


void CLibraryFooter::OnFileOpenlibrary()
{
   
   CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

   pframe->OnFileOpenlibrary();

}


void CLibraryFooter::OnNMCustomdrawSlider(NMHDR *pNMHDR,LRESULT *pResult)
{
   LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
   if(pNMCD->hdr.code == TB_THUMBPOSITION
      || pNMCD->hdr.code ==  TB_THUMBTRACK)
   {
      
   }
   *pResult = 0;
}

void CLibraryFooter::OnThumbposchangingSlider(NMHDR *pNMHDR,LRESULT *pResult)
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


void CLibraryFooter::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
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


void CLibraryFooter::OnChangeSearch()
{
   // TODO:  If this is a RICHEDIT control, the control will not
   // send this notification unless you override the CFormView::OnInitDialog()
   // function and call CRichEditCtrl().SetEventMask()
   // with the ENM_CHANGE flag ORed into the mask.

   m_plist->SendMessage(WM_APP + 1025,1);
}



void CLibraryFooter::OnFileRenameitem()
{

   CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

   pframe->OnFileRenameitem();

}


void CLibraryFooter::OnUpdateFileRenameitem(CCmdUI *pCmdUI)
{
   CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

   pframe->OnUpdateFileRenameitem(pCmdUI);
}




void CLibraryFooter::OnPaint()
{
   
   CPaintDC dc(this); 

   CRect rectClient;

   GetClientRect(rectClient);

   dc.FillSolidRect(rectClient,GetSysColor(COLOR_3DFACE));
   //dc.FillSolidRect(rectClient,RGB(0, 255, 0));

}
