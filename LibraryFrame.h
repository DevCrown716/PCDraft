// Library - PCDraft 6 - Infinisys Ltd.
#pragma once

#include "CustomMiniFrameWnd.h"

class CLibraryTreeView;
class CLibraryListView;
class CLibraryMainView;
class CLibraryImageListView;


class CLibraryFrame :
   public CCustomMiniFrameWnd,
   public IFolderFilter
{
public:

   ULONG                   m_ulRef;
   bool                    m_bOpenLibrary;
   CString                 m_strNewPath;
   CString                 m_strSelPath;
   CLibraryTreeView *      m_plibrarytreeview;
   CLibraryListView *      m_plibrarylistview;
   CLibraryImageListView * m_pphotolibrarylistview;
   CLibraryMainView *      m_plibrarymainview;
   //CToolBar                m_toolbar;
   CFrameWnd *             m_pMainFrame;
   bool                    m_bTracking;
   BROWSEINFOW              m_bi;


	CLibraryFrame();           // protected constructor used by dynamic creation
	virtual ~CLibraryFrame();


   bool ValidateNewName(const char * psz,CString & strCandidateNewPath);


   BOOL Create(CWnd* pParent,DWORD dwBarStyle,PVOID p);
   virtual BOOL LoadFrame(UINT nIDResource,DWORD dwDefaultStyle,CWnd* pParentWnd,CCreateContext* pContext);
   virtual void NotifyFloatingWindows(DWORD dwFlags);
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

   
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpcs);
   afx_msg BOOL OnNcActivate(BOOL bActive);
   afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized);
   afx_msg void OnNcLButtonDblClk(UINT nHitTest,CPoint point);
   afx_msg void OnNcLButtonDown(UINT nHitTest,CPoint point);
   afx_msg void OnNcLButtonUp(UINT nHitTest,CPoint point);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
   afx_msg void OnFileNewlibrary();
   afx_msg void OnFileOpenlibrary();
   afx_msg void OnClose();
   afx_msg void OnViewLibrary();
   afx_msg void OnViewPhotoLibrary();
   afx_msg void OnFileRenameitem();
   afx_msg void OnUpdateFileRenameitem(CCmdUI *pCmdUI);
   afx_msg LRESULT OnResize(WPARAM wparam, LPARAM lparam);


   virtual ULONG STDMETHODCALLTYPE AddRef();
   virtual ULONG STDMETHODCALLTYPE Release();
   virtual HRESULT STDMETHODCALLTYPE QueryInterface(const IID &,void **);
   virtual HRESULT STDMETHODCALLTYPE GetEnumFlags(IShellFolder * psf,PCIDLIST_ABSOLUTE pidlFolder,HWND * phwnd,DWORD * pgrfFlags);
   virtual HRESULT STDMETHODCALLTYPE ShouldShow(IShellFolder * psf,PCIDLIST_ABSOLUTE pidlFolder,PCUITEMID_CHILD pidlItem);


   DECLARE_DYNCREATE(CLibraryFrame)
   DECLARE_MESSAGE_MAP()

};


