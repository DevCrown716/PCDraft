// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#include "BaseControl.h"


class GetStartedCheckBox :
   public BaseControl
{
public:


   Gdiplus::Font * m_pfont;
   Gdiplus::Bitmap * m_pbitmapChecked;
   Gdiplus::Bitmap * m_pbitmapUnchecked;


   bool m_bChecked;

   bool m_bSetHover;

   GetStartedCheckBox();
   ~GetStartedCheckBox() override;


   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;

   void OnClick() override;

   DECLARE_MESSAGE_MAP()
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnMouseLeave();
   afx_msg BOOL OnEraseBkgnd(CDC* pDC);
   afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};

