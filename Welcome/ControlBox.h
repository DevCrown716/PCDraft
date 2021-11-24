#pragma once
// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "ControlBoxButton.h"




class ControlBox :
   public BaseControl
{
public:

   
   Gdiplus::Color          m_colorBackground;
   Gdiplus::Color          m_colorForeground;
   ControlBoxButton        m_buttonMinimize;
   ControlBoxButton        m_buttonClose;


   ControlBox();
   ~ControlBox() override;


   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;


   DECLARE_MESSAGE_MAP()
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);


   DECLARE_DYNAMIC(ControlBox);

};

