// ContractExpandDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ContractExpandDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

double	CContractExpandDlg::m_nX = 1.5;
double	CContractExpandDlg::m_nY = 1.5;
BOOL CContractExpandDlg::m_bLinked=TRUE;

/////////////////////////////////////////////////////////////////////////////
// CContractExpandDlg dialog

CContractExpandDlg::CContractExpandDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CContractExpandDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CContractExpandDlg)
	//}}AFX_DATA_INIT
	m_bUseX=true;
}


void CContractExpandDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CContractExpandDlg)
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_LINJKED, m_bLinked);
	DDX_Text(pDX, IDC_X, m_nX);
	DDX_Text(pDX, IDC_Y, m_nY);
}

BEGIN_MESSAGE_MAP(CContractExpandDlg, CDialog)
	//{{AFX_MSG_MAP(CContractExpandDlg)
	ON_BN_CLICKED(IDC_LINKED, OnLinked)
	ON_EN_CHANGE(IDC_X, OnChangeX)
	ON_EN_CHANGE(IDC_Y, OnChangeY)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContractExpandDlg message handlers

BOOL CContractExpandDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	UINT nTitleId,nMsgId;

	if(m_bContract)
	{
		nTitleId=IDS_CONTRACT_TITLE;
		nMsgId=IDS_CONTRACT_MSG;
	}
	else
	{
		nTitleId=IDS_EXPAND_TITLE;
		nMsgId=IDS_EXPAND_MSG;
	}
	CString theText;
	theText.LoadString(nTitleId);
	SetWindowText(theText);
	theText.LoadString(nMsgId);
	SetDlgItemText(IDC_CONTRACT_EXPAND,theText);

	UpdateLinkedFeedback();

	return TRUE;
}

void CContractExpandDlg::OnLinked() 
{
	UpdateData();
	if(m_bLinked)
		if(m_bUseX)
			OnChangeX();
		else
			OnChangeY();
	UpdateLinkedFeedback();
}

void CContractExpandDlg::UpdateLinkedFeedback()
{
	GetDlgItem(IDC_LINKED_FEEDBACK)->ShowWindow(m_bLinked?SW_SHOW:SW_HIDE);
}

static bool gIgnoreChange=false;

void CContractExpandDlg::OnChangeX() 
{
	if(m_bLinked && !gIgnoreChange)
	{
		CString theText;
		GetDlgItemText(IDC_X,theText);
		gIgnoreChange=true;
		SetDlgItemText(IDC_Y,theText);
		gIgnoreChange=false;
	}
	else
		m_bUseX=true;
}

void CContractExpandDlg::OnChangeY() 
{
	if(m_bLinked && !gIgnoreChange)
	{
		CString theText;
		GetDlgItemText(IDC_Y,theText);
		gIgnoreChange=true;
		SetDlgItemText(IDC_X,theText);
		gIgnoreChange=false;
	}
	else
		m_bUseX=false;
}

void CContractExpandDlg::OnCancel() 
{
	UpdateData();
	ValidateData();
	CDialog::OnCancel();
}

void CContractExpandDlg::OnOK() 
{
	CDialog::OnOK();
	ValidateData();
}

void CContractExpandDlg::ValidateData()
{
	if(m_nX<=0) m_nX=1;
	if(m_nY<=0) m_nY=1;
}
