// ChamferSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ChamferSetupDlg.h"

#include "phmath.h"
#include "PCDraftDoc.h"
#include "ChamferContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChamferSetupDlg dialog


CChamferSetupDlg::CChamferSetupDlg(CPCDraftDoc* pDoc,CChamferContext* pCtx,CWnd* pParent /*=NULL*/)
	: CDialog(CChamferSetupDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	m_pCtx=pCtx;
	//{{AFX_DATA_INIT(CChamferSetupDlg)
	m_nTrim = m_pCtx->m_nTrimMethod;
	//}}AFX_DATA_INIT
}


void CChamferSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChamferSetupDlg)
	DDX_Control(pDX, IDC_LENGTH2, m_Length2);
	DDX_Control(pDX, IDC_LENGTH1, m_Length1);
	DDX_Radio(pDX, IDC_AUTO_TRIM, m_nTrim);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChamferSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CChamferSetupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChamferSetupDlg message handlers

BOOL CChamferSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Length1.SetDocument(m_pDoc);
	m_Length2.SetDocument(m_pDoc);
	m_Length1.SetValue(kUnitlessValue,m_pCtx->m_fLine1SetBack);
	m_Length2.SetValue(kUnitlessValue,m_pCtx->m_fLine2SetBack);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CChamferSetupDlg::OnOK() 
{
	CDialog::OnOK();

	m_pCtx->m_nTrimMethod=m_nTrim;
	m_Length1.GetValue(m_pCtx->m_fLine1SetBack);
	m_Length2.GetValue(m_pCtx->m_fLine2SetBack);
}
