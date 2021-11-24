// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "BaseControl.h"
#include "BaseWnd.h"
#include "ControlBox.h"
#include "WelcomeEdit.h"
#include "../resource.h"
#include "ScrollHelper.h"
#include "memory_dc.h"


BOOL AFXAPI AfxEndDeferRegisterClass(LONG fToRegister);
#define AFX_WNDFRAMEORVIEW_REG          0x00008
#define AfxDeferRegisterClass(fClass) AfxEndDeferRegisterClass(fClass)

extern const TCHAR _afxWndFrameOrView[];

IMPLEMENT_DYNAMIC(BaseControl, CWnd);

BaseControl::BaseControl()
{
   m_bBaseControlArrowNavigation = true;
   m_bComposited = true;
   m_scrollHelper = nullptr;

   m_bDefaultClick = false;
   m_bHoverNotify = false;
   m_pbasewnd = nullptr;
   m_crBackground = RGB(255, 255, 255);

}


BaseControl::~BaseControl()
{
   if (m_scrollHelper)
   {
      delete m_scrollHelper;
   }

}


BEGIN_MESSAGE_MAP(BaseControl, CWnd)
   ON_WM_MOUSEMOVE()
   ON_WM_MOUSELEAVE()
   ON_WM_CREATE()
   ON_WM_PAINT()
   ON_WM_SIZE()
   ON_WM_KEYDOWN()
   ON_WM_KILLFOCUS()
   ON_WM_SETFOCUS()
   ON_WM_ERASEBKGND()
   ON_WM_MOUSEACTIVATE()
   ON_WM_HSCROLL()
   ON_WM_VSCROLL()
   ON_WM_MOUSEWHEEL()
   ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void BaseControl::enable_scroll_helper()
{

   if (m_scrollHelper)
   {

      return;

   }

   // Create the scroll helper class and attach to this wnd.
   m_scrollHelper = new CScrollHelper;
   m_scrollHelper->AttachWnd(this);



}

int BaseControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (!m_pbasewnd)
   {

      m_pbasewnd = (BaseWnd *)GetTopLevelParent();

   }

   if (CWnd::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   if (m_bHoverNotify)
   {

      Invalidate();

   }

   return 0;

}


void BaseControl::OnMouseMove(UINT nFlags, CPoint point)
{

   if (m_bHoverNotify)
   {

      if (!m_bSetHover)
      {

         m_bSetHover = true;

         TRACKMOUSEEVENT tme = { sizeof(tme) };

         tme.dwFlags = TME_LEAVE;

         tme.hwndTrack = GetSafeHwnd();

         TrackMouseEvent(&tme);

         CWnd::OnMouseMove(nFlags, point);

         Invalidate();

      }

   }
   else
   {

      CWnd::OnMouseMove(nFlags, point);

   }

}

LPCTSTR BaseControl::GetIconWndClass(DWORD dwDefaultStyle, UINT nIDResource)
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

void BaseControl::OnMouseLeave()
{

   if (m_bHoverNotify)
   {

      m_bSetHover = false;

      CWnd::OnMouseLeave();

      Invalidate();

   }
   else
   {

      CWnd::OnMouseLeave();


   }


}


void BaseControl::OnPaint()
{

   CPaintDC dcPaint(this); // device context for painting
                      // TODO: Add your message handler code here
                      // Do not call CWnd::OnPaint() for painting messages


      // Get the client rect.
 /*  CRect rect;
   GetClientRect(&rect);
*/
   //// Prepare for memory DC drawing.
   //CDC memoryDC;
   //if (rect.Width() > 0 && rect.Height() > 0 &&
   //   memoryDC.CreateCompatibleDC(&dc))
   //{
   //   CBitmap bitmap;
   //   if (bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height()))
   //   {
   //      CDC* pDC = &memoryDC;

   //      // Select bitmap into memory DC.
   //      CBitmap* pOldBitmap = pDC->SelectObject(&bitmap);

         // Set up the memory DC for drawing graphics and text.
         //CPen pen(PS_SOLID, 1, RGB(0, 0, 255));
         //CPen* pOldPen = pDC->SelectObject(&pen);
         //CBrush* pOldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
         //COLORREF oldTextColor = pDC->SetTextColor(RGB(0, 0, 255));
         //int oldBkMode = pDC->SetBkMode(TRANSPARENT);

         // Fill the background.
   //      pDC->FillSolidRect(&rect, RGB(0, 255, 0));

   //      // Draw a rectangle representing the display size.
   //      // The rectangle needs to be offset due to scrolling position.
   //      CRect dispRect;
   //      dispRect.SetRect(0, 0,
   //         m_scrollHelper->GetDisplaySize().cx,
   //         m_scrollHelper->GetDisplaySize().cy);
   //      dispRect.OffsetRect(-m_scrollHelper->GetScrollPos().cx,
   //         -m_scrollHelper->GetScrollPos().cy);
   //      pDC->Rectangle(&dispRect);

   //      // Draw text information.
   //      DrawScrollInfo(pDC);

   //      // Do the bitblt.
   //      dc.BitBlt(0, 0, rect.Width(), rect.Height(),
   //         pDC, 0, 0, SRCCOPY);

   //      // Restore DC state.
   //      pDC->SetBkMode(oldBkMode);
   //      pDC->SetTextColor(oldTextColor);
   //      pDC->SelectObject(pOldBrush);
   //      pDC->SelectObject(pOldPen);
   //      pDC->SelectObject(pOldBitmap);
   //   }
   //}

   memory_dc dc(dcPaint, this);


   ////Define a bitmap object
   //CBitmap MemBitmap;
   ////Create a memory device description table handle (backup buffer) compatible with the screen device description table (front buffer)
   //dc.CreateCompatibleDC(&dcPaint);
   ////Cannot draw at this time, because the device description table without bitmap cannot be drawn
   ////The following creates a bitmap compatible with the screen device description table (or memory device description table)
   //MemBitmap.CreateCompatibleBitmap(&dc, nWidth, nHeight);
   ////Select the bitmap into the memory device description table
   ////Only the device description table with the bitmap selected has a place to draw on the specified bitmap
   //CBitmap *pOldBit = dc.SelectObject(&MemBitmap);
   ////First clear the bitmap with a background color, here I use white as the background
   ////You can also use the colors you should use
   ////MemDC.FillSolidRect(0, 0, nWidth, nHeight, RGB(255, 255, 255));
   ////Drawing

   CDC * pdc = &dc;

   Gdiplus::Graphics g(pdc->GetSafeHdc());

   Gdiplus::Graphics * pgraphics = &g;

   CRect rectClient;

   GetClientRect(rectClient);


   dc.FillSolidRect(rectClient, m_crBackground);

   pgraphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
   pgraphics->SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
   pgraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
   pgraphics->SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
   pgraphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
   if (m_scrollHelper)
   {

     pgraphics->TranslateTransform(-m_scrollHelper->GetScrollPos().cx,
         -m_scrollHelper->GetScrollPos().cy);

   }


   OnDraw(pdc, pgraphics);

   ////Copy the graphics in the back buffer to the front buffer
   //dcPaint.BitBlt(0, 0, nWidth, nHeight, &dc, 0, 0, SRCCOPY);
   ////Clean up after drawing
   ////MemBitmap.DeleteObject();
   ////MemDC.DeleteDC();

}


void BaseControl::OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics)
{

}


double BaseControl::font_scaler()
{

   return m_pbasewnd->font_scaler();

}


double BaseControl::size_scaler()
{

   return m_pbasewnd->size_scaler();

}


void BaseControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (nChar == VK_TAB)
   {

      OutputDebugString("AAA");

      if (GetAsyncKeyState(VK_SHIFT) < 0)
      {

         SetFocusPrevious();

      }
      else
      {

         SetFocusNext();

      }

   }
   else if (m_bBaseControlArrowNavigation)
   {

      if (nChar == VK_RIGHT || nChar == VK_DOWN)
      {

         OutputDebugString("BBB");

         SetFocusNext();

      }
      else if (nChar == VK_LEFT || nChar == VK_UP)
      {

         SetFocusPrevious();

      }

   }


   CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}


void BaseControl::SetFocusPrevious()
{

   auto pWndNext = GetNextWindow();

   if (pWndNext == nullptr)
   {

      pWndNext = GetWindow(GW_HWNDFIRST);

   }

   if (pWndNext->IsKindOf(RUNTIME_CLASS(ControlBox)))
   {

      pWndNext = pWndNext->GetNextWindow();

      if (pWndNext == nullptr)
      {

         pWndNext = GetWindow(GW_HWNDFIRST);

      }

   }

   if (pWndNext->IsKindOf(RUNTIME_CLASS(WelcomeEdit)))
   {

      ((WelcomeEdit*)pWndNext)->m_edit.SetFocus();

      ((WelcomeEdit*)pWndNext)->Invalidate();

      ((WelcomeEdit*)pWndNext)->m_edit.Invalidate();

   }
   else
   {

      pWndNext->SetFocus();

      pWndNext->Invalidate();

   }

   Invalidate();

}


void BaseControl::OnClick()
{

   m_pbasewnd->OnButtonPress(GetDlgCtrlID());

}

void BaseControl::SetFocusNext()
{

   auto pWndNext = GetWindow(GW_HWNDPREV);

   OutputDebugString("1");

   if (pWndNext == nullptr)
   {

      pWndNext = GetWindow(GW_HWNDLAST);

      OutputDebugString("2");

   }

   if (pWndNext->IsKindOf(RUNTIME_CLASS(ControlBox)))
   {

      OutputDebugString("3");

      pWndNext = pWndNext->GetWindow(GW_HWNDPREV);

      if (pWndNext == nullptr)
      {

         OutputDebugString("4");

         pWndNext = GetWindow(GW_HWNDLAST);

      }

   }

   if (pWndNext->IsKindOf(RUNTIME_CLASS(WelcomeEdit)))
   {

      OutputDebugString("5");

      ((WelcomeEdit*)pWndNext)->m_edit.SetFocus();

      ((WelcomeEdit*)pWndNext)->Invalidate();

      ((WelcomeEdit*)pWndNext)->m_edit.Invalidate();

   }
   else
   {

      OutputDebugString("6");

      pWndNext->SetFocus();

      pWndNext->Invalidate();

   }

   Invalidate();

}


void BaseControl::OnKillFocus(CWnd* pNewWnd)
{

   CWnd::OnKillFocus(pNewWnd);

   Invalidate();
   
}


void BaseControl::OnSetFocus(CWnd* pOldWnd)
{
   
   CWnd::OnSetFocus(pOldWnd);

   if (m_bHoverNotify)
   {

      Invalidate();

   }

   m_pbasewnd->m_pWndFocus = this;

}


BOOL BaseControl::OnEraseBkgnd(CDC* pDC)
{
   
   return TRUE;

}

bool g_bBaseControlRegisterClass = false;

BOOL BaseControl::PreCreateWindow(CREATESTRUCT& cs)
{

   if (!g_bBaseControlRegisterClass)
   {
      
      WNDCLASS wndcls = {};
      memset(&wndcls, 0, sizeof(wndcls));
      wndcls.lpfnWndProc = AfxGetAfxWndProc();//Setting window procedure functions for window classes
      wndcls.hInstance = AfxGetInstanceHandle();
      wndcls.hCursor = ::LoadCursor(NULL, IDC_ARROW);
      wndcls.style |= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
      wndcls.lpszClassName = "g_bBaseControlRegisterClass";

      // OLE Control windows - use parent DC for speed
      if (AfxRegisterClass(&wndcls))
      {
         g_bBaseControlRegisterClass = true;

      }

   }

   cs.lpszClass = "g_bBaseControlRegisterClass";
   if (m_bComposited)
   {
      cs.dwExStyle = WS_EX_COMPOSITED;
   }
   cs.style &= ~WS_BORDER;
   return CWnd::PreCreateWindow(cs);
}


//void BaseControl::OnNcPaint()
//{
//   //CWindowDC dc(this);
//   //CRect r;
//   //GetWindowRect(&r);
//   //r.OffsetRect(-r.left, -r.top);
//   //dc.FillSolidRect(r, RGB(255, 255, 255));
//   // TODO: Add your message handler code here
//   // Do not call CWnd::OnNcPaint() for painting messages
//}


//void BaseControl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
//{
//   if (bCalcValidRects)
//   {
//
//      lpncsp->rgrc[1] = lpncsp->rgrc[0];
//      lpncsp->rgrc[2] = lpncsp->rgrc[0];
//   }
//
//   //CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
//}



void BaseControl::OnLButtonUp(UINT nFlags, CPoint point)
{

   if (m_bDefaultClick)
   {

      OnClick();

   }

   CWnd::OnLButtonUp(nFlags, point);

}



void BaseControl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{

   if (m_bDefaultClick)
   {

      if (nChar == VK_RETURN || nChar == VK_SPACE)
      {

         OnClick();

      }

   }

   CWnd::OnChar(nChar, nRepCnt, nFlags);

}




//BaseControl::BaseControl(CWnd* parentWnd)
//{
//
//   // Create the GUI window. Notice we specify the styles WS_HSCROLL
//   // and WS_VSCROLL. These are needed to enable the horizontal and
//   // vertical scrollbars for this window.
//   Create(NULL, "BaseControl", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
//      CRect(0, 0, 0, 0), parentWnd, 0, NULL);
//}


void BaseControl::PostNcDestroy()
{
   if (m_scrollHelper)
   {
      m_scrollHelper->DetachWnd();
   }

   // Delete the C++ instance so the parent does not have
   // to worry about it.
   //delete this;
}



int BaseControl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{

   if (GetTopLevelParent() != GetActiveWindow())
   {
      
      CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);

      SetFocus();

      return MA_NOACTIVATEANDEAT;


   }

   // We handle this message so that when user clicks once in the
   // window, it will be given the focus, and this will allow
   // mousewheel messages to be directed to this window.

   if (GetFocus() != this)
   {
    
      SetFocus();

      return MA_NOACTIVATEANDEAT;

   }

   return MA_NOACTIVATE;

}


void BaseControl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   if (m_scrollHelper)
   {
      m_scrollHelper->OnHScroll(nSBCode, nPos, pScrollBar);
   }
}

void BaseControl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   if (m_scrollHelper)
   {
      m_scrollHelper->OnVScroll(nSBCode, nPos, pScrollBar);
   }
}

BOOL BaseControl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
   if (m_scrollHelper)
   {
      BOOL wasScrolled = m_scrollHelper->OnMouseWheel(nFlags, zDelta, pt);
      return wasScrolled;
   }
   return FALSE;
}

void BaseControl::OnSize(UINT nType, int cx, int cy)
{
   CWnd::OnSize(nType, cx, cy);

   CRect rect;
   GetWindowRect(&rect);

   if (m_scrollHelper)
   {
      // Initialization: Set the display size if needed.
      if (m_scrollHelper->GetDisplaySize() == CSize(0, 0))
      {
         if (rect.Width() > 0 && rect.Height() > 0)
            m_scrollHelper->SetDisplaySize(rect.Width(), rect.Height());
      }

      m_scrollHelper->OnSize(nType, cx, cy);
   }
}

void BaseControl::DrawScrollInfo(CDC* pDC)
{
   int x = 3, y = 2;
   if (m_scrollHelper)
   {

      // Offset starting position due to scrolling.
      x -= m_scrollHelper->GetScrollPos().cx;
      y -= m_scrollHelper->GetScrollPos().cy;

      // Draw the page size.
      CString text;
      text.Format("PageSize: %d x %d", m_scrollHelper->GetPageSize().cx, m_scrollHelper->GetPageSize().cy);
      pDC->TextOut(x, y, text);

      // Draw the display size.
      y += 16;
      text.Format("DisplaySize: %d x %d", m_scrollHelper->GetDisplaySize().cx, m_scrollHelper->GetDisplaySize().cy);
      pDC->TextOut(x, y, text);

      // Change the caption text of the MDI child frame window to show
      // the current scroll position.
      text.Format("TestScroll (ScrollPos: %d, %d)", m_scrollHelper->GetScrollPos().cx, m_scrollHelper->GetScrollPos().cy);
      CWnd* pWnd = GetParentFrame();
      if (pWnd != NULL)
      {
         pWnd->SetWindowText(text);
      }

      // Draw the GetClientRect() value.
      CRect rect;
      GetClientRect(&rect);
      y += 16;
      text.Format("ClientRect(x,y,cx,cy): %d, %d, %d, %d", rect.left, rect.top, rect.Width(), rect.Height());
      pDC->TextOut(x, y, text);
   }
}

// END
