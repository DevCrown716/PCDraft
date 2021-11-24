// Layer - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LayerMainView.h"
#include "LayerFrame.h"
#include "LayerHeader.h"
#include "LayerListView.h"
#include "LayerFooter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CSize get_screen_dpi()
{

   HDC hdcScreen = ::GetDC(NULL);

   CSize s;
   
   s.cx = GetDeviceCaps(hdcScreen,LOGPIXELSX);

   s.cy = GetDeviceCaps(hdcScreen,LOGPIXELSY);

   ::ReleaseDC(NULL,hdcScreen);

   return s;

}


CWnd* CreateView(CRuntimeClass * pclass,UINT nID,CWnd * pwndParent,CDocument * pdoc);


IMPLEMENT_DYNCREATE(CLayerMainView, CView)


CLayerMainView::CLayerMainView()
{
   m_pheader = NULL;
   m_plistview = NULL;
   m_pfooter = NULL;
   m_bResize = false;
}

CLayerMainView::~CLayerMainView()
{
}


BEGIN_MESSAGE_MAP(CLayerMainView, CView)
   ON_WM_CREATE()
   ON_WM_SIZE()
   ON_WM_DESTROY()
   ON_WM_ERASEBKGND()
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_NCMOUSEMOVE()
END_MESSAGE_MAP()


void CLayerMainView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
   CRect rectClient;
   GetClientRect(rectClient);
   pDC->FillSolidRect(rectClient,GetSysColor(COLOR_3DFACE));
   CPen pen;
   pen.CreatePen(PS_SOLID,1,GetSysColor(COLOR_3DLIGHT));
   pDC->SelectObject(&pen);
   pDC->MoveTo(rectClient.left,m_y1);
   pDC->LineTo(rectClient.right,m_y1);
   pDC->MoveTo(rectClient.left,m_y2);
   pDC->LineTo(rectClient.right,m_y2);

   for(int i = 0; i < 12; i+=2)
   {
      for(int j = i;j < 12; j+=2)
      {

         pDC->SetPixel(rectClient.right - i - 2,rectClient.bottom - (11 - j) - 1,
            RGB(120,120,120));
         pDC->SetPixel(rectClient.right - i - 1,rectClient.bottom - (11 - j),
            RGB(255,255,255));

      }
   }

}


// CLayerMainView diagnostics

#ifdef _DEBUG
void CLayerMainView::AssertValid() const
{
	CView::AssertValid();
}

void CLayerMainView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

#endif //_DEBUG


BOOL CLayerMainView::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.dwExStyle = 0;
   cs.style = WS_CHILD | WS_VISIBLE;

   return CView::PreCreateWindow(cs);
}


int CLayerMainView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (CView::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   CLayerFrame * playerframe = (CLayerFrame *)GetParentFrame();

   playerframe->m_playermainview = this;

   m_pheader = (CLayerHeader *) CreateView(RUNTIME_CLASS(CLayerHeader),1,this, GetDocument());

   m_pheader->OnInitialUpdate();

   m_plistview= (CLayerListView *)CreateView(RUNTIME_CLASS(CLayerListView),2,this,GetDocument());

   m_plistview->OnInitialUpdate();

   m_pfooter = (CLayerFooter *)CreateView(RUNTIME_CLASS(CLayerFooter),3,this,GetDocument());

   m_pfooter->OnInitialUpdate();

   return 0;

}


void CLayerMainView::OnSize(UINT nType,int cx,int cy)
{
   
   CView::OnSize(nType,cx,cy);

   layout();

}


void CLayerMainView::layout()
{

   CRect rectClient;

   GetClientRect(rectClient);

   CSize sizeHeader = m_pheader->m_size;

   CSize sizeDpi = get_screen_dpi();

   m_y1 = 40 * sizeDpi.cy / 96;

   m_y2 = rectClient.Height() - 16;

   m_pheader->ShowScrollBar(SB_BOTH,FALSE);

   m_pheader->SetWindowPos(&CWnd::wndTop,0,0,rectClient.Width(),m_y1,SWP_SHOWWINDOW);

   m_plistview->SetWindowPos(&CWnd::wndTop,0,m_y1,rectClient.Width(),rectClient.Height() - m_y1 - (rectClient.Height() - m_y2),SWP_SHOWWINDOW);

   //m_pfooter->SetWindowPos(&CWnd::wndTop,0,m_y2+1,rectClient.Width() + 1,rectClient.Height() - m_y2-1,SWP_SHOWWINDOW);

   m_pfooter->SetWindowPos(&CWnd::wndTop,0,m_y2 + 1,rectClient.Width() + 1,1,SWP_SHOWWINDOW);

   m_pheader->m_plist = m_plistview;

}


void CLayerMainView::OnDestroy()
{

   //if(m_plibrary != NULL)
   //{

   //   try
   //   {

   //      m_plibrary->DestroyWindow();
   //   
   //   }
   //   catch(...)
   //   {

   //   }

   //   try
   //   {

   //      delete m_plibrary;

   //   }
   //   catch(...)
   //   {

   //   }

   //}

   //if(m_pimage != NULL)
   //{

   //   try
   //   {

   //      m_pimage->DestroyWindow();

   //   }
   //   catch(...)
   //   {

   //   }

   //   try
   //   {

   //      delete m_pimage;

   //   }
   //   catch(...)
   //   {

   //   }

   //}

   CView::OnDestroy();
   
}


BOOL CLayerMainView::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default
   return TRUE;
//   return CView::OnEraseBkgnd(pDC);
}


void CLayerMainView::OnMouseMove(UINT nFlags,CPoint point)
{
   if(m_bResize)
   {

      defer_resize(point,false);

      SetCursor(LoadCursor(NULL,IDC_SIZENWSE));

   }
   else if(is_resize(point))
   {
      SetCursor(LoadCursor(NULL,IDC_SIZENWSE));
   }



}

bool CLayerMainView::is_resize(CPoint pt)
{

   if(m_bResize)
   {

      return true;

   }

   CRect rectClient;

   GetClientRect(rectClient);

   CRect rectResize(rectClient.right - 12,rectClient.bottom - 12,
      rectClient.right,rectClient.bottom);

   return rectResize.PtInRect(pt);

}

void CLayerMainView::OnLButtonDown(UINT nFlags,CPoint point)
{

   if(is_resize(point))
   {

      m_ptResize = point;
      CRect rectWindow;
      GetParent()->GetWindowRect(rectWindow);
      m_sizeParent = rectWindow.Size();
      m_bResize = true;
      SetCapture();

   }

}


void CLayerMainView::OnLButtonUp(UINT nFlags,CPoint point)
{
   defer_resize(point,true);
   ReleaseCapture();
   m_bResize = false;

}


void CLayerMainView::defer_resize(CPoint pt,bool bFinal)
{
   if(!m_bResize)
      return;

   if(!bFinal && (GetTickCount() - m_dwLastResize < 50))
   {

      return;

   }

   m_dwLastResize = GetTickCount();

   CSize s = m_sizeParent + pt - m_ptResize;

   GetParent()->PostMessage(WM_USER + 123,0,MAKELPARAM(s.cx,s.cy));
   //GetParent()->SetWindowPos(NULL,0,0,s.cx,s.cy,SWP_NOMOVE | SWP_NOZORDER | RDW_INVALIDATE | RDW_UPDATENOW);




}


void CLayerMainView::OnNcMouseMove(UINT nHitTest,CPoint point)
{
   if(m_bResize)
   {

      ScreenToClient(&point);

      defer_resize(point,false);

   }

}



