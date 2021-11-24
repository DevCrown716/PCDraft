// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "../PCDraft.h"
#include "WelcomeMainView.h"
#include "WelcomeFrame.h"
#include "BaseWnd.h" // Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CWelcomeMainView

IMPLEMENT_DYNCREATE(CWelcomeMainView, CView)

CWelcomeMainView::CWelcomeMainView()
{
   m_plibrary = NULL;
   m_pimage = NULL;
   m_bResize = false;
   m_pWelcomeWnd = nullptr;
}

CWelcomeMainView::~CWelcomeMainView()
{
}

BEGIN_MESSAGE_MAP(CWelcomeMainView, CView)
   ON_WM_CREATE()
   ON_WM_SIZE()
   ON_WM_DESTROY()
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_NCMOUSEMOVE()
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CWelcomeMainView drawing

void CWelcomeMainView::OnDraw(CDC* pDC)
{
   CDocument* pDoc = GetDocument();
   CRect rectClient;
   GetClientRect(rectClient);
   pDC->FillSolidRect(rectClient, GetSysColor(COLOR_3DFACE));

   for (int i = 0; i < 12; i += 2)
   {
      for (int j = i; j < 12; j += 2)
      {

         pDC->SetPixel(rectClient.right - i - 2, rectClient.bottom - (11 - j) - 1,
            RGB(120, 120, 120));
         pDC->SetPixel(rectClient.right - i - 1, rectClient.bottom - (11 - j),
            RGB(255, 255, 255));

      }

   }

   // TODO: add draw code here
}


// CWelcomeMainView diagnostics

#ifdef _DEBUG
void CWelcomeMainView::AssertValid() const
{
   CView::AssertValid();
}

#ifndef _WIN32_WCE
void CWelcomeMainView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWelcomeMainView message handlers


BOOL CWelcomeMainView::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.dwExStyle = 0;
   cs.style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;

   return CView::PreCreateWindow(cs);
}


int CWelcomeMainView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CView::OnCreate(lpCreateStruct) == -1)
      return -1;
   m_pWelcomeWnd = new BaseWnd();

   //m_pWelcomeWnd->Create(nullptr, "", WS_CHILD | WS_VISIBLE, CRect(), this, 1000);

   CWelcomeFrame * pframe = (CWelcomeFrame *)GetParentFrame();

   m_bWelcome = true;


   return 0;
}




void CWelcomeMainView::OnSize(UINT nType, int cx, int cy)
{

   CView::OnSize(nType, cx, cy);

   layout();

}


void CWelcomeMainView::layout()
{

   CRect rectClient;

   GetClientRect(rectClient);

   m_pWelcomeWnd->SetWindowPos(nullptr, 0, 0, rectClient.Width(), rectClient.Height(), SWP_SHOWWINDOW);
   //if (m_plibrary != NULL)
   //{

   //   m_plibrary->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height() - 12, m_bWelcome ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);

   //}

   //if (m_pimage != NULL)
   //{

   //   m_pimage->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height() - 12, m_bWelcome ? SWP_HIDEWINDOW : SWP_SHOWWINDOW);

   //}


}

void CWelcomeMainView::OnDestroy()
{

   //if (m_plibrary != NULL)
   //{

   //   try
   //   {

   //      m_plibrary->DestroyWindow();

   //   }
   //   catch (...)
   //   {

   //   }

   //   m_plibrary = NULL;

   //}

   //if (m_pimage != NULL)
   //{

   //   try
   //   {

   //      m_pimage->DestroyWindow();

   //   }
   //   catch (...)
   //   {

   //   }

   //   //try
   //   //{

   //   //   delete m_pimage;

   //   //}
   //   //catch(...)
   //   //{

   //   //}

   //}

   CView::OnDestroy();

}


void CWelcomeMainView::OnMouseMove(UINT nFlags, CPoint point)
{
   if (m_bResize)
   {

      defer_resize(point, false);

      SetCursor(LoadCursor(NULL, IDC_SIZENWSE));

   }
   else if (is_resize(point))
   {
      SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
   }



}


bool CWelcomeMainView::is_resize(CPoint pt)
{

   if (m_bResize)
   {

      return true;

   }

   CRect rectClient;

   GetClientRect(rectClient);

   CRect rectResize(rectClient.right - 12, rectClient.bottom - 12,
      rectClient.right, rectClient.bottom);

   return rectResize.PtInRect(pt);

}


void CWelcomeMainView::OnLButtonDown(UINT nFlags, CPoint point)
{

   if (is_resize(point))
   {

      m_ptResize = point;
      CRect rectWindow;
      GetParent()->GetWindowRect(rectWindow);
      m_sizeParent = rectWindow.Size();
      m_bResize = true;
      SetCapture();

   }

}


void CWelcomeMainView::OnLButtonUp(UINT nFlags, CPoint point)
{
   defer_resize(point, true);
   ReleaseCapture();
   m_bResize = false;

}


void CWelcomeMainView::defer_resize(CPoint pt, bool bFinal)
{
   if (!m_bResize)
      return;

   if (!bFinal && (GetTickCount() - m_dwLastResize < 50))
   {

      return;

   }

   m_dwLastResize = GetTickCount();

   CSize s = m_sizeParent + pt - m_ptResize;

   GetParent()->PostMessage(WM_USER + 123, 0, MAKELPARAM(s.cx, s.cy));
   //GetParent()->SetWindowPos(NULL,0,0,s.cx,s.cy,SWP_NOMOVE | SWP_NOZORDER | RDW_INVALIDATE | RDW_UPDATENOW);




}

void CWelcomeMainView::OnNcMouseMove(UINT nHitTest, CPoint point)
{
   if (m_bResize)
   {

      ScreenToClient(&point);

      defer_resize(point, false);

   }

}


BOOL CWelcomeMainView::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default
   return TRUE;
   //return CView::OnEraseBkgnd(pDC);
}
