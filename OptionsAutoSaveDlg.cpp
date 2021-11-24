#include "StdAfx.h"
#include "PCDraft.h"
#include "OptionsAutoSaveDlg.h"
#include "afxdialogex.h"


// OptionsAutoSaveDialog dialog

IMPLEMENT_DYNAMIC(OptionsAutoSaveDialog, CDialogEx)

OptionsAutoSaveDialog::OptionsAutoSaveDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPTIONS_AUTO_SAVE, pParent)
   , m_iAutoSaveMinutes(0)
{

}

OptionsAutoSaveDialog::~OptionsAutoSaveDialog()
{
}

void OptionsAutoSaveDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_AUTO_SAVE_ON, m_btnAutoSaveOn);
   DDX_Control(pDX, IDC_AUTO_SAVE_OFF, m_btnAutoSaveOff);
   DDX_Control(pDX, IDC_EDIT1, m_editAutoSaveMinutes);
   DDX_Text(pDX, IDC_EDIT1, m_iAutoSaveMinutes);
	DDV_MinMaxInt(pDX, m_iAutoSaveMinutes, 1, 99);
}


BEGIN_MESSAGE_MAP(OptionsAutoSaveDialog, CDialogEx)
   ON_BN_CLICKED(IDC_AUTO_SAVE_OFF, &OptionsAutoSaveDialog::OnBnClickedAutoSaveOff)
   ON_BN_CLICKED(IDC_AUTO_SAVE_ON, &OptionsAutoSaveDialog::OnBnClickedAutoSaveOn)
END_MESSAGE_MAP()


// OptionsAutoSaveDialog message handlers

void OptionsAutoSaveDialog::OnBnClickedAutoSaveOn()
{
   UpdateAutoSaveDialog();
}

void OptionsAutoSaveDialog::OnBnClickedAutoSaveOff()
{
   UpdateAutoSaveDialog();
}

void OptionsAutoSaveDialog::UpdateAutoSaveDialog()
{

   if (m_btnAutoSaveOn.GetCheck() == BST_CHECKED)
   {

      m_btnAutoSaveOff.SetCheck(BST_UNCHECKED);

      m_editAutoSaveMinutes.EnableWindow(TRUE);

      GetDlgItem(IDC_STATIC_AUTOSAVE_MINUTES)->EnableWindow(TRUE);

      GetDlgItem(IDC_STATIC_AUTO_SAVE_LABEL)->EnableWindow(TRUE);


   }
   else if (m_btnAutoSaveOff.GetCheck() == BST_CHECKED)
   {

      m_btnAutoSaveOn.SetCheck(BST_UNCHECKED);

      m_editAutoSaveMinutes.EnableWindow(FALSE);

      GetDlgItem(IDC_STATIC_AUTOSAVE_MINUTES)->EnableWindow(FALSE);

      GetDlgItem(IDC_STATIC_AUTO_SAVE_LABEL)->EnableWindow(FALSE);

   }

}


BOOL OptionsAutoSaveDialog::OnInitDialog()
{

   CDialogEx::OnInitDialog();

   CPCDraftApp * papp = (CPCDraftApp *) AfxGetApp();

   if (papp->m_bAutoSave)
   {

      m_btnAutoSaveOff.SetCheck(BST_UNCHECKED);
      m_btnAutoSaveOn.SetCheck(BST_CHECKED);

   }
   else
   {

      m_btnAutoSaveOn.SetCheck(BST_UNCHECKED);
      m_btnAutoSaveOff.SetCheck(BST_CHECKED);

   }

   m_iAutoSaveMinutes = papp->m_iAutoSaveMinutes;

   CString strFormat;

   strFormat.Format("%d", m_iAutoSaveMinutes);

   m_editAutoSaveMinutes.SetWindowText(strFormat);

   UpdateAutoSaveDialog();

   UpdateData();

   // TODO:  Add extra initialization here

   return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}




void OptionsAutoSaveDialog::OnOK()
{

   if (UpdateData(TRUE))
   {

      CPCDraftApp * papp = (CPCDraftApp *)AfxGetApp();

      papp->m_bAutoSave = m_btnAutoSaveOn.GetCheck() == BST_CHECKED;

      papp->m_iAutoSaveMinutes = m_iAutoSaveMinutes;

      papp->SaveApplicationDefaults();

   }
   else
   {

      return;

   }

   CDialogEx::OnOK();


}
