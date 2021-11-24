// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once


#include "../CustomMiniFrameWnd.h"



class CWelcomeFrame :
   public CFrameWnd
{
public:


   ULONG                   m_ulRef;
   bool                    m_bOpenLibrary;
   CString                 m_strNewPath;
   CString                 m_strSelPath;
   //CToolBar                m_toolbar;
   CFrameWnd *             m_pMainFrame;
   bool                    m_bTracking;
   BROWSEINFOW              m_bi;
   CRect m_rectCaption;
   HICON m_hiconCaption; 
   bool m_bTopLeftButtons;
   


   CWelcomeFrame();           // protected constructor used by dynamic creation
	~CWelcomeFrame() override;




   BOOL Create(CWnd* pParent,DWORD dwBarStyle,PVOID p);
   virtual BOOL LoadFrame(UINT nIDResource,DWORD dwDefaultStyle,CWnd* pParentWnd,CCreateContext* pContext);
   //virtual void NotifyFloatingWindows(DWORD dwFlags);
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

   afx_msg void OnPaint(); 
   afx_msg LRESULT OnDwm(WPARAM wparam, LPARAM lparam); 
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpcs);
   afx_msg BOOL OnNcActivate(BOOL bActive);
   afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized);
   afx_msg void OnNcLButtonDblClk(UINT nHitTest,CPoint point);
   afx_msg void OnNcLButtonDown(UINT nHitTest,CPoint point);
   afx_msg void OnNcLButtonUp(UINT nHitTest,CPoint point);
   afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
   afx_msg void OnClose();
   afx_msg LRESULT OnResize(WPARAM wparam, LPARAM lparam);


   virtual ULONG STDMETHODCALLTYPE AddRef();
   virtual ULONG STDMETHODCALLTYPE Release();
   virtual HRESULT STDMETHODCALLTYPE QueryInterface(const IID &,void **);
   virtual HRESULT STDMETHODCALLTYPE GetEnumFlags(IShellFolder * psf,PCIDLIST_ABSOLUTE pidlFolder,HWND * phwnd,DWORD * pgrfFlags);
   virtual HRESULT STDMETHODCALLTYPE ShouldShow(IShellFolder * psf,PCIDLIST_ABSOLUTE pidlFolder,PCUITEMID_CHILD pidlItem);


   DECLARE_DYNCREATE(CWelcomeFrame)
   DECLARE_MESSAGE_MAP()

   afx_msg void OnSize(UINT nType, int cx, int cy);
};


