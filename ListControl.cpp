// Library - PCDraft 6 - Infinisys Ltd.
#include "stdafx.h"
#include "ListControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CListEditBox::CListEditBox()
{
   m_pwnd=NULL;
}


CListEditBox::~CListEditBox()
{
}
BEGIN_MESSAGE_MAP(CListEditBox,CEdit)
   ON_CONTROL_REFLECT(EN_KILLFOCUS,&CListEditBox::OnEnKillfocus)
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


void CListEditBox::OnEnKillfocus()
{
   // TODO: Add your control notification handler code here
   //m_pwnd->OnEditKillFocus();
   m_pwnd->SendMessage(WM_USER + 384,0,(LPARAM)GetSafeHwnd());
}


void CListEditBox::OnLButtonDown(UINT nFlags,CPoint point)
{
   // TODO: Add your message handler code here and/or call default

   CEdit::OnLButtonDown(nFlags,point);
}


void CListEditBox::OnLButtonUp(UINT nFlags,CPoint point)
{
   // TODO: Add your message handler code here and/or call default

   CEdit::OnLButtonUp(nFlags,point);
}


int CListEditBox::OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message)
{
   // TODO: Add your message handler code here and/or call default
   return MA_ACTIVATE;
   return CEdit::OnMouseActivate(pDesktopWnd,nHitTest,message);
}



CListComboBox::CListComboBox()
{
   m_pwnd=NULL;
}


CListComboBox::~CListComboBox()
{
}
BEGIN_MESSAGE_MAP(CListComboBox,CComboBox)
   ON_CONTROL_REFLECT(CBN_KILLFOCUS,&CListComboBox::OnCbnKillfocus)
   ON_WM_LBUTTONDOWN()
   ON_WM_LBUTTONUP()
   ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()


void CListComboBox::OnCbnKillfocus()
{
   // TODO: Add your control notification handler code here
   //m_pwnd->OnEditKillFocus();
   m_pwnd->SendMessage(WM_USER + 384, 0, (LPARAM) GetSafeHwnd());
}


void CListComboBox::OnLButtonDown(UINT nFlags,CPoint point)
{
   // TODO: Add your message handler code here and/or call default

   CComboBox::OnLButtonDown(nFlags,point);
}


void CListComboBox::OnLButtonUp(UINT nFlags,CPoint point)
{
   // TODO: Add your message handler code here and/or call default

   CComboBox::OnLButtonUp(nFlags,point);
}


int CListComboBox::OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message)
{
   // TODO: Add your message handler code here and/or call default
   return MA_ACTIVATE;
   return CComboBox::OnMouseActivate(pDesktopWnd,nHitTest,message);
}
