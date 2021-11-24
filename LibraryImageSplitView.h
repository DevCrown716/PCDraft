// Library - PCDraft 6 - Infinisys Ltd.
#pragma once

#include "SplitView.h"

class CLibraryFooter;

class CLibraryImageSplitView : public CSplitView
{
public:


   int               m_iFooterHeight;
   CLibraryFooter *  m_pfooter;


   CLibraryImageSplitView();
   virtual ~CLibraryImageSplitView();


   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType,int cx,int cy);
   afx_msg void OnDestroy();


   DECLARE_DYNCREATE(CLibraryImageSplitView)
   DECLARE_MESSAGE_MAP()
};


