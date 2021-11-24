// Infinisys Ltd. 2021 - Unicode File names
#pragma once
#include "BaseControl.h"


class StaticEx :
   public CWnd
{
public:

   Gdiplus::Font *      m_pfont;
   CStringW m_wstr;
   COLORREF m_crBackground;


   StaticEx();
   ~StaticEx() override;



   DECLARE_MESSAGE_MAP()

   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnPaint();


};

