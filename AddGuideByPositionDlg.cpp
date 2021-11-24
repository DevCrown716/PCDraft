// AddGuideByPositionDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "PCDraft.h"
#include "AddGuideByPositionDlg.h"
#include "afxdialogex.h"
#include "Resource.h"


// AddGuideByPositionDlg dialog

IMPLEMENT_DYNAMIC(AddGuideByPositionDlg, CDialog)

AddGuideByPositionDlg::AddGuideByPositionDlg(CPCDraftDoc * pdoc, CGuide & guide, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ADD_GUIDE_BY_POSITION, pParent),
   m_pdoc(pdoc),
   m_guide(guide),
   m_edit(kUnitlessValue)
{
   m_edit.SetDocument(pdoc);
}

AddGuideByPositionDlg::~AddGuideByPositionDlg()
{
}

void AddGuideByPositionDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_STATIC_WARNING, m_staticWarning);
   DDX_Control(pDX, IDC_EDIT_POSITION, m_edit);
}


BEGIN_MESSAGE_MAP(AddGuideByPositionDlg, CDialog)
   ON_BN_CLICKED(IDC_HORIZONTAL, &AddGuideByPositionDlg::OnBnClickedHorizontal)
   ON_BN_CLICKED(IDC_VERTICAL, &AddGuideByPositionDlg::OnBnClickedVertical)
   ON_WM_CTLCOLOR()
   ON_MESSAGE(WM_APP + 123, &AddGuideByPositionDlg::OnMessage123)
END_MESSAGE_MAP()


// AddGuideByPositionDlg message handlers


void AddGuideByPositionDlg::OnBnClickedHorizontal()
{
   SetVertical(false);
}


void AddGuideByPositionDlg::OnBnClickedVertical()
{
   SetVertical();
}

BOOL AddGuideByPositionDlg::OnInitDialog()
{
   CDialog::OnInitDialog();

   
   CButton * pbutton = (CButton *)GetDlgItem(IDC_HORIZONTAL);
   pbutton->SetCheck(1);

   CString strWarning;

   strWarning.LoadString(IDS_WARNING_GUIDE_OUTSIDE_DRAWING);

   m_staticWarning.SetWindowText(strWarning);

   SetVertical(false);

   //m_edit.SetValue(kUnitlessValue, 0);
   //GetDlgItem(IDOK)->EnableWindow(FALSE);
   GetDlgItem(IDC_STATIC_WARNING)->ShowWindow(SW_HIDE);

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


void AddGuideByPositionDlg::SetVertical(bool bVertical)
{

   m_guide.m_bVertical = bVertical;

   if (bVertical)
   {

      GetDlgItem(IDC_POSITION_LABEL)->SetWindowText("X:");

   }
   else
   {

      GetDlgItem(IDC_POSITION_LABEL)->SetWindowText("Y:");

   }

}


void AddGuideByPositionDlg::OnOK()
{
   bool bOk;

   bool bValid;

   validate(bOk, bValid);

   if (bOk && bValid)
   {

      CDialog::OnOK();

   }
   else
   {

      GetDlgItem(IDC_STATIC_WARNING)->ShowWindow(bValid ? SW_HIDE : SW_SHOW);

   }



}




HBRUSH AddGuideByPositionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
   HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

   // Add the following code
   if (pWnd->GetDlgCtrlID() == IDC_STATIC_WARNING)
   {
      pDC->SetTextColor(RGB(255, 0, 0)); 

   }
      // End

   return hbr;
}


void AddGuideByPositionDlg::validate(bool & bOk, bool & bValid)
{

   CString strText;

   m_edit.GetWindowText(strText);

   bOk = !strText.IsEmpty();

   bValid = true;

   m_edit.GetValue(m_guide.m_coord);

   if (m_guide.m_bVertical)
   {

      bValid = m_pdoc->XInDocument(m_guide.m_coord);

   }
   else
   {

      bValid = m_pdoc->YInDocument(m_guide.m_coord);

   }


}


LRESULT AddGuideByPositionDlg::OnMessage123(WPARAM wparam, LPARAM lparam)
{

   bool bOk;

   bool bValid;

   validate(bOk, bValid);

   GetDlgItem(IDC_STATIC_WARNING)->ShowWindow(bValid ? SW_HIDE : SW_SHOW);

   return 0;

}
