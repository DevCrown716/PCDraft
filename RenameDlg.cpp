// RenameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "RenameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg dialog


CRenameDlg::CRenameDlg(const CString& sName,UINT nWhat,CWnd* pParent /*=NULL*/)
	: CDialog(CRenameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRenameDlg)
	m_sName = get_cp(get_uni(sName), CP_ACP);
	m_sEditPrompt = _T("");
	m_sRenamePrompt = _T("");
	//}}AFX_DATA_INIT
	CString strWhat;
	strWhat.LoadString(nWhat);
	m_sEditPrompt.Format(IDS_RENAME_LABEL,strWhat);
	strWhat.MakeLower();
	m_sRenamePrompt.Format(IDS_RENAME_PROMPT,sName,strWhat,strWhat);
}


void CRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameDlg)
	DDX_Text(pDX, IDC_NAME, m_sName);
	DDX_Text(pDX, IDC_EDIT_PROMPT, m_sEditPrompt);
	DDX_Text(pDX, IDC_RENAME_PROMPT, m_sRenamePrompt);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRenameDlg, CDialog)
	//{{AFX_MSG_MAP(CRenameDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg message handlers
