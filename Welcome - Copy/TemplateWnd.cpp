// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "../PCDraft.h"
#include "GetStartedWnd.h"
#include "TemplateWnd.h"
#include "TemplateLeftWnd.h"
#include "../resource.h"


BEGIN_MESSAGE_MAP(TemplateWnd, BaseWnd)
   ON_WM_LBUTTONUP()
   ON_WM_MBUTTONDBLCLK()
   ON_WM_LBUTTONDBLCLK()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_GETMINMAXINFO()
   ON_WM_NCCALCSIZE()
   ON_MESSAGE(WM_USER + 121, OnCategorySelChange)
   ON_MESSAGE(WM_USER + 50, OnChangeSearch)
   ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

TemplateWnd::TemplateWnd()
{

   m_escreen = e_screen_templates;
   m_bDefaultDrawBackground = false;
   m_iLeftWindow = 240;
   m_yMainClient = 100;
   m_colorBackground = Gdiplus::Color(255, 242, 242, 242);

}


TemplateWnd::~TemplateWnd()
{


}

void TemplateWnd::OnLButtonUp(UINT nFlags, CPoint point)
{

   BaseWnd::OnLButtonUp(nFlags, point);

}


void TemplateWnd::OnMButtonDblClk(UINT nFlags, CPoint point)
{

   BaseWnd::OnMButtonDblClk(nFlags, point);

}


void TemplateWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{

   BaseWnd::OnLButtonDblClk(nFlags, point);

}


LRESULT TemplateWnd::OnCategorySelChange(WPARAM wparam, LPARAM lparam)
{

   if (wparam == 300)
   {

      int iCategory = m_combobox.GetList()->m_iCurSel;

      m_categorywnd.UpdateList();

   }

   return 0;

}


LRESULT TemplateWnd::OnChangeSearch(WPARAM wparam, LPARAM lparam)
{

   CString strFilter;

   m_searchedit.GetWindowText(strFilter);

   if (m_recentwnd.IsWindowVisible())
   {

      m_recentwnd.UpdateList(strFilter);
         

   }
   else if (m_categorywnd.IsWindowVisible())
   {

      m_categorywnd.UpdateList(get_uni(strFilter, CP_ACP));


   }

   return 0;

}


CStringW TemplateWnd::GetCategory()
{

   int iSel = m_combobox.GetList()->m_iCurSel;

   if (iSel <= 0)
   {

      return "";

   }

   return m_combobox.GetList()->m_straItem[iSel];

}




int TemplateWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (!m_leftwnd.Create(NULL, "", WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, 1101, nullptr))
   {

      return -1;

   }


   if (!m_recentwnd.Create(NULL, "", WS_CHILD, CRect(0, 0, 0, 0), this, 1121, nullptr))
   {

      return -1;

   }

   if (BaseWnd::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   CStringW wstr;

   m_searchedit.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 20);
   //m_searchedit.SetWindowText(m_searchedit.m_strEmpty);
   wstr.LoadString(IDS_TEMPLATE_SEARCH_TEMPLATE);
   m_searchedit.SetCueBanner(wstr, TRUE);
   CString str;
   str.LoadString(IDS_TEMPLATE_CATEGORIES);
   m_staticCategories.Create(NULL, str, WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, 25, nullptr);
   m_combobox.Create(NULL,"", WS_VISIBLE | WS_CHILD, CRect(0, 0, 0, 0), this, 300);

   m_combobox.GetList()->m_iCurSel = 0;

   m_categorywnd.m_ptemplatewnd = this;

   if (!m_categorywnd.Create(NULL, "", WS_CHILD | WS_HSCROLL | WS_VSCROLL, CRect(0, 0, 0, 0), this, 1111, nullptr))
   {

      return -1;

   }

   OnButtonPress(ID_FILE_TEMPLATE);

   return 0;

}


void TemplateWnd::OnButtonPress(int i)
{

   CStringW wstr;

   if (i == ID_FILE_TEMPLATE)
   {

      m_escreen = e_screen_templates;

      layout();

      return;

   }
   else if (i == ID_FILE_MRU_FILE1)
   {

      m_escreen = e_screen_recent_documents;

      layout();

      return;

   }
   else if (i == ID_FILE_CLOSE)
   {

      ((CPCDraftApp*)AfxGetApp())->m_pTemplateWnd->ShowWindow(SW_HIDE);

      ((CPCDraftApp*)AfxGetApp())->ShowMainFrame();

      ((CPCDraftApp*)AfxGetApp())->OnFileNewDrawing();

      return;

   }

   BaseWnd::OnButtonPress(i);

}


void TemplateWnd::OnSize(UINT nType, int cx, int cy)
{

   BaseWnd::OnSize(nType, cx, cy);

   layout();

}


void TemplateWnd::layout()
{

   CRect rectClient;

   GetClientRect(rectClient);

   int cx = rectClient.Width();

   int cy = rectClient.Height();

   if (m_escreen == e_screen_templates)
   {

      m_leftwnd.m_buttonNew.m_bSel = true;

      m_leftwnd.m_buttonRecent.m_bSel = false;
      
      m_combobox.ShowWindow(SW_SHOW);

      m_staticCategories.ShowWindow(SW_SHOW);

      CStringW wstr;

      wstr.LoadString(IDS_TEMPLATE_SEARCH_TEMPLATE);

      m_searchedit.SetCueBanner(wstr, TRUE);

      m_categorywnd.ShowWindow(SW_SHOW);

      m_recentwnd.ShowWindow(SW_HIDE);

      m_categorywnd.SetFocus();

   }
   else if (m_escreen == e_screen_recent_documents)
   {

      m_leftwnd.m_buttonNew.m_bSel = false;

      m_leftwnd.m_buttonRecent.m_bSel = true;

      m_combobox.ShowWindow(SW_HIDE);

      m_staticCategories.ShowWindow(SW_HIDE);

      CStringW wstr;

      wstr.LoadString(IDS_TEMPLATE_SEARCH_RECENT);

      m_searchedit.SetCueBanner(wstr, TRUE);

      m_recentwnd.UpdateList();

      m_recentwnd.ShowWindow(SW_SHOW);

      m_categorywnd.ShowWindow(SW_HIDE);

      m_recentwnd.SetFocus();

   }


   m_leftwnd.SetWindowPos(nullptr, 1, 1, m_iLeftWindow, cy - 2, SWP_SHOWWINDOW);

   m_categorywnd.SetWindowPos(nullptr, m_iLeftWindow + 1, m_yMainClient, cx - m_iLeftWindow - 1, cy - m_yMainClient - 16, m_escreen == e_screen_templates ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);

   m_recentwnd.SetWindowPos(nullptr, m_iLeftWindow + 1, m_yMainClient, cx - m_iLeftWindow - 1, cy - m_yMainClient - 16, m_escreen == e_screen_recent_documents ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);

   m_searchedit.SetWindowPos(nullptr, m_iLeftWindow + 400, 48, cx - m_iLeftWindow - 400 - 30, 26, SWP_SHOWWINDOW);

   m_staticCategories.SetWindowPos(nullptr, m_iLeftWindow + 30, 48, 110, 26, m_escreen == e_screen_templates ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);

   m_combobox.SetWindowPos(nullptr, m_iLeftWindow + 140, 46, 220, 30, m_escreen == e_screen_templates ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);

}



BOOL TemplateWnd::Create()
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

   if (!CreateEx(0, lpszClass, "", dwStyle, rect, nullptr, 0))
   {

      return FALSE;

   }

   ModifyStyle(WS_MAXIMIZEBOX, 0, SWP_FRAMECHANGED);
   ModifyStyle(WS_MINIMIZEBOX, 0, SWP_FRAMECHANGED);

   CenterWindow();

   return true;


}

//void TemplateWnd::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) // Interface Update - Infinisys Ltd.
//{
//
//   BaseWnd::OnGetMinMaxInfo(lpMMI);
//
//   MINMAXINFO* mmi = (MINMAXINFO*)lpMMI;
//
//   CRect rWindow;
//
//   WINDOWPLACEMENT wp;
//
//   GetWindowPlacement(&wp);
//
//   rWindow = wp.rcNormalPosition;
//
//   HMONITOR monitor = MonitorFromRect(&rWindow, MONITOR_DEFAULTTONULL);
//
//   if (!monitor)
//   {
//
//      return;
//
//   }
//
//   MONITORINFO monitor_info = { 0 };
//   monitor_info.cbSize = sizeof(monitor_info);
//   GetMonitorInfo(monitor, &monitor_info);
//
//   RECT work_area = monitor_info.rcWork;
//   RECT monitor_rect = monitor_info.rcMonitor;
//
//   ::AdjustWindowRectEx(&work_area, GetStyle(), TRUE, GetExStyle());
//   int iCaption = GetSystemMetrics(SM_CYCAPTION);
//   mmi->ptMaxPosition.x = work_area.left - monitor_rect.left;
//   mmi->ptMaxPosition.y = work_area.top - monitor_rect.top + 32 + (32 - iCaption);
//   mmi->ptMaxSize.x = work_area.right - work_area.left;
//   mmi->ptMaxSize.y = work_area.bottom - work_area.top - +32 + (32 - iCaption);
//   mmi->ptMinTrackSize.x = 100;
//   mmi->ptMinTrackSize.y = 32;
//
//}



//// Interface Upgrade - Infinisys Ltd.
//void TemplateWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
//{
//
//   RECT nonclient = lpncsp->rgrc[0];
//   BaseWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
//   RECT client = lpncsp->rgrc[0];
//   if (IsZoomed())
//   {
//      WINDOWINFO wi = {};
//      wi.cbSize = sizeof(wi);
//      GetWindowInfo(&wi);
//      HMONITOR mon = MonitorFromWindow(GetSafeHwnd(), MONITOR_DEFAULTTOPRIMARY);
//      MONITORINFO mi = {};
//      mi.cbSize = sizeof(mi);
//      GetMonitorInfoW(mon, &mi);
//
//      /* Maximized windows always have a non-client border that hangs over
//      the edge of the screen, so the size proposed by WM_NCCALCSIZE is
//      fine. Just adjust the top border to remove the window title. */
//      lpncsp->rgrc[0].left = client.left;
//      lpncsp->rgrc[0].top = 0;
//      //lpncsp->rgrc[0].top = nonclient.top + wi.cyWindowBorders;
//      ////lpncsp->rgrc[0].top -= (wi.cyWindowBorders + 1);
//      //lpncsp->rgrc[0].top = nonclient.top;
//      lpncsp->rgrc[0].right = client.right;
//      lpncsp->rgrc[0].bottom = mi.rcWork.bottom;
//
//
//      /* If the client rectangle is the same as the monitor's rectangle,
//      the shell assumes that the window has gone fullscreen, so it removes
//      the topmost attribute from any auto-hide appbars, making them
//      inaccessible. To avoid this, reduce the size of the client area by
//      one pixel on a certain edge. The edge is chosen based on which side
//      of the monitor is likely to contain an auto-hide appbar, so the
//      missing client area is covered by it. */
//      if (EqualRect(&lpncsp->rgrc[0], &mi.rcMonitor)) {
//         if (has_autohide_appbar(ABE_BOTTOM, mi.rcMonitor))
//            lpncsp->rgrc[0].bottom--;
//         else if (has_autohide_appbar(ABE_LEFT, mi.rcMonitor))
//            lpncsp->rgrc[0].left++;
//         else if (has_autohide_appbar(ABE_TOP, mi.rcMonitor))
//            lpncsp->rgrc[0].top++;
//         else if (has_autohide_appbar(ABE_RIGHT, mi.rcMonitor))
//            lpncsp->rgrc[0].right--;
//      }
//   }
//   else {
//      /* For the non-maximized case, set the output RECT to what it was
//      before WM_NCCALCSIZE modified it. This will make the client size the
//      same as the non-client size. */
//
//      lpncsp->rgrc[0] = nonclient;
//
//   }
//
//}


void TemplateWnd::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

   BaseWnd::OnDraw(pdc, pgraphics);

   CRect rectClient;

   GetClientRect(rectClient);

   CRect rectRight(rectClient);

   rectRight.left = m_iLeftWindow;

   pdc->FillSolidRect(rectRight, RGB(242, 242, 242));

   CRect rectLeft(rectClient);

   rectLeft.right = m_iLeftWindow;

   pdc->FillSolidRect(rectLeft, RGB(180, 60, 32));

   CRect r;

   m_searchedit.GetWindowRect(r);

   ScreenToClient(r);

   r.InflateRect(20, 5);

   pdc->FillSolidRect(r, RGB(255, 255, 255));
   pdc->Draw3dRect(r, RGB(196, 196, 196), RGB(208, 208, 208));

   pdc->FillSolidRect(m_iLeftWindow, m_yMainClient - 1, rectClient.right - m_iLeftWindow, 1, RGB(196, 196, 196));

   for (int i = 0; i < 12; i += 2)
   {
      for (int j = i; j < 12; j += 2)
      {

         pdc->SetPixel(rectClient.right - i - 2, rectClient.bottom - (11 - j) - 1,
            RGB(120, 120, 120));
         pdc->SetPixel(rectClient.right - i - 1, rectClient.bottom - (11 - j),
            RGB(255, 255, 255));

      }

   }


}





void TemplateWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
   BaseWnd::OnShowWindow(bShow, nStatus);

   if (bShow)
   {

      layout();

      RedrawWindow();

   }
   
}
