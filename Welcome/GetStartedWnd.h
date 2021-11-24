// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#include "BaseWnd.h"
#include "GetStartedCheckBox.h"
#include "GetStartedButton.h"
#include "GetStartedHelpButton.h"


class GetStartedWnd :
   public BaseWnd
{
public:


   Gdiplus::Font *         m_pfontTopTitle;
   Gdiplus::Font *         m_pfontVersion;
   GetStartedCheckBox      m_checkbox;
   GetStartedButton        m_buttonNew;
   GetStartedButton        m_buttonTemplate;
   GetStartedButton        m_buttonOpen;
   GetStartedHelpButton    m_helpbutton;

   //virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   //void ShowFonts(Gdiplus::Graphics * pgraphics);

   GetStartedWnd();
   ~GetStartedWnd() override;


   void OnButtonPress(int i);

   BOOL Create();

   void layout() override;


   //LPCTSTR GetIconWndClass(DWORD dwDefaultStyle, UINT nIDResource);
   DECLARE_MESSAGE_MAP()
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   //afx_msg void OnSize(UINT nType, int cx, int cy);
   //afx_msg void OnPaint();

   //void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI); // Interface Update - Infinisys Ltd.
   //void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;

   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);


};

