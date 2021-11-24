// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#include "BaseWnd.h"
#include "TemplateComboBox.h"
#include "TemplateSearchEdit.h"
#include "TemplateStatic.h"
#include "TemplateLeftWnd.h"
#include "TemplateCategoryWnd.h"
#include "TemplateRecentWnd.h"


class TemplateLeftWnd;




class TemplateWnd :
   public BaseWnd
{
public:

   enum enum_screen
   {

      e_screen_templates,
      e_screen_recent_documents,

   };


   enum_screen                m_escreen;
   int                        m_iLeftWindow;
   int                        m_yMainClient;
   TemplateLeftWnd            m_leftwnd;
   TemplateSearchEdit         m_searchedit;
   TemplateStatic             m_staticCategories;
   TemplateComboBox           m_combobox;
   TemplateCategoryWnd        m_categorywnd;
   TemplateRecentWnd          m_recentwnd;



   TemplateWnd();
   ~TemplateWnd() override;
   //virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   //void ShowFonts(Gdiplus::Graphics * pgraphics);

   CStringW GetCategory();

   BOOL Create() override;

   void OnButtonPress(int i) override;

   void layout() override;



   //LPCTSTR GetIconWndClass(DWORD dwDefaultStyle, UINT nIDResource);
   DECLARE_MESSAGE_MAP()
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   //afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg LRESULT OnCategorySelChange(WPARAM wparam, LPARAM lparam);
   //afx_msg void OnPaint();
   afx_msg LRESULT OnChangeSearch(WPARAM wparam, LPARAM lparam);

   virtual void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics);

   //void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI); // Interface Update - Infinisys Ltd.
   //void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

   afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);


};

