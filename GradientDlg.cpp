// GradientDlg.cpp : implementation file
// Infinisys Ltd
//

#include "stdafx.h"
#include "PCDraft.h"
#include "GradientDlg.h"
#include "PCDraftDoc.h"
#include "Layer.h"
#include "DimStandardDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGradientDlg dialog

CGradientDlg::CGradientDlg(CPCDraftDoc* pDoc,CGradientPack * ppack, bool bEdit,CWnd* pParent /*=NULL*/)
   : CDialog(CGradientDlg::IDD,pParent),m_pDoc(pDoc)
{

   m_ppackRef = ppack;
   m_packCopy = *m_ppackRef;
   m_pack = *ppack;
   m_ppack = &m_pack;
   m_previewwnd.m_pParentDlg = this;
   m_editwnd.m_pParentDlg = this;
   m_bEdit = bEdit;
   m_bPreview = false;

}


void CGradientDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CGradientDlg)
   DDX_Control(pDX,IDC_PLACEHOLDER1,m_stPlaceHolder1);
   DDX_Control(pDX,IDC_GRADIENT_TYPE,m_comboGradientType);
   DDX_Control(pDX,IDC_CHECK_PREVIEW,m_checkPreview);
   DDX_Control(pDX,IDC_APPLY_TO_SELECTED,m_buttonApplyToSelected);
   DDX_Control(pDX,IDC_EDIT_ROTATION,m_editRotation);
   DDX_Control(pDX,IDC_STATIC_ROTATION,m_stRotation);
   DDX_Control(pDX,IDC_STATIC_MOVE_CENTER,m_stMoveCenter);
   DDX_Text(pDX,IDC_EDIT_ROTATION,m_ppack->m_dRotation);
   DDV_MinMaxFloat(pDX,IDC_EDIT_ROTATION,-10000.0f, 10000.0f);
}

BEGIN_MESSAGE_MAP(CGradientDlg,CDialog)
   ON_BN_CLICKED(IDC_CHECK_PREVIEW,&CGradientDlg::OnBnClickedCheckPreview)
   ON_CBN_SELCHANGE(IDC_GRADIENT_TYPE,&CGradientDlg::OnCbnSelchangeGradientType)
   ON_BN_CLICKED(IDCANCEL,&CGradientDlg::OnBnClickedCancel)
   ON_EN_CHANGE(IDC_EDIT_ROTATION,&CGradientDlg::OnEnChangeEditRotation)
   ON_BN_CLICKED(IDC_APPLY_TO_SELECTED,&CGradientDlg::OnBnClickedApplyToSelected)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGradientDlg message handlers


void CGradientDlg::defer_redraw(bool bForcePreview)
{

   m_editwnd.RedrawWindow();

   m_previewwnd.RedrawWindow();

   if(m_bPreview || bForcePreview)
   {

      m_pDoc->UpdateAllViews(NULL,kRefresh);

   }

}


BOOL CGradientDlg::OnInitDialog()
{

   CDialog::OnInitDialog();

   if(!m_bEdit)
   {

      CString strTitle;

      strTitle.LoadString(IDS_NEW_GRADIENT);

      SetWindowText(strTitle);

   }


   CRect rectPlaceHolder1;

   m_stPlaceHolder1.GetWindowRect(rectPlaceHolder1);



   ScreenToClient(rectPlaceHolder1);

   if(m_bEdit)
   {

      m_checkPreview.SetWindowPos(NULL,rectPlaceHolder1.left,rectPlaceHolder1.top,0,0,SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);

   }
   else
   {

      m_buttonApplyToSelected.SetWindowPos(NULL,rectPlaceHolder1.left,rectPlaceHolder1.top,0,0,SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);

   }


   {

      CRect rc;
      GetDlgItem(IDC_EDIT_PREVIEW)->GetWindowRect(&rc);
      ScreenToClient(&rc);
      m_previewwnd.Create(AfxRegisterWndClass(CS_PARENTDC | CS_HREDRAW | CS_VREDRAW,NULL,NULL,NULL),NULL,WS_CHILD | WS_VISIBLE,rc,this,IDC_EDIT_PREVIEW);

   }

   {
    
      CRect rc;
      GetDlgItem(IDC_EDIT_GRADIENT)->GetWindowRect(&rc);
      ScreenToClient(&rc);
      m_editwnd.Create(AfxRegisterWndClass(CS_PARENTDC | CS_HREDRAW | CS_VREDRAW,NULL,NULL,NULL),NULL,WS_CHILD | WS_VISIBLE,rc,this,IDC_EDIT_GRADIENT);

   }

   CString strItem;

   {

      strItem.LoadString(IDS_GRADIENT_LINEAR);

      m_comboGradientType.AddString(strItem);

   }

   {

      strItem.LoadString(IDS_GRADIENT_RADIAL);

      m_comboGradientType.AddString(strItem);

   }

   if(m_ppack->m_bRadial)
   {
      m_comboGradientType.SetCurSel(1);
   }
   else
   {
      m_comboGradientType.SetCurSel(0);
   }

   on_update_gradient_type();

   return FALSE;

}


void CGradientDlg::OnOK()
{

   if(!UpdateData()) return;
   
   ASSERT(&m_pack != m_ppackRef);
   
   *m_ppackRef = m_pack;

   CDialog::OnOK();
   
}





void CGradientDlg::OnBnClickedCheckPreview()
{
   
   int iChecked = m_checkPreview.GetState() & BST_CHECKED;

   if(iChecked)
   {

      m_packCopy = *m_ppackRef;

      *m_ppackRef = m_pack;

      m_bPreview = true;

      defer_redraw();

      m_ppack = m_ppackRef;

   }
   else
   {

      m_pack = *m_ppackRef;

      *m_ppackRef = m_packCopy;

      m_bPreview = false;

      m_ppack = &m_pack;

      defer_redraw(true);

   }

}


void CGradientDlg::OnCbnSelchangeGradientType()
{
   
   if(m_comboGradientType.GetCurSel() == 1)
   {

      m_ppack->m_bRadial = true;

   }
   else
   {

      m_ppack->m_bRadial = false;

   }

   on_update_gradient_type();

}


void CGradientDlg::on_update_gradient_type()
{

   m_editRotation.ShowWindow(m_ppack->m_bRadial ? SW_HIDE : SW_SHOW);

   m_stRotation.ShowWindow(m_ppack->m_bRadial ? SW_HIDE : SW_SHOW);

   m_stMoveCenter.ShowWindow(m_ppack->m_bRadial ? SW_SHOW : SW_HIDE);

   defer_redraw();

}


void CGradientDlg::OnBnClickedCancel()
{

   m_pack = *m_ppackRef;

   *m_ppackRef = m_packCopy;

   m_bPreview = false;

   m_ppack = &m_pack;

   defer_redraw(true);

   CDialog::OnCancel();

}


void CGradientDlg::OnEnChangeEditRotation()
{

   CString strText;

   m_editRotation.GetWindowText(strText);

   m_ppack->m_dRotation = atof(strText);

   defer_redraw();


}


void CGradientDlg::OnBnClickedApplyToSelected()
{

   if(!UpdateData()) return;

   ASSERT(&m_pack != m_ppackRef);

   *m_ppackRef = m_pack;

   EndDialog(ID_APPLY_TO_SELECTED);
   
}

