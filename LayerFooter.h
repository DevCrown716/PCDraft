// Layer - PCDraft 6 - Infinisys Ltd.
#pragma once


#include "SearchBox.h"
#include "afxwin.h"
#include "afxcmn.h"


class CLayerFooter : public CFormView
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
   CStatic     m_stLayerOpen;
   CSearchBox  m_search;
   CStatic     m_stCount;

   CLayerFooter();           // protected constructor used by dynamic creation

   virtual ~CLayerFooter();

   enum { IDD = IDD_LAYER_FOOTER };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
   virtual void OnInitialUpdate();
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

   afx_msg void OnSize(UINT nType,int cx,int cy);
   afx_msg void OnStnClickedLayerOpen();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnUnInitMenuPopup(CMenu *pPopupMenu,UINT nFlags);
   afx_msg void OnFileNewlibrary();
   afx_msg void OnFileOpenlibrary();
   afx_msg void OnNMCustomdrawSlider(NMHDR *pNMHDR,LRESULT *pResult);
   afx_msg void OnThumbposchangingSlider(NMHDR *pNMHDR,LRESULT *pResult);
   afx_msg void OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
   afx_msg void OnChangeSearch();
   afx_msg void OnFileRenameitem();
   afx_msg void OnUpdateFileRenameitem(CCmdUI *pCmdUI);


   DECLARE_DYNCREATE(CLayerFooter)
   DECLARE_MESSAGE_MAP()


};


