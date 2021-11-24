// BadDWGObjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "BadDWGObjectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBadDWGObjectDlg dialog


CBadDWGObjectDlg::CBadDWGObjectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBadDWGObjectDlg::IDD, pParent),m_bDontShow(false)
{
	//{{AFX_DATA_INIT(CBadDWGObjectDlg)
	m_strText = _T("");
	//}}AFX_DATA_INIT
}


void CBadDWGObjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBadDWGObjectDlg)
	DDX_Text(pDX, IDC_TEXT, m_strText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBadDWGObjectDlg, CDialog)
	//{{AFX_MSG_MAP(CBadDWGObjectDlg)
	ON_BN_CLICKED(IDC_DONTSHOW, OnDontshow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBadDWGObjectDlg message handlers

void CBadDWGObjectDlg::OnDontshow() 
{
	m_bDontShow=true;
	CDialog::OnOK();
}
