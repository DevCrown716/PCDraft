// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "Resource.h"
#include "PCDraft.h"
#include "LibraryTreeView.h"
#include "LibraryListView.h"
#include "LibraryDoc.h"
#include "LibraryFrame.h"
#include "ProgressBar.h"
#include "MainFrm.h"

int __progress_rate(double d) { return (int)(1000000.0 * d); }

double __double_rate(int i, int iCount) { return(double)i / (double)iCount; }
double __double(int i) { return(double)i; }


bool g_bUpdatingLibraryTree;
bool is_updating_library_tree()
{

   return g_bUpdatingLibraryTree;

}

class true_guard
{
public:
   bool & m_bKept;
   true_guard(bool &b) :m_bKept(b) { m_bKept = true; }
   ~true_guard() { m_bKept = false; }
   
};

CString get_file_name(const char *psz)
{

   if (psz == NULL)
   {

      return "";

   }

   if (*psz == '\0')
   {

      return "";

   }

   auto p1 = strrchr(psz, '\\');

   auto p2 = strrchr(psz, '/');
   
   if (p1 > p2)
   {

      return p1 + 1;

   }
   else if (p2 > p1)
   {

      return p2 + 1;

   }
   else
   {

      return "";
   }

}

void get_folder_name(CString & strFolder, const char *psz)
{

   if (psz == NULL)
   {

      strFolder.Empty();
      return;

   }

   if (*psz == '\0')
   {
      strFolder.Empty();
      return;

   }

   const char * p1 = strrchr(psz, '\\');

   const char * p2 = strrchr(psz, '/');

   if (p1 > p2)
   {

      strFolder = CString(psz,(int)( p1-psz));

   }
   else if (p2 > p1)
   {

      strFolder = CString(psz,(int) ( p2 - psz));

   }
   else
   {

      strFolder = psz;
   }

}
extern bool g_bEndApp;
void get_subdir(library_tree & tree);
void get_subdir(CString str, CStringArray & straPath, CStringArray & straName);

bool is_spotlibrary_path(CString strPath)
{
   
   CString strExt = ".spotlibrary";

   return strPath.Right(strExt.GetLength()).CompareNoCase(strExt) == 0;

}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CLibraryTreeView

IMPLEMENT_DYNCREATE(CLibraryTreeView, CTreeView)

CString new_uuid();

CLibraryTreeView::CLibraryTreeView()
{
   
   m_fontProgress.CreatePointFont(92, "Segoe UI");
   m_hitemDrop = NULL;
   m_bDropOver = false;
   m_bFirstLoad = true;

}


CLibraryTreeView::~CLibraryTreeView()
{
}

BEGIN_MESSAGE_MAP(CLibraryTreeView, CTreeView)
   ON_WM_CREATE()
   ON_NOTIFY_REFLECT(TVN_SELCHANGED,&CLibraryTreeView::OnTvnSelchanged)
   ON_MESSAGE(WM_APP + 1024,CLibraryTreeView::OnDropLibraryItem)
   ON_MESSAGE(WM_APP + 1026,CLibraryTreeView::OnDropFileList)
   ON_MESSAGE(WM_APP + 1027, CLibraryTreeView::OnPostSelChanged)
   ON_WM_KEYDOWN()
   ON_WM_PAINT()
   ON_WM_TIMER()
   ON_WM_SIZE()
END_MESSAGE_MAP()


// CLibraryTreeView diagnostics

#ifdef _DEBUG
void CLibraryTreeView::AssertValid() const
{
	CTreeView::AssertValid();
}

#ifndef _WIN32_WCE
void CLibraryTreeView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif
#endif //_DEBUG


// CLibraryTreeView message handlers

LRESULT CLibraryTreeView::OnDropFileList(WPARAM wparam,LPARAM lparam)
{

   CStringArray * pstra = (CStringArray *)lparam;

   CLibraryDoc * pDoc = (CLibraryDoc *)GetDocument();

   bool bUpdated = false;

   for(int i = 0; i < pstra->GetCount(); i++)
   {

      CString str = pstra->ElementAt(i);

      if(::PathIsDirectoryW(get_uni(str)))
      {

         pDoc->library_add(str,false);

         bUpdated = true;

      }


   }

   if(bUpdated)
   {

      pDoc->UpdateAllViews(NULL,LIBRARY_HINT_UPDATE_TREE);

   }

   return 0;

}


static UINT AFX_CDECL thread_update_tree(LPVOID p)
{

   CLibraryTreeView * ptree = (CLibraryTreeView *)p;

   true_guard guardUpdateTree(g_bUpdatingLibraryTree);

   ptree->on_update_tree1();

   return 0;

}


static UINT AFX_CDECL thread_selection_changed(LPVOID p)
{

   CLibraryTreeView * ptree = (CLibraryTreeView *)p;

   ptree->on_post_select();

   return 0;

}


int CLibraryTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   if (CTreeView::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   m_strProgress.LoadString(IDS_LOADING_LIBRARIES);

   CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

   pframe->m_plibrarytreeview = this;

   ModifyStyle(TVS_TRACKSELECT,TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_FULLROWSELECT);

   ModifyStyleEx(0,TVS_EX_FADEINOUTEXPANDOS);

   SetWindowTheme(GetSafeHwnd(),L"explorer",NULL);

   m_imagelist.Create(16,16,ILC_COLOR32,0,4);

   CLibraryDoc * pdoc = (CLibraryDoc *)GetDocument();

   pdoc->m_ptreeview = this;

   SHFILEINFO sfi;
   SecureZeroMemory(&sfi,sizeof sfi);
   HIMAGELIST hImageList = (HIMAGELIST)SHGetFileInfo(
      _T("Doesn't matter"),
      FILE_ATTRIBUTE_DIRECTORY,
      &sfi,sizeof sfi,
      SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
   HICON folderIcon = ImageList_GetIcon(hImageList,sfi.iIcon,ILD_TRANSPARENT);

   SecureZeroMemory(&sfi,sizeof sfi);
   hImageList = (HIMAGELIST)SHGetFileInfo(
      _T("Doesn't matter"),
      FILE_ATTRIBUTE_DIRECTORY,
      &sfi,sizeof sfi,
      SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | SHGFI_OPENICON);
   HICON openFolderIcon = ImageList_GetIcon(hImageList,sfi.iIcon,ILD_TRANSPARENT);

   SecureZeroMemory(&sfi,sizeof sfi);
   hImageList = (HIMAGELIST)SHGetFileInfo(
      _T("Doesn't matter"),
      FILE_ATTRIBUTE_NORMAL,
      &sfi,sizeof sfi,
      SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | SHGFI_OPENICON);
   HICON openFileIcon = ImageList_GetIcon(hImageList,sfi.iIcon,ILD_TRANSPARENT);

   SecureZeroMemory(&sfi,sizeof sfi);
   hImageList = (HIMAGELIST)SHGetFileInfo(
      _T("Doesn't matter"),
      FILE_ATTRIBUTE_NORMAL,
      &sfi,sizeof sfi,
      SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES | SHGFI_OPENICON);
   HICON fileIcon = ImageList_GetIcon(hImageList,sfi.iIcon,ILD_TRANSPARENT);

   m_imagelist.Add(folderIcon);
   m_imagelist.Add(openFolderIcon);
   m_imagelist.Add(fileIcon);
   m_imagelist.Add(openFileIcon);

   GetTreeCtrl().SetImageList(&m_imagelist,TVSIL_NORMAL);

   m_droptarget.Register(this);

   m_droptarget.m_ptarget = this;

   m_droptarget.m_pwndDrop = this;

   pdoc->library_load();

   g_bUpdatingLibraryTree = true;

   AfxBeginThread(&thread_update_tree, this);

   return 0;

}


void CLibraryTreeView::on_update_tree1()
{

   SetTimer(123, 20, NULL);

   update_tree();

   Sleep(30);

   KillTimer(123);

}


DROPEFFECT CLibraryTreeView::OnDragEnter(CWnd * pWnd,COleDataObject* pDataObject,DWORD dwKeyState,CPoint point)
{

   if(GetFocus() != this)
   {
   
      SetFocus();

   }

   return OnDragOver(pWnd,pDataObject,dwKeyState,point);

}


DROPEFFECT CLibraryTreeView::OnDragOver(CWnd * pWnd,COleDataObject* pDataObject,DWORD dwKeyState,CPoint point)
{

   if(pDataObject->IsDataAvailable(CF_OWNERDISPLAY))
   {

      TVHITTESTINFO info;

      info.pt = point;

      HTREEITEM hitem = GetTreeCtrl().HitTest(&info);

      m_bDropOver = true;

      try
      {

         if(hitem != m_hitemDrop)
         {

            if(hitem == NULL)
            {

               GetTreeCtrl().SelectItem(m_hitemMain);

            }
            else
            {

               GetTreeCtrl().SelectItem(hitem);

            }

            RedrawWindow();

         }

      }
      catch(...)
      {

      }

      m_bDropOver = false;

      CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

      int iFind = -1;

      CLibraryDoc * pdoc = (CLibraryDoc *)GetDocument();

      for(int i = 0; i < pdoc->m_itema.GetCount(); i++)
      {

         if(pdoc->m_itema[i] == hitem)
         {

            iFind = i;

         }

      }

      int iFindSel = -1;

      for(int i = 0; i < pdoc->m_itemaLibrary.GetCount(); i++)
      {

         if(pdoc->m_itemaLibrary[i] == hitem)
         {

            iFindSel = i;

         }

      }

      if(pframe->m_plibrarylistview != NULL)
      {

         if(iFind >= 0 && iFind < pdoc->m_straPath.GetCount())
         {

            if(is_spotlibrary_path(pdoc->m_straPath[iFind]))
            {

               return DROPEFFECT_COPY;

            }

         }

      }

      return DROPEFFECT_NONE;

   }
   else if(pDataObject->IsDataAvailable(CF_HDROP))
   {

      return DROPEFFECT_COPY;

   }
   else
   {

      return DROPEFFECT_NONE;

   }

}


void CLibraryTreeView::OnUpdate(CView* /*pSender*/,LPARAM lHint,CObject* /*pHint*/)
{

   if(lHint == LIBRARY_HINT_UPDATE_TREE)
   {

      update_tree();

   }

}


void CLibraryTreeView::update_tree(library_tree & tree, HTREEITEM hitemParent,CString strPath, double dProgressBegin, double dProgressRange)
{

   CStringArray straPath;

   CStringArray straName;

   CLibraryDoc * pdoc = (CLibraryDoc *)GetDocument();

   if(strPath.IsEmpty())
   {

      return;

   }

   for(int i = 0; i < tree.m_treea.GetCount(); i++)
   {

      if (g_bEndApp)
      {

         break;

      }

      auto & treeitem = tree.m_treea[i];

      CString strPath = treeitem.m_strPath;

      CString strName = treeitem.m_strName;

      bool bSpotlibrary = treeitem.m_bSpotLibrary;

      CString strTitle(strName);

      if(bSpotlibrary)
      {

         CString strExt = ".spotlibrary";

         strTitle = strTitle.Left(strTitle.GetLength() - strExt.GetLength());

      }

      CStringW wstrTitle = get_uni(strTitle);

      CString strTitleACP = get_cp(wstrTitle, CP_ACP);

      HTREEITEM hitem = GetTreeCtrl().InsertItem(strTitleACP,hitemParent);

      pdoc->m_itema.Add(hitem);

      pdoc->m_straPath.Add(strPath);

      if(bSpotlibrary)
      {

         GetTreeCtrl().SetItemImage(hitem,2,3);

      }
      else
      {

         GetTreeCtrl().SetItemImage(hitem, 0,1);

         update_tree(treeitem, hitem, strPath, dProgressBegin + dProgressRange * __double_rate(i, tree.m_treea.GetCount()), dProgressRange / __double(tree.m_treea.GetCount()));

      }

   }

}


void CLibraryTreeView::OnPaint()
{

   if (is_updating_library_tree()
      || 
      (m_bFirstLoad
         && (m_plibrarylistview->m_bLoadingLibrary
            || m_plibrarylistview->m_plibrary == NULL
            || !m_plibrarylistview->m_plibrary->m_bLibraryOk)))
   {

      CPaintDC dc(this); // device context for painting

      CRect rC;

      GetClientRect(rC);

      dc.SetMapMode(MM_TEXT);

      dc.FillSolidRect(rC, RGB(255, 255, 255));

      dc.FillSolidRect(m_rectProgress, RGB(230, 230, 230));

      CRect rectProgress(m_rectProgress);

      double dProgress = 0.9;

      if (m_plibrarylistview->m_plibraryProgress)
      {

         dProgress = m_plibrarylistview->m_plibraryProgress->m_dProgress;

      }
      else if (m_plibrarylistview->m_plibrary)
      {

         dProgress = m_plibrarylistview->m_plibrary->m_dProgress;

      }

      rectProgress.right = rectProgress.Width() * dProgress + rectProgress.left;

      dc.FillSolidRect(rectProgress, RGB(100, 200, 120));

      dc.Draw3dRect(m_rectProgress, RGB(192, 192, 192), RGB(192, 192, 192));

      dc.SetBkMode(TRANSPARENT);

      dc.SetTextColor(RGB(50, 50, 50));

      dc.SelectObject(&m_fontProgress);

      CRect rectText(m_rectProgress);

      rectText.OffsetRect(0, 16);

      rectText.bottom += 16;

      dc.DrawText(m_strProgress + CString('.', (GetTickCount() / 500 % 4)), rectText, DT_LEFT | DT_VCENTER);


      return;

   }

   CTreeView::OnPaint();

}

void CLibraryTreeView::gather_folders(library_tree & tree, double dProgressBegin, double dProgressRange)
{

   UpdateWindow();

   for (int i = 0; i < tree.m_treea.GetCount(); i++)
   {

      if (g_bEndApp)
      {

         return;

      }

      auto & treeitem = tree.m_treea[i];

      treeitem.m_strName = get_file_name(treeitem.m_strPath);

      treeitem.m_bSpotLibrary = is_spotlibrary_path(treeitem.m_strName);

      treeitem.m_iCountChildrenLibrary = 0;

      double dProgress = dProgressBegin + dProgressRange * __double_rate(i, tree.m_treea.GetCount());

      AfxGetApp()->m_pMainWnd->SendMessage(MYWM_PROGRESS, __progress_rate(dProgress));

      UpdateWindow();

      if (tree.m_bSpotLibrary)
      {

         tree.m_iCountChildrenLibrary++;

         continue;

      }

      get_subdir(treeitem);

      gather_folders(treeitem, dProgressBegin + dProgressRange * __double_rate(i, tree.m_treea.GetCount()), dProgressRange / __double(tree.m_treea.GetCount()));

      tree.m_iCountChildrenLibrary += treeitem.m_iCountChildrenLibrary;

   }


}


void CLibraryTreeView::filter_library(library_tree & pathtree, int iLevel)
{

   CMainFrame * pmainframe = (CMainFrame *) AfxGetApp()->m_pMainWnd;

   for (int i = 0; i < pathtree.m_treea.GetCount(); )
   {

      if (g_bEndApp)
      {

         return;

      }

      if (pathtree.m_treea[i].m_bSpotLibrary)
      {

         i++;
         
      }
      else if(pathtree.m_treea[i].m_iCountChildrenLibrary <= 0)
      {

         if (iLevel >= 1)
         {

            pathtree.m_treea.RemoveAt(i);

         }
         else
         { 
         
            i++;
            
         }

      }
      else
      {

         filter_library(pathtree.m_treea[i], iLevel + 1);

         i++;

      }

   }

}


void CLibraryTreeView::update_tree()
{
   
   CLibraryDoc * pdoc = (CLibraryDoc *)GetDocument();

   pdoc->m_itema.RemoveAll();

   pdoc->m_straPath.RemoveAll();

   pdoc->m_itemaLibrary.RemoveAll();

   GetTreeCtrl().DeleteAllItems();

   CString strText;

   strText.LoadString(IDS_MICROSPOT_LIBRARY);

   ((CMainFrame *)AfxGetApp()->m_pMainWnd)->m_wndStatusBar.m_progresscontrol.m_str.LoadString(IDS_LOADING_LIBRARIES);

   m_hitemMain = nullptr;

   CString strPath = pdoc->GetDefaultLibraryPath();

   GetTreeCtrl().SelectItem(m_hitemMain);

   CStringArray straLibrary;

   m_iLibraryCountCache = pdoc->library_count();

   pdoc->m_tree.m_treea.RemoveAll();

   pdoc->m_tree.m_iCountChildrenLibrary = 0;

   pdoc->m_tree.m_bSpotLibrary = false;

   library_tree item;

   item.m_strPath = strPath;

   pdoc->m_tree.m_treea.Add(item);

   for(int i = 0; i < m_iLibraryCountCache; i++)
   {

      library_tree item;

      item.m_strPath = pdoc->library_path(i);

      pdoc->m_tree.m_treea.Add(item);

      if (g_bEndApp)
      {

         return;

      }

   }

   gather_folders(pdoc->m_tree);

   filter_library(pdoc->m_tree);

   for(int i = 0; i < pdoc->m_tree.m_treea.GetCount(); i++)
   {

      CString strPath = pdoc->library_path(i-1);

      CString strLibrary = pdoc->library_name(i - 1);

      CStringW wstrLibrary = get_uni(strLibrary);

      CString strLibraryACP = get_cp(wstrLibrary, CP_ACP);

      HTREEITEM hitem = GetTreeCtrl().InsertItem(strLibraryACP,TVI_ROOT);

      if (i == 0)
      {

         m_hitemMain = hitem;

      }

      auto & treeitem = pdoc->m_tree.m_treea[i];

      pdoc->m_itemaLibrary.Add(hitem);

      pdoc->m_itema.Add(hitem);

      pdoc->m_straPath.Add(strPath);

      bool bSpotlibrary = treeitem.m_bSpotLibrary;

      if(bSpotlibrary)
      {

         GetTreeCtrl().SetItemImage(hitem,2,3);

      }
      else
      {

         GetTreeCtrl().SetItemImage(hitem,0,1);

         update_tree(treeitem, hitem, strPath, __double_rate(i, pdoc->m_tree.m_treea.GetCount()), 1.0 / __double(pdoc->m_tree.m_treea.GetCount()));

      }

   }

   GetTreeCtrl().Expand(m_hitemMain, TVE_EXPAND);

   AfxGetApp()->m_pMainWnd->SendMessage(MYWM_PROGRESS, __progress_rate(1.0));

   SetRedraw();

   Sleep(300);

   AfxGetApp()->m_pMainWnd->SendMessage(MYWM_PROGRESS, 0);

   SetRedraw();

   AfxGetApp()->m_pMainWnd->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);

   ((CMainFrame *)AfxGetApp()->m_pMainWnd)->m_wndStatusBar.RedrawWindow();

}


void CLibraryTreeView::OnTimer(UINT_PTR nIDEvent)
{

   CTreeView::OnTimer(nIDEvent);

   if (nIDEvent == 123)
   {

      RedrawWindow(NULL, NULL, RDW_INVALIDATE);

   }

}

void CLibraryTreeView::on_sel_tree_item(HTREEITEM hitem, bool bAsync, bool bRedraw)
{

   int iFind = -1;

   CLibraryDoc * pdoc = (CLibraryDoc *)GetDocument();

   for(int i = 0; i < pdoc->m_itema.GetCount(); i++)
   {

      if(pdoc->m_itema[i] == hitem)
      {

         iFind = i;

      }

   }

   int iFindSel = -1;

   for(int i = 0; i < pdoc->m_itemaLibrary.GetCount(); i++)
   {

      if(pdoc->m_itemaLibrary[i] == hitem)
      {

         iFindSel = i;

      }

   }

   pdoc->library_sel(iFindSel);

   if(m_plibrarylistview != NULL)
   {

      if(iFind >= 0 && iFind < pdoc->m_straPath.GetCount())
      {

         m_plibrarylistview->start_load(pdoc->m_straPath[iFind]);

      }
      else
      {

         m_plibrarylistview->start_load(pdoc->GetDefaultLibraryPath());

      }

   }

}


void CLibraryTreeView::on_post_select()
{

   SetTimer(123, 20, NULL);

   on_sel_tree_item(m_hitemSel, true, true);
   
}


LRESULT CLibraryTreeView::OnPostSelChanged(WPARAM wparam, LPARAM lparam)
{

   RedrawWindow();

   m_hitemSel = (HTREEITEM) lparam;

   AfxBeginThread(&thread_selection_changed, this);

   return 0;

}


void CLibraryTreeView::OnTvnSelchanged(NMHDR *pNMHDR,LRESULT *pResult)
{
   
   LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
   
   if(m_bDropOver)
   {
      
      m_hitemDrop = pNMTreeView->itemNew.hItem;
      
      *pResult = 0;
      
      return;

   }

   RedrawWindow();
   
   PostMessage(WM_APP + 1027, 0, (LPARAM)pNMTreeView->itemNew.hItem);
   
   *pResult = 0;

}



LRESULT CLibraryTreeView::OnDropLibraryItem(WPARAM wparam,LPARAM lparam)
{

   CLibraryItem * pitem = (CLibraryItem *)lparam;

   if(pitem->m_pdoc == NULL)
   {

      pitem->load_bk();

   }

   on_sel_tree_item(m_hitemDrop, true, false);

   m_hitemDrop = NULL;

   CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

   if(!is_spotlibrary_path(pframe->m_plibrarylistview->m_plibrary->m_strPath2))
   {

      return 0;

   }

   pframe->m_plibrarylistview->m_plibrary->library_add(pitem);

   CLibraryDoc * pdoc = (CLibraryDoc *)GetDocument();

   pframe->m_plibrarylistview->m_plibrary->defer_save();

   pframe->m_plibrarylistview->layout();

   pframe->m_plibrarylistview->RedrawWindow();

   RedrawWindow();

   return 0;

}



void CLibraryTreeView::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
{
   
   if(nChar == VK_DELETE)
   {

      int iFind = -1;

      CLibraryDoc * pdoc = (CLibraryDoc *)GetDocument();

      for(int i = 0; i < pdoc->m_itemaLibrary.GetCount(); i++)
      {

         if(pdoc->m_itemaLibrary[i] == GetTreeCtrl().GetSelectedItem())
         {

            iFind = i;

         }

      }
      iFind--;

      if(iFind >= 0)
      {

         CLibraryDoc * pdoc = (CLibraryDoc *)GetDocument();

         pdoc->library_sel(iFind);

         pdoc->library_remove();

         

         GetTreeCtrl().SelectItem(pdoc->m_itemaLibrary[iFind]);

         //CLibraryFrame * pframe = (CLibraryFrame *)GetParentFrame();

         //pframe->m_plibrarylistview->library_load(pdoc->GetDefaultLibraryPath(),true,true);

      }

   }

   __super::OnKeyDown(nChar,nRepCnt,nFlags);

}


void get_subdir(library_tree & tree)
{

   CStringArray straPath;

   CStringArray straName;

   get_subdir(tree.m_strPath, straPath, straName);

   for (int i = 0; i < straPath.GetCount(); i++)
   {

      library_tree treeitem;

      treeitem.m_strPath = straPath[i];

      treeitem.m_strName = straName[i];

      tree.m_treea.Add(treeitem);

   }

}





void CLibraryTreeView::OnSize(UINT nType, int cx, int cy)
{

   CView::OnSize(nType, cx, cy);

   CRect rC;

   GetClientRect(rC);

   CRect rectStatusBar;

   rectStatusBar.top = ((rC.top + rC.bottom) / 2) - 8;
   rectStatusBar.bottom = ((rC.top + rC.bottom) / 2) + 8;

   rectStatusBar.left = (rC.Width()) / 5;
   rectStatusBar.right = (rC.Width()) * 4 / 5;

   m_rectProgress = rectStatusBar;

}



