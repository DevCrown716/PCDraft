// Library - PCDraft 6 - Infinisys Ltd.
#pragma once


#include "LibraryDropTarget.h"
#include "LibraryDoc.h"

class CLibraryListView;

class CLibraryTreeView : public CTreeView,
   public drop_target
{
public:

   CFont                   m_fontProgress;
   CString                 m_strProgress;
   bool                    m_bFirstLoad;
   int                     m_iLibraryCountCache;
   HTREEITEM               m_hitemDrop;
   HTREEITEM               m_hitemMain;
   HTREEITEM               m_hitemSel;
   CStringArray            m_straPath;
   bool                    m_bDropOver;
   CLibraryDropTarget      m_droptarget;
   CImageList              m_imagelist;
   CLibraryListView *      m_plibrarylistview;
   CRect                   m_rectProgress;

	
   CLibraryTreeView();
	virtual ~CLibraryTreeView();


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


   void on_update_tree1();

   void on_post_select();


   virtual void OnUpdate(CView* /*pSender*/,LPARAM /*lHint*/,CObject* /*pHint*/);
   virtual void update_tree();
   virtual void gather_folders(library_tree & tree, double dProgressBegin = 0.0, double dProgressRange = 1.0);
   virtual void filter_library(library_tree & tree, int iLevel = 0);
   virtual void update_tree(library_tree & tree, HTREEITEM hitem, CString strPath, double dProgressBegin= 0.0, double dProgressRange =1.0);
   virtual void on_sel_tree_item(HTREEITEM hitem,bool bAsync,bool bRedraw);


   virtual DROPEFFECT OnDragEnter(CWnd * pWnd,COleDataObject* pDataObject,DWORD dwKeyState,CPoint point);
   virtual DROPEFFECT OnDragOver(CWnd * pWnd,COleDataObject* pDataObject,DWORD dwKeyState,CPoint point);

   afx_msg void OnTimer(UINT_PTR nIDEvent);
   afx_msg void OnPaint();
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnTvnSelchanged(NMHDR *pNMHDR,LRESULT *pResult);
   afx_msg void OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags);
   afx_msg LRESULT OnDropLibraryItem(WPARAM wparam,LPARAM lparam);
   afx_msg LRESULT OnDropFileList(WPARAM wparam,LPARAM lparam);
   afx_msg LRESULT OnPostSelChanged(WPARAM wparam, LPARAM lparam);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   
   DECLARE_DYNCREATE(CLibraryTreeView)
   DECLARE_MESSAGE_MAP()

};


