// SplitView.cpp : implementation file
//

#include "stdafx.h"
#include "PCDraft.h"
#include "SplitView.h"


// CSplitView

IMPLEMENT_DYNCREATE(CSplitView, CView)

CSplitView::CSplitView()
{
   m_rectBorder = CRect(0,0,0,0);
}

CSplitView::~CSplitView()
{
}

BEGIN_MESSAGE_MAP(CSplitView, CView)
   ON_WM_SIZE()
END_MESSAGE_MAP()


// CSplitView drawing

void CSplitView::OnDraw(CDC* pDC)
{

   CDocument* pDoc = GetDocument();

   CRect rectClient;

   GetClientRect(rectClient);

   pDC->FillSolidRect(rectClient,GetSysColor(COLOR_3DFACE));

}


// CSplitView diagnostics

#ifdef _DEBUG
void CSplitView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CSplitView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSplitView message handlers


void CSplitView::OnSize(UINT nType,int cx,int cy)
{
   CView::OnSize(nType,cx,cy);

   CRect rectClient;

   GetClientRect(rectClient);


   if(m_psplit != NULL)
   {

      m_psplit->SetWindowPos(&CWnd::wndTop,
         rectClient.left + m_rectBorder.left,
         rectClient.top + m_rectBorder.top,
         rectClient.Width() - m_rectBorder.left - m_rectBorder.right,
         rectClient.Height() - m_rectBorder.top - m_rectBorder.bottom,
         SWP_SHOWWINDOW);

   }

}
