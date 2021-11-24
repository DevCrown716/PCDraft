// Library - PCDraft 6 - Infinisys Ltd.
#pragma once


#include "LibraryDragDrop.h"
#include "LibraryObject.h"
#include "ListControl.h"


class CLibraryFooter;
class CLibraryDoc;


class CLibraryListView : public CScrollView
{
public:

   CFont                m_fontProgress;
   CString              m_strProgress;

   bool                 m_bProgressBarVisible;

   CSize                m_sizeMin;
   CSize                m_sizeMax;
   CSize                m_size;
   CArray < int >       m_iaSel;
   int                  m_iLastSel;
   CString              m_strLoadPath;

   DWORD                m_dwStartProgress;
   CLibrary *           m_plibrary;
   CLibrary *           m_plibraryProgress;
   CLibraryFooter *     m_pfooter;
   CListEditBox         m_edit;

   CLibraryDragDrop     m_dragdrop;
   CFont                m_font;

   CMutex               m_mutexLoad;

   bool                 m_bLoadingLibrary;
   CRect                m_rectProgress;

   //CProgressCtrl        m_wndProgBar;          // the progress bar


   CLibraryListView();           // protected constructor used by dynamic creation
   virtual ~CLibraryListView();


   virtual BOOL PreTranslateMessage(MSG* pMsg);
   virtual void OnDraw(CDC* pDC);      // overridden to draw this view

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

   virtual void start_load(CString strLoadPath);

   void rename_item();
   int get_sel();
   void set_size_rate(double dRate, bool bLayoutAndRedraw);
   void layout();
   bool is_selected(int iItem);
   void library_load(CString strPath,bool bAsync,bool bRedraw);
   void finish_rename_item();


   virtual bool is_drop_enabled();


   CLibraryDoc * GetDocument();
   CLibraryItem * LibraryItemFromSelection();


   afx_msg LRESULT OnDropLibraryItem(WPARAM wparam,LPARAM lparam);
   afx_msg LRESULT OnSetSizeRate(WPARAM wparam,LPARAM lparam);
   

   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnSize(UINT nType,int cx,int cy);
   afx_msg void OnDestroy();
   afx_msg LRESULT OnEditKillFocus(WPARAM wparam, LPARAM lparam);
   afx_msg void OnFileRenameitem();
   afx_msg void OnUpdateFileRenameitem(CCmdUI *pCmdUI);
   afx_msg void OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags);
   afx_msg HCURSOR OnQueryDragIcon();


   DECLARE_DYNCREATE(CLibraryListView)
   DECLARE_MESSAGE_MAP()


   virtual DROPEFFECT OnDragEnter(CWnd * pWnd,COleDataObject * pDataObject,DWORD dwKeyState,CPoint point);
   virtual DROPEFFECT OnDragOver(CWnd * pWnd,COleDataObject* pDataObject,DWORD dwKeyState,CPoint point);


   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnPaint();
   virtual void OnPrint(CDC* pDC,CPrintInfo* pInfo);
   afx_msg void OnShowWindow(BOOL bShow,UINT nStatus);
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   afx_msg LRESULT OnFinishLoading(WPARAM wparam, LPARAM lparam);


};


