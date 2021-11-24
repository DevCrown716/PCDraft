#pragma once
// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "BaseWnd.h"

class TemplateDropButton;


class TemplateDropList : public BaseWnd
{
public:

   int m_iCurSel;
   int m_iItemHeight;
   CArray < CStringW >      m_straItem;
   bool                    m_bSetHover;
   int m_iHover;
   Gdiplus::Font * m_pfontDropList;
   CFont*		m_pTextFont;
   CFont*		m_pHeaderFont;
   CPen*		m_pBkPen;
   TemplateDropButton *       m_pbutton;
   Gdiplus::SolidBrush *		m_pBk;
   Gdiplus::SolidBrush *		m_pBkHover;
   Gdiplus::SolidBrush *		m_pBkSelected;
   Gdiplus::SolidBrush *		m_pBkHoverSelected;
   Gdiplus::SolidBrush *		m_pFr;
   Gdiplus::SolidBrush *		m_pFrHover;
   Gdiplus::SolidBrush *		m_pFrSelected;
   Gdiplus::SolidBrush *		m_pFrHoverSelected;

public:
   TemplateDropList();
   virtual ~TemplateDropList();

   int hit_test(CPoint point);

   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;

   void DrawItem(Gdiplus::Graphics * pgraphics, CStringW str, CRect ItemRect, BOOL bSelected, bool bHover);

   void GetItemRect(LPRECT lprect, int iItem);


   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);

   DECLARE_MESSAGE_MAP()
   afx_msg void OnMouseMove(UINT nFlags, CPoint point);
   afx_msg void OnMouseLeave();

   afx_msg void OnKillFocus(CWnd* pNewWnd);
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};



