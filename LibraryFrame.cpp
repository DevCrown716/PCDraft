// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LibraryFrame.h"
#include "LibraryDoc.h"
#include "LibraryMainView.h"
#include "LibraryListView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
BOOL AFXAPI AfxIsDescendant(HWND hWndParent,HWND hWndChild);

HWND AFXAPI AfxGetParentOwner(HWND hWnd);
void get_subdir(CString str,CStringArray & straPath,CStringArray & straName);
void get_files(CString str,CStringArray & straPath,CStringArray & straName);
bool has_subdir(CString str)
{

   CStringW wstr(get_uni(str));

   WIN32_FIND_DATAW data;
   HANDLE hFind = FindFirstFileW(wstr + L"\\*.*",&data);      // DIRECTORY

   if(hFind != INVALID_HANDLE_VALUE)
   {
      do
      {
         if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
         {
            if(data .cFileName[0] == '.')
            {
               if(data.cFileName[1] == '\0' ||
                  (data.cFileName[1] == '.' &&
                  data.cFileName[2] == '\0'))
               {
                  // is dots  
               }
               else
               {
                  return true;
               }
            }
            else
            {
               return true;
            }

         } 

      } while(FindNextFileW(hFind,&data));
      FindClose(hFind);

   }

   //CFileFind find;

   //if(find.FindFile(str + "\\*.*"))
   //{

   //   bool bWorking = true;

   //   while(bWorking)
   //   {

   //      bWorking = find.FindNextFileA();

   //      if(find.IsDots())
   //      {

   //         

   //      }
   //      else if(find.IsDirectory())
   //      {

   //         return true;

   //      }


   //   } 

   //}


   return false;

}


void del_dir(const char * pszPath)
{
   

   {
      CStringArray straPath;

      CStringArray straName;

      get_subdir(pszPath,straPath,straName);

      for(int i = 0; i < straPath.GetCount(); i++)
      {

         del_dir(straPath[i]);

      }

   }

   {
      CStringArray straPath;

      CStringArray straName;

      get_files(pszPath,straPath,straName);

      for(int i = 0; i < straPath.GetCount(); i++)
      {

         CString strPath  = straPath[i];

         BOOL bDeleted = ::DeleteFileW(get_uni(strPath));

         if(!bDeleted)
         {

            DWORD dwLastError = GetLastError();

            TRACE("Last Error %d", dwLastError);

         }

      }

   }

   ::RemoveDirectoryW(get_uni(pszPath));

}


void get_subdir(CString str,CStringArray & straPath, CStringArray & straName)
{


   CStringW wstr(get_uni(str));

   WIN32_FIND_DATAW data;
   HANDLE hFind = FindFirstFileW(wstr + L"\\*.*",&data);      // DIRECTORY

   if(hFind != INVALID_HANDLE_VALUE)
   {
      do
      {
         if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
         {
            if(data .cFileName[0] == '.')
            {
               if(data.cFileName[1] == '\0' ||
                  (data.cFileName[1] == '.' &&
                  data.cFileName[2] == '\0'))
               {
                  // is dots  
               }
               else
               {
                         straPath.Add(get_cp(wstr + "\\" + data.cFileName));

                         straName.Add(get_cp(data.cFileName));

               }
            }
            else
            {
               straPath.Add(get_cp(wstr + "\\" + data.cFileName));

               straName.Add(get_cp(data.cFileName));
            }

         }

      } while(FindNextFileW(hFind,&data));
      FindClose(hFind);

   }

   //CFileFind find;

   //if(find.FindFile(str + "\\*.*"))
   //{

   //   bool bWorking = true;

   //   while(bWorking)
   //   {

   //      bWorking = find.FindNextFileA();

   //      if(find.IsDots())
   //      {

   //         

   //      }
   //      else if(find.IsDirectory())
   //      {

   //         return true;

   //      }


   //   } 

   //}



   //CFileFind find;

   //if(find.FindFile(str + "\\*.*"))
   //{

   //   bool bWorking = true;

   //   while(bWorking)
   //   {
   //      bWorking = find.FindNextFileA();
   //      if(find.IsDots())
   //      {



   //      }
   //      else if(find.IsDirectory())
   //      {

   //         straPath.Add(find.GetFilePath());

   //         straName.Add(find.GetFileName());

   //      }


   //   }

   //}



}

void get_files(CString str,CStringArray & straPath,CStringArray & straName)
{

   //CFileFind find;

   //if(find.FindFile(str + "\\*.*"))
   //{

   //   bool bWorking = true;

   //   while(bWorking)
   //   {
   //      bWorking = find.FindNextFileA();
   //      if(find.IsDots())
   //      {



   //      }
   //      else if(!find.IsDirectory())
   //      {

   //         straPath.Add(find.GetFilePath());

   //         straName.Add(find.GetFileName());

   //      }


   //   }

   //}


   CStringW wstr(get_uni(str));

   WIN32_FIND_DATAW data;
   HANDLE hFind = FindFirstFileW(wstr + L"\\*.*",&data);      // DIRECTORY

   if(hFind != INVALID_HANDLE_VALUE)
   {
      do
      {
         if(!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
         {
               straPath.Add(get_cp(wstr + "\\" + data.cFileName));

               straName.Add(get_cp(data.cFileName));

         }

      } while(FindNextFileW(hFind,&data));
      FindClose(hFind);

   }


}


CString get_utf8(LPCWSTR lpcwsz)
{
   return get_cp(lpcwsz);
}

CString get_cp(LPCWSTR lpcwsz, int iCp)
{

   CString str;

   int count = ::WideCharToMultiByte(iCp,0,lpcwsz,-1,NULL,0,NULL,0);

   if(count > 0)
   {
      char * lpDst = str.GetBufferSetLength(count-1);
      memset(lpDst,0,count-1);

      ::WideCharToMultiByte(iCp,0,lpcwsz,-1,lpDst,count-1,NULL,0);
      str.ReleaseBufferSetLength(count-1);
   }
   return str;


}

CStringW get_uni(LPCSTR lpcsz, int iCodePage)
{

   CStringW wstr;

   int count = ::MultiByteToWideChar(iCodePage,0,lpcsz,-1,NULL,0);
   
   if(count > 0)
   {

      LPWSTR lpwsz = wstr.GetBufferSetLength(count+1);

      ::MultiByteToWideChar(iCodePage,0,lpcsz,-1,lpwsz,count);

      wstr.ReleaseBuffer();

   }

   return wstr;


}

void get_folder_name(CString & strFolder, const char *psz);
// CLibraryFrame

IMPLEMENT_DYNCREATE(CLibraryFrame,CCustomMiniFrameWnd)

CLibraryFrame::CLibraryFrame()
{
   m_plibrarytreeview = NULL;
   m_bResizeGrip = true;
   m_bResizable = true;
   m_plibrarylistview = NULL;
   m_plibrarymainview = NULL;
   m_pphotolibrarylistview = NULL;
   m_bTracking = false;
   m_ulRef = 1;
}

CLibraryFrame::~CLibraryFrame()
{
}


BEGIN_MESSAGE_MAP(CLibraryFrame,CCustomMiniFrameWnd)
   ON_WM_CREATE()
   ON_WM_NCCREATE()
   ON_WM_ACTIVATE()
   ON_WM_NCACTIVATE()
   ON_WM_NCLBUTTONDBLCLK()
   ON_WM_NCLBUTTONDOWN()
   ON_WM_NCLBUTTONUP()
   ON_WM_GETMINMAXINFO()
   ON_MESSAGE(WM_USER + 123, &CLibraryFrame::OnResize)
   ON_COMMAND(ID_FILE_NEWLIBRARY,&CLibraryFrame::OnFileNewlibrary)
   ON_COMMAND(ID_FILE_OPENLIBRARY,&CLibraryFrame::OnFileOpenlibrary)
   ON_WM_CLOSE()
   ON_COMMAND(ID_VIEW_LIBRARY,&CLibraryFrame::OnViewLibrary)
   ON_COMMAND(ID_VIEW_PHOTO_LIBRARY,&CLibraryFrame::OnViewPhotoLibrary)
   ON_COMMAND(ID_FILE_RENAMEITEM,&CLibraryFrame::OnFileRenameitem)
   ON_UPDATE_COMMAND_UI(ID_FILE_RENAMEITEM,&CLibraryFrame::OnUpdateFileRenameitem)
   ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

//void CLibraryFrame::RecalcLayout(BOOL bNotify)
//{
//   CCustomMiniFrameWnd::RecalcLayout(bNotify);
//}
// CLibraryFrame message handlers


BOOL CLibraryFrame::PreCreateWindow(CREATESTRUCT& cs)
{
   // TODO: Add your specialized code here and/or call the base class
   if(!CCustomMiniFrameWnd::PreCreateWindow(cs))
      return FALSE;
   cs.dwExStyle &= ~WS_EX_TOOLWINDOW;
   //cs.dwExStyle |= WS_EX_COMPOSITED;
   cs.style |= MFS_SYNCACTIVE;
   cs.hwndParent = AfxGetApp()->m_pMainWnd->GetSafeHwnd();
   return TRUE;

}
Gdiplus::Bitmap * LoadPNG(LPCTSTR pName,LPCTSTR pType,HMODULE hInst);
//BOOL CLibraryFrame::CreateEx(DWORD dwExStyle,LPCTSTR lpClassName,
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
//   if(!CCustomMiniFrameWnd::CreateEx(dwExStyle,lpClassName ? lpClassName :
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

int CLibraryFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CCustomMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
      return -1;
   ModifyStyle(0,MFS_SYNCACTIVE | WS_THICKFRAME);
   
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


void CLibraryFrame::OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized)
{
   //CCustomMiniFrameWnd::OnActivate(nState,pWndOther,bMinimized);
   CWnd::OnActivate(nState,pWndOther,bMinimized);

   // hide the menu bar
   if(nState == WA_INACTIVE && (m_dwMenuBarVisibility & AFX_MBV_KEEPVISIBLE) == 0)
   {
      SetMenuBarState(AFX_MBS_HIDDEN);
   }

   // get top level frame unless this is a child window
   // determine if window should be active or not
   CFrameWnd* pTopLevel = (GetStyle() & WS_CHILD) ? this : m_pMainFrame;
   ENSURE_VALID(pTopLevel);
   CWnd* pActive = (nState == WA_INACTIVE ? pWndOther : this);
   BOOL bStayActive = (pActive != NULL) ?
      (pTopLevel == pActive ||
      (pTopLevel == m_pMainFrame &&
      (pActive == pTopLevel ||
      pActive->SendMessage(WM_FLOATSTATUS,FS_SYNCACTIVE) != 0)))
      : FALSE;
   pTopLevel->m_nFlags &= ~WF_STAYACTIVE;
   if(bStayActive)
      pTopLevel->m_nFlags |= WF_STAYACTIVE;

   // sync floating windows to the new state
   NotifyFloatingWindows(bStayActive ? FS_ACTIVATE : FS_DEACTIVATE);

   // get active view (use active frame if no active view)
   //CView* pActiveView = GetActiveView();
   //if(pActiveView == NULL)
   //   pActiveView = GetActiveFrame()->GetActiveView();

   //// when frame gets activated, re-activate current view
   //if(pActiveView != NULL)
   //{
   //   if(nState != WA_INACTIVE && !bMinimized)
   //      pActiveView->OnActivateView(TRUE,pActiveView,pActiveView);

   //   // always notify the view of frame activations
   //   pActiveView->OnActivateFrame(nState,this);
   //}

}


BOOL CLibraryFrame::OnNcActivate(BOOL bActive)
{
   // TODO: Add your message handler code here and/or call default
   //if(m_bTracking)
   //{
     // CFrameWnd::OnNcActivate(bActive);
//   }
   //else
   {
      return CCustomMiniFrameWnd::OnNcActivate(bActive);

   }
}
BOOL CLibraryFrame::OnNcCreate(LPCREATESTRUCT lpcs)
{
   if(!CFrameWnd::OnNcCreate(lpcs))
      return FALSE;

   if(GetStyle() & MFS_SYNCACTIVE)
   {
      // syncronize activation state with top level parent
      CWnd* pParentWnd = AfxGetApp()->m_pMainWnd;
      CWnd* pActiveWnd = GetForegroundWindow();
      BOOL bActive = (pParentWnd == pActiveWnd) ||
         (pParentWnd->GetLastActivePopup() == pActiveWnd &&
         pActiveWnd->SendMessage(WM_FLOATSTATUS,FS_SYNCACTIVE) != 0);

      // the WM_FLOATSTATUS does the actual work
      SendMessage(WM_FLOATSTATUS,bActive ? FS_ACTIVATE : FS_DEACTIVATE);
   }

   return TRUE;
}


void CLibraryFrame::NotifyFloatingWindows(DWORD dwFlags)
{
   ASSERT_VALID(this);
   ASSERT(m_hWnd != NULL);

   // get top level parent frame window first unless this is a child window
   CFrameWnd* pParent = (GetStyle() & WS_CHILD) ? this : m_pMainFrame;
   ENSURE(pParent != NULL);
   if(dwFlags & (FS_DEACTIVATE | FS_ACTIVATE))
   {
      // update parent window activation state
      BOOL bActivate = !(dwFlags & FS_DEACTIVATE);
      BOOL bEnabled = pParent->IsWindowEnabled();

      if(bActivate && bEnabled && pParent != this)
      {
         // Excel will try to Activate itself when it receives a
         // WM_NCACTIVATE so we need to keep it from doing that here.
         m_nFlags |= WF_KEEPMINIACTIVE;
         pParent->SendMessage(WM_NCACTIVATE,TRUE);
         m_nFlags &= ~WF_KEEPMINIACTIVE;
      }
      else
      {
         pParent->SendMessage(WM_NCACTIVATE,FALSE);
      }
   }

   // then update the state of all floating windows owned by the parent
   HWND hWnd = ::GetWindow(::GetDesktopWindow(),GW_CHILD);
   while(hWnd != NULL)
   {
      if(AfxIsDescendant(pParent->m_hWnd,hWnd))
         ::SendMessage(hWnd,WM_FLOATSTATUS,dwFlags,0);
      hWnd = ::GetWindow(hWnd,GW_HWNDNEXT);
   }
}


void CLibraryFrame::OnNcLButtonDown(UINT nHitTest,CPoint point)
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
   CCustomMiniFrameWnd::OnNcLButtonDown(nHitTest,point);

   //Default();
}

void CLibraryFrame::OnNcLButtonDblClk(UINT nHitTest,CPoint point)
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
   CCustomMiniFrameWnd::OnNcLButtonDblClk(nHitTest,point);
}

BOOL CLibraryFrame::Create(CWnd* pParent,DWORD dwBarStyle, PVOID p)
{
   // set m_bInRecalcLayout to avoid flashing during creation
   // RecalcLayout will be called once something is docked
   m_bInRecalcLayout = TRUE;

   //DWORD dwStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | MFS_MOVEFRAME |
   //   MFS_4THICKFRAME | MFS_SYNCACTIVE | MFS_BLOCKSYSMENU |
   //   FWS_SNAPTOBARS;
   DWORD dwStyle = WS_POPUP |  WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU |  MFS_SYNCACTIVE |
      WS_THICKFRAME;

   if(dwBarStyle & CBRS_SIZE_DYNAMIC)
      dwStyle &= ~MFS_MOVEFRAME;

   DWORD dwExStyle = 0;
   m_strCaption.LoadString(IDS_LIBRARY);

   CRect rect = rectDefault;
   if(!CWnd::CreateEx(dwExStyle,AfxRegisterWndClass(CS_DBLCLKS,::LoadCursor(NULL,IDC_ARROW)),
      m_strCaption,dwStyle,rect.left,rect.top,rect.right - rect.left,
      rect.bottom - rect.top,pParent->GetSafeHwnd(),(HMENU)(UINT_PTR)0,p))
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
   m_bInRecalcLayout = FALSE;

   return TRUE;
}

BOOL CLibraryFrame::LoadFrame(UINT nIDResource,DWORD dwDefaultStyle,
   CWnd* pParentWnd,CCreateContext* pContext)
{
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


void CLibraryFrame::OnNcLButtonUp(UINT nHitTest,CPoint point)
{
   CCustomMiniFrameWnd::OnNcLButtonUp(nHitTest,point);

   m_bTracking = false;
}


void CLibraryFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
   // TODO: Add your message handler code here and/or call default

   CCustomMiniFrameWnd::OnGetMinMaxInfo(lpMMI);

   lpMMI->ptMinTrackSize.x = 320;
   lpMMI->ptMinTrackSize.y = 450;
}

struct BrowseWindowHandles {
   HWND hwndTitle;
   HWND hwndStatic;  // not sure what this is
   HWND hwndStatic2;  // not sure what this is
   HWND hwndTree;
   HWND hwndOK;
   HWND hwndCancel;
};


bool GetBrowseWindowHandles(HWND hwnd,BrowseWindowHandles *bwh) {

   bwh->hwndTitle  = FindWindowEx(hwnd,NULL,"Static",NULL);
   bwh->hwndStatic = FindWindowEx(hwnd,bwh->hwndTitle,"Static",NULL);
   bwh->hwndStatic2 = FindWindowEx(hwnd,bwh->hwndStatic,"Static",NULL);
   bwh->hwndTree   = FindWindowEx(hwnd,NULL,"SysTreeView32",NULL);
   bwh->hwndOK     = FindWindowEx(hwnd,NULL,"Button","OK");
   bwh->hwndCancel = FindWindowEx(hwnd,NULL,"Button","Cancel");

   return (bwh->hwndTitle &&
      bwh->hwndStatic &&
      bwh->hwndTree &&
      bwh->hwndOK &&
      bwh->hwndCancel);

}
int CALLBACK LibraryFrameBrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
   CLibraryFrame * pframe = (CLibraryFrame *)lpData;

   if(uMsg == BFFM_INITIALIZED) {
      BrowseWindowHandles bwh;
      //SetWindowPos(hwnd,NULL,0,0,500,500,SWP_NOZORDER | SWP_NOMOVE);
      GetBrowseWindowHandles(hwnd,&bwh);
         //MessageBox(hwnd,"doh","doh",MB_ICONERROR);
      // this test just resizes the tree control
      wchar_t szText[1024];
      GetWindowTextW(bwh.hwndStatic2,szText, sizeof(szText)/sizeof(wchar_t));
      CStringW strText;
      strText.LoadString(IDS_LIBRARY_LABEL);
      SetWindowTextW(bwh.hwndStatic2,strText);
      SetWindowTextW(hwnd,pframe->m_bi.lpszTitle);

      CString strFolder;
      strFolder = AfxGetApp()->GetProfileString("Library", "Folder", "");
      {
         if (!strFolder.IsEmpty())
         {
            CStringW wstr = get_uni(strFolder);
  //          LPITEMIDLIST p = nullptr;
//SHILCreateFromPath(wstr, (LPITEMIDLIST *)&p, NULL);
            SendMessageW(hwnd, BFFM_SETEXPANDED, TRUE,(LPARAM) (const wchar_t*)wstr);

         }
      }
   }
   else if(uMsg == BFFM_IUNKNOWN)
   {
      IUnknown * pUk = (IUnknown *) lParam;
      if(pUk != NULL)
      {
         CComPtr<IFolderFilterSite> pfilter;

         pUk->QueryInterface(&pfilter);
         if(pfilter != NULL)
         {
            IUnknown * pThis = (IUnknown *)pframe;
            pfilter->SetFilter(pThis);


         }

      }

   }
   else if(uMsg == BFFM_SELCHANGED)
   {
      LPITEMIDLIST pidl = (LPITEMIDLIST) lParam;
      // get the name of the folder

      if(pidl != NULL)
      {
         WCHAR path[MAX_PATH];
         memset(path,0,sizeof(WCHAR) * MAX_PATH);
         SHGetPathFromIDListW(pidl,path);
         pframe->m_strSelPath = get_cp(path);

      }

   }
   else if(uMsg == BFFM_VALIDATEFAILEDW)
   {
      LPCWSTR lpcwszNewName = (LPCWSTR)lParam;

      if(lpcwszNewName != NULL)
      {

         CString str;

         str = get_cp(lpcwszNewName);

         CString strCandidateNewPath;

         if(!pframe->ValidateNewName(str,strCandidateNewPath))
         {
            CStringW str;
            str.LoadString(IDS_LIBRARY_ALREADY_EXISTS_WARNING);
            str.Replace(L"%1",lpcwszNewName);
            MessageBoxW(pframe->GetSafeHwnd(),str,get_uni(AfxGetApp()->m_pszAppName), MB_ICONINFORMATION);
            return 1;
         }


      }

   }
   return 0;

}

bool CLibraryFrame::ValidateNewName(const char * pszNewName,CString & strCandidateNewPath)
{
   CString & strNewPath = strCandidateNewPath;

   strNewPath = m_strSelPath;

   strNewPath.TrimRight("/");
   strNewPath.TrimRight("\\");

   strNewPath += "\\" + CString(pszNewName);
   CString strExt = ".spotlibrary";
   if(strNewPath.Right(strExt.GetLength()).CompareNoCase(strExt) != 0)
   {

      strNewPath += ".spotlibrary";

   }

   if(PathIsDirectoryW(get_uni(strNewPath)))
   {
      return false;
   }
   if(PathFileExistsW(get_uni(strNewPath)))
   {
      return false;
   }
   m_strNewPath = strNewPath;

   return true;


}


HRESULT STDMETHODCALLTYPE CLibraryFrame::QueryInterface(const IID & iid,void ** p)
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


ULONG STDMETHODCALLTYPE CLibraryFrame::AddRef()
{

   return ++m_ulRef;

}


ULONG STDMETHODCALLTYPE CLibraryFrame::Release()
{
   
   return --m_ulRef;

}


HRESULT CLibraryFrame::GetEnumFlags(IShellFolder * psf, PCIDLIST_ABSOLUTE pidlFolder, HWND * phwnd, DWORD * pgrfFlags)
{
   *pgrfFlags = SHCONTF_FOLDERS;
   
   return S_OK;
}


HRESULT CLibraryFrame::ShouldShow(IShellFolder * psf,PCIDLIST_ABSOLUTE pidlFolder,PCUITEMID_CHILD pidlItem)
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

         int iLenFind = strlen(pszFind);

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


void CLibraryFrame::OnFileNewlibrary()
{

   m_bOpenLibrary = false;

   m_strNewPath.Empty();

   BROWSEINFOW & bi = m_bi;

   memset(&bi,0,sizeof(bi));

   bi.hwndOwner = GetSafeHwnd();

   bi.lParam = (LPARAM) (CLibraryFrame *) this;

   bi.ulFlags = BIF_USENEWUI | BIF_NONEWFOLDERBUTTON | BIF_VALIDATE;

   bi.lpfn = &LibraryFrameBrowseCallbackProc;

   CStringW strTitle;
   strTitle.LoadString(IDS_CREATE_NEW_LIBRARY);

   bi.lpszTitle = strTitle;

   LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);

   if(pidl != 0)
   {

      WCHAR path[MAX_PATH];

      SHGetPathFromIDListW(pidl,path);

      CString strPath(get_cp(path));

      if(!m_strNewPath.IsEmpty())
      {

         strPath = m_strNewPath;

      }

      CLibraryDoc * pDoc = (CLibraryDoc *)GetActiveDocument();

      CString strExt = ".spotlibrary";

      if(strPath.Right(strExt.GetLength()).CompareNoCase(strExt) != 0)
      {

         strPath += ".spotlibrary";

      }

      pDoc->library_add(strPath, true);

      pDoc->UpdateAllViews(NULL,LIBRARY_HINT_UPDATE_TREE);
      // free memory used
      IMalloc * imalloc = 0;
      if(SUCCEEDED(SHGetMalloc(&imalloc)))
      {
         imalloc->Free(pidl);
         imalloc->Release();
      }
   }
}


void CLibraryFrame::OnFileOpenlibrary()
{
   m_bOpenLibrary = true;
   BROWSEINFOW & bi = m_bi;
   memset(&bi,0,sizeof(bi));
   bi.lParam = (LPARAM)(CLibraryFrame *) this;
   bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;
   bi.lpfn = &LibraryFrameBrowseCallbackProc;
   CStringW strTitle;
   strTitle.LoadString(IDS_SELECT_MICROSPOT_LIBRARY);


   bi.lpszTitle = strTitle;
   bi.hwndOwner = GetSafeHwnd();
   LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
   if(pidl != 0)
   {
      // get the name of the folder
      WCHAR path[MAX_PATH];
      SHGetPathFromIDListW(pidl,path);
      CString strPath(get_cp(path));
      CString str1;
      
      get_folder_name(str1, strPath);
      AfxGetApp()->WriteProfileString("Library", "Folder", str1);
      CLibraryDoc * pDoc = (CLibraryDoc * ) GetActiveDocument();
      if(strPath.CompareNoCase(pDoc->GetDefaultLibraryPath()) != 0)
      {

         pDoc->library_add(strPath);

         pDoc->UpdateAllViews(NULL,LIBRARY_HINT_UPDATE_TREE);

      }
      // free memory used
      IMalloc * imalloc = 0;
      if(SUCCEEDED(SHGetMalloc(&imalloc)))
      {
         imalloc->Free(pidl);
         imalloc->Release();
      }
   }
}


void CLibraryFrame::OnClose()
{
   // TODO: Add your message handler code here and/or call default

   ShowWindow(SW_HIDE);

}


void CLibraryFrame::OnViewLibrary()
{
   m_plibrarymainview->m_bLibrary = true;
   m_plibrarymainview->layout();
}


void CLibraryFrame::OnViewPhotoLibrary()
{
   m_plibrarymainview->m_bLibrary = false;
   m_plibrarymainview->layout();
}


void CLibraryFrame::OnFileRenameitem()
{
   // TODO: Add your command handler code here

   m_plibrarylistview->rename_item();

}


void CLibraryFrame::OnUpdateFileRenameitem(CCmdUI *pCmdUI)
{
   // TODO: Add your command update UI handler code here
   pCmdUI->Enable(m_plibrarylistview->IsWindowVisible() && m_plibrarylistview->get_sel() >= 0);
}



afx_msg LRESULT CLibraryFrame::OnResize(WPARAM wparam,LPARAM lparam)
{
   
   int cx = GET_X_LPARAM(lparam);
   int cy = GET_Y_LPARAM(lparam);
   SetWindowPos(NULL,0,0,cx,cy,SWP_NOMOVE | SWP_NOZORDER);
   return 0;

}

