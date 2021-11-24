// SelectViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "SelectViewDlg.h"

#include "SavedView.h"
#include "PCDraftDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectViewDlg dialog

CSelectViewDlg::CSelectViewDlg(CPCDraftDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CSelectViewDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CSelectViewDlg)
	m_nView = -1;
	//}}AFX_DATA_INIT
}

void CSelectViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectViewDlg)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_VIEW_LIST, m_ViewList);
	DDX_LBIndex(pDX, IDC_VIEW_LIST, m_nView);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectViewDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectViewDlg)
	ON_LBN_SELCHANGE(IDC_VIEW_LIST, OnSelchangeViewList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectViewDlg message handlers

BOOL CSelectViewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	for(int i=0;i<m_pDoc->m_SavedViews.GetSize();i++)
	{
		CSavedView* pView=(CSavedView*)m_pDoc->m_SavedViews[i];
		m_ViewList.AddString(pView->m_sTitle);
	}

	return TRUE;
}

void CSelectViewDlg::OnSelchangeViewList() 
{
	m_OK.EnableWindow(m_ViewList.GetCurSel()!=LB_ERR);
}
