// FilletSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "FilletSetupDlg.h"

#include "phmath.h"
#include "PCDraftDoc.h"
#include "FilletContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilletSetupDlg dialog


CFilletSetupDlg::CFilletSetupDlg(CPCDraftDoc* pDoc,CFilletContext* pCtx,CWnd* pParent /*=NULL*/)
	: CDialog(CFilletSetupDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	m_pCtx=pCtx;
	//{{AFX_DATA_INIT(CFilletSetupDlg)
	m_nTrim = m_pCtx->m_nTrimMethod;
	m_nArc = m_pCtx->m_bSmallerArc?0:1;;
	//}}AFX_DATA_INIT
}


void CFilletSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFilletSetupDlg)
	DDX_Control(pDX, IDC_RADIUS, m_Radius);
	DDX_Radio(pDX, IDC_AUTO_TRIM, m_nTrim);
	DDX_Radio(pDX, IDC_SMALLER_ARC, m_nArc);
	//}}AFX_DATA_MAP
	if(pDX->m_bSaveAndValidate)
	{
		double fRadius;
		m_Radius.GetValue(fRadius);
		if(fRadius<=0)
		{
			MessageBeep(MB_ICONEXCLAMATION);
			pDX->PrepareEditCtrl(IDC_RADIUS);
			pDX->Fail();
		}
	}
}


BEGIN_MESSAGE_MAP(CFilletSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CFilletSetupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilletSetupDlg message handlers

BOOL CFilletSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Radius.SetDocument(m_pDoc);
	m_Radius.SetValue(kUnitlessValue,m_pCtx->m_fRadius);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CFilletSetupDlg::OnOK() 
{
	CDialog::OnOK();

	m_pCtx->m_nTrimMethod=m_nTrim;
	m_pCtx->m_bSmallerArc=(m_nArc==0);
	m_Radius.GetValue(m_pCtx->m_fRadius);
}
