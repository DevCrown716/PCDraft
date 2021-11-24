// AttributeOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "AttributeOptionsDlg.h"

#include "DSShape.h"
#include "PCDraftDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAttributeOptionsDlg dialog


CAttributeOptionsDlg::CAttributeOptionsDlg(CPCDraftDoc* pDoc,ShapeAttributes* pAttr,bool bApply,CWnd* pParent /*=NULL*/)
	: CDialog(CAttributeOptionsDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	m_pAttr=pAttr;
	m_bApply=bApply;
	//{{AFX_DATA_INIT(CAttributeOptionsDlg)
	m_bUseFill = m_pAttr->m_nFill>0;
	m_bUsePen = m_pAttr->m_nPen>0;
	m_bUseInfo = m_pAttr->m_bInfo;
	m_sLabel1 = m_pDoc->m_sFieldNames[0]+':';
	m_sValue1 = m_pAttr->m_sInfo[0];
	m_sLabel2 = m_pDoc->m_sFieldNames[1]+':';
	m_sValue2 = m_pAttr->m_sInfo[1];
	m_sLabel3 = m_pDoc->m_sFieldNames[2]+':';
	m_sValue3 = m_pAttr->m_sInfo[2];
	m_sLabel4 = m_pDoc->m_sFieldNames[3]+':';
	m_sValue4 = m_pAttr->m_sInfo[3];
	m_sLabel5 = m_pDoc->m_sFieldNames[4]+':';
	m_sValue5 = m_pAttr->m_sInfo[4];
	m_bUsePara = m_pAttr->m_nPara;
	//}}AFX_DATA_INIT
}


void CAttributeOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAttributeOptionsDlg)
	DDX_Check(pDX, IDC_OBJECT_FILL, m_bUseFill);
	DDX_Control(pDX, IDC_CURRENT_FILL, m_curFill);
	DDX_Check(pDX, IDC_OBJECT_PEN, m_bUsePen);
	DDX_Control(pDX, IDC_CURRENT_PEN, m_curPen);
	DDX_Check(pDX, IDC_OBJECT_INFO, m_bUseInfo);
	DDX_Text(pDX, IDC_FIELD1_LABEL, m_sLabel1);
	DDX_Text(pDX, IDC_FIELD1_VALUE, m_sValue1);
	DDX_Text(pDX, IDC_FIELD2_LABEL, m_sLabel2);
	DDX_Text(pDX, IDC_FIELD2_VALUE, m_sValue2);
	DDX_Text(pDX, IDC_FIELD3_LABEL, m_sLabel3);
	DDX_Text(pDX, IDC_FIELD3_VALUE, m_sValue3);
	DDX_Text(pDX, IDC_FIELD4_LABEL, m_sLabel4);
	DDX_Text(pDX, IDC_FIELD4_VALUE, m_sValue4);
	DDX_Text(pDX, IDC_FIELD5_LABEL, m_sLabel5);
	DDX_Text(pDX, IDC_FIELD5_VALUE, m_sValue5);
	DDX_Check(pDX, IDC_PARALLEL_FILL, m_bUsePara);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAttributeOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CAttributeOptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAttributeOptionsDlg message handlers

BOOL CAttributeOptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if(m_bApply)
	{
		CString text;
		text.LoadString(IDS_APPLY_ATTRIBUTES);
		SetWindowText(text);
		text.LoadString(IDS_APPLY);
		SetDlgItemText(IDOK,text);
	}
	
	m_curFill.SetDoc(m_pDoc);
	if(m_pAttr->m_nFill<0)
	{
		GetDlgItem(IDC_OBJECT_FILL)->EnableWindow(FALSE);
		m_curFill.EnableWindow(FALSE);
	}
	else m_curFill.SetFill(m_pAttr->m_Fill);

	m_curPen.SetDoc(m_pDoc);
	CStatic *pPenStyle = (CStatic *)GetDlgItem(IDC_STATIC_PENSTYLE);
	if (pPenStyle) pPenStyle->SetIcon(::LoadIcon(::AfxGetInstanceHandle(), MAKEINTRESOURCE(m_pAttr->m_Pen.isPenStyleRound() ? IDI_PENSTYLE_ROUND : IDI_PENSTYLE_BLADE)));
	if(m_pAttr->m_nPen<0)
	{
		GetDlgItem(IDC_OBJECT_PEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_PENSTYLE)->EnableWindow(FALSE);
		m_curPen.EnableWindow(FALSE);
	}
	else m_curPen.SetPen(m_pAttr->m_Pen);
	if(m_pAttr->m_nPara<0)
		GetDlgItem(IDC_PARALLEL_FILL)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CAttributeOptionsDlg::OnOK() 
{
	CDialog::OnOK();
	if(m_pAttr->m_nFill>=0) m_pAttr->m_nFill=m_bUseFill?1:0;
	if(m_pAttr->m_nPen>=0) m_pAttr->m_nPen=m_bUsePen?1:0;
	m_pAttr->m_bInfo=m_bUseInfo?true:false;
	if(m_pAttr->m_nPara>=0) m_pAttr->m_nPara=m_bUsePara?1:0;
}
