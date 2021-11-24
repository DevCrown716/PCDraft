// PrintSlidesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "PrintSlidesDlg.h"

#include "PCDraftDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPrintSlidesDlg dialog


CPrintSlidesDlg::CPrintSlidesDlg(CPCDraftDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CPrintSlidesDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CPrintSlidesDlg)
	m_nFitFull = 0;
	m_bLayer1AsMaster = FALSE;
	//}}AFX_DATA_INIT
}


void CPrintSlidesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintSlidesDlg)
	DDX_Radio(pDX, IDC_REDUCE_TO_FIT, m_nFitFull);
	DDX_Check(pDX, IDC_USE1_AS_MASTER, m_bLayer1AsMaster);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPrintSlidesDlg, CDialog)
	//{{AFX_MSG_MAP(CPrintSlidesDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintSlidesDlg message handlers

BOOL CPrintSlidesDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_pDoc->GetLayersCount()==1)	GetDlgItem(IDC_USE1_AS_MASTER)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}
