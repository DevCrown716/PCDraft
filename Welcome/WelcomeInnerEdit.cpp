// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "WelcomeInnerEdit.h"
#include "WelcomeEdit.h"
#include "BaseWnd.h"
#include "BaseControl.h"
#include "../resource.h"


WelcomeInnerEdit::WelcomeInnerEdit()
{

   m_bUpdate = false;
   m_pfont = nullptr;
   m_pbasewnd = nullptr;

}


WelcomeInnerEdit::~WelcomeInnerEdit()
{

   delete m_pfont;

}



BEGIN_MESSAGE_MAP(WelcomeInnerEdit, CEdit)
   ON_WM_SIZE()
   ON_WM_CREATE()
   ON_WM_SETFOCUS()
   ON_WM_KILLFOCUS()
   ON_CONTROL_REFLECT(EN_UPDATE, &WelcomeInnerEdit::OnEnUpdate)
   ON_CONTROL_REFLECT(EN_CHANGE, &WelcomeInnerEdit::OnEnChange)
   ON_WM_KEYDOWN()
   ON_WM_KEYUP()
   ON_WM_CHAR()
END_MESSAGE_MAP()


int WelcomeInnerEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   m_pbasewnd = (BaseWnd *)GetTopLevelParent();

   if (CEdit::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   CFont font;

   float f = font_scaler();

   font.CreatePointFont(180 * f * (72.0/m_pbasewnd->m_dDpi), "Segoe UI");

   SetFont(&font);

   return 0;

}


void WelcomeInnerEdit::OnSize(UINT nType, int cx, int cy)
{

   CEdit::OnSize(nType, cx, cy);

   m_rectWindow.X = 0; 
   m_rectWindow.Y = 0;
   m_rectWindow.Width = cx;
   m_rectWindow.Height = cy;

   m_rectEdit.X = 2;
   m_rectEdit.Y = 2;
   m_rectEdit.Width = cx - 4;
   m_rectEdit.Height = cy - 4;

   UpdateWindow();

}


void WelcomeInnerEdit::OnSetFocus(CWnd * pOldWnd)
{

   CEdit::OnSetFocus(pOldWnd);

   CString str;

   GetWindowText(str);

   if (str == m_strEmpty)
   {

      SetWindowText("");

   }

   GetParent()->Invalidate();

   m_pbasewnd->m_pWndFocus = this;

}


void WelcomeInnerEdit::OnKillFocus(CWnd* pNewWnd)
{

   CEdit::OnKillFocus(pNewWnd);

   CString str;

   GetWindowText(str);

   if (str.IsEmpty())
   {

      SetWindowText(m_strEmpty);

   }

   GetParent()->Invalidate();

}


void WelcomeInnerEdit::OnEnUpdate()
{  


}


void WelcomeInnerEdit::OnEnChange()
{

   m_pbasewnd->OnEditChangeText(GetParent()->GetDlgCtrlID());

}


double WelcomeInnerEdit::font_scaler()
{

   return m_pbasewnd->font_scaler();

}


double WelcomeInnerEdit::size_scaler()
{

   return m_pbasewnd->font_scaler();

}





void WelcomeInnerEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

   if (nChar == VK_TAB)
   {

      OutputDebugString("XXXXX");

      if (GetAsyncKeyState(VK_SHIFT) < 0)
      {

         ((BaseControl*)GetParent())->SetFocusPrevious();

      }
      else
      {

         ((BaseControl*)GetParent())->SetFocusNext();

      }

   }
   else if (nChar == VK_UP)
   {

      ((BaseControl*)GetParent())->SetFocusPrevious();

   }
   else if (nChar == VK_DOWN)
   {

      ((BaseControl*)GetParent())->SetFocusNext();

   }
   else if (nChar == VK_RIGHT)
   {

      int nBeg = 0;

      int nEnd = 0;

      GetSel(nBeg, nEnd);

      int nLen = GetWindowTextLength();

      if (nBeg == nEnd && nEnd == nLen)
      {

         ((BaseControl*)GetParent())->SetFocusNext();

      }
      else
      {

         CEdit::OnKeyDown(nChar, nRepCnt, nFlags);

      }

   }
   else if (nChar == VK_LEFT || nChar == VK_UP)
   {

      int nBeg = 0;

      int nEnd = 0;

      GetSel(nBeg, nEnd);

      if (nBeg == nEnd && nEnd == 0)
      {

         ((BaseControl*)GetParent())->SetFocusPrevious();

      }
      else
      {

         CEdit::OnKeyDown(nChar, nRepCnt, nFlags);

      }

   }
   else
   {

      CEdit::OnKeyDown(nChar, nRepCnt, nFlags);

   }

}


void WelcomeInnerEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{

   if (nChar != VK_TAB)
   {

      CEdit::OnKeyUp(nChar, nRepCnt, nFlags);

   }

}


void WelcomeInnerEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{

   if (nChar != VK_TAB)
   {

      CEdit::OnChar(nChar, nRepCnt, nFlags);

   }

}
