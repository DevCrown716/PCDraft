// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "PCDraft.h"
#include "LibraryImageTreeView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CLibraryImageTreeView

IMPLEMENT_DYNCREATE(CLibraryImageTreeView, CTreeView)

CLibraryImageTreeView::CLibraryImageTreeView()
{

}

CLibraryImageTreeView::~CLibraryImageTreeView()
{
}

BEGIN_MESSAGE_MAP(CLibraryImageTreeView, CTreeView)
   ON_WM_CREATE()
END_MESSAGE_MAP()


// CLibraryImageTreeView diagnostics

#ifdef _DEBUG
void CLibraryImageTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CLibraryImageTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLibraryImageTreeView message handlers


int CLibraryImageTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if(CTreeView::OnCreate(lpCreateStruct) == -1)
      return -1;

   SetRedraw(FALSE);

   GetTreeCtrl().DeleteAllItems();

   CLibraryDoc * pdoc = (CLibraryDoc *)GetDocument();

   m_hitemPictures= GetTreeCtrl().InsertItem("Pictures",TVI_ROOT);

   GetTreeCtrl().Expand(m_hitemPictures,TVE_EXPAND);

   SetRedraw();


   return 0;
}
