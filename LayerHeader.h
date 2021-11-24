// Layer - PCDraft 6 - Infinisys Ltd.
#pragma once

class CLayerListView;

#include "SearchBox.h"
#include "afxwin.h"
#include "afxcmn.h"


class CBtnStatic:
   public CStatic
{
public:

   CBitmap  m_bitmap;
   CBitmap  m_bitmapSel;
   CBitmap  m_bitmapDisabled;

   bool m_bPressed;

   CBtnStatic();
   virtual ~CBtnStatic();

   void OnUpdateState();

   DECLARE_MESSAGE_MAP()
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags,CPoint point);
   afx_msg void OnEnable(BOOL bEnable);

};


class CLayerHeader :
   public CFormView
{
public:

   CMenu          m_menu;
   CMenu *        m_pmenu;
   DWORD          m_dwMenuClose;

   CLayerListView * m_plist;
   CBitmap        m_bitmapMenu;
   CBitmap        m_bitmapMenuSel;

   CBtnStatic     m_stMerge;
   CBtnStatic     m_stAdd;
   CBtnStatic     m_stDelete;
   CStatic        m_stMenu;
   CSize          m_size;

   CLayerHeader();           // protected constructor used by dynamic creation
	virtual ~CLayerHeader();


   enum { IDD = IDD_LAYER_HEADER };


#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


   CLayerDoc * GetDocument();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


   virtual void OnInitialUpdate();
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

   virtual CSize GetTotalSize() const;

   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType,int cx,int cy);
   afx_msg void OnStnClickedLayerMenu();
   afx_msg void OnStnClickedLayerAdd();
   afx_msg void OnStnClickedLayerDelete();
   afx_msg void OnStnClickedLayerMerge();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
   afx_msg void OnUnInitMenuPopup(CMenu *pPopupMenu,UINT nFlags);
   afx_msg void OnFileNewlibrary();
   afx_msg void OnFileOpenlibrary();
   afx_msg void OnNMCustomdrawSlider(NMHDR *pNMHDR,LRESULT *pResult);
   afx_msg void OnThumbposchangingSlider(NMHDR *pNMHDR,LRESULT *pResult);
   afx_msg void OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
   afx_msg void OnChangeSearch();
   afx_msg void OnFileRenameitem();

   virtual void Refresh();

   DECLARE_DYNCREATE(CLayerHeader)
   DECLARE_MESSAGE_MAP()

};


