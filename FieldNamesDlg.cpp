// FieldNamesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "FieldNamesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFieldNamesDlg dialog


CFieldNamesDlg::CFieldNamesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFieldNamesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFieldNamesDlg)
	m_sName1 = _T("");
	m_sName2 = _T("");
	m_sName3 = _T("");
	m_sName4 = _T("");
	m_sName5 = _T("");
	//}}AFX_DATA_INIT
}


void CFieldNamesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFieldNamesDlg)
	DDX_Text(pDX, IDC_FIELD1_LABEL, m_sName1);
	DDX_Text(pDX, IDC_FIELD2_LABEL, m_sName2);
	DDX_Text(pDX, IDC_FIELD3_LABEL, m_sName3);
	DDX_Text(pDX, IDC_FIELD4_LABEL, m_sName4);
	DDX_Text(pDX, IDC_FIELD5_LABEL, m_sName5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFieldNamesDlg, CDialog)
	//{{AFX_MSG_MAP(CFieldNamesDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFieldNamesDlg message handlers
