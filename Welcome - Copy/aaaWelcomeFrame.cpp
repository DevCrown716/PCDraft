// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "../PCDraft.h"
#include "WelcomeFrame.h"

void copy(Gdiplus::Rect & r, LPCRECT lpcrect);
void copy(Gdiplus::RectF & r, LPCRECT lpcrect);


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


BOOL AFXAPI AfxIsDescendant(HWND hWndParent, HWND hWndChild);

HWND AFXAPI AfxGetParentOwner(HWND hWnd);

void get_subdir(CString str, CStringArray & straPath, CStringArray & straName);
void get_files(CString str, CStringArray & straPath, CStringArray & straName);
bool has_subdir(CString str);



IMPLEMENT_DYNCREATE(CWelcomeFrame,CFrameWnd)

CWelcomeFrame::CWelcomeFrame()
{
   //m_bResizeGrip = true;
   //m_bResizable = true;
   m_bTracking = false;
   m_ulRef = 1;
}

CWelcomeFrame::~CWelcomeFrame()
{
}


BEGIN_MESSAGE_MAP(CWelcomeFrame, CFrameWnd)
   ON_WM_CREATE()
   ON_WM_NCCREATE()
   ON_WM_ACTIVATE()
   ON_WM_NCACTIVATE()
   ON_WM_NCLBUTTONDBLCLK()
   ON_WM_NCLBUTTONDOWN()
   ON_WM_NCLBUTTONUP()
   ON_WM_GETMINMAXINFO()
   ON_MESSAGE(WM_USER + 123, &CWelcomeFrame::OnResize)
   ON_WM_CLOSE()
   ON_WM_WINDOWPOSCHANGED()
   ON_WM_SIZE()
   ON_MESSAGE(WM_DWMNCRENDERINGCHANGED, OnDwm) // Interface Update - Infinisys Ltd.
   ON_WM_PAINT()
END_MESSAGE_MAP()

//void CWelcomeFrame::RecalcLayout(BOOL bNotify)
//{
//   CFrameWnd::RecalcLayout(bNotify);
//}
// CWelcomeFrame message handlers


BOOL CWelcomeFrame::PreCreateWindow(CREATESTRUCT& cs)
{
   // TODO: Add your specialized code here and/or call the base class
   if(!CFrameWnd::PreCreateWindow(cs))
      return FALSE;
   //cs.dwExStyle &= ~WS_EX_TOOLWINDOW;
   //cs.dwExStyle |= WS_EX_COMPOSITED;
   //cs.style |= MFS_SYNCACTIVE;
   // cs.hwndParent = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
   return TRUE;

}
Gdiplus::Bitmap * LoadPNG(LPCTSTR pName,LPCTSTR pType,HMODULE hInst);
//BOOL CWelcomeFrame::CreateEx(DWORD dwExStyle,LPCTSTR lpClassName,
//   LPCTSTR lpszWindowName,DWORD dwStyle,const RECT& rect,
//   CWnd* pParentWnd,UINT nID)
//{
//   // set m_bInRecalcLayout to avoid flashing during creation
//   // RecalcLayout will be called once something is docked
//   m_bInRecalcLayout = TRUE;
//
//   dwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | MFS_MOVEFRAME |
//      MFS_4THICKFRAME | MFS_SYNCACTIVE | MFS_BLOCKSYSMENU |
//
//      FWS_SNAPTOBARS;
//
//   DWORD dwBarStyle = 0;
//
//   if(dwBarStyle & CBRS_SIZE_DYNAMIC)
//      dwStyle &= ~MFS_MOVEFRAME;
//
////   DWORD dwExStyle = 0;
//   if(!CFrameWnd::CreateEx(dwExStyle,lpClassName ? lpClassName :
//      AfxRegisterWndClass(CS_DBLCLKS,::LoadCursor(NULL,IDC_ARROW)),
//      lpszWindowName,dwStyle,rect,pParentWnd,(UINT_PTR)nID))
//   {
//      return false;
//   }
//   dwStyle = dwBarStyle & (CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT) ?
//   CBRS_ALIGN_LEFT : CBRS_ALIGN_TOP;
//   dwStyle |= dwBarStyle & CBRS_FLOAT_MULTI;
//
//   CMenu* pSysMenu = GetSystemMenu(FALSE);
//   if(pSysMenu)
//   {
//      pSysMenu->DeleteMenu(SC_SIZE,MF_BYCOMMAND);
//      pSysMenu->DeleteMenu(SC_MINIMIZE,MF_BYCOMMAND);
//      pSysMenu->DeleteMenu(SC_MAXIMIZE,MF_BYCOMMAND);
//      pSysMenu->DeleteMenu(SC_RESTORE,MF_BYCOMMAND);
//
//      CString strHide;
//      if(strHide.LoadString(AFX_IDS_HIDE))
//      {
//         pSysMenu->DeleteMenu(SC_CLOSE,MF_BYCOMMAND);
//         pSysMenu->AppendMenu(MF_STRING | MF_ENABLED,SC_CLOSE,strHide);
//      }
//   }
//
//   // must initially create with parent frame as parent
//   //if(!m_wndDockBar.Create(pParent,WS_CHILD | WS_VISIBLE | dwStyle,
//   if(!m_wndDockBar.Create(pParentWnd,WS_CHILD  | dwStyle,
//      AFX_IDW_DOCKBAR_FLOAT))
//   {
//      m_bInRecalcLayout = FALSE;
//      return FALSE;
//   }
//
//   // set parent to CMiniDockFrameWnd
//   m_wndDockBar.SetParent(this);
//   m_bInRecalcLayout = FALSE;
//
//   return TRUE;
//
//}

int CWelcomeFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CFrameWnd::OnCreate(lpCreateStruct) == -1)
      return -1;
   //ModifyStyle(0,MFS_SYNCACTIVE | WS_THICKFRAME);

   ModifyStyle(0, WS_THICKFRAME);
   
   EnableDocking(CBRS_ALIGN_TOP);
   //if(m_toolbar.CreateEx(this,TBSTYLE_FLAT | TBSTYLE_LIST,WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
   //{
   //   Gdiplus::Bitmap * p = LoadPNG(MAKEINTRESOURCEA(IDR_LIBRARY),"PNG",NULL);
   //   if(p != NULL)
   //   {
   //      HBITMAP h = NULL;
   //      p->GetHBITMAP(Gdiplus::Color(0,0,0,0),& h);
   //      if(m_toolbar.SetBitmap(h))
   //      {

   //         UINT ui[]=
   //         {
   //            ID_VIEW_LIBRARY,
   //            ID_VIEW_PHOTO_LIBRARY
   //         };

   //         m_toolbar.SetSizes(CSize(39,38),CSize(32,32));

   //         if(m_toolbar.SetButtons(ui,sizeof(ui) / sizeof(ui[0])))
   //         {
   //            m_toolbar.SetButtonText(0,"Microspot Data");
   //            m_toolbar.SetButtonText(1,"Photos");

   //            m_toolbar.EnableDocking(CBRS_ALIGN_TOP);
   //            ShowControlBar(&m_toolbar,true,true);

   //         }

   //      }

   //      delete p;

   //   }

   //}


   return 0;
}


void CWelcomeFrame::OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized)
{
   CFrameWnd::OnActivate(nState,pWndOther,bMinimized);
   //CWnd::OnActivate(nState,pWndOther,bMinimized);

   //// hide the menu bar
   //if(nState == WA_INACTIVE && (m_dwMenuBarVisibility & AFX_MBV_KEEPVISIBLE) == 0)
   //{
   //   SetMenuBarState(AFX_MBS_HIDDEN);
   //}

   //// get top level frame unless this is a child window
   //// determine if window should be active or not
   //CFrameWnd* pTopLevel = (GetStyle() & WS_CHILD) ? this : m_pMainFrame;
   //ENSURE_VALID(pTopLevel);
   //CWnd* pActive = (nState == WA_INACTIVE ? pWndOther : this);
   //BOOL bStayActive = (pActive != NULL) ?
   //   (pTopLevel == pActive ||
   //   (pTopLevel == m_pMainFrame &&
   //   (pActive == pTopLevel ||
   //   pActive->SendMessage(WM_FLOATSTATUS,FS_SYNCACTIVE) != 0)))
   //   : FALSE;
   //pTopLevel->m_nFlags &= ~WF_STAYACTIVE;
   //if(bStayActive)
   //   pTopLevel->m_nFlags |= WF_STAYACTIVE;

   //// sync floating windows to the new state
   //NotifyFloatingWindows(bStayActive ? FS_ACTIVATE : FS_DEACTIVATE);

   //// get active view (use active frame if no active view)
   ////CView* pActiveView = GetActiveView();
   ////if(pActiveView == NULL)
   ////   pActiveView = GetActiveFrame()->GetActiveView();

   ////// when frame gets activated, re-activate current view
   ////if(pActiveView != NULL)
   ////{
   ////   if(nState != WA_INACTIVE && !bMinimized)
   ////      pActiveView->OnActivateView(TRUE,pActiveView,pActiveView);

   ////   // always notify the view of frame activations
   ////   pActiveView->OnActivateFrame(nState,this);
   ////}

}


BOOL CWelcomeFrame::OnNcActivate(BOOL bActive)
{
   // TODO: Add your message handler code here and/or call default
   //if(m_bTracking)
   //{
     // CFrameWnd::OnNcActivate(bActive);
//   }
   //else
   {
      return CFrameWnd::OnNcActivate(bActive);

   }
}
BOOL CWelcomeFrame::OnNcCreate(LPCREATESTRUCT lpcs)
{
   if(!CFrameWnd::OnNcCreate(lpcs))
      return FALSE;

   //if(GetStyle() & MFS_SYNCACTIVE)
   //{
   //   // syncronize activation state with top level parent
   //   CWnd* pParentWnd = AfxGetApp()->m_pMainWnd;
   //   CWnd* pActiveWnd = GetForegroundWindow();
   //   BOOL bActive = (pParentWnd == pActiveWnd) ||
   //      (pParentWnd->GetLastActivePopup() == pActiveWnd &&
   //      pActiveWnd->SendMessage(WM_FLOATSTATUS,FS_SYNCACTIVE) != 0);

   //   // the WM_FLOATSTATUS does the actual work
   //   SendMessage(WM_FLOATSTATUS,bActive ? FS_ACTIVATE : FS_DEACTIVATE);
   //}

   return TRUE;
}


//void CWelcomeFrame::NotifyFloatingWindows(DWORD dwFlags)
//{
//
//   //CFrameWnd::NotifyFloatingWindows(dwFlags);
//   //ASSERT_VALID(this);
//   //ASSERT(m_hWnd != NULL);
//
//   //// get top level parent frame window first unless this is a child window
//   //CFrameWnd* pParent = (GetStyle() & WS_CHILD) ? this : m_pMainFrame;
//   //ENSURE(pParent != NULL);
//   //if(dwFlags & (FS_DEACTIVATE | FS_ACTIVATE))
//   //{
//   //   // update parent window activation state
//   //   BOOL bActivate = !(dwFlags & FS_DEACTIVATE);
//   //   BOOL bEnabled = pParent->IsWindowEnabled();
//
//   //   if(bActivate && bEnabled && pParent != this)
//   //   {
//   //      // Excel will try to Activate itself when it receives a
//   //      // WM_NCACTIVATE so we need to keep it from doing that here.
//   //      m_nFlags |= WF_KEEPMINIACTIVE;
//   //      pParent->SendMessage(WM_NCACTIVATE,TRUE);
//   //      m_nFlags &= ~WF_KEEPMINIACTIVE;
//   //   }
//   //   else
//   //   {
//   //      pParent->SendMessage(WM_NCACTIVATE,FALSE);
//   //   }
//   //}
//
//   //// then update the state of all floating windows owned by the parent
//   //HWND hWnd = ::GetWindow(::GetDesktopWindow(),GW_CHILD);
//   //while(hWnd != NULL)
//   //{
//   //   if(AfxIsDescendant(pParent->m_hWnd,hWnd))
//   //      ::SendMessage(hWnd,WM_FLOATSTATUS,dwFlags,0);
//   //   hWnd = ::GetWindow(hWnd,GW_HWNDNEXT);
//   //}
//}


void CWelcomeFrame::OnNcLButtonDown(UINT nHitTest,CPoint point)
{
   //if(nHitTest == HTCAPTION)
   //{
   //   // special activation for floating toolbars
   //   ActivateTopParent();

   //   // initiate toolbar drag for non-CBRS_FLOAT_MULTI toolbars
   //   //if((m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0)
   //   //{
   //   //   int nPos = 1;
   //   //   CControlBar* pBar = NULL;
   //   //   while(pBar == NULL && nPos < m_wndDockBar.m_arrBars.GetSize())
   //   //      pBar = m_wndDockBar.GetDockedControlBar(nPos++);

   //   //   ENSURE_VALID(pBar);
   //   //   ASSERT_KINDOF(CControlBar,pBar);
   //   //   ENSURE(pBar->m_pDockContext != NULL);
   //   //   pBar->m_pDockContext->StartDrag(point);
   //   //   return;
   //   //}
   //}
   //else if(nHitTest >= HTSIZEFIRST && nHitTest <= HTSIZELAST)
   //{
   //   // special activation for floating toolbars
   //   ActivateTopParent();

   //   //int nPos = 1;
   //   //CControlBar* pBar = NULL;
   //   //while(pBar == NULL && nPos < m_wndDockBar.m_arrBars.GetSize())
   //   //   pBar = m_wndDockBar.GetDockedControlBar(nPos++);

   //   //ENSURE_VALID(pBar);
   //   //ASSERT_KINDOF(CControlBar,pBar);
   //   //ENSURE(pBar->m_pDockContext != NULL);

   //   //// CBRS_SIZE_DYNAMIC toolbars cannot have the CBRS_FLOAT_MULTI style
   //   //ASSERT((m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0);
   //   //pBar->m_pDockContext->StartResize(nHitTest,point);
   //   return;
   //}
   m_bTracking = true;
   CFrameWnd::OnNcLButtonDown(nHitTest,point);

   //Default();
}

void CWelcomeFrame::OnNcLButtonDblClk(UINT nHitTest,CPoint point)
{
   if(nHitTest == HTCAPTION)
   {
      // special activation for floating toolbars
      ActivateTopParent();

      // initiate toolbar toggle for non-CBRS_FLOAT_MULTI toolbars
      //if((m_wndDockBar.m_dwStyle & CBRS_FLOAT_MULTI) == 0)
      //{
      //   int nPos = 1;
      //   CControlBar* pBar = NULL;
      //   while(pBar == NULL && nPos < m_wndDockBar.m_arrBars.GetSize())
      //      pBar = m_wndDockBar.GetDockedControlBar(nPos++);

      //   ENSURE_VALID(pBar);
      //   ASSERT_KINDOF(CControlBar,pBar);
      //   ENSURE(pBar->m_pDockContext != NULL);
      //   pBar->m_pDockContext->ToggleDocking();
      //   return;
      //}
   }
   CFrameWnd::OnNcLButtonDblClk(nHitTest,point);
}

BOOL CWelcomeFrame::Create(CWnd* pParent,DWORD dwBarStyle, PVOID p)
{
   // set m_bInRecalcLayout to avoid flashing during creation
   // RecalcLayout will be called once something is docked
   //m_bInRecalcLayout = TRUE;

   //DWORD dwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | MFS_MOVEFRAME |
   //   MFS_4THICKFRAME | MFS_SYNCACTIVE | MFS_BLOCKSYSMENU |
   //   FWS_SNAPTOBARS;
   //DWORD dwStyle = WS_POPUP |  WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU |  MFS_SYNCACTIVE |
     // WS_THICKFRAME;
   DWORD dwStyle = WS_POPUP | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME;

   //if(dwBarStyle & CBRS_SIZE_DYNAMIC)
   //   dwStyle &= ~MFS_MOVEFRAME;

   DWORD dwExStyle = 0;
   //m_strCaption.LoadString(IDS_LIBRARY);

   auto pWndClass = AfxRegisterWndClass(CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW));

   CRect rect;

   rect.left = 100;
   rect.top = 100;
   rect.right = 1500;
   rect.bottom = 900;

   if(!CWnd::CreateEx(dwExStyle,pWndClass, "",dwStyle,rect,pParent,(UINT_PTR)0,p))
   {
      return FALSE;
   }

   dwStyle = dwBarStyle & (CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT) ?
   CBRS_ALIGN_LEFT : CBRS_ALIGN_TOP;
   dwStyle |= dwBarStyle & CBRS_FLOAT_MULTI;

   //CMenu* pSysMenu = GetSystemMenu(FALSE);
   //if(pSysMenu)
   //{
   //   pSysMenu->DeleteMenu(SC_SIZE,MF_BYCOMMAND);
   //   pSysMenu->DeleteMenu(SC_MINIMIZE,MF_BYCOMMAND);
   //   pSysMenu->DeleteMenu(SC_MAXIMIZE,MF_BYCOMMAND);
   //   pSysMenu->DeleteMenu(SC_RESTORE,MF_BYCOMMAND);

   //   CString strHide;
   //   if(strHide.LoadString(AFX_IDS_HIDE))
   //   {
   //      pSysMenu->DeleteMenu(SC_CLOSE,MF_BYCOMMAND);
   //      pSysMenu->AppendMenu(MF_STRING | MF_ENABLED,SC_CLOSE,strHide);
   //   }
   //}

   //// must initially create with parent frame as parent
   //if(!m_wndDockBar.Create(pParent,WS_CHILD | WS_VISIBLE | dwStyle,
   //   AFX_IDW_DOCKBAR_FLOAT))
   //{
   //   m_bInRecalcLayout = FALSE;
   //   return FALSE;
   //}

   //// set parent to CMiniDockFrameWnd
   //m_wndDockBar.SetParent(this);
   //m_bInRecalcLayout = FALSE;

   return TRUE;
}

BOOL CWelcomeFrame::LoadFrame(UINT nIDResource,DWORD dwDefaultStyle,
   CWnd* pParentWnd,CCreateContext* pContext)
{

   //return CFrameWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
   // only do this once
   ASSERT_VALID_IDR(nIDResource);
   ASSERT(m_nIDHelp == 0 || m_nIDHelp == nIDResource);

   m_nIDHelp = nIDResource;    // ID for help context (+HID_BASE_RESOURCE)

   CString strFullString;
   if(strFullString.LoadString(nIDResource))
      AfxExtractSubString(m_strTitle,strFullString,0);    // first sub-string

//   VERIFY(AfxDeferRegisterClass(AFX_WNDFRAMEORVIEW_REG));

   // attempt to create the window
   LPCTSTR lpszClass = GetIconWndClass(dwDefaultStyle,nIDResource);
   CString strTitle = m_strTitle;
   if(!Create(AfxGetApp()->m_pMainWnd, 0, pContext))
   {
      return FALSE;   // will self destruct on failure normally
   }

   // save the default menu handle
   ASSERT(m_hWnd != NULL);
   m_hMenuDefault = m_dwMenuBarState == AFX_MBS_VISIBLE ? ::GetMenu(m_hWnd) : m_hMenu;

   // load accelerator resource
   LoadAccelTable(ATL_MAKEINTRESOURCE(nIDResource));

   if(pContext == NULL)   // send initial update
      SendMessageToDescendants(WM_INITIALUPDATE,0,0,TRUE,TRUE);

   return TRUE;
}


void CWelcomeFrame::OnNcLButtonUp(UINT nHitTest,CPoint point)
{
   CFrameWnd::OnNcLButtonUp(nHitTest,point);

   m_bTracking = false;
}


void CWelcomeFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
   // TODO: Add your message handler code here and/or call default

   CFrameWnd::OnGetMinMaxInfo(lpMMI);

   //lpMMI->ptMinTrackSize.x = 320;
   //lpMMI->ptMinTrackSize.y = 450;
}

struct BrowseWindowHandles {
   HWND hwndTitle;
   HWND hwndStatic;  // not sure what this is
   HWND hwndStatic2;  // not sure what this is
   HWND hwndTree;
   HWND hwndOK;
   HWND hwndCancel;
};




HRESULT STDMETHODCALLTYPE CWelcomeFrame::QueryInterface(const IID & iid,void ** p)
{

   if(iid == IID_IUnknown)
   {
      
      *p = (IUnknown *) this;
      return S_OK;
   }
   else if(iid == IID_IFolderFilter)
   {

      *p = (IFolderFilter *) this;
      return S_OK;
   }
   return E_FAIL;
}


ULONG STDMETHODCALLTYPE CWelcomeFrame::AddRef()
{

   return ++m_ulRef;

}


ULONG STDMETHODCALLTYPE CWelcomeFrame::Release()
{
   
   return --m_ulRef;

}


HRESULT CWelcomeFrame::GetEnumFlags(IShellFolder * psf, PCIDLIST_ABSOLUTE pidlFolder, HWND * phwnd, DWORD * pgrfFlags)
{
   *pgrfFlags = SHCONTF_FOLDERS;
   
   return S_OK;
}


HRESULT CWelcomeFrame::ShouldShow(IShellFolder * psf,PCIDLIST_ABSOLUTE pidlFolder,PCUITEMID_CHILD pidlItem)
{
   IShellItem *psi;

   HRESULT hr = SHCreateItemWithParent(NULL,psf,pidlItem,IID_PPV_ARGS(&psi));
   if(SUCCEEDED(hr))
   {
      // Determine here whether the item should be shown. This determination
      // is application-dependent.
      LPWSTR lpwstr = NULL;
      psi->GetDisplayName(SIGDN_FILESYSPATH,&lpwstr);
      CString str = get_utf8(lpwstr);
      CoTaskMemFree(lpwstr);

      if(m_bOpenLibrary)
      {

         CString strLo(str);

         //strLo.MakeLower();

         const char * pszFind = ".spotlibrary";

         auto iLenFind = strlen(pszFind);

         int iFind = strLo.Find(pszFind);

         if(iFind >= 0 && strLo.GetLength() > iFind + iLenFind + 1)
         {
            return S_FALSE;

         }

         if(!has_subdir(str))
         {

            return S_FALSE;

         }

      }
      else
      {

      }

      
      psi->Release();
   }

   return S_OK;
}





void CWelcomeFrame::OnClose()
{
   // TODO: Add your message handler code here and/or call default

   ShowWindow(SW_HIDE);

}




afx_msg LRESULT CWelcomeFrame::OnResize(WPARAM wparam,LPARAM lparam)
{
   
   //int cx = GET_X_LPARAM(lparam);
   //int cy = GET_Y_LPARAM(lparam);
   //SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE | SWP_NOZORDER);
   return 0;

}



void CWelcomeFrame::OnSize(UINT nType, int cx, int cy)
{
   CFrameWnd::OnSize(nType, cx, cy);

   CPCDraftApp* pApp = (CPCDraftApp*)AfxGetApp();
   WINDOWPLACEMENT wp;
   GetWindowPlacement(&wp);
   pApp->WriteProfileBinary("WelcomeScreen", "Placement", (LPBYTE)&wp, sizeof(wp));


}


LRESULT CWelcomeFrame::OnDwm(WPARAM wparam, LPARAM lparam)
{

   BOOL enabled = FALSE;

   DwmIsCompositionEnabled(&enabled);

   if (enabled)
   {

      MARGINS m = { 0,0,0,1 };

      DWORD  dw = DWMNCRP_ENABLED;

      DwmExtendFrameIntoClientArea(GetSafeHwnd(), &m);

      DwmSetWindowAttribute(GetSafeHwnd(), DWMWA_NCRENDERING_POLICY, &dw, sizeof(DWORD));

   }

   return 0;

}



// Interface Upgrade - Infinisys Ltd.
void CWelcomeFrame::OnPaint()
{

   CPaintDC dc(this);

   CRect rWindow;

   GetClientRect(rWindow);
   //GetWindowRect(rWindow);

   rWindow.OffsetRect(-rWindow.TopLeft());

   CRect rPaint(rWindow);
   rPaint.bottom = 32;
   CRect rCaption(rPaint);
   char sz[1024];
   CFont font;
   font.CreatePointFont(100, "Segoe UI", &dc);
   ::GetWindowText(GetSafeHwnd(), sz, sizeof(sz));
   dc.SelectObject(&font);
   CSize s1 = dc.GetTextExtent(sz);

   {
      m_rectCaption = rWindow;

      m_rectCaption.bottom = 32;

      CRect r = rWindow;

      r.top += 32;
      r.bottom--;
      r.left++;
      r.right--;

      dc.ExcludeClipRect(r);

   }

   dc.FillSolidRect(&rWindow, 0);

   ::DrawIconEx(dc.GetSafeHdc(), 4, 4, m_hiconCaption, 24, 24, 0, NULL, DI_IMAGE);

   int x1 = 24 + 4;

   int x = 52;

   int xm = (x + x1) / 2;

   int mspan = xm - x1;




   CRect rLine = rWindow;

   rLine.top = rLine.bottom - 1;

   Gdiplus::Graphics gr(dc.GetSafeHdc());

   Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0));

   gr.DrawLine(&pen, rLine.left, rLine.bottom - 1, rLine.right, rLine.bottom - 1);


   Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericDefault());

   format.SetAlignment(Gdiplus::StringAlignmentCenter);
   format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
   format.SetTrimming(Gdiplus::StringTrimmingWord);
   format.SetFormatFlags(0);

   Gdiplus::Font f(L"Marlett", 10.0);
   Gdiplus::SolidBrush brText(Gdiplus::Color(255, 255, 255, 255));
   Gdiplus::RectF r2;
   copy(r2, &rWindow);
   r2.X = r2.X + r2.Width - 40;
   r2.Height = 40;
   r2.Width = 40;

   gr.DrawString(L"r", 1, &f, r2, &format, &brText);
   r2.X -= 40;
   if (IsZoomed())
   {
      gr.DrawString(L"2", 1, &f, r2, &format, &brText);
   }
   else
   {
      gr.DrawString(L"1", 1, &f, r2, &format, &brText);
   }
   r2.X -= 40;
   gr.DrawString(L"0", 1, &f, r2, &format, &brText);
   rCaption.right = r2.X;
   dc.SelectClipRgn(NULL);

   dc.SelectObject(&font);
   dc.SetBkMode(TRANSPARENT);

   dc.SetTextColor(RGB(255, 255, 255));
   if (s1.cx > 0)
   {
      CStringW wstr = get_uni(sz, CP_OEMCP);
      ::DrawTextW(dc.GetSafeHdc(), wstr, wstr.GetLength(), &rCaption, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
   }
}

