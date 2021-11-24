// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once


#include "TemplateLeftButton.h"


class TemplateLeftWnd :
   public BaseControl
{
public:


   Gdiplus::Bitmap *       m_pbitmap;
   TemplateLeftButton      m_buttonNew;
   TemplateLeftButton      m_buttonRecent;
   TemplateLeftButton      m_buttonOpen;

   TemplateLeftButton      m_buttonWhatsNew;
   TemplateLeftButton      m_buttonHelp;
   TemplateLeftButton      m_buttonCommunity;

   int                     m_xSeparator;


   TemplateLeftWnd();
   ~TemplateLeftWnd() override;


   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;

   DECLARE_MESSAGE_MAP()
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);

};

