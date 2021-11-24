// Library/Layer - PCDraft 6 - Infinisys Ltd.
#pragma once


class CListEditBox:
   public CEdit
{
   DECLARE_MESSAGE_MAP()

public:

   CWnd * m_pwnd;

   CListEditBox();
   virtual ~CListEditBox();
   
   
   afx_msg int OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
   afx_msg void OnEnKillfocus();
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags,CPoint point);


};



class CListComboBox:
   public CComboBox
{
   DECLARE_MESSAGE_MAP()

public:

   CWnd * m_pwnd;

   CListComboBox();
   virtual ~CListComboBox();


   afx_msg int OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
   afx_msg void OnCbnKillfocus();
   afx_msg void OnLButtonDown(UINT nFlags,CPoint point);
   afx_msg void OnLButtonUp(UINT nFlags,CPoint point);


};

