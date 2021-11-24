// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LibraryImageSplitView.h"
#include "LibraryImageTreeView.h"
#include "LibraryImageListView.h"
#include "LibraryPhoto.h"
#include "LibraryFrame.h"
#include "LibraryFooter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CWnd* CreateView(CRuntimeClass * pclass,UINT nID,CWnd * pwndParent,CDocument * pdoc);

// CLibraryImageSplitView

IMPLEMENT_DYNCREATE(CLibraryImageSplitView, CSplitView)

CLibraryImageSplitView::CLibraryImageSplitView()
{
   m_iFooterHeight = 60;
   m_rectBorder = CRect(8,8,8,8 + m_iFooterHeight);
   m_pfooter = NULL;

}

CLibraryImageSplitView::~CLibraryImageSplitView()
{
}


BEGIN_MESSAGE_MAP(CLibraryImageSplitView, CSplitView)
   ON_WM_CREATE()
   ON_WM_SIZE()
   ON_WM_DESTROY()
END_MESSAGE_MAP()



// CLibraryImageSplitView message handlers




int CLibraryImageSplitView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CSplitView::OnCreate(lpCreateStruct) == -1)
      return -1;
   m_psplit = new CSplitterWndEx();

   m_psplit->CreateStatic(this,2,1);

   m_psplit->CreateView(0,0,RUNTIME_CLASS(CLibraryImageTreeView),CSize(150,120),(CCreateContext*)lpCreateStruct->lpCreateParams);

   m_psplit->CreateView(1,0,RUNTIME_CLASS(CLibraryImageListView),CSize(100,100),(CCreateContext*)lpCreateStruct->lpCreateParams);

   m_pfooter = (CLibraryFooter *)CreateView(RUNTIME_CLASS(CLibraryFooter),3,this,GetDocument());

   m_pfooter->OnInitialUpdate();

   m_pfooter->m_stLibraryOpen.ShowWindow(SW_HIDE);

   CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

   m_pfooter->m_plist = pframe->m_pphotolibrarylistview;

   pframe->m_pphotolibrarylistview->m_pfooter = m_pfooter;

   return 0;

}


void CLibraryImageSplitView::OnSize(UINT nType,int cx,int cy)
{
   CSplitView::OnSize(nType,cx,cy);


   CRect rectClient;

   GetClientRect(rectClient);


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


void CLibraryImageSplitView::OnDestroy()
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

      //try
      //{

      //   delete m_pfooter;

      //}
      //catch(...)
      //{

      //}

   }

   CSplitView::OnDestroy();

   
}
