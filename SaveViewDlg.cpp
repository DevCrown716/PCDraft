// SaveViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "SaveViewDlg.h"

#include "SavedView.h"
#include "PCDraftDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSaveViewDlg dialog


CSaveViewDlg::CSaveViewDlg(CPCDraftDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CSaveViewDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CSaveViewDlg)
	m_sTitle = _T("");
	m_bUseKey = FALSE;
	//}}AFX_DATA_INIT
}

void CSaveViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaveViewDlg)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_USE_KEY, m_UseKey);
	DDX_Text(pDX, IDC_VIEW_NAME, m_sTitle);
	DDX_Check(pDX, IDC_USE_KEY, m_bUseKey);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSaveViewDlg, CDialog)
	//{{AFX_MSG_MAP(CSaveViewDlg)
	ON_EN_CHANGE(IDC_VIEW_NAME, OnChangeViewName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaveViewDlg message handlers

BOOL CSaveViewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_nShortcut=1;
	for(int i=0;i<m_pDoc->m_SavedViews.GetSize();i++)
	{
		CSavedView* pView=(CSavedView*)m_pDoc->m_SavedViews[i];
		if(pView->m_nShortcut==m_nShortcut)
		{
			if(++m_nShortcut>5) break;
			i=-1; // restart checking
		}
	}

	if(m_nShortcut<6)
	{
		CString text;
		text.Format(IDS_SAVEVIEWSHORTCUT,m_nShortcut+4);
		m_UseKey.SetWindowText(text);
		m_UseKey.SetCheck(true);
	}
	else
	{
		m_UseKey.ShowWindow(SW_HIDE);
		m_nShortcut=0;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSaveViewDlg::OnChangeViewName() 
{
	bool bState=GetDlgItem(IDC_VIEW_NAME)->GetWindowTextLength()>0;
	m_UseKey.EnableWindow(bState);
	m_OK.EnableWindow(bState);
}

