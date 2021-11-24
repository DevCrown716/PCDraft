// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "BaseWnd.h"
#include "../PCDraft.h"
#include "GetStartedWnd.h"
#include "TemplateWnd.h"
#include "../resource.h"
#include "memory_dc.h"

UINT window_dpi(HWND hwnd);

BOOL AFXAPI AfxEndDeferRegisterClass(LONG fToRegister);
bool has_autohide_appbar(UINT edge, RECT mon); // Interface Update - Infinisys Ltd.
#define AFX_WNDFRAMEORVIEW_REG          0x00008
#define AfxDeferRegisterClass(fClass) AfxEndDeferRegisterClass(fClass)

extern const TCHAR _afxWndFrameOrView[];

BEGIN_MESSAGE_MAP(BaseWnd, CWnd)
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_WM_NCCALCSIZE()
   ON_WM_ACTIVATE()
   ON_WM_ERASEBKGND()
   ON_WM_NCHITTEST()
END_MESSAGE_MAP()




CSize GetDesktopSize()
{
   CRect desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   return CSize(desktop.Width(), desktop.Height());
}

BaseWnd::BaseWnd()
{

   m_bControlBox = true;
   m_bDefaultDrawBackground = true;
   m_colorBackground = Gdiplus::Color(255, 255, 255, 255);
   m_bResizeGrip = true;
   m_bResizable = true;
   CSize sizeDesktop = GetDesktopSize();

   //m_dSizeScaler =(double) sizeDesktop.cy / 900.0;

   m_dSizeScaler = (double)sizeDesktop.cy / 720.0;

   m_pWndFocus = nullptr;

   m_hWndTop = nullptr;

   m_pParentWnd = nullptr;

}

BaseWnd::~BaseWnd()
{


}


void BaseWnd::OnEditChangeText(int nID)
{

}


void BaseWnd::ShowFonts(Gdiplus::Graphics * pgraphics)
{

   Gdiplus::FontFamily   fontFamily(L"Arial");
   Gdiplus::Font         font(&fontFamily, 8, Gdiplus::FontStyleRegular, Gdiplus::UnitPoint);
   Gdiplus::RectF        rectF(400.0f, 10.0f, 800.0f, 500.0f);
   Gdiplus::SolidBrush   solidBrush(Gdiplus::Color(255, 0, 0, 0));

   INT          count = 0;
   INT          found = 0;
   WCHAR        familyName[LF_FACESIZE];  // enough space for one family name
   WCHAR*       familyList = NULL;
   Gdiplus::FontFamily*  pFontFamily = NULL;

   Gdiplus::InstalledFontCollection installedFontCollection;

   // How many font families are installed?
   count = installedFontCollection.GetFamilyCount();

   // Allocate a buffer to hold the array of FontFamily
   // objects returned by GetFamilies.
   pFontFamily = new Gdiplus::FontFamily[count];

   // Get the array of FontFamily objects.
   installedFontCollection.GetFamilies(count, pFontFamily, &found);

   // The loop below creates a large string that is a comma-separated
   // list of all font family names.
   // Allocate a buffer large enough to hold that string.
   familyList = new WCHAR[count*(sizeof(familyName) + 3)];
   wcscpy(familyList, L"");

   for (INT j = 0; j < count; ++j)
   {
      pFontFamily[j].GetFamilyName(familyName);
      wcscat(familyList, familyName);
      wcscat(familyList, L",  ");
   }

   // Draw the large string (list of all families) in a rectangle.
   pgraphics->DrawString(
      familyList, -1, &font, rectF, NULL, &solidBrush);

   delete[] pFontFamily;
   delete[] familyList;

}



void BaseWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
   // TODO: Add your message handler code here and/or call default

   //ShowWindow(SW_HIDE);

   CWnd::OnLButtonUp(nFlags, point);


}


void BaseWnd::OnMButtonDblClk(UINT nFlags, CPoint point)
{
   // TODO: Add your message handler code here and/or call default

   CWnd::OnMButtonDblClk(nFlags, point);
}


void BaseWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
   // TODO: Add your message handler code here and/or call default

   CWnd::OnLButtonDblClk(nFlags, point);
}


int BaseWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (m_bControlBox)
   {

      m_controlbox.m_colorBackground = m_colorBackground;
      m_controlbox.m_buttonMinimize.m_colorBackground = m_colorBackground;
      m_controlbox.m_buttonClose.m_colorBackground = m_colorBackground;

   }

   if (CWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   int iDpi = window_dpi(GetSafeHwnd());

   if (iDpi == 0)
   {

      iDpi = 96.0;

   }

   m_dDpiScaler = (double) iDpi /96.0;

   if (m_bControlBox)
   {

      m_controlbox.Create(NULL, "", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 10);

   }

   // TODO:  Add your specialized creation code here

   return 0;
}



void BaseWnd::OnSize(UINT nType, int cx, int cy)
{
   
   CWnd::OnSize(nType, cx, cy);

   if (m_bControlBox)
   {

      int iSquare = 40 * size_scaler();

      int w = iSquare * 2;

      int h = iSquare;

      int x = cx - w - 1;

      m_controlbox.SetWindowPos(nullptr, x, 1, w, h, SWP_SHOWWINDOW | SWP_NOZORDER);

   }

}


void BaseWnd::OnPaint()
{

   CPaintDC dcPaint(this); // device context for painting
                      // TODO: Add your message handler code here
                      // Do not call CWnd::OnPaint() for painting messages

   dcPaint.SelectClipRgn(NULL);
   memory_dc dc(dcPaint, this);
   dc.SelectClipRgn(NULL);

   CDC * pdc = &dc;

   Gdiplus::Graphics g(pdc->GetSafeHdc());

   Gdiplus::Graphics * pgraphics = &g;

   pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   pgraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
   pgraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
   pgraphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
   pgraphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);


   CRect rectClient;

   GetClientRect(rectClient);

   if (m_bDefaultDrawBackground)
   {

      Gdiplus::SolidBrush brush(m_colorBackground);

      Gdiplus::RectF rClient;

      rClient.X = 0;
      rClient.Y = 0;
      rClient.Width = rectClient.Width();
      rClient.Height = rectClient.Height();

      pgraphics->FillRectangle(&brush, rClient);

   }

   OnDraw(pdc, &g);

   // Border
   {

      Gdiplus::Rect rectBorder;

      rectBorder.Width = rectClient.Width() - 1;

      rectBorder.Height = rectClient.Height() - 1;

      rectBorder.X = 0;

      rectBorder.Y = 0;

      Gdiplus::Pen penBorder(Gdiplus::Color(255, 196, 196, 196), 1.0);

      pgraphics->DrawRectangle(&penBorder, rectBorder);

   }


}


void BaseWnd::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

}


BOOL BaseWnd::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.dwExStyle |= WS_EX_COMPOSITED;

   return TRUE;

}


LPCTSTR BaseWnd::GetIconWndClass(DWORD dwDefaultStyle, UINT nIDResource)
{
   ASSERT_VALID_IDR(nIDResource);
   HINSTANCE hInst = AfxFindResourceHandle(
      ATL_MAKEINTRESOURCE(nIDResource), ATL_RT_GROUP_ICON);
   HICON hIcon = ::LoadIconW(hInst, ATL_MAKEINTRESOURCEW(nIDResource));
   if (hIcon != NULL)
   {
      CREATESTRUCT cs;
      memset(&cs, 0, sizeof(CREATESTRUCT));
      cs.style = dwDefaultStyle;
      AfxDeferRegisterClass(AFX_WNDFRAMEORVIEW_REG);
      cs.lpszClass = _afxWndFrameOrView;

      PreCreateWindow(cs);


      // will fill lpszClassName with default WNDCLASS name
      // ignore instance handle from PreCreateWindow.

      WNDCLASS wndcls;
      if (cs.lpszClass != NULL &&
         GetClassInfo(AfxGetInstanceHandle(), cs.lpszClass, &wndcls) &&
         wndcls.hIcon != hIcon)
      {
         // register a very similar WNDCLASS
         return AfxRegisterWndClass(wndcls.style,
            wndcls.hCursor, wndcls.hbrBackground, hIcon);
      }
   }
   return NULL;        // just use the default
}

BOOL BaseWnd::Create()
{

   CRect rect;

   rect.left = 100;
   rect.top = 100;
   rect.right = 1500;
   rect.bottom = 900;

   DWORD dwDefaultStyle = 0;

   UINT nIDResource = IDR_WELCOME;

   LPCTSTR lpszClass = GetIconWndClass(dwDefaultStyle, nIDResource);
   //DWORD dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
   // DWORD dwStyle = WS_VISIBLE | WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
   DWORD dwStyle = WS_VISIBLE | WS_THICKFRAME;

   if (!CreateEx(0, lpszClass, "",dwStyle, rect, nullptr, 0))
   {

      return FALSE;

   }

   ModifyStyle(WS_MAXIMIZEBOX, 0, SWP_FRAMECHANGED);
   ModifyStyle(WS_MINIMIZEBOX, 0, SWP_FRAMECHANGED);

   CenterWindow();

   return true;


}

void BaseWnd::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) // Interface Update - Infinisys Ltd.
{

   CWnd::OnGetMinMaxInfo(lpMMI);

   MINMAXINFO* mmi = (MINMAXINFO*)lpMMI;

   CRect rWindow;

   WINDOWPLACEMENT wp;

   GetWindowPlacement(&wp);

   rWindow = wp.rcNormalPosition;

   HMONITOR monitor = MonitorFromRect(&rWindow, MONITOR_DEFAULTTONULL);

   if (!monitor)
   {

      return;

   }

   MONITORINFO monitor_info = { 0 };
   monitor_info.cbSize = sizeof(monitor_info);
   GetMonitorInfo(monitor, &monitor_info);

   RECT work_area = monitor_info.rcWork;
   RECT monitor_rect = monitor_info.rcMonitor;

   ::AdjustWindowRectEx(&work_area, GetStyle(), TRUE, GetExStyle());
   int iCaption = GetSystemMetrics(SM_CYCAPTION);
   mmi->ptMaxPosition.x = work_area.left - monitor_rect.left;
   mmi->ptMaxPosition.y = work_area.top - monitor_rect.top + 32 + (32 - iCaption);
   mmi->ptMaxSize.x = work_area.right - work_area.left;
   mmi->ptMaxSize.y = work_area.bottom - work_area.top - +32 + (32 - iCaption);
   mmi->ptMinTrackSize.x = 100;
   mmi->ptMinTrackSize.y = 32;

}



// Interface Upgrade - Infinisys Ltd.
void BaseWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{

   RECT nonclient = lpncsp->rgrc[0];
   CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
   RECT client = lpncsp->rgrc[0];
   if (IsZoomed())
   {
      WINDOWINFO wi = {};
      wi.cbSize = sizeof(wi);
      GetWindowInfo(&wi);
      HMONITOR mon = MonitorFromWindow(GetSafeHwnd(), MONITOR_DEFAULTTOPRIMARY);
      MONITORINFO mi = {};
      mi.cbSize = sizeof(mi);
      GetMonitorInfoW(mon, &mi);

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
      if (EqualRect(&lpncsp->rgrc[0], &mi.rcMonitor)) {
         if (has_autohide_appbar(ABE_BOTTOM, mi.rcMonitor))
            lpncsp->rgrc[0].bottom--;
         else if (has_autohide_appbar(ABE_LEFT, mi.rcMonitor))
            lpncsp->rgrc[0].left++;
         else if (has_autohide_appbar(ABE_TOP, mi.rcMonitor))
            lpncsp->rgrc[0].top++;
         else if (has_autohide_appbar(ABE_RIGHT, mi.rcMonitor))
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




INT_PTR BaseWnd::DoModal()
{
   //// can be constructed with a resource template or InitModalIndirect
   //ASSERT(m_lpszTemplateName != NULL || m_hDialogTemplate != NULL ||
   //   m_lpDialogTemplate != NULL);

   //// load resource as necessary
   //LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
   //HGLOBAL hDialogTemplate = m_hDialogTemplate;
   //HINSTANCE hInst = AfxGetResourceHandle();
   //if (m_lpszTemplateName != NULL)
   //{
   //   hInst = AfxFindResourceHandle(m_lpszTemplateName, RT_DIALOG);
   //   HRSRC hResource = ::FindResource(hInst, m_lpszTemplateName, RT_DIALOG);
   //   hDialogTemplate = LoadResource(hInst, hResource);
   //}
   //if (hDialogTemplate != NULL)
   //   lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hDialogTemplate);

   //// return -1 in case of failure to load the dialog template resource
   //if (lpDialogTemplate == NULL)
   //   return -1;

   // disable parent (before creating dialog)
   HWND hWndParent = PreModal();
   AfxUnhookWindowCreate();
   BOOL bEnableParent = FALSE;
   CWnd* pMainWnd = NULL;
   BOOL bEnableMainWnd = FALSE;
   if (hWndParent && hWndParent != ::GetDesktopWindow() && ::IsWindowEnabled(hWndParent))
   {
      ::EnableWindow(hWndParent, FALSE);
      bEnableParent = TRUE;
      pMainWnd = AfxGetMainWnd();
      if (pMainWnd && pMainWnd->IsFrameWnd() && pMainWnd->IsWindowEnabled())
      {
         //
         // We are hosted by non-MFC container
         // 
         pMainWnd->EnableWindow(FALSE);
         bEnableMainWnd = TRUE;
      }
   }

   try
   {
      // create modeless dialog
      //AfxHookWindowCreate(this);

      if (!Create())
      {
         return -1;

      }

      // enter modal loop
      DWORD dwFlags = MLF_SHOWONIDLE;
      if (GetStyle() & DS_NOIDLEMSG)
         dwFlags |= MLF_NOIDLEMSG;
      VERIFY(RunModalLoop(dwFlags) == m_nModalResult);

      // hide the window before enabling the parent, etc.
      if (m_hWnd != NULL)
         SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW |
            SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
   //}

      //if (!CreateRunDlgIndirect(nullptr, CWnd::FromHandle(hWndParent), nullptr))
      //{
      //   // If the resource handle is a resource-only DLL, the dialog may fail to launch. Use the
      //   // module instance handle as the fallback dialog creator instance handle if necessary.
      //   //CreateRunDlgIndirect(nullptr, CWnd::FromHandle(hWndParent), AfxGetInstanceHandle());
      //}

      //m_bClosedByEndDialog = FALSE;
   }
   catch (...)
   {

   }
      if (bEnableMainWnd)
         pMainWnd->EnableWindow(TRUE);
   if (bEnableParent)
      ::EnableWindow(hWndParent, TRUE);
   if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
      ::SetActiveWindow(hWndParent);

   // destroy modal window
   DestroyWindow();
   PostModal();

   //// unlock/free resources as necessary
   //if (m_lpszTemplateName != NULL || m_hDialogTemplate != NULL)
   //   UnlockResource(hDialogTemplate);
   //if (m_lpszTemplateName != NULL)
   //   FreeResource(hDialogTemplate);

   return m_nModalResult;
}

void BaseWnd::EndDialog(int nResult)
{
   ASSERT(::IsWindow(m_hWnd));

//   m_bClosedByEndDialog = TRUE;

   if (m_nFlags & (WF_MODALLOOP | WF_CONTINUEMODAL))
      EndModalLoop(nResult);

   ::EndDialog(m_hWnd, nResult);
}



HWND BaseWnd::PreModal()
{
   // cannot call DoModal on a dialog already constructed as modeless
   ASSERT(m_hWnd == NULL);

   // allow OLE servers to disable themselves
   CWinApp* pApp = AfxGetApp();
   if (pApp != NULL)
      pApp->EnableModeless(FALSE);

   // find parent HWND
   HWND hWnd = CWnd::GetSafeOwner_(m_pParentWnd->GetSafeHwnd(), &m_hWndTop);

   // hook for creation of dialog
   AfxHookWindowCreate(this);

   // return window to use as parent for dialog
   return hWnd;
}

void BaseWnd::PostModal()
{
   AfxUnhookWindowCreate();   // just in case
   Detach();               // just in case

   // re-enable windows
   if (::IsWindow(m_hWndTop))
      ::EnableWindow(m_hWndTop, TRUE);
   m_hWndTop = NULL;
   CWinApp* pApp = AfxGetApp();
   if (pApp != NULL)
      pApp->EnableModeless(TRUE);
}


void BaseWnd::OnButtonPress(int i)
{

   if (i == ID_FILE_CLOSE)
   {

      ShowWindow(SW_HIDE);

      return;

   }
   else if (i == SW_MINIMIZE)
   {

      ShowWindow(SW_MINIMIZE);

   }
   else if (i == ID_FILE_OPEN)
   {

      if (((CPCDraftApp*)AfxGetApp())->m_pTemplateWnd)
      {

         ((CPCDraftApp*)AfxGetApp())->m_pTemplateWnd->ShowWindow(SW_HIDE);

      }

      if (((CPCDraftApp*)AfxGetApp())->m_pGetStartedWnd)
      {

         ((CPCDraftApp*)AfxGetApp())->m_pGetStartedWnd->ShowWindow(SW_HIDE);

      }

      ((CPCDraftApp*)AfxGetApp())->ShowMainFrame();

      ((CPCDraftApp*)AfxGetApp())->OnFileOpen();

   }
   else if (i == 1000)
   {

      CString strUrl;

      strUrl.LoadString(IDS_URL_WHATSNEW);

      ShellExecute(0, NULL, strUrl, NULL, NULL, SW_SHOWDEFAULT);

      return;

   }
   else if (i == 1001 || i == ID_HELP)
   {

      CString strUrl;

      strUrl.LoadString(IDS_URL_HELPSUPPORT);

      ShellExecute(0, NULL, strUrl, NULL, NULL, SW_SHOWDEFAULT);

      return;

   }
   else if (i == 1002)
   {

      CString strUrl;

      strUrl.LoadString(IDS_URL_COMMUNITY);

      ShellExecute(0, NULL, strUrl, NULL, NULL, SW_SHOWDEFAULT);

      return;

   }


}






double BaseWnd::font_scaler()
{

   //return m_dDpiScaler * m_dSizeScaler * 0.9;
   //return m_dSizeScaler * 0.9;
   return m_dSizeScaler;

}


double BaseWnd::size_scaler()
{

   return m_dSizeScaler;

}




void BaseWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
   CWnd::OnActivate(nState, pWndOther, bMinimized);

   if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE)
   {

      if (m_pWndFocus)
      {

         m_pWndFocus->SetFocus();

      }

   }

}


BOOL BaseWnd::OnEraseBkgnd(CDC* pDC)
{
 
   return TRUE;

}


bool BaseWnd::is_narrow_window()
{

   CRect rectWindow;

   GetWindowRect(rectWindow);

   return rectWindow.Width() < 120;

}


void BaseWnd::OnLButtonDown(UINT nFlags, CPoint point)
{

   CWnd::OnLButtonDown(nFlags, point);
}


LRESULT BaseWnd::OnNcHitTest(CPoint mouse)
{

   if (!m_bResizable)
   {

      return HTCLIENT;

   }

   if (IsZoomed())
      return HTCLIENT;

   CRect rC;
   GetClientRect(rC);

   ScreenToClient(&mouse);

   int frame_size = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
   int diagonal_width = frame_size * 2 + GetSystemMetrics(SM_CXBORDER);
   int frame_sizeGrip = frame_size * 2;
   int diagonal_widthGrip = diagonal_width * 2;

   if (m_bResizeGrip)
   {

      if (mouse.y >= rC.Height() - frame_sizeGrip)
      {

         if (mouse.x >= rC.Width() - diagonal_widthGrip)
            return HTBOTTOMRIGHT;
      }

   }

   if (mouse.y < frame_size)
   {
      if (mouse.x < diagonal_width)
         return HTTOPLEFT;
      if (mouse.x >= rC.Width() - diagonal_width)
         return HTTOPRIGHT;
      return HTTOP;
   }

   if (mouse.y >= rC.Height() - frame_size)
   {
      if (mouse.x < diagonal_width)
         return HTBOTTOMLEFT;
      if (mouse.x >= rC.Width() - diagonal_width)
         return HTBOTTOMRIGHT;
      return HTBOTTOM;
   }

   if (mouse.x < frame_size)
      return HTLEFT;
   if (mouse.x >= rC.Width() - frame_size)
      return HTRIGHT;

   if (mouse.y <= 40)
   {

      return HTCAPTION;

   }

   return HTCLIENT;

}
