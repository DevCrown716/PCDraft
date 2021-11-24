// ExtendLineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ExtendLineDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtendLineDlg dialog


CExtendLineDlg::CExtendLineDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExtendLineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExtendLineDlg)
	m_nExtendTo = 1;
	//}}AFX_DATA_INIT
}


void CExtendLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtendLineDlg)
	DDX_Radio(pDX, IDC_EXISTING_LINE, m_nExtendTo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CExtendLineDlg, CDialog)
	//{{AFX_MSG_MAP(CExtendLineDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtendLineDlg message handlers
