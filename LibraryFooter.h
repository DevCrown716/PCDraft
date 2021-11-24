// Library - PCDraft 6 - Infinisys Ltd.
#pragma once


#include "SearchBox.h"
#include "afxwin.h"
#include "afxcmn.h"


class CLibraryFooter : public CFormView
{
public:
   
   

   CMenu       m_menu;
   CMenu *     m_pmenu;
   DWORD       m_dwMenuClose;

   CWnd *      m_plist;
   CBitmap     m_bitmapOpen;
   CBitmap     m_bitmapOpenSel;

   CStatic     m_stLess;
   CStatic     m_stMore;
   CSliderCtrl m_slider;

   CStatic     m_stLibraryOpen;
   CSearchBox  m_search;
   CStatic     m_stCount;


   CLibraryFooter();           // protected constructor used by dynamic creation
	virtual ~CLibraryFooter();


   enum { IDD = IDD_LIBRARY_FOOTER };

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void OnInitialUpdate();

	
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType,int cx,int cy);
   afx_msg void OnStnClickedLibraryOpen();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   afx_msg void OnUnInitMenuPopup(CMenu *pPopupMenu,UINT nFlags);
   afx_msg void OnFileNewlibrary();
   afx_msg void OnFileOpenlibrary();
   afx_msg void OnNMCustomdrawSlider(NMHDR *pNMHDR,LRESULT *pResult);
   afx_msg void OnThumbposchangingSlider(NMHDR *pNMHDR,LRESULT *pResult);
   afx_msg void OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
   afx_msg void OnChangeSearch();
   afx_msg void OnFileRenameitem();
   afx_msg void OnUpdateFileRenameitem(CCmdUI *pCmdUI);


   DECLARE_DYNCREATE(CLibraryFooter)
   DECLARE_MESSAGE_MAP()


   afx_msg void OnPaint();
};


