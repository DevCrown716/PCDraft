// Interface Update - Infinisys Ltd.
#include "stdafx.h"
#include "pcdraft.h"
#include "CustomMiniFrameWnd.h"
#include <VersionHelpers.h>
#include <shellapi.h>
#pragma comment(lib, "Dwmapi.lib" )
using namespace Gdiplus;
void copy(Gdiplus::Rect & r,LPCRECT lpcrect); // Interface Update - Infinisys Ltd.

void copy(Gdiplus::RectF & r,LPCRECT lpcrect); // Interface Update - Infinisys Ltd.

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


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static	CBitmap bitmapMaximize;
static	CBitmap bitmapRestore;
static	CBitmap bitmapMaximizeDown;
static	CBitmap bitmapRestoreDown;
static	SIZE sizeButton ={GetSystemMetrics(SM_CXSMSIZE) - GetSystemMetrics(SM_CXBORDER) * 2,GetSystemMetrics(SM_CYSMSIZE) - GetSystemMetrics(SM_CXBORDER) * 4};



/////////////////////////////////////////////////////////////////////////////
// CCustomMiniFrameWnd

IMPLEMENT_DYNCREATE(CCustomMiniFrameWnd,CMiniFrameWnd)

CCustomMiniFrameWnd::CCustomMiniFrameWnd()
{
   m_bResizeGrip = false;
   m_bResizable = false;
   if(!bitmapMaximize.GetSafeHandle())
      bitmapMaximize.LoadMappedBitmap(IDB_MINI_MAXIMIZE);
   if(!bitmapRestore.GetSafeHandle())
      bitmapRestore.LoadMappedBitmap(IDB_MINI_RESTORE);
   if(!bitmapMaximizeDown.GetSafeHandle())
      bitmapMaximizeDown.LoadMappedBitmap(IDB_MINI_MAXIMIZE_DOWN);
   if(!bitmapRestoreDown.GetSafeHandle())
      bitmapRestoreDown.LoadMappedBitmap(IDB_MINI_RESTORE_DOWN);
   m_bMaximized=false;
   m_bTracking=false;
   m_bShowMaximizeBox=false;
   m_bInRecalcLayout = false; // Interface Update - Infinisys Ltd.
}

CCustomMiniFrameWnd::~CCustomMiniFrameWnd()
{
}


BEGIN_MESSAGE_MAP(CCustomMiniFrameWnd,CMiniFrameWnd)
   //{{AFX_MSG_MAP(CCustomMiniFrameWnd)
   //ON_WM_NCPAINT()
   ON_WM_NCACTIVATE()
   ON_WM_NCLBUTTONDOWN()
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONUP()
   ON_WM_NCHITTEST()
   //}}AFX_MSG_MAP
   ON_WM_ACTIVATE() // Interface Update - Infinisys Ltd.
   ON_WM_NCCALCSIZE() // Interface Update - Infinisys Ltd.
   ON_WM_PAINT() // Interface Update - Infinisys Ltd.
   ON_MESSAGE(WM_DWMNCRENDERINGCHANGED,OnDwm) // Interface Update - Infinisys Ltd.
   ON_WM_CREATE() // Interface Update - Infinisys Ltd.
   ON_WM_LBUTTONDOWN() // Interface Update - Infinisys Ltd.
   ON_WM_WINDOWPOSCHANGED()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCustomMiniFrameWnd message handlers

bool CCustomMiniFrameWnd::is_narrow_window()
{

   CRect rectWindow;

   GetWindowRect(rectWindow);

   return rectWindow.Width() < 120;

}

// Interface Upgrade - Infinisys Ltd.
void CCustomMiniFrameWnd::OnPaint() // Interface Update - Infinisys Ltd.
{

   CPaintDC dc(this);

   int iSaveDC = dc.SaveDC();

   COLORREF cr = dc.GetPixel(0,dc.m_ps.rcPaint.bottom);

   {

      CRect r;

      r = dc.m_ps.rcPaint;

      r.top += 32;
      r.bottom--;
      r.left++;
      r.right--;

      dc.ExcludeClipRect(r);

   }
   

   dc.FillSolidRect(&dc.m_ps.rcPaint,0);

   dc.SelectClipRgn(NULL);
   //BYTE a = (cr >> 24) & 0xf;
   //BYTE r;
   //BYTE g;
   //BYTE b;

   //if(a == 0)
   //{

   //   a = 255;
   //   r = 0;
   //   g = 0;
   //   b = 0;

   //}
   //else
   //{

   //   r = GetRValue(cr) * 255 / a;
   //   g = GetGValue(cr) * 255 / a;
   //   b = GetBValue(cr) * 255 / a;

   //}

   //BYTE m=max(r,max(g,b));
   CRect rCaption=dc.m_ps.rcPaint;
   rCaption.bottom = 32;
   //dc.FillSolidRect(&rCaption,0);
   CRect rLine=dc.m_ps.rcPaint;
   rLine.top = rLine.bottom - 1;
   Graphics gr(dc.GetSafeHdc());

   Pen pen(Color(255,0,0,0));
   gr.DrawLine(&pen,rLine.left,rLine.bottom - 1,rLine.right,rLine.bottom - 1);
   //r120
   Gdiplus::StringFormat format;
   format.SetAlignment(Gdiplus::StringAlignmentCenter);
   format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
   format.SetTrimming(Gdiplus::StringTrimmingWord);
   format.SetFormatFlags(0);
   Gdiplus::Font f(L"Marlett",10.0);
   SolidBrush brText(Color(255,255,255,255));
   RectF r2;
   copy(r2,&dc.m_ps.rcPaint);

   int iW = is_narrow_window() ? 24 : 40;
   r2.X = r2.X + r2.Width - iW;
   r2.Height =40;
   r2.Width = iW;

   gr.DrawString(L"r",1,&f,r2,&format,&brText);
   //r2.x -= 40;
   //if(IsZoomed())
   //{
   //   gr.DrawString(L"2",1,&f,r2,&format,&brText);
   //}
   //else
   //{
   //   gr.DrawString(L"1",1,&f,r2,&format,&brText);
   //}
   //r2.x -= 40;
   //gr.DrawString(L"0",1,&f,r2,&format,&brText);

   //dc.SelectClipRgn(NULL);
   CFont font;
   font.CreatePointFont(100,"Segoe UI",&dc);
   dc.SelectObject(&font);
   dc.SetBkMode(TRANSPARENT);

   dc.SetTextColor(RGB(255, 255, 255));

   CString str;
   GetWindowText(str);

   dc.DrawText(str,rCaption,DT_CENTER | DT_VCENTER | DT_SINGLELINE);

   dc.RestoreDC(iSaveDC);
}

//void CCustomMiniFrameWnd::OnNcPaint() 
//{
//	CMiniFrameWnd::OnNcPaint();
//	DrawButton();
//}

BOOL CCustomMiniFrameWnd::OnNcActivate(BOOL bActive)
{
   BOOL bResult=CMiniFrameWnd::OnNcActivate(bActive);
   //DrawButton();
   return bResult;
}

void CCustomMiniFrameWnd::DrawButton()
{
   if(m_bShowMaximizeBox)
   {
      CWindowDC dc(this);
      CGDIBitmapDC bmp(&dc,(m_bTracking && m_bInButton)?(m_bMaximized?&bitmapMaximizeDown:&bitmapRestoreDown):(m_bMaximized?&bitmapMaximize:&bitmapRestore));
      CRect rc;
      CalcButtonRect(rc);
      dc.StretchBlt(rc.left,rc.top,sizeButton.cx,sizeButton.cy,&bmp,0,0,11,11,SRCCOPY);
   }
}

void CCustomMiniFrameWnd::CalcButtonRect(CRect& rc)
{
   CSize sizeBorder(GetSystemMetrics(SM_CXBORDER),GetSystemMetrics(SM_CYBORDER));
   CSize sizeFrame(GetSystemMetrics(SM_CXFRAME),GetSystemMetrics(SM_CYFRAME));
   GetWindowRect(&rc);
   rc.OffsetRect(-rc.left,-rc.top);
   rc.right-=sizeFrame.cx + sizeBorder.cx * 4 + sizeButton.cx;
   rc.top+=sizeFrame.cy + sizeBorder.cy * 2;
   rc.left=rc.right - sizeButton.cx;
   rc.bottom=rc.top + sizeButton.cy;
}

void CCustomMiniFrameWnd::SetMaximized(bool bMaximized)
{
   if(m_bMaximized != bMaximized)
   {
      m_bMaximized=bMaximized;
      Invalidate();
      SetWindowPos(NULL,0,0,0,0,SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
   }
}

void CCustomMiniFrameWnd::InvertMaximized()
{
   m_bMaximized=!m_bMaximized;
   Invalidate();
   SetWindowPos(NULL,0,0,0,0,SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}

void CCustomMiniFrameWnd::OnNcLButtonDown(UINT nHitTest,CPoint point)
{

   // Blinking fix/Drag Trail Fix - Infinisys Ltd.
   //if(m_bShowMaximizeBox)
   //{
   //	CPoint pt=point;
   //	CRect rc;
   //	GetWindowRect(&rc);
   //	pt.x-=rc.left;
   //	pt.y-=rc.top;
   //	CalcButtonRect(rc);
   //	if(rc.PtInRect(pt))
   //	{
   //		SetCapture();
   //		m_bTracking=true;
   //		m_bInButton=true;
   //		DrawButton();
   //		return;
   //	}
   //}
   //CMiniFrameWnd::OnNcLButtonDown(nHitTest, point);
   Default();
}


// Interface Upgrade - Infinisys Ltd.
LRESULT CCustomMiniFrameWnd::OnNcHitTest(CPoint mouse)
{

   if(!m_bResizable)
   {

      return HTCLIENT;

   }

   if(IsZoomed())
      return HTCLIENT;

   CRect rC;
   GetClientRect(rC);

   ScreenToClient(&mouse);

   int frame_size = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
   int diagonal_width = frame_size * 2 + GetSystemMetrics(SM_CXBORDER);
   int frame_sizeGrip = frame_size * 2;
   int diagonal_widthGrip = diagonal_width * 2;

   if(m_bResizeGrip)
   {

      if(mouse.y >= rC.Height() - frame_sizeGrip)
      {

         if(mouse.x >= rC.Width() - diagonal_widthGrip)
            return HTBOTTOMRIGHT;
      }

   }

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



void CCustomMiniFrameWnd::OnMouseMove(UINT nFlags,CPoint point)
{
   // Blinking fix/Drag Trail Fix - Infinisys Ltd.
   //if(m_bTracking)
   //{
   //	if(GetCapture()!=this)
   //	{
   //		m_bTracking=false;
   //		return;
   //	}
   //	CPoint pt=point;
   //	ClientToScreen(&pt);
   //	CRect rc;
   //	GetWindowRect(&rc);
   //	pt.x-=rc.left;
   //	pt.y-=rc.top;
   //	CalcButtonRect(rc);
   //	m_bInButton=rc.PtInRect(pt)?true:false;
   //	DrawButton();
   //	return;
   //}
   //CMiniFrameWnd::OnMouseMove(nFlags, point);
   Default();
}

void CCustomMiniFrameWnd::OnLButtonUp(UINT nFlags,CPoint point)
{
   // Blinking fix/Drag Trail Fix - Infinisys Ltd.
   //if(m_bTracking)
   //{
   //	m_bTracking=false;
   //	ReleaseCapture();
   //	DrawButton();
   //	if(m_bInButton)
   //	{
   //		CWnd* pDocBarWnd=GetWindow(GW_CHILD);
   //		CWnd* pChildWnd=pDocBarWnd->GetWindow(GW_CHILD);
   //		pChildWnd->SendMessage(UM_MAXIMIZE_CLICK,0,0);
   //	}
   //	return;
   //}
   //CMiniFrameWnd::OnLButtonUp(nFlags, point);
   Default();
}

void CCustomMiniFrameWnd::ShowMaximizeBox(bool bShowMaximizeBox)
{
   if(m_bShowMaximizeBox != bShowMaximizeBox)
   {
      m_bShowMaximizeBox=bShowMaximizeBox;
      Invalidate();
      SetWindowPos(NULL,0,0,0,0,SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
   }
}

// Interface Update - Infinisys Ltd.
void CCustomMiniFrameWnd::OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized)
{

   //   CMiniFrameWnd::OnActivate(nState,pWndOther,bMinimized);
   // CWnd::OnActivate(nState,pWndOther,bMinimized);

   // hide the menu bar
   if(nState == WA_INACTIVE && (m_dwMenuBarVisibility & AFX_MBV_KEEPVISIBLE) == 0)
   {
      SetMenuBarState(AFX_MBS_HIDDEN);
   }

   // get top level frame unless this is a child window
   // determine if window should be active or not
   CFrameWnd* pTopLevel = (GetStyle() & WS_CHILD) ? this : GetTopLevelFrame();
   ENSURE_VALID(pTopLevel);
   CWnd* pActive = (nState == WA_INACTIVE ? pWndOther : this);
   BOOL bStayActive = (pActive != NULL) ?
      (pTopLevel == pActive ||
      (pTopLevel == pActive->GetTopLevelFrame() &&
      (pActive == pTopLevel ||
      pActive->SendMessage(WM_FLOATSTATUS,FS_SYNCACTIVE) != 0)))
      : FALSE;
   pTopLevel->m_nFlags &= ~WF_STAYACTIVE;
   if(bStayActive)
      pTopLevel->m_nFlags |= WF_STAYACTIVE;

   // sync floating windows to the new state
   NotifyFloatingWindows(bStayActive ? FS_ACTIVATE : FS_DEACTIVATE);

   // get active view (use active frame if no active view)
   CView* pActiveView = GetActiveView();
   if(pActiveView == NULL)
      pActiveView = GetActiveFrame()->GetActiveView();

   // when frame gets activated, re-activate current view
   //if(pActiveView != NULL)
   //{
   //   if(nState != WA_INACTIVE && !bMinimized)
   //      pActiveView->OnActivateView(TRUE,pActiveView,pActiveView);

   //   // always notify the view of frame activations
   //   pActiveView->OnActivateFrame(nState,this);
   //}

   // TODO: Add your message handler code here
}


LRESULT CCustomMiniFrameWnd::OnDwm(WPARAM wparam,LPARAM lparam)
{
   //return Default();
   BOOL enabled = FALSE;
   DwmIsCompositionEnabled(&enabled);
   //data->composition_enabled = enabled;

   if(enabled) {
      /* The window needs a frame to show a shadow, so give it the smallest
      amount of frame possible */
      //MARGINS m={0,0,0,1};
      MARGINS m={0,0,0,1};
      DWORD  dw=DWMNCRP_ENABLED;
      DwmExtendFrameIntoClientArea(GetSafeHwnd(),&m);
      DwmSetWindowAttribute(GetSafeHwnd(),DWMWA_NCRENDERING_POLICY,
         &dw,sizeof(DWORD));
   }
   else
   {
   }

   //update_region(data);
   //      long dwEx = ::GetWindowLong(GetSafeHwnd(),GWL_EXSTYLE);
   //      dwEx &= ~(WS_EX_LAYERED);
   //      ::SetWindowLong(GetSafeHwnd(),GWL_EXSTYLE,dwEx);
   //      ::InvalidateRect(GetSafeHwnd(),NULL,true);
   //      ::UpdateWindow(GetSafeHwnd());
   //      //MoveAnchorsImmediatelly(hwndDlg);
   return 0;
}



void CCustomMiniFrameWnd::OnNcCalcSize(BOOL bCalcValidRects,NCCALCSIZE_PARAMS* lpncsp)
{
   // TODO: Add your message handler code here and/or call default
   //if(bCalcValidRects)
   //{
   //   TRACE("1");
   //   lpncsp->rgrc[0].left = lpncsp->lppos->x + 1;
   //   lpncsp->rgrc[0].right = lpncsp->lppos->x + lpncsp->lppos->cx - 1;
   //   lpncsp->rgrc[0].top = lpncsp->lppos->y + 32;
   //   lpncsp->rgrc[0].bottom = lpncsp->lppos->y + lpncsp->lppos->cy - 1;
   //}
   //else
   //{
   //   CRect * prect = (CRect *) lpncsp;
   //   prect->top += 32;
   //   prect->left++;
   //   prect->bottom--;
   //   prect->right--;

   //   TRACE("2");
   //}
   RECT nonclient = lpncsp->rgrc[0];
   CMiniFrameWnd::OnNcCalcSize(bCalcValidRects,lpncsp);
   RECT client = lpncsp->rgrc[0];
   if(IsZoomed())
   {
      WINDOWINFO wi={};
      wi.cbSize = sizeof(wi);
      GetWindowInfo(&wi);

      /* Maximized windows always have a non-client border that hangs over
      the edge of the screen, so the size proposed by WM_NCCALCSIZE is
      fine. Just adjust the top border to remove the window title. */
      lpncsp->rgrc[0].left = client.left;
      lpncsp->rgrc[0].top = 0;
      lpncsp->rgrc[0].right = client.right;
      lpncsp->rgrc[0].bottom = client.bottom;

      HMONITOR mon = MonitorFromWindow(GetSafeHwnd(),MONITOR_DEFAULTTOPRIMARY);
      MONITORINFO mi ={};
      mi.cbSize = sizeof(mi);
      GetMonitorInfoW(mon,&mi);

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


// Interface Update - Infinisys Ltd.
void CCustomMiniFrameWnd::RecalcLayout(BOOL bNotify)
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

   // call the layout hook -- OLE support uses this hook
   //if(bNotify && m_pNotifyHook != NULL)
   //   m_pNotifyHook->OnRecalcLayout();

   // reposition all the child windows (regardless of ID)
   if(GetStyle() & FWS_SNAPTOBARS)
   {
      CRect rect(0,0,32767,32767);
      RepositionBars(0,0xffff,AFX_IDW_PANE_FIRST,reposQuery,
         &rect,&rect,FALSE);
      rect.OffsetRect(0,32);
      RepositionBars(0,0xffff,AFX_IDW_PANE_FIRST,reposExtra,
         &m_rectBorder,&rect,TRUE);
      ::AdjustWindowRectEx(&rect,GetStyle(),TRUE,GetExStyle());
      int iCaption =  GetSystemMetrics(SM_CYCAPTION);
      int cxFrame = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
      rect.left +=cxFrame;
      rect.right -= cxFrame;

      int cyFrame  = GetSystemMetrics(SM_CYFRAME);
      rect.top +=  cyFrame + iCaption;
      //rect.bottom-= 32 + 32 - iCaption;
      SetWindowPos(NULL,0,0,rect.Width(),rect.Height(),
         SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
   }
   else
      RepositionBars(0,0xffff,AFX_IDW_PANE_FIRST,reposExtra,&m_rectBorder,rC);
   m_bInRecalcLayout = FALSE;
}


int CCustomMiniFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   m_bInRecalcLayout = false;


   return 0;
}


void CCustomMiniFrameWnd::OnLButtonDown(UINT nFlags,CPoint point)
{

   CRect rectClient;

   GetClientRect(rectClient);

   CRect r(rectClient);

   r.left = r.right - (is_narrow_window() ? 20 : 40);
   r.bottom = 40;

   if(r.PtInRect(point))
   {

      PostMessage(WM_CLOSE);

   }

   //r.right = r.left;
   //r.left = r.right - 40;
   //if(r.PtInRect(point))
   //{

   //   if(IsZoomed())
   //   {

   //      ShowWindow(SW_RESTORE);

   //   }
   //   else
   //   {

   //      ShowWindow(SW_MAXIMIZE);

   //   }

   //   RedrawWindow(NULL,NULL,RDW_FRAME | RDW_UPDATENOW | RDW_INVALIDATE);

   //}
   //r.right = r.left;
   //r.left = r.right - 40;
   //if(r.PtInRect(point))
   //{

   //   ShowWindow(SW_MINIMIZE);

   //}
   r.right = r.left;
   r.left = 0;

   if(r.PtInRect(point))
   {
      SendMessage(WM_SYSCOMMAND,0xf012) ;
   }

   CMiniFrameWnd::OnLButtonDown(nFlags,point);
}


void CCustomMiniFrameWnd::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
   CMiniFrameWnd::OnWindowPosChanged(lpwndpos);

   RedrawWindow();
}
