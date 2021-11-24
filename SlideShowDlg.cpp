// SlideShowDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "SlideShowDlg.h"

#include "PCDraftDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSlideShowDlg dialog


CSlideShowDlg::CSlideShowDlg(CPCDraftDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CSlideShowDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CSlideShowDlg)
	m_nDisplayType = m_pDoc->m_nSlideSeconds==0?0:1;
	m_bLayer1AsMaster = (m_pDoc->GetLayersCount()>1 && m_pDoc->m_bLayer1AsMaster)?TRUE:FALSE;
	m_nSeconds = m_pDoc->m_nSlideSeconds?m_pDoc->m_nSlideSeconds:2;
	//}}AFX_DATA_INIT
}


void CSlideShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSlideShowDlg)
	DDX_Radio(pDX, IDC_MANUALLY, m_nDisplayType);
	DDX_Check(pDX, IDC_LAYER1_AS_MASTER, m_bLayer1AsMaster);
	DDX_Text(pDX, IDC_SECONDS, m_nSeconds);
	DDV_MinMaxInt(pDX, m_nSeconds, 1, 999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSlideShowDlg, CDialog)
	//{{AFX_MSG_MAP(CSlideShowDlg)
	ON_BN_CLICKED(IDC_MANUALLY, OnManually)
	ON_BN_CLICKED(IDC_AUTOMATIC, OnAutomatic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSlideShowDlg message handlers

BOOL CSlideShowDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(m_pDoc->GetLayersCount()==1)	GetDlgItem(IDC_LAYER1_AS_MASTER)->EnableWindow(FALSE);
	GetDlgItem(IDC_SECONDS)->EnableWindow(m_nDisplayType?TRUE:FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSlideShowDlg::OnManually() 
{
	GetDlgItem(IDC_SECONDS)->EnableWindow(FALSE);
}

void CSlideShowDlg::OnAutomatic() 
{
	GetDlgItem(IDC_SECONDS)->EnableWindow();
}

void CSlideShowDlg::OnOK() 
{
	CDialog::OnOK();
	m_pDoc->m_nSlideSeconds=m_nDisplayType?m_nSeconds:0;
	m_pDoc->m_bLayer1AsMaster=m_bLayer1AsMaster?true:false;
}
