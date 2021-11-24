// TranparencySettingsWnd.cpp : implementation file
// Infinisys Ltd

#include "stdafx.h"
#include "TransparencySettingsWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransparencySettingsWnd

CTransparencySettingsWnd::CTransparencySettingsWnd(CEdit * pedit):
m_pedit(pedit)
{
}


CTransparencySettingsWnd::~CTransparencySettingsWnd()
{
}

BEGIN_MESSAGE_MAP(CTransparencySettingsWnd,CWnd)
   ON_REGISTERED_MESSAGE(UM_ATTRIBSETTINGS_UPDATE,OnAttributeSettings)
END_MESSAGE_MAP()



LPARAM CTransparencySettingsWnd::OnAttributeSettings(WPARAM wParam,LPARAM lParam)
{

   if(GetFocus() == m_pedit)
   {

      return 0;

   }
   
   int iOpacity = (int)wParam;

   CString strOld;

   CString strNew;

   if(m_pedit != NULL)
   {

      m_pedit->GetWindowText(strOld);

   }
   else
   {

      strOld = m_str;

   }

   if(iOpacity < 0)
   {

      strNew.Empty();

   }
   else
   {

      int iPercentage = (int)round((double)iOpacity * 100.0 / 255.0);
      
      CString str;

      strNew.Format("%d",iPercentage);

   }

   if(strOld != strNew)
   {
      
      if(m_pedit != NULL)
      {

         m_pedit->SetWindowText(strNew);

      }
      else
      {

         m_str = strNew;

      }

   }

   return 0;

}



