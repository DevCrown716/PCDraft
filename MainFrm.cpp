// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "PCDraft.h"
#include "ribbon.h" // Infinisys Ltd.

#include "MainFrm.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "SavedView.h"
#include "MDReport.h"
#include "CustomMiniDockFrameWnd.h"

#include "Welcome/GetStartedWnd.h"
#include "Welcome/TemplateWnd.h"

#include "MoreWindowsDlg.h"

#include "LayerDoc.h" // Layer - Infinisys Ltd.
#include "LayerFrame.h" // Layer - Infinisys Ltd.
#include "LayerMainView.h" // Layer - Infinisys Ltd.
#include <uiribbon.h> // Interface Update - Infinisys Ltd.
#include <VersionHelpers.h> // Interface Update - Infinisys Ltd.
#include <shellapi.h> // Interface Update - Infinisys Ltd.


#pragma comment(lib, "Dwmapi.lib" ) // Interface Update - Infinisys Ltd.
void copy(Gdiplus::Rect & r,LPCRECT lpcrect); // Interface Update - Infinisys Ltd.
void copy(Gdiplus::RectF & r,LPCRECT lpcrect); // Interface Update - Infinisys Ltd.

extern bool g_bEndApp;

static bool has_autohide_appbar(UINT edge,RECT mon) // Interface Update - Infinisys Ltd.
{
   if(IsWindows8Point1OrGreater()) {
      APPBARDATA data={};
      data.cbSize = sizeof(APPBARDATA);
      data.uEdge = edge;
      data.rc = mon;
      return SHAppBarMessage(0x0000000b,&data);
   }

   /* Before Windows 8.1, it was not possible to specify a monitor when
   checking for hidden appbars, so check only on the primary monitor */
   if(mon.left != 0 || mon.top != 0)
      return false;
   APPBARDATA data={};
   data.cbSize = sizeof(APPBARDATA);
   data.uEdge = edge;
   return SHAppBarMessage(ABM_GETAUTOHIDEBAR,&data);
}
// Interface Update - Infinisys Ltd.
#ifndef WM_NCUAHDRAWCAPTION
#define WM_NCUAHDRAWCAPTION (0x00AE)
#endif
#ifndef WM_NCUAHDRAWFRAME
#define WM_NCUAHDRAWFRAME (0x00AF)
#endif

// Interface Update - Infinisys Ltd.
HRESULT EnableNcDwm(HWND hwnd,bool enable)
{
   HRESULT hr = S_OK;
   DWMNCRENDERINGPOLICY ncrp;
   if(enable)
      ncrp = DWMNCRP_ENABLED;
   else
      ncrp = DWMNCRP_DISABLED;

   // Disable non-client area rendering on the window.
   hr = DwmSetWindowAttribute(hwnd,DWMWA_NCRENDERING_POLICY,&ncrp,sizeof(ncrp));
   if(SUCCEEDED(hr))
   {
      return hr;
   }
   return S_FALSE;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	kMinWidth	350
#define	kMinHeight	410

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
   //{{AFX_MSG_MAP(CMainFrame)
   ON_WM_CREATE()
   ON_WM_SIZE() // Interface Update - Infinisys Ltd.
   ON_UPDATE_COMMAND_UI(ID_INDICATOR_PROGRESS, OnUpdateIndicatorProgress)
   ON_UPDATE_COMMAND_UI(ID_VIEW_SHOW_TOOLBAR,OnUpdateViewShowToolbar)
   ON_COMMAND(ID_VIEW_SHOW_TOOLBAR,OnViewShowToolbar)
   ON_UPDATE_COMMAND_UI(ID_VIEW_HIDE_TOOLBAR,OnUpdateViewHideToolbar)
   ON_COMMAND(ID_VIEW_HIDE_TOOLBAR,OnViewHideToolbar)
   ON_UPDATE_COMMAND_UI(ID_VIEW_PALETTE_TOOL,OnUpdateViewPaletteTool)
   ON_COMMAND(ID_VIEW_PALETTE_TOOL,OnViewPaletteTool)
   ON_UPDATE_COMMAND_UI(ID_VIEW_PALETTE_ATTRIB,OnUpdateViewPaletteAttrib)
   ON_COMMAND(ID_VIEW_PALETTE_ATTRIB,OnViewPaletteAttrib)
   ON_UPDATE_COMMAND_UI(ID_VIEW_PALETTE_DIM,OnUpdateViewPaletteDim)
   ON_COMMAND(ID_VIEW_PALETTE_DIM,OnViewPaletteDim)
   ON_COMMAND(ID_VIEW_PALETTE_ALL,OnViewPaletteAll)
   ON_WM_INITMENU()
   ON_UPDATE_COMMAND_UI(ID_VIEW_PALETTE_RESIZE,OnUpdateViewPaletteResize)
   ON_COMMAND(ID_VIEW_PALETTE_RESIZE,OnViewPaletteResize)
   ON_UPDATE_COMMAND_UI(ID_VIEW_PALETTE_SHOWSIZE,OnUpdateViewPaletteShowsize)
   ON_COMMAND(ID_VIEW_PALETTE_SHOWSIZE,OnViewPaletteShowsize)
   ON_UPDATE_COMMAND_UI(ID_VIEW_PALETTE_ALIGN,OnUpdateViewPaletteAlign)
   ON_COMMAND(ID_VIEW_PALETTE_ALIGN,OnViewPaletteAlign)
   ON_COMMAND(ID_DATA_FIND_REPLACE,OnDataFindReplace)
   ON_COMMAND(ID_LAYOUT_LAYER_SETUP,OnLayoutLayerSetup)
#ifndef _PERSONAL_EDITION
   ON_COMMAND(ID_VIEW_LAYER,OnViewLayer) // Layer - Infinisys Ltd.
#endif
   ON_WM_QUERYNEWPALETTE()
   ON_WM_CLOSE()
   ON_WM_GETMINMAXINFO()
   ON_UPDATE_COMMAND_UI(ID_DATA_FIND_REPLACE,OnUpdateDataFindReplace)
   ON_UPDATE_COMMAND_UI(ID_LAYOUT_LAYER_SETUP,OnUpdateLayoutLayerSetup)
#ifndef _PERSONAL_EDITION
   ON_UPDATE_COMMAND_UI(ID_VIEW_LAYER,OnUpdateViewLayer) // Layer - Infinisys Ltd.
#endif
   ON_WM_ACTIVATE()
   ON_WM_ACTIVATEAPP()
   //}}AFX_MSG_MAP
#ifndef _PERSONAL_EDITION
   ON_UPDATE_COMMAND_UI(ID_VIEW_PALETTE_ACC,OnUpdateViewPaletteAcc)
   ON_COMMAND(ID_VIEW_PALETTE_ACC,OnViewPaletteAcc)
#endif
   ON_COMMAND(ID_HELP_FINDER,CMDIFrameWnd::OnHelpFinder)
   ON_COMMAND(AFX_IDM_FIRST_MDICHILD + 9,OnMoreWindows)
   ON_REGISTERED_MESSAGE(gNetGuardMsg,OnNetGuardMsg)
   ON_NOTIFY_EX(NM_CUSTOMDRAW,0,OnTipCustomDraw)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA,0,0xFFFF,OnToolTipNotify)
   ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW,0,0xFFFF,OnToolTipNotify)
   //ON_WM_NCPAINT()
   ON_MESSAGE(WM_DWMNCRENDERINGCHANGED,OnDwm) // Interface Update - Infinisys Ltd.
   ON_WM_PAINT() // Interface Update - Infinisys Ltd.

   ON_WM_NCACTIVATE() // Interface Update - Infinisys Ltd.
   ON_WM_NCCALCSIZE() // Interface Update - Infinisys Ltd.
   ON_MESSAGE(MYWM_PROGRESS, OnProgress)
   //ON_MESSAGE(WM_NCUAHDRAWCAPTION, OnVoid)
   //ON_MESSAGE(WM_NCUAHDRAWFRAME,OnVoid)
   ON_WM_NCHITTEST() // Interface Update - Infinisys Ltd.
   ON_WM_LBUTTONDOWN() // Interface Update - Infinisys Ltd.
   ON_WM_COPYDATA()
   ON_WM_SHOWWINDOW()
   ON_WM_MOUSEACTIVATE()
   //ON_WM_SETTEXT()
END_MESSAGE_MAP()

static UINT indicators[] =
{
   ID_SEPARATOR,           // status line indicator
   ID_INDICATOR_PROGRESS,
   ID_INDICATOR_CAPS,
   ID_INDICATOR_NUM,
   ID_INDICATOR_SCRL,
};

void CMainFrame::OnMoreWindows()
{
   CMoreWindowsDlg theDlg;
   theDlg.DoModal();
}


// Interface Update - Infinisys Ltd.
CMenu* CMainFrame::GetMenu() const 
{
   // return cached menu (menu in fact, is not shown, there is only the ribbon);
   return m_pMenu;

}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
   m_bTopLeftButtons = false;
//   m_pUIFramework = NULL;
   m_bActivateApp=FALSE;
   m_bIsFirstShow = true;
}

CMainFrame::~CMainFrame()
{
   delete m_pbitmapSave; // Interface Update - Infinisys Ltd.
   delete m_pbitmapUndo; // Interface Update - Infinisys Ltd.
   delete m_pbitmapRedo; // Interface Update - Infinisys Ltd.
}
Gdiplus::Bitmap * LoadPNG(LPCTSTR pName,LPCTSTR pType,HMODULE hInst)
{

   Gdiplus::Bitmap * p = NULL;

   HRSRC hResource = ::FindResource(hInst,pName,pType);
   if(!hResource)
      return p;

   DWORD imageSize = ::SizeofResource(hInst,hResource);
   if(!imageSize)
      return p;

   const void* pResourceData = ::LockResource(::LoadResource(hInst,
                               hResource));
   if(!pResourceData)
      return p;

   HGLOBAL m_hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE,imageSize);
   if(m_hBuffer)
   {
      void* pBuffer = ::GlobalLock(m_hBuffer);
      if(pBuffer)
      {
         CopyMemory(pBuffer,pResourceData,imageSize);

         IStream* pStream = NULL;
         if(::CreateStreamOnHGlobal(m_hBuffer,FALSE,&pStream) == S_OK)
         {
            p = Gdiplus::Bitmap::FromStream(pStream);
            pStream->Release();
            if(p)
            {
               if(p->GetLastStatus() != Gdiplus::Ok)
               {
                  return p;

               }
            }
         }
         ::GlobalUnlock(m_hBuffer);
      }
      ::GlobalFree(m_hBuffer);
      m_hBuffer = NULL;
   }
   return p;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   //ModifyStyle(,0,0); // Interface Update - Infinisys Ltd.
   if(CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
      return -1;




   ModifyStyle(WS_MAXIMIZEBOX,0,SWP_FRAMECHANGED);
   //ModifyStyle(WS_CAPTION,0,SWP_FRAMECHANGED);
   //ModifyStyle(WS_CAPTION,WS_BORDER,SWP_FRAMECHANGED);
   //ModifyStyle(WS_CAPTION | WS_THI,WS_BORDER,SWP_FRAMECHANGED);
   //EnableNcDwm(GetSafeHwnd(),true);
   //SetWindowRgn(NULL,FALSE);
   m_hiconCaption =(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME),IMAGE_ICON,24,24,0); // Interface Update - Infinisys Ltd.
   m_hiconSave =(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CAPTION_SAVE),IMAGE_ICON,20,20,0); // Interface Update - Infinisys Ltd.
   m_hiconUndo =(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CAPTION_UNDO),IMAGE_ICON,20,20,0); // Interface Update - Infinisys Ltd.
   m_hiconRedo =(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_CAPTION_REDO),IMAGE_ICON,20,20,0); // Interface Update - Infinisys Ltd.
   m_pbitmapSave = LoadPNG(MAKEINTRESOURCE(ID_CMD_FILE_SAVE_SmallImages_RESID),"IMAGE", NULL); // Interface Update - Infinisys Ltd.
   m_pbitmapUndo = LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_UNDO_SmallImages_RESID),"IMAGE",NULL); // Interface Update - Infinisys Ltd.
   m_pbitmapRedo = LoadPNG(MAKEINTRESOURCE(ID_CMD_EDIT_REDO_SmallImages_RESID),"IMAGE",NULL); // Interface Update - Infinisys Ltd.
   //BOOL bNameValid;
   //SetIcon((HICON)::LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME),IMAGE_ICON,0,0,LR_DEFAULTSIZE), TRUE);
   //CMDITabInfo mdiTabParams;
   //mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // other styles available...
   //mdiTabParams.m_bActiveTabCloseButton = TRUE;      // set to FALSE to place close button at right of tab area
   //mdiTabParams.m_bTabIcons = FALSE;    // set to TRUE to enable document icons on MDI taba
   //mdiTabParams.m_bAutoColor = TRUE;    // set to FALSE to disable auto-coloring of MDI tabs
   //mdiTabParams.m_bDocumentMenu = TRUE; // enable the document menu at the right edge of the tab area
   //EnableMDITabbedGroups(TRUE,mdiTabParams);


   EnableDocking(CBRS_ALIGN_TOP); // Interface Update - Infinisys Ltd.
   m_pFloatingFrameClass = RUNTIME_CLASS(CCustomMiniDockFrameWnd); // Interface Update - Infinisys Ltd.


   if(!m_RibbonBar.CreateEx(this,TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP,CRect(0, 0, 0, 0), 101)) // Interface Update - Infinisys Ltd.
      return -1;      // fail to create
   m_RibbonBar.EnableDocking(CBRS_ALIGN_TOP); // Interface Update - Infinisys Ltd.
   ShowControlBar(&m_RibbonBar,true,true); // Interface Update - Infinisys Ltd.

   if(!m_wndStatusBar.Create(this) ||
      !m_wndStatusBar.SetIndicators(indicators,
      sizeof(indicators) / sizeof(UINT)))
   {
      TRACE0("Failed to create status bar\n");
      return -1;      // fail to create
   }

   m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_PROGRESS, SBPS_NORMAL, 500);

   if(!m_TabBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,CRect(0,0,0,0),102)) // Interface Update - Infinisys Ltd.
      return -1;      // fail to create
   m_TabBar.EnableDocking(CBRS_ALIGN_BOTTOM); // Interface Update - Infinisys Ltd.
   ShowControlBar(&m_TabBar,true,true); // Interface Update - Infinisys Ltd.


   m_ToolTip.Create(this,TTS_ALWAYSTIP | TTS_NOPREFIX);
   m_ToolTip.Activate(TRUE);


   //if(FAILED(InitRibbon(this,&m_pUIFramework)))
   //{

   //   return -1;

   //}


   //HRESULT hr = SetModes(m_pUIFramework,UI_MAKEAPPMODE(0) | UI_MAKEAPPMODE(1));
   //if(FAILED(hr))
   //{
   //   
   //   return -1;

   //}


// Interface Update - Infinisys Ltd.

   //m_RibbonBar.EnableDocking(CBRS_ALIGN_TOP);
   //ShowControlBar(&m_RibbonBar,true,true);

//   if(false)
//   {
//      if(m_toolbar.Create(this,WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS))
//      {
//         Gdiplus::Bitmap * p = LoadPNG(MAKEINTRESOURCEA(IDR_TOOLBAR),"PNG",NULL);
//         if(p != NULL)
//         {
//            HBITMAP h = NULL;
//            p->GetHBITMAP(Gdiplus::Color(0,0,0,0),& h);
//            if(m_toolbar.SetBitmap(h))
//            {
//
//               UINT ui[]=
//               {
//                  ID_FILE_OPEN,
//                  ID_FILE_SAVE,
//                  ID_FILE_IMPORT_IMAGE,
//                  ID_FILE_PAGE_SETUP,
//                  ID_FILE_PRINT,
//                  ID_SEPARATOR,
//                  ID_EDIT_UNDO,
//                  ID_EDIT_DUPLUCATE,
//                  ID_EDIT_SELECT_ALL,
//                  ID_EDIT_SELECT_NONE,
//                  ID_SEPARATOR,
//                  ID_VIEW_RESTORE_PREV_ZOOM,
//                  ID_VIEW_HOME_VIEW,
//                  ID_SEPARATOR,
//                  ID_ARRANGE_TO_FRONT,
//                  ID_ARRANGE_TO_BACK,
//                  ID_ARRANGE_FORWARD,
//                  ID_ARRANGE_BACKWARD,
//                  ID_ARRANGE_GROUP,
//                  ID_ARRANGE_UNGROUP,
//                  ID_ARRANGE_FLIP_HORIZ,
//                  ID_ARRANGE_FLIP_VERT,
//                  ID_ARRANGE_LOCK,
//                  ID_ARRANGE_UNLOCK,
//                  ID_SEPARATOR
//#ifndef _PERSONAL_EDITION
//                  ,ID_VIEW_LAYER
//                  ,ID_VIEW_LIBRARY
//#endif
//               };
//
//               m_toolbar.SetSizes(CSize(39,38),CSize(32,32));
//
//               if(m_toolbar.SetButtons(ui,sizeof(ui) / sizeof(ui[0])))
//               {
//
//                  m_toolbar.EnableDocking(CBRS_ALIGN_TOP);
//                  ShowControlBar(&m_toolbar,true,true);
//
//               }
//
//            }
//
//            delete p;
//
//         }
//
//      }
//
//   }

   if (!m_wndToolPalette.Create(this, false))
   {
      TRACE0("Failed to create ToolPalette\n");
      return -1;      // fail to create
   }

   //if (!m_wndAttribPalette.Create(this))
   //{
   //   TRACE0("Failed to create AccPalette\n");
   //   return -1;      // fail to create
   //}

#ifndef _PERSONAL_EDITION
   if (!m_wndAccPalette.Create(this))
   {
      TRACE0("Failed to create AccPalette\n");
      return -1;      // fail to create
   }
#endif


   if (!m_wndDimPalette.Create(this))
   {
      TRACE0("Failed to create DimPalette\n");
      return -1;      // fail to create
   }

   if (!m_wndResizePalette.Create(this))
   {
      TRACE0("Failed to create ResizePalette\n");
      return -1;      // fail to create
   }

   if (!m_wndShowSizePalette.Create(this))
   {
      TRACE0("Failed to create ShowSizePalette\n");
      return -1;      // fail to create
   }

   if (!m_wndAlignPalette.Create(this))
   {
      TRACE0("Failed to create AlignPalette\n");
      return -1;      // fail to create
   }



   CRect rc;
   SystemParametersInfo(SPI_GETWORKAREA,0,&rc,0);
   FloatControlBar(&m_wndToolPalette, CPoint(rc.left+10,rc.top+50));
   //FloatControlBar(&m_wndAttribPalette, CPoint(rc.left+10,rc.top+320));
#ifndef _PERSONAL_EDITION
   FloatControlBar(&m_wndAccPalette, CPoint(rc.left+250,rc.bottom-90));
#endif
   FloatControlBar(&m_wndDimPalette, CPoint(rc.right-100,rc.top+50));
   FloatControlBar(&m_wndResizePalette, CPoint(rc.right-230,rc.top+50));
   FloatControlBar(&m_wndShowSizePalette, CPoint(rc.left+45,rc.bottom-95));
   FloatControlBar(&m_wndAlignPalette, CPoint(rc.left+100,rc.bottom-170));

   CPCDraftApp* pApp=(CPCDraftApp*)AfxGetApp();
   if(pApp->m_bToolSaveLocation)
      LoadBarState("PalleteState");

   //// should always be visible initially
   //if(!m_wndToolPalette.IsWindowVisible()) ShowControlBar(&m_wndToolPalette,TRUE,FALSE);
   ////if(!m_wndAttribPalette.IsWindowVisible()) ShowControlBar(&m_wndAttribPalette,TRUE,FALSE);
   ShowControlBar(&m_wndToolPalette, FALSE, FALSE);

   if(pApp->m_bToolDisplay)
   {
      // restore visibility
      if(!pApp->m_bToolSaveLocation) // othervisw restored by LoadBarState
      {
         int state=pApp->GetProfileInt("PalleteState-Summary","Display",0);
#ifndef _PERSONAL_EDITION
         if(state&0x1) ShowControlBar(&m_wndAccPalette,TRUE,FALSE);
#endif
         if(state&0x2) ShowControlBar(&m_wndDimPalette,TRUE,FALSE);
         if(state&0x4) ShowControlBar(&m_wndResizePalette,TRUE,FALSE);
         if(state&0x8) ShowControlBar(&m_wndShowSizePalette,TRUE,FALSE);
         if(state&0x10) ShowControlBar(&m_wndAlignPalette,TRUE,FALSE);
      }
   }
   else
   {
#ifndef _PERSONAL_EDITION
      // hide other if not saving display state
      if(m_wndAccPalette.IsWindowVisible()) ShowControlBar(&m_wndAccPalette,FALSE,FALSE);
#endif
      if(m_wndDimPalette.IsWindowVisible()) ShowControlBar(&m_wndDimPalette,FALSE,FALSE);
      if(m_wndResizePalette.IsWindowVisible()) ShowControlBar(&m_wndResizePalette,FALSE,FALSE);
      if(m_wndShowSizePalette.IsWindowVisible()) ShowControlBar(&m_wndShowSizePalette,FALSE,FALSE);
      if(m_wndAlignPalette.IsWindowVisible()) ShowControlBar(&m_wndAlignPalette,FALSE,FALSE);
   }

   //((CCustomMiniDockFrameWnd*)(m_wndAttribPalette.GetParentFrame()))->ShowMaximizeBox(true);
   //((CCustomMiniDockFrameWnd*)(m_wndAttribPalette.GetParentFrame()))->InvertMaximized();
   ((CCustomMiniDockFrameWnd*)(m_wndDimPalette.GetParentFrame()))->ShowMaximizeBox(true);
   ((CCustomMiniDockFrameWnd*)(m_wndDimPalette.GetParentFrame()))->InvertMaximized();
   ((CCustomMiniDockFrameWnd*)(m_wndResizePalette.GetParentFrame()))->ShowMaximizeBox(true);

   return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.style|=WS_HSCROLL|WS_VSCROLL;
   //cs.style&=~FWS_ADDTOTITLE;
   cs.style|=FWS_ADDTOTITLE;
   cs.dwExStyle|=WS_EX_ACCEPTFILES;
   //::DestroyMenu(cs.hMenu);
   if(!CMDIFrameWnd::PreCreateWindow(cs))
   {
      return FALSE;
   }
   // Interface Update - Infinisys Ltd.
   m_pMenu = new CMenu();
   m_pMenu->Attach(cs.hMenu);
   cs.hMenu = NULL;

   //WNDCLASS wndclass ={};
   //wndclass.lpfnWndProc = &::DefWindowProc;
   //wndclass.lpszClassName = "PCDraft_Main_Frame";
   //wndclass.hInstance = AfxGetInstanceHandle();
   //wndclass.hCursor = ::LoadCursor(NULL,IDC_ARROW);
   //wndclass.style =  CS_DBLCLKS;
   //wndclass.hIcon = ::LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
   //if(!RegisterClass(&wndclass))
   //{
   //   return false;
   //}
   //cs.lpszClass = wndclass.lpszClassName;
   cs.style &= ~WS_CAPTION;
   cs.style &= ~WS_VISIBLE;

   return TRUE;

}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs,CCreateContext* p)
{
   //CMenu* pMenu = NULL;
   //if(m_hMenuDefault == NULL)
   //{
   //   // default implementation for MFC V1 backward compatibility
   //   pMenu = GetMenu();
   //   ASSERT(pMenu != NULL);
   //   // This is attempting to guess which sub-menu is the Window menu.
   //   // The Windows user interface guidelines say that the right-most
   //   // menu on the menu bar should be Help and Window should be one
   //   // to the left of that.
   //   int iMenu = pMenu->GetMenuItemCount() - 2;

   //   // If this assertion fails, your menu bar does not follow the guidelines
   //   // so you will have to override this function and call CreateClient
   //   // appropriately or use the MFC V2 MDI functionality.
   //   ASSERT(iMenu >= 0);
   //   pMenu = pMenu->GetSubMenu(iMenu);
   //   ASSERT(pMenu != NULL);
   //}

      return CreateClient(lpcs,NULL);
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
   CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
   CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnUpdateIndicatorProgress(CCmdUI* pCmdUI)
{

   pCmdUI->SetText("");

}

void CMainFrame::OnUpdateViewShowToolbar(CCmdUI* pCmdUI)
{
//   pCmdUI->SetCheck(m_toolbar.IsWindowVisible());
  // pCmdUI->Enable(!m_toolbar.IsWindowVisible());
}

void CMainFrame::OnViewShowToolbar()
{
   //ShowControlBar(&m_toolbar,TRUE,FALSE);
}

void CMainFrame::OnUpdateViewHideToolbar(CCmdUI* pCmdUI)
{
//   pCmdUI->SetCheck(!m_toolbar.IsWindowVisible());
  // pCmdUI->Enable(m_toolbar.IsWindowVisible());
}

void CMainFrame::OnViewHideToolbar()
{
   //ShowControlBar(&m_toolbar,FALSE,FALSE);
}

void CMainFrame::OnUpdateViewPaletteTool(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_wndToolPalette.IsWindowVisible());
}

void CMainFrame::OnViewPaletteTool()
{
   ShowControlBar(&m_wndToolPalette,!m_wndToolPalette.IsWindowVisible(),FALSE);
}

void CMainFrame::OnUpdateViewPaletteAttrib(CCmdUI* pCmdUI)
{
   //pCmdUI->SetCheck(m_wndAttribPalette.IsWindowVisible());
}

void CMainFrame::OnViewPaletteAttrib()
{
   //ShowControlBar(&m_wndAttribPalette,!m_wndAttribPalette.IsWindowVisible(),FALSE);
}

void CMainFrame::OnUpdateViewPaletteDim(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_wndDimPalette.IsWindowVisible());
}

void CMainFrame::OnViewPaletteDim()
{
   ShowControlBar(&m_wndDimPalette,!m_wndDimPalette.IsWindowVisible(),FALSE);
}

#ifndef _PERSONAL_EDITION

void CMainFrame::OnUpdateViewPaletteAcc(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_wndAccPalette.IsWindowVisible());
}


void CMainFrame::OnViewPaletteAcc()
{
   ShowControlBar(&m_wndAccPalette,!m_wndAccPalette.IsWindowVisible(),FALSE);
}

#endif

void CMainFrame::OnUpdateViewPaletteResize(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_wndResizePalette.IsWindowVisible());
}

void CMainFrame::OnViewPaletteResize()
{
   ShowControlBar(&m_wndResizePalette,!m_wndResizePalette.IsWindowVisible(),FALSE);
}

void CMainFrame::OnUpdateViewPaletteShowsize(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_wndShowSizePalette.IsWindowVisible());
}

void CMainFrame::OnViewPaletteShowsize()
{
   ShowControlBar(&m_wndShowSizePalette,!m_wndShowSizePalette.IsWindowVisible(),FALSE);
}

void CMainFrame::OnUpdateViewPaletteAlign(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(m_wndAlignPalette.IsWindowVisible());
}

void CMainFrame::OnViewPaletteAlign()
{
   ShowControlBar(&m_wndAlignPalette,!m_wndAlignPalette.IsWindowVisible(),FALSE);
}

void CMainFrame::OnViewPaletteAll()
{
   /*BOOL bShowHide=!(m_wndToolPalette.IsWindowVisible() &&
                    m_wndAttribPalette.IsWindowVisible() &&
#ifndef _PERSONAL_EDITION
                    m_wndAccPalette.IsWindowVisible() &&
#endif
                    m_wndDimPalette.IsWindowVisible() &&
                    m_wndResizePalette.IsWindowVisible() &&
                    m_wndShowSizePalette.IsWindowVisible() &&
                    m_wndAlignPalette.IsWindowVisible());*/
   // Interface Update - Infinisys Ltd.
   BOOL bShowHide=!(m_wndToolPalette.IsWindowVisible() &&
   #ifndef _PERSONAL_EDITION
   m_wndAccPalette.IsWindowVisible() &&
   #endif
   m_wndDimPalette.IsWindowVisible() &&
   m_wndShowSizePalette.IsWindowVisible() &&
   m_wndAlignPalette.IsWindowVisible());

   ShowControlBar(&m_wndToolPalette,bShowHide,FALSE);

   //ShowControlBar(&m_wndAttribPalette,bShowHide,FALSE); // Interface Update - Infinisys Ltd.
#ifndef _PERSONAL_EDITION
   ShowControlBar(&m_wndAccPalette,bShowHide,FALSE);
#endif
   ShowControlBar(&m_wndDimPalette,bShowHide,FALSE);
   //ShowControlBar(&m_wndResizePalette,bShowHide,FALSE); // Interface Update - Infinisys Ltd.
   ShowControlBar(&m_wndShowSizePalette,bShowHide,FALSE);
   ShowControlBar(&m_wndAlignPalette,bShowHide,FALSE);
}


#ifndef _PERSONAL_EDITION

#define PREF_LINE_STYLE 5
#define PREF_COLORS 10 // Gradient/Pattern - Infinisys Ltd 
#define PREF_PATTERNS 11 // Gradient/Pattern - Infinisys Ltd
#define PREF_DUP_PATTERNS 12 // Gradient/Pattern - Infinisys Ltd

#else

#define PREF_COLORS 2
#define PREF_PATTERNS 3
#define PREF_DUP_PATTERNS 4 // Gradient/Pattern - Infinisys Ltd

#endif

#ifndef _PERSONAL_EDITION

#define LAYOUT_MENU_OFFSET 7
#define VIEW_MENU_SIZE 16
#define SET_GRID_INDEX 3
#define ANGLE_SNAP_INDEX 4

#else

#define LAYOUT_MENU_OFFSET 6
#define VIEW_MENU_SIZE 11
#define SET_GRID_INDEX 1
#define ANGLE_SNAP_INDEX 2

#endif

void CMainFrame::OnInitMenu(CMenu* pMenu)
{

   CMDIFrameWnd::OnInitMenu(pMenu);

   CPCDraftView* pView=(CPCDraftView*)GetActiveFrame()->GetActiveView();
   if(pView && pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
   {
      // update Reshape menu items
      pMenu->EnableMenuItem(ID_EDIT_RESHAPE_SMOOTH,MF_BYCOMMAND|(pView->CanReshapeSmooth()?MF_ENABLED:MF_DISABLED));
      pMenu->EnableMenuItem(ID_EDIT_RESHAPE_UNSMOOTH,MF_BYCOMMAND|(pView->CanReshapeUnsmooth()?MF_ENABLED:MF_DISABLED));
   }

   CMenu* pPopupMenu;
   CPCDraftDoc *pDoc=CPCDraftDoc::GetActiveDocument();

   if(pDoc)
   {
      // first find File menu
      int nFileMenu=-1;
      do
      {
         pPopupMenu=pMenu->GetSubMenu(++nFileMenu);
         if(!pPopupMenu) return;
      }
      while(pPopupMenu->GetMenuItemID(1)!=ID_FILE_OPEN);

      CString strMenuName;
      pPopupMenu=pMenu->GetSubMenu(nFileMenu+3);		// Preferences
      if(pPopupMenu)
      {
#ifndef _PERSONAL_EDITION

         m_LineStyleMenu.Detach();	// HMENU will be deleted by ModifyMenu
         m_LineStyleMenu.Create(pDoc->m_DashStyles,ID_EDIT_STYLES_BASE,FALSE);
         strMenuName.LoadString(IDS_EDIT_LINE_STYLES);
         pPopupMenu->ModifyMenu(PREF_LINE_STYLE,MF_BYPOSITION|MF_ENABLED|MF_STRING|MF_POPUP,(UINT)m_LineStyleMenu.GetSafeHmenu(), strMenuName);

#endif //_PERSONAL_EDITION

         m_ColorMenu.Detach();		// HMENU will be deleted by ModifyMenu
         m_ColorMenu.Create(&pDoc->m_Palette,IDS_COLORS);
         strMenuName.LoadString(IDS_EDIT_COLORS);
         pPopupMenu->ModifyMenu(PREF_COLORS,MF_BYPOSITION|MF_ENABLED|MF_STRING|MF_POPUP,(UINT)m_ColorMenu.GetSafeHmenu(), strMenuName);

         m_PatternMenu.Detach();		// HMENU will be deleted by ModifyMenu
         m_PatternMenu.Create(pDoc->m_packeditema.GetData(),pDoc->m_packeditema.GetCount(),IDS_PATTERNS,ID_PATTERNS_BASE);
         strMenuName.LoadString(IDS_EDIT_PATTERNS);
         pPopupMenu->ModifyMenu(PREF_PATTERNS, MF_BYPOSITION|MF_ENABLED|MF_STRING|MF_POPUP,(UINT)m_PatternMenu.GetSafeHmenu(), strMenuName);


         m_DupPatternMenu.Detach();		// HMENU will be deleted by ModifyMenu // Gradient/Pattern - Infinisys Ltd
         m_DupPatternMenu.Create(pDoc->m_packeditema.GetData(),pDoc->m_packeditema.GetCount(),IDS_PATTERNS,ID_DUP_PATTERNS_BASE); // Gradient/Pattern - Infinisys Ltd
         strMenuName.LoadString(IDS_DUP_PATTERNS); // Gradient/Pattern - Infinisys Ltd
         pPopupMenu->ModifyMenu(PREF_DUP_PATTERNS,MF_BYPOSITION | MF_ENABLED | MF_STRING | MF_POPUP,(UINT)m_DupPatternMenu.GetSafeHmenu(),strMenuName); // Gradient/Pattern - Infinisys Ltd

      }

#ifndef _PERSONAL_EDITION
      pPopupMenu=pMenu->GetSubMenu(nFileMenu+4);		// View
      if(pPopupMenu)
      {
         // clean up
         while(pPopupMenu->GetMenuItemCount()>VIEW_MENU_SIZE) pPopupMenu->DeleteMenu(VIEW_MENU_SIZE,MF_BYPOSITION);
         if(pDoc->m_SavedViews.GetSize()>0)
         {
            pPopupMenu->AppendMenu(MF_SEPARATOR); // placeholder for delete view submenu
            strMenuName.LoadString(IDS_DELETEALLVIEWS);
            pPopupMenu->AppendMenu(MF_ENABLED|MF_STRING,ID_VIEW_DELETE_ALL_VIEWS,strMenuName);
            pPopupMenu->AppendMenu(MF_SEPARATOR);

            CMenu theMenu;
            theMenu.CreatePopupMenu();
            int i = 0;
            for(; i<pDoc->m_SavedViews.GetSize(); i++)
            {
               CSavedView* pView=(CSavedView*)pDoc->m_SavedViews[i];
               theMenu.AppendMenu(MF_ENABLED|MF_STRING|(i%20 || 0==i?0:MF_MENUBARBREAK),ID_VIEWS_BASE+i,pView->m_sTitle);
               if(i<5)
               {
                  pPopupMenu->AppendMenu(MF_ENABLED|MF_STRING,ID_VIEW_SAVED_VIEW1+i,pView->GetMenuTitle());
               }
            }
            strMenuName.LoadString(IDS_DELETEVIEW);
            pPopupMenu->ModifyMenu(VIEW_MENU_SIZE,MF_BYPOSITION|MF_ENABLED|MF_STRING|MF_POPUP,(UINT)theMenu.Detach(),strMenuName);


            if(i>5)
            {
               strMenuName.LoadString(IDS_MOREVIEWS);
               pPopupMenu->AppendMenu(MF_ENABLED|MF_STRING,ID_VIEW_MORE_VIEWS,strMenuName);
            }
         }
      }

      pPopupMenu=pMenu->GetSubMenu(nFileMenu+6);		// Data
      if(pPopupMenu)
      {
         bool bHaveLinked=false;
         CMenu theReportMenu,theUpdateMenu,theBreakMenu;
         strMenuName.LoadString(IDS_PRINTREPORT);
         if(pDoc->m_Reports.GetSize()>0)
         {
            theReportMenu.CreatePopupMenu();
            theUpdateMenu.CreatePopupMenu();
            theBreakMenu.CreatePopupMenu();
            for(int i=0; i<pDoc->m_Reports.GetSize(); i++)
            {
               CMDReport* pReport=pDoc->m_Reports[i];
               theReportMenu.AppendMenu(MF_ENABLED|MF_STRING|(i%20 || 0==i?0:MF_MENUBARBREAK),ID_REPORTS_BASE+i,pReport->m_sTitle);
               if(pReport->IsLinked())
               {
                  bHaveLinked=true;
                  theUpdateMenu.AppendMenu(MF_ENABLED|MF_STRING,ID_UPDATE_LINK_BASE+i,pReport->m_sTitle);
                  theBreakMenu.AppendMenu(MF_ENABLED|MF_STRING,ID_BREAK_LINK_BASE+i,pReport->m_sTitle);
               }
            }
            pPopupMenu->ModifyMenu(5,MF_BYPOSITION|MF_ENABLED|MF_STRING|MF_POPUP,(UINT)theReportMenu.Detach(),strMenuName);
         }
         else pPopupMenu->ModifyMenu(5,MF_BYPOSITION|MF_GRAYED|MF_STRING,ID_DATA_PRINTREPORT,strMenuName);
         if(bHaveLinked)
         {
            strMenuName.LoadString(IDS_UPDATELINK);
            pPopupMenu->ModifyMenu(7,MF_BYPOSITION|MF_ENABLED|MF_STRING|MF_POPUP,(UINT)theUpdateMenu.Detach(),strMenuName);
            strMenuName.LoadString(IDS_BREAKLINK);
            pPopupMenu->ModifyMenu(8,MF_BYPOSITION|MF_ENABLED|MF_STRING|MF_POPUP,(UINT)theBreakMenu.Detach(),strMenuName);
         }
         else
         {
            strMenuName.LoadString(IDS_UPDATELINK);
            pPopupMenu->ModifyMenu(7,MF_BYPOSITION|MF_GRAYED|MF_STRING,ID_DATA_UPDATELINK,strMenuName);
            strMenuName.LoadString(IDS_BREAKLINK);
            pPopupMenu->ModifyMenu(8,MF_BYPOSITION|MF_GRAYED|MF_STRING,ID_DATA_BREAKLINK,strMenuName);
         }
      }

#endif
      
      pPopupMenu=pMenu->GetSubMenu(nFileMenu+LAYOUT_MENU_OFFSET);		// Layout

      if(pPopupMenu)
      {

         // Infinisys Ltd. 2021 - Context Menus
         CreateGridSubMenu(pDoc, pPopupMenu);

         // Infinisys Ltd. 2021 - Context Menus
         CreateAngleSnapSubMenu(pDoc, pPopupMenu);

      }

   }

}


// Infinisys Ltd. 2021 - Context Menus
void CMainFrame::CreateAngleSnapSubMenu(CPCDraftDoc * pDoc, CMenu * pPopupMenu)
{

   int iAngleSnapIndex = -1;

   for (int i = 0; i < pPopupMenu->GetMenuItemCount(); i++)
   {

      if (pPopupMenu->GetMenuItemID(i) == ID_ANGLE_SNAP_BASE)
      {

         iAngleSnapIndex = i;

         break;

      }

   }

   if (iAngleSnapIndex < 0)
   {

      return;

   }

   CString strMenuName;

   {

      CMenu theMenu;
      theMenu.CreatePopupMenu();
      strMenuName.LoadString(IDS_NOANGLESNAP);
      theMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_ANGLE_SNAP_BASE, strMenuName);
      CStringArray strList;
      pDoc->GetAngleSnaps(strList);
      for (int i = 0; i < strList.GetSize(); i++)
      {
         theMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_ANGLE_SNAP_BASE + i + 1, strList[i]);
      }
      strMenuName.LoadString(IDS_SETANGLESNAP);
      pPopupMenu->ModifyMenu(iAngleSnapIndex, MF_BYPOSITION | MF_ENABLED | MF_STRING | MF_POPUP, (UINT)theMenu.Detach(), strMenuName);
   }

}



// Infinisys Ltd. 2021 - Context Menus
void CMainFrame::CreateGridSubMenu(CPCDraftDoc * pDoc, CMenu * pPopupMenu)
{

   int iGridBaseIndex = -1;

   for (int i = 0; i < pPopupMenu->GetMenuItemCount(); i++)
   {

      if (pPopupMenu->GetMenuItemID(i) == ID_GRID_BASE)
      {

         iGridBaseIndex = i;

         break;

      }

   }

   if (iGridBaseIndex < 0)
   {

      return;

   }

   CString strMenuName;

   {
      CMenu theMenu;
      theMenu.CreatePopupMenu();
      strMenuName.LoadString(IDS_NOGRIDSNAP);
      theMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_GRID_BASE, strMenuName);

      CStringArray strList;
      pDoc->GetActiveGridSnaps(strList);
      for (int i = 0; i < strList.GetSize(); i++)
      {
         theMenu.AppendMenu(MF_ENABLED | MF_STRING, ID_GRID_BASE + i + 1, strList[i]);
      }
      strMenuName.LoadString(IDS_SETGRID);
      pPopupMenu->ModifyMenu(iGridBaseIndex, MF_BYPOSITION | MF_ENABLED | MF_STRING | MF_POPUP, (UINT)theMenu.Detach(), strMenuName);
   }

}


void CMainFrame::TrackPopupMenu(int iId) // Interface Update - Infinisys Ltd.
{

   CPCDraftDoc *pDoc=CPCDraftDoc::GetActiveDocument();

   CPCDraftView * pDraftView = NULL;
   if(pDoc != NULL)
   {
      POSITION pos=pDoc->GetFirstViewPosition();
      while(pos != NULL)
      {
         CView* pView=pDoc->GetNextView(pos);
         if(pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
         {
            pDraftView = (CPCDraftView *)pView;
            break;
         }
      }
   }
   int iTrack = -1;
   if(iId == ID_FILE_PRINT_ACTIVE_LAYER)
   {
      CMenu * pmenuFile = m_pMenu->GetSubMenu(0);
      CMenu * pmenuPrintOptions = pmenuFile->GetSubMenu(17);
      CPoint pt;
      GetCursorPos(&pt);
      pt.Offset(18,9);
      iTrack = pmenuPrintOptions->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,pt.x,pt.y,this);
      if(iTrack == 0)
      {
         iTrack = -1;
      }
      else
      {
         iId = 0;
      }
   }
   else if(iId == ID_FILE_MRU_FILE1)
   {
      CMenu menu;
      menu.LoadMenu(IDR_MRU);
      CMenu * pmenuMRU =menu.GetSubMenu(0);
      CPoint pt;
      GetCursorPos(&pt);
      pt.Offset(18,9);
      iTrack = pmenuMRU->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,pt.x,pt.y,this);
      if(iTrack == 0)
      {
         iTrack = -1;
      }
      else
      {
         iId = 0;
      }
   }
   else if(iId == ID_HELP_MANUAL)
   {
#ifdef _PERSONAL_EDITION
      CMenu * pmenuHelp = m_pMenu->GetSubMenu(8);
#else
      CMenu * pmenuHelp = m_pMenu->GetSubMenu(9);
#endif
      CPoint pt;
      GetCursorPos(&pt);
      pt.Offset(18,9);
      iTrack = pmenuHelp->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,pt.x,pt.y,this);
      if(iTrack == 0)
      {
         iTrack = -1;
      }
      else
      {
         iId = 0;
      }
   }
   else if (iId == ID_FILE_WELCOME_NEW)
   {
      CMenu * pmenuEdit = m_pMenu->GetSubMenu(0);
      CMenu * pmenuPasteOptions = pmenuEdit->GetSubMenu(22);
      CPoint pt;
      GetCursorPos(&pt);
      pt.Offset(18, 9);
      iTrack = pmenuPasteOptions->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this);
      if (iTrack == 0)
      {
         iTrack = -1;
      }
      else
      {
         iId = 0;
      }

   }
   else if(iId == ID_EDIT_PASTE_UNSCALED)
   {
      CMenu * pmenuEdit = m_pMenu->GetSubMenu(1);
      CMenu * pmenuPasteOptions = pmenuEdit->GetSubMenu(9);
      CPoint pt;
      GetCursorPos(&pt);
      pt.Offset(18,9);
      iTrack = pmenuPasteOptions->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,pt.x,pt.y,this);
      if(iTrack == 0)
      {
         iTrack = -1;
      }
      else
      {
         iId = 0;
      }
   }
   else if(iId == ID_GUIDES_ADDHORIZONTALGUIDE)
   {
      CMenu * pmenuEdit = m_pMenu->GetSubMenu(3);
#ifdef _PERSONAL_EDITION
      CMenu * pmenuGuides = pmenuEdit->GetSubMenu(21);
#else
      CMenu * pmenuGuides = pmenuEdit->GetSubMenu(14);
#endif
      CPoint pt;
      GetCursorPos(&pt);
      pt.Offset(18,9);
      iTrack = pmenuGuides->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,pt.x,pt.y,this);
      if(iTrack == 0)
      {
         iTrack = -1;
      }
      else
      {
         iId = 0;
      }
   }
   else if(iId == ID_VIEW_ZOOM_IN_4X)
   {
      CMenu * pmenuView = m_pMenu->GetSubMenu(4);
      CMenu * pmenuZoom = pmenuView->GetSubMenu(0);
      CPoint pt;
      GetCursorPos(&pt);
      pt.Offset(18,9);
      iTrack = pmenuZoom->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,pt.x,pt.y,this);
      if(iTrack == 0)
      {
         iTrack = -1;
      }
      else
      {
         iId = 0;
      }
   }
   else if(iId == ID_EDIT_STYLES_BASE)
   {

      if(pDoc && pDraftView != NULL)
      {

         CLineStyleMenu theMenu;
         theMenu.Create(pDoc->m_DashStyles,ID_EDIT_STYLES_BASE,FALSE);
         iTrack = theMenu.Track(pDraftView);

      }

   }
   else if(iId == ID_COLORS_BASE)
   {

      if(pDoc && pDraftView != NULL)
      {

         CColorMenu theMenu;
         theMenu.Create(&pDoc->m_Palette,IDS_COLORS);
         iTrack = theMenu.Track(pDraftView);
      }

   }
   else if(iId == ID_PATTERNS_BASE)
   {

      if(pDoc && pDraftView != NULL)
      {

         CPatternMenu theMenu;
         theMenu.Create(pDoc->m_packeditema.GetData(),pDoc->m_packeditema.GetCount(),IDS_PATTERNS,ID_PATTERNS_BASE);
         iTrack = theMenu.Track(pDraftView);

      }

   }
   else if(iId == ID_DUP_PATTERNS_BASE)
   {

      if(pDoc && pDraftView != NULL)
      {

         CPatternMenu theMenu;
         theMenu.Create(pDoc->m_packeditema.GetData(),pDoc->m_packeditema.GetCount(),IDS_PATTERNS,ID_DUP_PATTERNS_BASE); // Gradient/Pattern - Infinisys Ltd
         iTrack = theMenu.Track(this);

      }

   }
   else if(iId == ID_GRID_BASE)
   {

      if(pDoc && pDraftView != NULL)
      {
         CString strMenuName;
         CMenu theMenu;
         theMenu.CreatePopupMenu();
         strMenuName.LoadString(IDS_NOGRIDSNAP);
         theMenu.AppendMenu(MF_ENABLED | MF_STRING,ID_GRID_BASE,strMenuName);

         CStringArray strList;
         pDoc->GetActiveGridSnaps(strList);
         for(int i=0; i<strList.GetSize(); i++)
         {
            theMenu.AppendMenu(MF_ENABLED | MF_STRING,ID_GRID_BASE + i + 1,strList[i]);
         }
         strMenuName.LoadString(IDS_SETGRID);
         CPoint pt;
         GetCursorPos(&pt);
         pt.Offset(18,9);
         iTrack = theMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,pt.x,pt.y,this);
         if(iTrack == 0)
         {
            iTrack = -1;
         }
         else
         {
            iId = 0;
         }

      }

   }
   else if (iId == ID_EDIT_RESHAPE_SMOOTH)
   {
   CMenu * pmenuEdit = m_pMenu->GetSubMenu(1);
#ifdef _PERSONAL_EDITION
   CMenu * pmenuReshape = pmenuEdit->GetSubMenu(21);
#else
   CMenu * pmenuReshape = pmenuEdit->GetSubMenu(24);
#endif
   CPoint pt;
   GetCursorPos(&pt);
   pt.Offset(18, 9);
   iTrack = pmenuReshape->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this);
   if (iTrack == 0)
   {
      iTrack = -1;
   }
   else
   {
      iId = 0;
   }
   }
   else if(iId == ID_ANGLE_SNAP_BASE)
   {

      if(pDoc && pDraftView != NULL)
      {

         CString strMenuName;
         CMenu theMenu;
         theMenu.CreatePopupMenu();
         strMenuName.LoadString(IDS_NOANGLESNAP);
         theMenu.AppendMenu(MF_ENABLED | MF_STRING,ID_ANGLE_SNAP_BASE,strMenuName);
         CStringArray strList;
         pDoc->GetAngleSnaps(strList);
         for(int i=0; i<strList.GetSize(); i++)
         {
            theMenu.AppendMenu(MF_ENABLED | MF_STRING,ID_ANGLE_SNAP_BASE + i + 1,strList[i]);
         }
         strMenuName.LoadString(IDS_SETANGLESNAP);
         CPoint pt;
         GetCursorPos(&pt);
         pt.Offset(18,9);
         iTrack = theMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,pt.x,pt.y,this);
         if(iTrack == 0)
         {
            iTrack = -1;
         }
         else
         {
            iId = 0;
         }

      }

   }
   if(iTrack >= 0)
   {
      PostMessage(WM_COMMAND,iId + iTrack);

   }


}

void CMainFrame::OnDataFindReplace()
{
   if(m_dlgFindReplace.GetSafeHwnd())
   {
      if(!m_dlgFindReplace.IsWindowVisible())
         m_dlgFindReplace.ShowWindow(SW_SHOW);
      m_dlgFindReplace.BringWindowToTop();
   }
   else
   {
      m_dlgFindReplace.Create(IDD_FIND_REPLACE,this);
      m_dlgFindReplace.ShowWindow(SW_SHOW);
   }
}


void CMainFrame::OnViewLayer() // Layer - Infinisys Ltd.
{

   CPCDraftApp * papp = (CPCDraftApp *) AfxGetApp();

   if(papp->m_pLayerFrame == NULL)
   {

      POSITION pos = papp->m_pLayerDoc->GetFirstViewPosition();

      CLayerMainView * pView = (CLayerMainView *) papp->m_pLayerDoc->GetNextView(pos);

      CLayerFrame * pLayerFrame = (CLayerFrame *)pView->GetParentFrame();

      papp->m_pLayerFrame = pLayerFrame;

      papp->m_pLayerFrame->SetWindowPos(&CWnd::wndTop,100,400,280,400,SWP_SHOWWINDOW);

      papp->m_pLayerFrame->RecalcLayout(TRUE);

      pView->layout();

   }
   else if(!papp->m_pLayerFrame->IsWindowVisible())
   {

      papp->m_pLayerFrame->ShowWindow(SW_RESTORE);

      papp->m_pLayerFrame->ActivateFrame();

   }
   else
   {

      papp->m_pLayerFrame->ShowWindow(SW_HIDE);

   }

}


void CMainFrame::OnUpdateViewLayer(CCmdUI * pCmdUI)
{

   CPCDraftApp * papp = (CPCDraftApp *)AfxGetApp();

   if(papp->m_pLayerFrame == NULL || !papp->m_pLayerFrame->IsWindowVisible())
   {

      pCmdUI->SetCheck(0);

   }
   else
   {

      pCmdUI->SetCheck(1);

   }

}


void CMainFrame::OnLayoutLayerSetup()
{
   if(m_dlgLayerSetup.GetSafeHwnd())
   {
      if(!m_dlgLayerSetup.IsWindowVisible())
         m_dlgLayerSetup.ShowWindow(SW_SHOW);
      m_dlgLayerSetup.BringWindowToTop();
   }
   else
   {
      m_dlgLayerSetup.Create(IDD_LAYER_SETUP,this);
      m_dlgLayerSetup.ShowWindow(SW_SHOW);
   }
}

BOOL CMainFrame::OnQueryNewPalette()
{
   SendMessageToDescendants(WM_QUERYNEWPALETTE);
   return CMDIFrameWnd::OnQueryNewPalette();
}

void CMainFrame::DoIdleUpdate()
{
   if(!((CPCDraftApp*)AfxGetApp())->HasOpenDocuments())
   {
      if(m_dlgFindReplace.GetSafeHwnd())
         m_dlgFindReplace.DestroyWindow();
      if(m_dlgLayerSetup.GetSafeHwnd())
         m_dlgLayerSetup.DestroyWindow();
   }
   {
      
	  // Layer - Infinisys Ltd.
      CPCDraftApp * papp = (CPCDraftApp *)AfxGetApp();

      if(papp->m_pLayerFrame != NULL
         && ::IsWindow(papp->m_pLayerFrame->GetSafeHwnd())
         && papp->m_pLayerFrame->IsWindowVisible())
      {
         //HWND hWnd=::GetWindow(m_hWndMDIClient,GW_CHILD);
         //bool bFoundPCDraftView=false;
         //while(hWnd != NULL)
         //{
         //   CWnd* pWnd=CWnd::FromHandle(hWnd);
         //   if(pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) && ((CMDIChildWnd*)pWnd)->GetActiveView()->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
         //   {
         //      bFoundPCDraftView=true;
         //      break;
         //   }
         //   hWnd=::GetWindow(hWnd,GW_HWNDNEXT);
         //}
         //if(bFoundPCDraftView)
         papp->m_pLayerFrame->DoIdleUpdate();
            //m_dlgLayerSetup.DoIdleUpdate();
         //else
         //   papp->CloseLayerPalette();
      }

   }
   if(m_dlgLayerSetup.GetSafeHwnd() && m_dlgLayerSetup.IsWindowVisible())
   {
      HWND hWnd=::GetWindow(m_hWndMDIClient,GW_CHILD);
      bool bFoundPCDraftView=false;
      while(hWnd!=NULL)
      {
         CWnd* pWnd=CWnd::FromHandle(hWnd);
         if(pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMDIChildWnd)) && ((CMDIChildWnd*)pWnd)->GetActiveView()->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
         {
            bFoundPCDraftView=true;
            break;
         }
         hWnd=::GetWindow(hWnd,GW_HWNDNEXT);
      }
      if(bFoundPCDraftView)
         m_dlgLayerSetup.DoIdleUpdate();
      else
         m_dlgLayerSetup.DestroyWindow();
   }
}


///////////////////
// Handle progress message預ny object can call send one.
//
LRESULT CMainFrame::OnProgress(WPARAM wp, LPARAM lp)
{
   m_wndStatusBar.OnProgress(wp); // pass to prog/status bar
   return 0;
}


void CMainFrame::OnClose()
{


   g_bEndApp = true;
   CPCDraftApp* pApp=(CPCDraftApp*)AfxGetApp();
   WINDOWPLACEMENT wp;
   GetWindowPlacement(&wp);
   pApp->WriteProfileBinary("MainWindow","Placement",(LPBYTE)&wp,sizeof(wp));
   if(pApp->m_bToolSaveLocation)
      SaveBarState("PalleteState");
   else
   {
      int state=0;
      if(pApp->m_bToolDisplay)
      {
#ifndef _PERSONAL_EDITION
         if(m_wndAccPalette.IsWindowVisible()) state|=0x1;

#endif
         if(m_wndDimPalette.IsWindowVisible()) state|=0x2;
         if(m_wndResizePalette.IsWindowVisible()) state|=0x4;
         if(m_wndShowSizePalette.IsWindowVisible()) state|=0x8;
         if(m_wndAlignPalette.IsWindowVisible()) state|=0x10;
      }
      pApp->WriteProfileInt("PalleteState-Summary","Display",state);
   }

   if (pApp->m_pGetStartedWnd)
   {

      pApp->m_pGetStartedWnd->DestroyWindow();

      pApp->m_pGetStartedWnd = nullptr;

   }

   if (pApp->m_pTemplateWnd)
   {

      pApp->m_pTemplateWnd->DestroyWindow();

      pApp->m_pTemplateWnd = nullptr;

   }

   CMDIFrameWnd::OnClose();

}


void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) // Interface Update - Infinisys Ltd.
{

   CMDIFrameWnd::OnGetMinMaxInfo(lpMMI);

   MINMAXINFO* mmi = (MINMAXINFO*)lpMMI;

   CRect rWindow;

   WINDOWPLACEMENT wp;

   GetWindowPlacement(&wp);
   
   rWindow = wp.rcNormalPosition;

   HMONITOR monitor = MonitorFromRect(&rWindow,MONITOR_DEFAULTTONULL);

   if(!monitor)
   {

      return;

   }

   MONITORINFO monitor_info ={0};
   monitor_info.cbSize = sizeof(monitor_info);
   GetMonitorInfo(monitor,&monitor_info);

   RECT work_area = monitor_info.rcWork;
   RECT monitor_rect = monitor_info.rcMonitor;

   ::AdjustWindowRectEx(&work_area,GetStyle(),TRUE,GetExStyle());
   int iCaption =  GetSystemMetrics(SM_CYCAPTION);
   mmi->ptMaxPosition.x = work_area.left - monitor_rect.left;
   mmi->ptMaxPosition.y = work_area.top - monitor_rect.top + 32 + (32- iCaption);
   mmi->ptMaxSize.x = work_area.right - work_area.left;
   mmi->ptMaxSize.y = work_area.bottom - work_area.top - + 32 + (32 - iCaption);
   mmi->ptMinTrackSize.x = 100;
   mmi->ptMinTrackSize.y = 32;

}

void CMainFrame::DoActivateResizePalette()
{
   if(m_wndResizePalette.GetSafeHwnd() && m_wndResizePalette.IsWindowVisible())
   {
      m_wndResizePalette.SetFocus();
   }
}

bool CMainFrame::IsShowSizeVisible()
{
   return m_wndShowSizePalette.IsWindowVisible()?true:false;
}

void CMainFrame::OnUpdateDataFindReplace(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(FALSE);
}

void CMainFrame::OnUpdateLayoutLayerSetup(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(FALSE);
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{

   CPCDraftApp * papp = (CPCDraftApp *)AfxGetApp();

//   CMDIFrameWnd::OnActivate(nState,pWndOther,bMinimized);

   //CWnd::OnActivate(nState,pWndOther,bMinimized);

   //// hide the menu bar
   //if(nState == WA_INACTIVE && (m_dwMenuBarVisibility & AFX_MBV_KEEPVISIBLE) == 0)
   //{
   //   SetMenuBarState(AFX_MBS_HIDDEN);
   //}

   //// get top level frame unless this is a child window
   //// determine if window should be active or not
   //CFrameWnd* pTopLevel = (GetStyle() & WS_CHILD) ? this : GetTopLevelFrame();
   //ENSURE_VALID(pTopLevel);
   //CWnd* pActive = (nState == WA_INACTIVE ? pWndOther : this);
   //BOOL bStayActive = (pActive != NULL) ?
   //   (pTopLevel == pActive ||
   //   (pTopLevel == pActive->GetTopLevelFrame() &&
   //   (pActive == pTopLevel ||
   //   pActive->SendMessage(WM_FLOATSTATUS,FS_SYNCACTIVE) != 0)))
   //   : FALSE;
   //pTopLevel->m_nFlags &= ~WF_STAYACTIVE;
   //if(bStayActive)
   //   pTopLevel->m_nFlags |= WF_STAYACTIVE;

   //// sync floating windows to the new state
   //NotifyFloatingWindows(bStayActive ? FS_ACTIVATE : FS_DEACTIVATE);

   //// get active view (use active frame if no active view)
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


   OnNcPaint();

   if (nState > 0)
   {

      if (papp->m_pTemplateWnd && papp->m_pTemplateWnd->IsWindowVisible())
      {

         if (papp->m_pTemplateWnd != papp->m_pTemplateWnd->GetFocus())
         {

            papp->m_pTemplateWnd->BringWindowToTop();

         }

      }

   }

   //RedrawWindow(NULL,NULL,RDW_UPDATENOW);
   /*	if(nState==WA_ACTIVE && bMinimized && m_listHiddenBars.GetCount()==0)
   	{
   		POSITION pos=m_listControlBars.GetHeadPosition();
   		while (pos != NULL)
   		{
   			CControlBar* pBar = (CControlBar*)m_listControlBars.GetNext(pos);
   			if(pBar->IsFloating() && pBar->GetParentFrame()->IsWindowVisible())
   			{
   				m_listHiddenBars.AddTail(pBar);
   				ShowControlBar(pBar,FALSE,FALSE);
   			}
   		}
   	}*/
}

void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
   if(bActive)
   {
      CPCDraftApp * papp = (CPCDraftApp *)AfxGetApp();
      if (papp->m_pTemplateWnd && papp->m_pTemplateWnd->IsWindowVisible())
      {

         if (papp->m_pTemplateWnd != papp->m_pTemplateWnd->GetFocus())
         {

            papp->m_pTemplateWnd->BringWindowToTop();

         }

      }
      else

//		if(dwThreadID)
      {
         POSITION pos = m_listHiddenBars.GetHeadPosition();
         while (pos != NULL)
         {
            CControlBar* pBar = (CControlBar*)m_listHiddenBars.GetNext(pos);
            ShowControlBar(pBar,TRUE,FALSE);
         }
      }
   }
   else
   {
      m_listHiddenBars.RemoveAll();
      POSITION pos=m_listControlBars.GetHeadPosition();
      while (pos != NULL)
      {
         CControlBar* pBar = (CControlBar*)m_listControlBars.GetNext(pos);
         if(pBar->IsFloating() && pBar->GetParentFrame()->IsWindowVisible())
         {
            m_listHiddenBars.AddTail(pBar);
            ShowControlBar(pBar,FALSE,FALSE);
         }
      }
   }
}

LRESULT CMainFrame::OnNetGuardMsg(WPARAM wParam,LPARAM lParam)
{
   char* s=(char*)lParam;
   char* c=s;
   while(*c)
   {
      if(*c=='\t')
      {
         *c++=0;
         break;
      }
      c++;
   }
   char* n=c;
   while(*n)
   {
      if(*n=='\t')
      {
         *n++=0;
         break;
      }
      n++;
   }

   AfxGetApp()->WriteProfileInt("Registration","ForceDialog",1);
   // If we were started up for DDE restore the Show state
   if (AfxGetApp()->m_pCmdInfo != NULL)
   {
      AfxGetApp()->m_nCmdShow = (int)AfxGetApp()->m_pCmdInfo;
      AfxGetApp()->m_pCmdInfo = NULL;
   }
   CString sText;
   sText.Format(IDS_SERIAL_CLASH,s,n,c);
   AfxMessageBox(LPCTSTR(sText));

// GK: I replace destroy main window with ID_APP_EXIT to take a possibility to save working files if they are opened
//	DestroyWindow();
//
   HWND hWnd =  m_hWnd;

   while (::IsWindow (hWnd))
   {
      AfxGetApp()->OnCmdMsg (ID_APP_EXIT, 0, 0, 0);
   }
   return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
   m_ToolTip.RelayEvent(pMsg);
   return CMDIFrameWnd::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnTipCustomDraw(UINT /*id*/, NMHDR* pNMHDR, LRESULT* pResult)
{
   LPNMTTCUSTOMDRAW pc=LPNMTTCUSTOMDRAW(pNMHDR);
   pc->uDrawFlags&=~DT_SINGLELINE;
   pc->uDrawFlags|=DT_EXPANDTABS;
   *pResult=CDRF_DODEFAULT;
   return TRUE;
}

BOOL CMainFrame::OnToolTipNotify(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
   static char szTipsBuffer[512];
   static WCHAR wszTipsBuffer[512];

   // need to handle both ANSI and UNICODE versions of the message
   TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
   TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
   UINT nID = pNMHDR->idFrom;
   if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
         pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
   {
      // idFrom is actually the HWND of the tool
      nID = ::GetDlgCtrlID((HWND)nID);
   }
   if(nID)
   {
      static CString theTips;
      if(theTips.LoadString(nID))
      {

         if (pNMHDR->code == TTN_NEEDTEXTA)
         {
            lstrcpyn(szTipsBuffer, theTips, 512);
            pTTTA->lpszText=szTipsBuffer;
         }
         else
         {
            _mbstowcsz(wszTipsBuffer, theTips, 512);
            pTTTW->lpszText=wszTipsBuffer;
         }

         *pResult = 0;

         // bring the tooltip window above other popup windows
         ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
                        SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

         return TRUE;
      }
   }
   return FALSE;
}

void CMainFrame::UpdatePallettesForCommand(UINT nId)
{
   m_wndToolPalette.SelectButtonWithId(nId);
}

using namespace Gdiplus;


// Interface Upgrade - Infinisys Ltd.
void CMainFrame::OnSize(UINT nType,int cx,int cy)
{

   CMDIFrameWnd::OnSize(nType,cx,cy);

   CRect r;

   GetWindowRect(r);

   RedrawWindow(NULL,NULL,RDW_UPDATENOW | RDW_INVALIDATE);
   
}


// Interface Upgrade - Infinisys Ltd.
LRESULT CMainFrame::OnDwm(WPARAM wparam,LPARAM lparam)
{

   BOOL enabled = FALSE;

   DwmIsCompositionEnabled(&enabled);

   if(enabled)
   {

      MARGINS m={0,0,0,1};

      DWORD  dw=DWMNCRP_ENABLED;

      DwmExtendFrameIntoClientArea(GetSafeHwnd(),&m );

      DwmSetWindowAttribute(GetSafeHwnd(),DWMWA_NCRENDERING_POLICY, &dw,sizeof(DWORD));

   }

   return 0;

}



// Interface Upgrade - Infinisys Ltd.
void CMainFrame::OnPaint()
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
   font.CreatePointFont(100,"Segoe UI",&dc);
   ::GetWindowText(GetSafeHwnd(),sz,sizeof(sz));
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

   dc.FillSolidRect(&rWindow,0);

   ::DrawIconEx(dc.GetSafeHdc(),4,4,m_hiconCaption,24,24,0,NULL,DI_IMAGE);

   int x1 = 24 + 4;

   int x = 52;

    int xm = (x + x1) / 2;

   int mspan = xm - x1;


   if(rPaint.Width() - s1.cx - 120 >= 136)
   {
      m_bTopLeftButtons = true;
      dc.FillSolidRect(xm,8,2,16,RGB(70,70,70));


      Gdiplus::Graphics g(dc.GetSafeHdc());
      g.SetCompositingMode(Gdiplus::CompositingMode::CompositingModeSourceOver);
      int w = m_pbitmapSave->GetWidth();
      int h = m_pbitmapSave->GetHeight();
//      g.DrawImage(m_pbitmapSave,x,2,0, 0,20,20,Gdiplus::UnitPixel);
      ::DrawIconEx(dc.GetSafeHdc(),x,6,m_hiconSave,20,20,0,NULL,DI_IMAGE);

      x += 28;

      //g.DrawImage(m_pbitmapUndo,x,2,0,0,20,20,Gdiplus::UnitPixel);
      ::DrawIconEx(dc.GetSafeHdc(),x,6,m_hiconUndo,20,20,0,NULL,DI_IMAGE);
      x += 28;

      //g.DrawImage(m_pbitmapRedo,x,2,0,0,20,20,Gdiplus::UnitPixel);

      ::DrawIconEx(dc.GetSafeHdc(),x,6,m_hiconRedo,20,20,0,NULL,DI_IMAGE);
      x += 20;

      x += mspan;

      dc.FillSolidRect(x,8,2,16,RGB(70,70,70));
      rCaption.left = x;
   }
   else
   {
      rCaption.left = 24 + 4;
      m_bTopLeftButtons = false;

      if(rPaint.Width() - s1.cx - 120 - 52 <0)
      {

         CString strDefaultTitle;

         strDefaultTitle.LoadStringA(IDR_MAINFRAME);
         strcpy(sz,strDefaultTitle);
         dc.SelectObject(&font);
         s1 = dc.GetTextExtent(sz);
         if(rPaint.Width() - s1.cx - 120  - 52<0)
         {
            s1.cx = 0;
         }

      }

   }

   


   CRect rLine=rWindow;

   rLine.top = rLine.bottom - 1;

   Graphics gr(dc.GetSafeHdc());

   Pen pen(Color(255,0,0,0));

   gr.DrawLine(&pen, rLine.left, rLine.bottom-1, rLine.right, rLine.bottom-1);

   
   Gdiplus::StringFormat format(Gdiplus::StringFormat::GenericDefault());

   format.SetAlignment(Gdiplus::StringAlignmentCenter);
   format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
   format.SetTrimming(Gdiplus::StringTrimmingWord);
   format.SetFormatFlags(0);

   Gdiplus::Font f(L"Marlett", 10.0);
   SolidBrush brText(Color(255, 255, 255, 255));
   RectF r2;
   copy(r2,&rWindow);
   r2.X = r2.X + r2.Width - 40;
   r2.Height =40;
   r2.Width = 40;
   
   gr.DrawString(L"r",1,&f,r2,&format,&brText);
   r2.X -= 40;
   if(IsZoomed())
   {
      gr.DrawString(L"2",1,&f,r2,&format,&brText);
   }
   else
   {
      gr.DrawString(L"1",1,&f,r2,&format,&brText);
   }
   r2.X -= 40;
   gr.DrawString(L"0",1,&f,r2,&format,&brText);
   rCaption.right = r2.X;
   dc.SelectClipRgn(NULL);

   dc.SelectObject(&font);
   dc.SetBkMode(TRANSPARENT);

   dc.SetTextColor(RGB(255,255,255));
   if(s1.cx > 0)
   {
      CStringW wstr = get_uni(sz, CP_ACP);
      //CStringW wstr = sz;
      ::DrawTextW(dc.GetSafeHdc(), wstr, wstr.GetLength(), &rCaption,DT_CENTER | DT_VCENTER | DT_SINGLELINE);
   }
}


// Interface Upgrade - Infinisys Ltd.
LRESULT CMainFrame::OnVoid(WPARAM wparam,LPARAM lparam)
{
   return 0;
}

// Interface Upgrade - Infinisys Ltd.
void CMainFrame::OnNcCalcSize(BOOL bCalcValidRects,NCCALCSIZE_PARAMS* lpncsp)
{

   RECT nonclient = lpncsp->rgrc[0];
   CMDIFrameWnd::OnNcCalcSize(bCalcValidRects,lpncsp);
   RECT client = lpncsp->rgrc[0];
   if(IsZoomed())
   {
      WINDOWINFO wi={};
      wi.cbSize = sizeof(wi);
      GetWindowInfo(&wi);
      HMONITOR mon = MonitorFromWindow(GetSafeHwnd(),MONITOR_DEFAULTTOPRIMARY);
      MONITORINFO mi ={};
      mi.cbSize = sizeof(mi);
      GetMonitorInfoW(mon,&mi);

      /* Maximized windows always have a non-client border that hangs over
      the edge of the screen, so the size proposed by WM_NCCALCSIZE is
      fine. Just adjust the top border to remove the window title. */
      lpncsp->rgrc[0].left = client.left;
      lpncsp->rgrc[0].top = 0;
      //lpncsp->rgrc[0].top = nonclient.top + wi.cyWindowBorders;
      ////lpncsp->rgrc[0].top -= (wi.cyWindowBorders + 1);
      //lpncsp->rgrc[0].top = nonclient.top;
      lpncsp->rgrc[0].right = client.right;
      lpncsp->rgrc[0].bottom = mi.rcWork.bottom;


      /* If the client rectangle is the same as the monitor's rectangle,
      the shell assumes that the window has gone fullscreen, so it removes
      the topmost attribute from any auto-hide appbars, making them
      inaccessible. To avoid this, reduce the size of the client area by
      one pixel on a certain edge. The edge is chosen based on which side
      of the monitor is likely to contain an auto-hide appbar, so the
      missing client area is covered by it. */
      if(EqualRect(&lpncsp->rgrc[0],&mi.rcMonitor)) {
         if(has_autohide_appbar(ABE_BOTTOM,mi.rcMonitor))
            lpncsp->rgrc[0].bottom--;
         else if(has_autohide_appbar(ABE_LEFT,mi.rcMonitor))
            lpncsp->rgrc[0].left++;
         else if(has_autohide_appbar(ABE_TOP,mi.rcMonitor))
            lpncsp->rgrc[0].top++;
         else if(has_autohide_appbar(ABE_RIGHT,mi.rcMonitor))
            lpncsp->rgrc[0].right--;
      }
   }
   else {
      /* For the non-maximized case, set the output RECT to what it was
      before WM_NCCALCSIZE modified it. This will make the client size the
      same as the non-client size. */

      lpncsp->rgrc[0] = nonclient;
      
   }

}

// Interface Upgrade - Infinisys Ltd.
LRESULT CMainFrame::OnNcHitTest(CPoint mouse)
{

   if(IsZoomed())
      return HTCLIENT;

   CRect rC;
   GetClientRect(rC);
   
   ScreenToClient(&mouse);

   int frame_size = GetSystemMetrics(SM_CXFRAME) +  GetSystemMetrics(SM_CXPADDEDBORDER);
   int diagonal_width = frame_size * 2 + GetSystemMetrics(SM_CXBORDER);

   if(mouse.y < frame_size)
   {
      if(mouse.x < diagonal_width)
         return HTTOPLEFT;
      if(mouse.x >= rC.Width() - diagonal_width)
         return HTTOPRIGHT;
      return HTTOP;
   }

   if(mouse.y >= rC.Height() - frame_size)
   {
      if(mouse.x < diagonal_width)
         return HTBOTTOMLEFT;
      if(mouse.x >= rC.Width() - diagonal_width)
         return HTBOTTOMRIGHT;
      return HTBOTTOM;
   }

   if(mouse.x < frame_size)
      return HTLEFT;
   if(mouse.x >= rC.Width() - frame_size)
      return HTRIGHT;
   return HTCLIENT;

}


// Interface Upgrade - Infinisys Ltd.
void CMainFrame::RecalcLayout(BOOL bNotify)
{
   if(m_bInRecalcLayout)
      return;

   CRect rC;

   GetClientRect(rC);

   rC.top += 32;
   rC.left++;
   rC.right--;
   rC.bottom--;

   m_bInRecalcLayout = TRUE;
   // clear idle flags for recalc layout if called elsewhere
   if(m_nIdleFlags & idleNotify)
      bNotify = TRUE;
   m_nIdleFlags &= ~(idleLayout | idleNotify);

   //if (m_bWelcomeScreen)
   //{

   //   CRect rectClient;

   //   GetClientRect(rectClient);

   //   m_pwelcomewnd->SetWindowPos(&CWnd::wndTop, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_SHOWWINDOW);

   //}

   // call the layout hook -- OLE support uses this hook
   //if(bNotify && m_pNotifyHook != NULL)
   //   m_pNotifyHook->OnRecalcLayout();

   // reposition all the child windows (regardless of ID)
   if(GetStyle() & FWS_SNAPTOBARS)
   {
      CRect rect(0,0,32767,32767);
      RepositionBars(0,0xffff,AFX_IDW_PANE_FIRST,reposQuery,
         &rect,&rect,FALSE);
      RepositionBars(0,0xffff,AFX_IDW_PANE_FIRST,reposExtra,
         &m_rectBorder,&rect,TRUE);
      CalcWindowRect(&rect);
      SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),
         SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
   }
   else
      RepositionBars(0,0xffff,AFX_IDW_PANE_FIRST,reposExtra, &m_rectBorder, rC);
   m_bInRecalcLayout = FALSE;
}


// Interface Upgrade - Infinisys Ltd.
void CMainFrame::OnLButtonDown(UINT nFlags,CPoint point)
{
   
   CRect rectClient;

   GetClientRect(rectClient);

   CRect r(rectClient);
      
   r = rectClient;

   r.left = 52;
   r.right = r.left + 28;

   if(r.PtInRect(point))
   {

      PostMessage(WM_COMMAND, ID_FILE_SAVE);

      return;

   }

   r.left = r.right;
   r.right = r.left + 28;

   if(r.PtInRect(point))
   {

      PostMessage(WM_COMMAND,ID_EDIT_UNDO);

      return;

   }

   r.left = r.right;
   r.right = r.left + 28;

   if(r.PtInRect(point))
   {

      PostMessage(WM_COMMAND,ID_EDIT_REDO);

      return;

   }

   r = rectClient;

   r.left = r.right - 40;
   r.bottom = 40;

   if(r.PtInRect(point))
   {

      PostMessage(WM_CLOSE);

      return;

   }

   r.right = r.left;
   r.left = r.right - 40;
   if(r.PtInRect(point))
   {

      if(IsZoomed())
      {

         ShowWindow(SW_RESTORE);

      }
      else
      {

         ShowWindow(SW_MAXIMIZE);

      }

      RedrawWindow(NULL,NULL,RDW_FRAME | RDW_UPDATENOW | RDW_INVALIDATE);

      return;

   }
   r.right = r.left;
   r.left = r.right - 40;
   if(r.PtInRect(point))
   {

      ShowWindow(SW_MINIMIZE);
      return;

   }
   r.right = r.left;
   r.left = 0;

   if(r.PtInRect(point))
   {
      SendMessage(WM_SYSCOMMAND,0xf012) ;

      return;
   }



   CMDIFrameWnd::OnLButtonDown(nFlags,point);

}

// Interface Upgrade - Infinisys Ltd.
CMenu * CMainFrame::GetMenu()
{
   return m_pMenu;
}


// Interface Upgrade - Infinisys Ltd.
void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
   // TODO: Add your specialized code here and/or call the base class

   if ((GetStyle() & FWS_ADDTOTITLE) == 0)
      return;     // leave it alone!

   // allow hook to set the title (used for OLE support)
   //if (m_pNotifyHook != NULL && m_pNotifyHook->OnUpdateFrameTitle())
     // return;

   CMDIChildWnd* pActiveChild = NULL;
   CDocument* pDocument = GetActiveDocument();
   if (bAddToTitle &&
      (pActiveChild = MDIGetActive()) != NULL &&
      (pActiveChild->GetStyle() & WS_MAXIMIZE) == 0 &&
      (pDocument != NULL ||
      (pDocument = pActiveChild->GetActiveDocument()) != NULL))
      UpdateFrameTitleForDocument2(pDocument->GetTitle());
   else
   {
      LPCTSTR lpstrTitle = NULL;
      CString strTitle;

      if (pActiveChild != NULL &&
         (pActiveChild->GetStyle() & WS_MAXIMIZE) == 0)
      {
         strTitle = pActiveChild->GetTitle();
         if (!strTitle.IsEmpty())
            lpstrTitle = strTitle;
      }
      UpdateFrameTitleForDocument2(lpstrTitle);
   }
   RedrawWindow(NULL, NULL, RDW_NOCHILDREN | RDW_INVALIDATE| RDW_NOFRAME);
}
void AFXAPI AfxSetWindowText2(HWND hWndCtrl, LPCWSTR lpszNew)
{

   ENSURE(lpszNew);

   CString str = get_cp(lpszNew, CP_ACP);

   ENSURE(hWndCtrl);
   
   size_t nNewLen = str.GetLength();
   CHAR szOld[256] = "";
   // fast check to see if text really changes (reduces flash in controls)
   if (nNewLen > _countof(szOld) ||
      ::GetWindowText(hWndCtrl, szOld, _countof(szOld)) != static_cast<int>(nNewLen) ||
      strcmp(szOld, str) != 0)
   {
      // change it


      ::SetWindowText(hWndCtrl, str);
   }

}
void CMainFrame::UpdateFrameTitleForDocument2(LPCTSTR lpszDocName)
{
   CString WindowText;

   if (GetStyle() & FWS_PREFIXTITLE)
   {
      // get name of currently active view
      if (lpszDocName != NULL)
      {
         WindowText += lpszDocName;

         // add current window # if needed
         if (m_nWindow > 0)
         {
            TCHAR szText[32];

            // :%d will produce a maximum of 11 TCHARs
            _stprintf_s(szText, _countof(szText), _T(":%d"), m_nWindow);
            WindowText += szText;
         }
         WindowText += _T(" - ");
      }
      WindowText += m_strTitle;
   }
   else
   {
      // get name of currently active view
      WindowText += m_strTitle;
      if (lpszDocName != NULL)
      {
         WindowText += _T(" - ");
         WindowText += lpszDocName;

         // add current window # if needed
         if (m_nWindow > 0)
         {
            TCHAR szText[32];

            // :%d will produce a maximum of 11 TCHARs
            _stprintf_s(szText, _countof(szText), _T(":%d"), m_nWindow);
            WindowText += szText;
         }
      }
   }

   // set title if changed, but don't remove completely
   // Note: will be excessive for MDI Frame with maximized child
   AfxSetWindowText2(m_hWnd, get_uni(WindowText));
}


// Infinisys Ltd.
BOOL CMainFrame::OnCopyData(CWnd* pWnd,COPYDATASTRUCT* pCopyDataStruct)
{
   if(pCopyDataStruct->dwData == 1000)
   {

      int argc = 0;
      LPWSTR * argv = ::CommandLineToArgvW((LPCWSTR)pCopyDataStruct->lpData,&argc);

      CStringArray stra;

      for(int i = 1; i < argc; i++)
      {

         if(argv[i][0] == L'-' || argv[i][0] == L'/')
         {
         }
         else
         {
            CString str(get_cp(argv[i]));
            AfxGetApp()->OpenDocumentFile(str);
         }
      }

      //::MessageBox(NULL,"COPYDATA","COPYDATa",MB_OK);


         return TRUE;
   }

   return CMDIFrameWnd::OnCopyData(pWnd,pCopyDataStruct);
}


void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
   CMDIFrameWnd::OnShowWindow(bShow, nStatus);

   if (m_bIsFirstShow)
   {

      // should always be visible initially
      if (!m_wndToolPalette.IsWindowVisible()) ShowControlBar(&m_wndToolPalette, TRUE, FALSE);
      //if(!m_wndAttribPalette.IsWindowVisible()) ShowControlBar(&m_wndAttribPalette,TRUE,FALSE);

      m_bIsFirstShow = false;

   }
   // TODO: Add your message handler code here
}


int CMainFrame::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
   // TODO: Add your message handler code here and/or call default

   return CMDIFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}


afx_msg int CMainFrame::OnSetText(LPCTSTR lpszText)
{
   return 0;
}
