// SelectObjectInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "SelectObjectInfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectObjectInfoDlg dialog


CSelectObjectInfoDlg::CSelectObjectInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectObjectInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectObjectInfoDlg)
	m_bUse1 = FALSE;
	m_sValue1 = _T("");
	m_bUse2 = FALSE;
	m_sValue2 = _T("");
	m_bUse3 = FALSE;
	m_sValue3 = _T("");
	m_bUse4 = FALSE;
	m_sValue4 = _T("");
	m_bUse5 = FALSE;
	m_sValue5 = _T("");
	m_sLabel1 = _T("");
	m_sLabel2 = _T("");
	m_sLabel3 = _T("");
	m_sLabel4 = _T("");
	m_sLabel5 = _T("");
	//}}AFX_DATA_INIT
}


void CSelectObjectInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectObjectInfoDlg)
	DDX_Check(pDX, IDC_FIELD1_USE, m_bUse1);
	DDX_Text(pDX, IDC_FIELD1_VALUE, m_sValue1);
	DDX_Check(pDX, IDC_FIELD2_USE, m_bUse2);
	DDX_Text(pDX, IDC_FIELD2_VALUE, m_sValue2);
	DDX_Check(pDX, IDC_FIELD3_USE, m_bUse3);
	DDX_Text(pDX, IDC_FIELD3_VALUE, m_sValue3);
	DDX_Check(pDX, IDC_FIELD4_USE, m_bUse4);
	DDX_Text(pDX, IDC_FIELD4_VALUE, m_sValue4);
	DDX_Check(pDX, IDC_FIELD5_USE, m_bUse5);
	DDX_Text(pDX, IDC_FIELD5_VALUE, m_sValue5);
	DDX_Text(pDX, IDC_FIELD1_LABEL, m_sLabel1);
	DDX_Text(pDX, IDC_FIELD2_LABEL, m_sLabel2);
	DDX_Text(pDX, IDC_FIELD3_LABEL, m_sLabel3);
	DDX_Text(pDX, IDC_FIELD4_LABEL, m_sLabel4);
	DDX_Text(pDX, IDC_FIELD5_LABEL, m_sLabel5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectObjectInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectObjectInfoDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectObjectInfoDlg message handlers
