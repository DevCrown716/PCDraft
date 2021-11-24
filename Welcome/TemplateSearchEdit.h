#pragma once
// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#include "WelcomeStatic.h"

class BaseWnd;



class TemplateSearchEdit :
   public CEdit
{
public:


   BaseWnd  *              m_pbasewnd;

   Gdiplus::RectF          m_rectWindow;
   Gdiplus::RectF          m_rectEdit;

   Gdiplus::Font *         m_pfont;
   CString                 m_strEmpty;
   bool m_bUpdate;


   TemplateSearchEdit();
   ~TemplateSearchEdit() override;



   virtual double font_scaler();
   virtual double size_scaler();


   DECLARE_MESSAGE_MAP()
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);

   afx_msg void OnSetFocus(CWnd* pOldWnd);
   afx_msg void OnKillFocus(CWnd* pNewWnd);
   afx_msg void OnEnUpdate();
   afx_msg void OnEnChange();


   afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
   afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};

