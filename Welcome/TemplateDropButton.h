#pragma once
// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "BaseControl.h"
#include "TemplateDropList.h"


/////////////////////////////////////////////////////////////////////////////
// TemplateDropButton window
class TemplateDropButton : public BaseControl
{
public:

   int                  m_nListWidth;
   int                  m_nListHeight;
   bool			         m_bDeleteFont;
   bool                 m_bChangeTitle;
   bool                 m_bOver;
   Gdiplus::Font *      m_pfontDropButton;
   TemplateDropList*		m_pDropList;


   CRect                m_ListRect;
   CPen*			         m_pBorderPen;
   CPen*			         m_pTriPen;
   CFont*			      m_pTextFont;
   CBrush*			      m_pBkBrush;
   CBrush*			      m_pBkOverBrush;
   CBrush*			      m_pTriBrush;
   COLORREF		         m_crTxtColor;
   COLORREF		         m_crTxtOverColor;




   TemplateDropButton();
   virtual ~TemplateDropButton();

   void OnChangeSel();

   bool defer_create_list();

   void	SetListWidth(int nWidth) { m_nListWidth = nWidth; };
   void	SetListHeight(int nHeight) { m_nListHeight = nHeight; };
   void	ChangeTitleWithSelection(bool bChangeTitle) { m_bChangeTitle = bChangeTitle; };


   void	DrawTriangleUp(Gdiplus::Graphics * pgraphics, LPCRECT lpcrect);
   void	DrawTriangleDown(Gdiplus::Graphics * pgraphics, LPCRECT lpcrect);
   void	DisplayList();

   virtual void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics);


   TemplateDropList* GetList() { return m_pDropList; };

   DECLARE_MESSAGE_MAP()
   
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
   afx_msg void OnSize(UINT nType, int cx, int cy);

};



