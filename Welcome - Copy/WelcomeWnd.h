// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#pragma once
#include "BaseWnd.h"
#include "WelcomeEdit.h"
#include "WelcomeButton.h"


class WelcomeWnd :
   public BaseWnd
{
public:


   Gdiplus::Font *      m_pfontTopTitle;
   Gdiplus::Font *      m_pfontVersion;
   Gdiplus::Font *      m_pfontMessage;
   Gdiplus::RectF       m_rectMessage;
   WelcomeEdit          m_editName;
   WelcomeEdit          m_editSerial;
   WelcomeButton        m_buttonCancel;
   WelcomeButton        m_buttonOk;


   CString	m_strName;
   CString	m_strOrganization;
   CString	m_strSerialNumber;



   WelcomeWnd();
   ~WelcomeWnd() override;

   void Check();

   BOOL Create();

   void OnEditChangeText(int nID) override;

   //LPCTSTR GetIconWndClass(DWORD dwDefaultStyle, UINT nIDResource);
   DECLARE_MESSAGE_MAP()
   afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
   afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
   afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);

   void  OnButtonPress(int i) override;

   void OnDraw(CDC * pdc, Gdiplus::Graphics * pgraphics) override;

   afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};

