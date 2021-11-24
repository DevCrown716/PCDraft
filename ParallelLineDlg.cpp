// ParallelLineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ParallelLineDlg.h"

#include "DSShape.h"
#include "PCDraftDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const Coord kMaxLineWidth = WrFixedFromFixed ( 0x01000000L ) ;

/////////////////////////////////////////////////////////////////////////////
// CParallelLineDlg dialog

CParallelLineDlg::CParallelLineDlg(ParallelAttrs* pAttrs,CPCDraftDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CParallelLineDlg::IDD, pParent)
{
	m_pAttrs=pAttrs;
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CParallelLineDlg)
	m_nAlign = m_pAttrs->m_nAlign;
	m_nEndCap = m_pAttrs->m_nEndCap;
	m_nJoint = m_pAttrs->m_nJoint;
	m_sLimits = _T("");
	m_sUnits = _T("");
	//}}AFX_DATA_INIT

	LongRatio ratio;
	m_pDoc->GetActiveScaleRatio(ratio);
	m_sLimits.Format("(%s - %s)",m_pDoc->GetDimUnit()->numUser2Str(0,false),m_pDoc->GetDimUnit()->numUser2Str(m_pDoc->GetDimUnit()->numDB2User(ratio,kMaxLineWidth),false));
}


void CParallelLineDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CParallelLineDlg)
	DDX_Control(pDX, IDC_LINE_WIDTH, m_LineWidth);
	DDX_Radio(pDX, IDC_PL_ALIGN1, m_nAlign);
	DDX_Radio(pDX, IDC_PL_ENDCAP1, m_nEndCap);
	DDX_Radio(pDX, IDC_PL_JOINT1, m_nJoint);
	DDX_Text(pDX, IDC_LIMITS, m_sLimits);
	DDX_Text(pDX, IDC_UNITS, m_sUnits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CParallelLineDlg, CDialog)
	//{{AFX_MSG_MAP(CParallelLineDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParallelLineDlg message handlers

BOOL CParallelLineDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_PL_ALIGN1))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PL_ALIGN1,0,NULL,0));
	((CButton*)GetDlgItem(IDC_PL_ALIGN2))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PL_ALIGN2,0,NULL,0));
	((CButton*)GetDlgItem(IDC_PL_ALIGN3))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PL_ALIGN3,0,NULL,0));

	((CButton*)GetDlgItem(IDC_PL_ENDCAP1))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PL_ENDCAP1,0,NULL,0));
	((CButton*)GetDlgItem(IDC_PL_ENDCAP2))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PL_ENDCAP2,0,NULL,0));
	((CButton*)GetDlgItem(IDC_PL_ENDCAP3))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PL_ENDCAP3,0,NULL,0));
	((CButton*)GetDlgItem(IDC_PL_ENDCAP4))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PL_ENDCAP4,0,NULL,0));

	((CButton*)GetDlgItem(IDC_PL_JOINT1))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PL_JOINT1,0,NULL,0));
	((CButton*)GetDlgItem(IDC_PL_JOINT2))->SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_PL_JOINT2,0,NULL,0));

	m_LineWidth.SetDocument(m_pDoc);
	m_LineWidth.SetValue(kUnitlessValue,m_pAttrs->m_fWidth);
	m_LineWidth.SetMinMax(FixedZero,kMaxLineWidth);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CParallelLineDlg::OnOK() 
{
	CDialog::OnOK();
	m_pAttrs->m_nAlign=m_nAlign;
	m_pAttrs->m_nEndCap=m_nEndCap;
	m_pAttrs->m_nJoint=m_nJoint;
	m_LineWidth.GetValue(m_pAttrs->m_fWidth);
}
