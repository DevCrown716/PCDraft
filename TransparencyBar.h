// TransparencyBar.h : header file
// Infinisys Ltd
//
#pragma once

#include "Resource.h"

class CPCDraftView;

/////////////////////////////////////////////////////////////////////////////
// CTransparencyBar window

enum e_attrib // Interface Update - Infinisys Ltd.
{
   attrib_none,
   attrib_fill,
   attrib_pfill,
   attrib_pen
};

class CTransparencyBar: public CDialog
{
public:

   enum { IDD = IDD_TRANSPARENCY_BAR };

   e_attrib          m_eattrib;
   CPCDraftView *    m_pview;
   CSliderCtrl       m_slider;
   CEdit *           m_pedit;
   CWnd *            m_pwnd;
   bool              m_bFirstChange;
   
   CTransparencyBar();
   virtual ~CTransparencyBar();


   void DoDataExchange(CDataExchange* pDX);

   afx_msg void OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized);
   afx_msg BOOL OnInitDialog();

   DECLARE_MESSAGE_MAP()

   afx_msg void OnHScroll(UINT,UINT,CScrollBar*);
};
