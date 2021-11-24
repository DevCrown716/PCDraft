// TransparencyBar.cpp : implementation file
// Infinisys Ltd
//

#include "stdafx.h"
#include "pcdraft.h"
#include "TransparencyBar.h"
#include "CustomMenus.h"
#include "CustomMiniDockFrameWnd.h"
#include "PCDraftDoc.h"
#include "PCDraftView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CTransparencyBar

CTransparencyBar::CTransparencyBar()
   : CDialog(CTransparencyBar::IDD)
{
   m_eattrib = attrib_none; // Interface Update - Infinisys Ltd.
   m_pview = NULL;
   m_bFirstChange = true;
}

CTransparencyBar::~CTransparencyBar()
{
}



void CTransparencyBar::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CScaleUnitsDlg)
   DDX_Control(pDX,IDC_SLIDER,m_slider);
   //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTransparencyBar,CDialog)
   ON_WM_ACTIVATE()
   ON_WM_HSCROLL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransparencyBar message handlers

BOOL CTransparencyBar::OnInitDialog()
{

   if(!CDialog::OnInitDialog())
   {
      return FALSE;
   }

   m_slider.SetTicFreq(10);
   
   return TRUE;

}





void CTransparencyBar::OnActivate(UINT nState,CWnd* pWndOther,BOOL bMinimized)
{
   CDialog::OnActivate(nState,pWndOther,bMinimized);
   if(nState == WA_INACTIVE)
   {
      ShowWindow(SW_HIDE);
      m_bFirstChange = true; // reset because transparency bar may be reused.
   }
}

void CTransparencyBar::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{
   //if(nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION)
   {
      CString str;

      str.Format("%d",m_slider.GetPos());
      if(m_pedit != NULL)
      {
         m_pedit->SetWindowText(str);

      }

      int iOpacity = (int)round((double)m_slider.GetPos() * 255.0 / 100.0);
      CMainFrame * pframe = (CMainFrame *)m_pview->GetTopLevelFrame();



      if(m_eattrib == attrib_fill) // Interface Update - Infinisys Ltd.
      {

         m_pview->ApplyOpacity(iOpacity,false, m_bFirstChange);

      }
      else if(m_eattrib == attrib_pfill) // Interface Update - Infinisys Ltd.
      {

         m_pview->ApplyOpacity(iOpacity,true, m_bFirstChange);

      }
      else if(m_eattrib == attrib_pen) // Interface Update - Infinisys Ltd.
      {

         PenModel pm;
         pm.m_iPenOpacity = iOpacity;
         if(m_bFirstChange)
         {
            m_pview->SaveUndo(IDS_PENCHANGE);
         }
         m_pview->ApplyPen(pm,pmUseOpacity);

      }

      m_bFirstChange = false;

   }
   //else
   {

   }

}
