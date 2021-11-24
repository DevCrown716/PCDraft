// Layer - PCDraft 6 - Infinisys Ltd.
#pragma once


#include "CustomMiniFrameWnd.h"

class CLayerListView;
class CLayerMainView;
class CLayerImageListView;
class CLayerHeader;

class CLayerFrame :
   public CCustomMiniFrameWnd,
   public IFolderFilter
{
public:


   bool                       m_bIgnoreActivate;


   ULONG                      m_ulRef;
   bool                       m_bOpenLayer;
   CString                    m_strNewPath;
   CString                    m_strSelPath;
   CLayerListView *           m_playerlistview;
   CLayerHeader *             m_playerheader;
   CLayerMainView *           m_playermainview;
   CToolBar                   m_toolbar;
   CFrameWnd *                m_pMainFrame;
   bool                       m_bTracking;



   CLayerFrame();           // protected constructor used by dynamic creation
   virtual ~CLayerFrame();


   

   bool ValidateNewName(const char * psz,CString & strCandidateNewPath);


   BOOL Create(CWnd* pParent,DWORD dwBarStyle,PVOID p);
   virtual void NotifyFloatingWindows(DWORD dwFlags);
   virtual BOOL LoadFrame(UINT nIDResource,DWORD dwDefaultStyle,CWnd* pParentWnd,CCreateContext* pContext);
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg BOOL OnNcActivate(BOOL bActive);
   afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpcs);
   afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized);
   afx_msg void OnNcLButtonDblClk(UINT nHitTest,CPoint point);
   afx_msg void OnNcLButtonDown(UINT nHitTest,CPoint point);
   afx_msg void OnNcLButtonUp(UINT nHitTest,CPoint point);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
   afx_msg void OnClose();
   afx_msg void OnFileRenameitem();
   afx_msg void OnUpdateFileRenameitem(CCmdUI *pCmdUI);
   afx_msg void OnUnInitMenuPopup(CMenu *pPopupMenu,UINT nFlags);
   afx_msg LRESULT OnResize(WPARAM wparam,LPARAM lparam);
      
   virtual void DoIdleUpdate();
   virtual void Refresh();


   virtual ULONG STDMETHODCALLTYPE AddRef();
   virtual ULONG STDMETHODCALLTYPE Release();
   virtual HRESULT STDMETHODCALLTYPE QueryInterface(const IID &,void **);
   virtual HRESULT STDMETHODCALLTYPE GetEnumFlags(IShellFolder * psf,PCIDLIST_ABSOLUTE pidlFolder,HWND * phwnd,DWORD * pgrfFlags);
   virtual HRESULT STDMETHODCALLTYPE ShouldShow(IShellFolder * psf,PCIDLIST_ABSOLUTE pidlFolder,PCUITEMID_CHILD pidlItem);


   DECLARE_DYNCREATE(CLayerFrame)
   DECLARE_MESSAGE_MAP() 

};


