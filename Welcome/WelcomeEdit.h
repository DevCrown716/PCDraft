#pragma once
// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "WelcomeStatic.h"
#include "WelcomeInnerEdit.h"


class WelcomeEdit :
   public BaseControl
{
public:

   Gdiplus::Font *         m_pfont;

   Gdiplus::RectF          m_rectEdit;
   WelcomeStatic           m_static;
   WelcomeInnerEdit        m_edit;


   WelcomeEdit();
   ~WelcomeEdit() override;


   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;



   DECLARE_MESSAGE_MAP()
   
   
   afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd * pWnd, UINT nCtlColor);

   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);

   DECLARE_DYNAMIC(WelcomeEdit);

};

