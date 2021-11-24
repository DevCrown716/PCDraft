// Infinisys Ltd. 2021 - PCDraft 7 - Welcome Screen
#include "stdafx.h"
#include "TemplateComboBox.h"
#include "WelcomeEdit.h"
#include "BaseWnd.h"
#include "BaseControl.h"
#include "../resource.h"


TemplateComboBox::TemplateComboBox()
{

   m_bUpdate = false;
   m_pfont = nullptr;
   m_pbasewnd = nullptr;

}


TemplateComboBox::~TemplateComboBox()
{

   delete m_pfont;

}



BEGIN_MESSAGE_MAP(TemplateComboBox, TemplateDropButton)
   ON_WM_SIZE()
   ON_WM_CREATE()
   ON_WM_KEYDOWN()
   ON_WM_KEYUP()
   ON_WM_CHAR()
END_MESSAGE_MAP()


int TemplateComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

   m_pbasewnd = (BaseWnd *)GetTopLevelParent();

   if (TemplateDropButton::OnCreate(lpCreateStruct) == -1)
   {

      return -1;

   }

   return 0;

}


void TemplateComboBox::OnSize(UINT nType, int cx, int cy)
{

   TemplateDropButton::OnSize(nType, cx, cy);


}


double TemplateComboBox::font_scaler()
{

   return m_pbasewnd->font_scaler();

}


double TemplateComboBox::size_scaler()
{

   return m_pbasewnd->font_scaler();

}


void TemplateComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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

   }
   else if (nChar == VK_LEFT || nChar == VK_UP)
   {

   }
   else
   {

      TemplateDropButton::OnKeyDown(nChar, nRepCnt, nFlags);

   }

}


void TemplateComboBox::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{

   if (nChar != VK_TAB)
   {

      TemplateDropButton::OnKeyUp(nChar, nRepCnt, nFlags);

   }

}


void TemplateComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{

   if (nChar != VK_TAB)
   {

      TemplateDropButton::OnChar(nChar, nRepCnt, nFlags);

   }

}



