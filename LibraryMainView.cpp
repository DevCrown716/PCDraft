// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LibraryMainView.h"
#include "LibrarySplitView.h"
#include "LibraryImageSplitView.h"
#include "LibraryFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CWnd* CreateView(CRuntimeClass * pclass,UINT nID,CWnd * pwndParent,CDocument * pdoc);
// CLibraryMainView

IMPLEMENT_DYNCREATE(CLibraryMainView, CView)

CLibraryMainView::CLibraryMainView()
{
   m_plibrary = NULL;
   m_pimage = NULL;
   m_bResize = false;

}

CLibraryMainView::~CLibraryMainView()
{
}

BEGIN_MESSAGE_MAP(CLibraryMainView, CView)
   ON_WM_CREATE()
   ON_WM_SIZE()
   ON_WM_DESTROY()
   ON_WM_MOUSEMOVE()
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_NCMOUSEMOVE()
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CLibraryMainView drawing

void CLibraryMainView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
   CRect rectClient;
   GetClientRect(rectClient);
   pDC->FillSolidRect(rectClient,GetSysColor(COLOR_3DFACE));

   for(int i = 0; i < 12; i+=2)
   {
      for(int j = i;j < 12; j+=2)
      {

         pDC->SetPixel(rectClient.right - i - 2,rectClient.bottom - (11 - j)-1,
            RGB(120,120,120));
         pDC->SetPixel(rectClient.right - i - 1,rectClient.bottom - (11 - j),
            RGB(255,255,255));

      }

   }

	// TODO: add draw code here
}


// CLibraryMainView diagnostics

#ifdef _DEBUG
void CLibraryMainView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CLibraryMainView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLibraryMainView message handlers


BOOL CLibraryMainView::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.dwExStyle = 0;
   cs.style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;

   return CView::PreCreateWindow(cs);
}


int CLibraryMainView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CView::OnCreate(lpCreateStruct) == -1)
      return -1;

   CLibraryFrame * pframe = (CLibraryFrame *) GetParentFrame();

   pframe->m_plibrarymainview = this;

   m_bLibrary = true;
   m_plibrary = (CLibrarySplitView *)CreateView(RUNTIME_CLASS(CLibrarySplitView),1,this,GetDocument());
   //m_pimage = (CLibraryImageSplitView *)CreateView(RUNTIME_CLASS(CLibraryImageSplitView),2,this,GetDocument());
   

   return 0;
}




CWnd* CreateView(CRuntimeClass * pclass, UINT nID, CWnd * pwndParent, CDocument * pdoc)
{
   ASSERT(pwndParent->m_hWnd != NULL);
   ASSERT(::IsWindow(pwndParent->m_hWnd));
   ENSURE_ARG(pclass != NULL);

   // Note: can be a CWnd with PostNcDestroy self cleanup
   CWnd* pView = (CWnd*)pclass->CreateObject();
   if(pView == NULL)
   {
      TRACE(traceAppMsg,0,"Warning: Dynamic create of view type %hs failed.\n",
         pclass->m_lpszClassName);
      return NULL;
   }
   ASSERT_KINDOF(CWnd,pView);

   CCreateContext cs;

   cs.m_pCurrentDoc = pdoc;
   cs.m_pNewViewClass = pclass;

   // views are always created with a border!
   if(!pView->Create(NULL,NULL,AFX_WS_DEFAULT_VIEW,
      CRect(0,0,0,0),pwndParent,nID,&cs))
   {
      TRACE(traceAppMsg,0,"Warning: could not create view for frame.\n");
      return NULL;        // can't continue without a view
   }

   return pView;

}

void CLibraryMainView::OnSize(UINT nType,int cx,int cy)
{
   
   CView::OnSize(nType,cx,cy);
   
   layout();

}


void CLibraryMainView::layout()
{

   CRect rectClient;

   GetClientRect(rectClient);


   if(m_plibrary != NULL)
   {

      m_plibrary->SetWindowPos(&CWnd::wndTop,rectClient.left,rectClient.top,rectClient.Width(),rectClient.Height() -12,m_bLibrary ? SWP_SHOWWINDOW : SWP_HIDEWINDOW);

   }

   if(m_pimage != NULL)
   {

      m_pimage->SetWindowPos(&CWnd::wndTop,rectClient.left,rectClient.top,rectClient.Width(),rectClient.Height() - 12,m_bLibrary ?  SWP_HIDEWINDOW : SWP_SHOWWINDOW);

   }


}

void CLibraryMainView::OnDestroy()
{

   if(m_plibrary != NULL)
   {

      try
      {

         m_plibrary->DestroyWindow();
      
      }
      catch(...)
      {

      }

      m_plibrary = NULL;

   }

   if(m_pimage != NULL)
   {

      try
      {

         m_pimage->DestroyWindow();

      }
      catch(...)
      {

      }

      //try
      //{

      //   delete m_pimage;

      //}
      //catch(...)
      //{

      //}

   }

   CView::OnDestroy();
   
}


void CLibraryMainView::OnMouseMove(UINT nFlags,CPoint point)
{
   if(m_bResize)
   {

      defer_resize(point, false);

      SetCursor(LoadCursor(NULL,IDC_SIZENWSE));

   }
   else if(is_resize(point))
   {
      SetCursor(LoadCursor(NULL,IDC_SIZENWSE));
   }


   
}


bool CLibraryMainView::is_resize(CPoint pt)
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


void CLibraryMainView::OnLButtonDown(UINT nFlags,CPoint point)
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


void CLibraryMainView::OnLButtonUp(UINT nFlags,CPoint point)
{
   defer_resize(point, true);
   ReleaseCapture();
   m_bResize = false;

}


void CLibraryMainView::defer_resize(CPoint pt, bool bFinal)
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

void CLibraryMainView::OnNcMouseMove(UINT nHitTest,CPoint point)
{
   if(m_bResize)
   {

      ScreenToClient(&point);

      defer_resize(point,false);

   }

}


BOOL CLibraryMainView::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default
   return TRUE;
   //return CView::OnEraseBkgnd(pDC);
}
