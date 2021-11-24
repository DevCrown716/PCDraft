// LoadLayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "LoadLayerDlg.h"
#include "PCDraftDoc.h"
#include "Layer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoadLayerDlg dialog


CLoadLayerDlg::CLoadLayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoadLayerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoadLayerDlg)
	m_nLayerIndex = -1;
	//}}AFX_DATA_INIT
}


void CLoadLayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoadLayerDlg)
	DDX_Control(pDX, IDC_LAYER_LIST, m_LayerList);
	DDX_LBIndex(pDX, IDC_LAYER_LIST, m_nLayerIndex);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoadLayerDlg, CDialog)
	//{{AFX_MSG_MAP(CLoadLayerDlg)
	ON_LBN_SELCHANGE(IDC_LAYER_LIST, OnSelchangeLayerList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoadLayerDlg message handlers

void CLoadLayerDlg::OnSelchangeLayerList() 
{
	GetDlgItem(IDOK)->EnableWindow(m_LayerList.GetCurSel()!=LB_ERR);
}

BOOL CLoadLayerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_LayerList.ResetContent();
	for(int i=0;i<m_pDoc->GetLayersCount();i++)
	{
		CLayer* pLayer=m_pDoc->GetLayer(i);
		m_LayerList.AddString(get_cp(get_uni(pLayer->m_strName), CP_ACP));
	}

	CString theTitle;
	theTitle.Format(IDS_LAYERS,m_pDoc->GetTitle());
	SetWindowText(theTitle);
	
	return TRUE;
}
