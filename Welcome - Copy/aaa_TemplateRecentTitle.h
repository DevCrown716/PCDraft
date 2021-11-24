#pragma once
// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "BaseControl.h"




class TemplateRecentTitle :
   public BaseControl
{
public:


   Gdiplus::Font *      m_pfont;


   TemplateRecentTitle();
   ~TemplateRecentTitle() override;


   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;

   DECLARE_MESSAGE_MAP()
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);


};

