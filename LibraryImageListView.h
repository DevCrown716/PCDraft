// Library - PCDraft 6 - Infinisys Ltd.
#pragma once


#include "LibraryDragSource.h"


class CPhotoLibrary;
class CLibraryFooter;


class CLibraryImageListView : public CScrollView
{
public:
   
   
   CSize                m_sizeMin;
   CSize                m_sizeMax;
   CSize                m_size;
   int                  m_iSel;

   CLibraryDragSource   m_drag;
   CLibraryFooter *     m_pfooter;
   CPhotoLibrary *      m_plibrary;


	CLibraryImageListView();           // protected constructor used by dynamic creation
	virtual ~CLibraryImageListView();


   virtual void OnDraw(CDC* pDC);      // overridden to draw this view

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

   void set_size_rate(double dRate);
   void layout();
   void library_load(CString strPath,bool bRedraw);

   afx_msg LRESULT OnSetSizeRate(WPARAM wparam,LPARAM lparam);

   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType,int cx,int cy);
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);


   DECLARE_DYNCREATE(CLibraryImageListView)
   DECLARE_MESSAGE_MAP()

};


