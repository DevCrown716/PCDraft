// RegistrationDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "RegistrationDlg.h"
#include "SerialNumber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegistrationDlg dialog


CRegistrationDlg::CRegistrationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegistrationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegistrationDlg)
	m_sName = _T("");
	m_sOrganization = _T("");
	m_sSerialNumber = _T("");
	//}}AFX_DATA_INIT
}


void CRegistrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegistrationDlg)
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDX_Text(pDX, IDC_ORGANIZATION, m_sOrganization);
	DDX_Text(pDX, IDC_SERIAL, m_sSerialNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegistrationDlg, CDialog)
	//{{AFX_MSG_MAP(CRegistrationDlg)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_SERIAL, OnChangeSerial)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegistrationDlg message handlers

void CRegistrationDlg::Check()
{
	GetDlgItem(IDOK)->EnableWindow(!(m_sName.IsEmpty() || IsBadSerialNumber(m_sSerialNumber)));
}

void CRegistrationDlg::OnChangeName() 
{
	GetDlgItem(IDC_NAME)->GetWindowText(m_sName);
	Check();
}

void CRegistrationDlg::OnChangeSerial() 
{
	GetDlgItem(IDC_SERIAL)->GetWindowText(m_sSerialNumber);
	Check();
}

BOOL CRegistrationDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	Check();
	return TRUE;
}
