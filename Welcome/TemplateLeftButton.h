// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#include "BaseControl.h"


class TemplateLeftButton :
   public BaseControl
{
public:

   Gdiplus::Font * m_pfont;

   bool  m_bSel;


   TemplateLeftButton();
   ~TemplateLeftButton() override;


   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;


   DECLARE_MESSAGE_MAP()
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

   afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};

