// Library - PCDraft 6 - Infinisys Ltd.
#pragma once

#include "SplitView.h"

class CLibraryFooter;


class CLibrarySplitView : public CSplitView
{
public:


   int               m_iFooterHeight;
   int               m_iFooterHeightTemplate;
   CLibraryFooter *  m_pfooter;


   CLibrarySplitView();
	virtual ~CLibrarySplitView();

   
   virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual void OnDraw(CDC* /*pDC*/);


   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType,int cx,int cy);
   afx_msg void OnDestroy();

   
   DECLARE_DYNCREATE(CLibrarySplitView)
   DECLARE_MESSAGE_MAP()


   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


