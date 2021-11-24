// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LibrarySplitView.h"
#include "LibraryTreeView.h"
#include "LibraryListView.h"
#include "LibraryFooter.h"
#include "LibraryFrame.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWnd* CreateView(CRuntimeClass * pclass,UINT nID,CWnd * pwndParent,CDocument * pdoc);


// CLibrarySplitView

IMPLEMENT_DYNCREATE(CLibrarySplitView, CSplitView)

CLibrarySplitView::CLibrarySplitView()
{
   m_iFooterHeightTemplate = 60;
   m_iFooterHeight = 60;
   m_pfooter = NULL;
}

CLibrarySplitView::~CLibrarySplitView()
{
}


BEGIN_MESSAGE_MAP(CLibrarySplitView,CSplitView)
   ON_WM_CREATE()
   ON_WM_SIZE()
   ON_WM_DESTROY()
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CLibrarySplitView message handlers




BOOL CLibrarySplitView::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.dwExStyle = 0;
   cs.style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;

   return CSplitView::PreCreateWindow(cs);
}
//
// CSplitterWndEx window

class CSplitterWndEx2: public CSplitterWndEx
{

   // Construction
public:
   CSplitterWndEx2(){}

   virtual void OnDraw(CDC* pDC)
   {
      CSplitterWndEx::OnDraw(pDC);
   }
   // Implementation
public:
   virtual ~CSplitterWndEx2(){}

protected:
   DECLARE_MESSAGE_MAP()
public:
//   DECLARE_MESSAGE_MAP()
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};

int CLibrarySplitView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CSplitView::OnCreate(lpCreateStruct) == -1)
      return -1;

   m_psplit = new CSplitterWndEx2();

   m_psplit->CreateStatic(this,2,1);

   //m_psplit->CreateView(0,0,RUNTIME_CLASS(CLibraryTreeView),CSize(150,120),(CCreateContext*)lpCreateStruct->lpCreateParams);
   CCreateContext cc1= *(CCreateContext*)lpCreateStruct->lpCreateParams;
   m_psplit->CreateView(0,0,RUNTIME_CLASS(CLibraryTreeView),CSize(150,120), &cc1);
   CCreateContext cc2= *(CCreateContext*)lpCreateStruct->lpCreateParams;
   m_psplit->CreateView(1,0,RUNTIME_CLASS(CLibraryListView),CSize(100,100), &cc2);

   //m_psplit->CreateView(1,0,RUNTIME_CLASS(CLibraryListView),CSize(100,100),(CCreateContext*)lpCreateStruct->lpCreateParams);

   m_pfooter = (CLibraryFooter *) CreateView(RUNTIME_CLASS(CLibraryFooter),3, this, GetDocument());

   m_pfooter->OnInitialUpdate();

   CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

   m_pfooter->m_plist = pframe->m_plibrarylistview;

   pframe->m_plibrarylistview->m_pfooter = m_pfooter;

   return 0;

}


void CLibrarySplitView::OnSize(UINT nType,int cx,int cy)
{
   CSplitView::OnSize(nType,cx,cy);

   CRect rectClient;

   GetClientRect(rectClient);

   m_iFooterHeight = m_iFooterHeightTemplate * get_screen_dpi().cy / 96;

   m_rectBorder = CRect(8,8,8,8 + m_iFooterHeight);


   if(m_pfooter != NULL)
   {

      m_pfooter->SetWindowPos(&CWnd::wndTop,
         rectClient.left + m_rectBorder.left,
         rectClient.bottom - m_iFooterHeight,
         rectClient.Width() - m_rectBorder.left - m_rectBorder.right,
         m_iFooterHeight,
         SWP_SHOWWINDOW);

   }

}


void CLibrarySplitView::OnDraw(CDC* pDC)
{
   
   CDocument* pDoc = GetDocument();

   CRect rectClient;

   GetClientRect(rectClient);

   pDC->FillSolidRect(rectClient,GetSysColor(COLOR_3DFACE));

}


void CLibrarySplitView::OnDestroy()
{


   if(m_psplit != NULL)
   {

      try
      {

         m_psplit->DestroyWindow();

      }
      catch(...)
      {

      }

      //try
      //{

      //   delete m_psplit;

      //}
      //catch(...)
      //{

      //}

   }

   if(m_pfooter != NULL)
   {

      try
      {

         m_pfooter->DestroyWindow();

      }
      catch(...)
      {

      }

      m_pfooter = NULL;

   }

   CSplitView::OnDestroy();

}


BOOL CLibrarySplitView::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default
   return TRUE;
   //return CSplitView::OnEraseBkgnd(pDC);
}
BEGIN_MESSAGE_MAP(CSplitterWndEx2,CSplitterWnd)
   ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


BOOL CSplitterWndEx2::OnEraseBkgnd(CDC* pDC)
{
   // TODO: Add your message handler code here and/or call default
   return TRUE;
   return CSplitterWnd::OnEraseBkgnd(pDC);
}


BOOL CSplitterWndEx2::PreCreateWindow(CREATESTRUCT& cs)
{
   // TODO: Add your specialized code here and/or call the base class
   cs.dwExStyle |= WS_EX_COMPOSITED;
   return CSplitterWndEx::PreCreateWindow(cs);
}
