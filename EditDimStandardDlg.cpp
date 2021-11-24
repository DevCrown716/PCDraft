// EditDimStandardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "EditDimStandardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditDimStandardDlg dialog


CEditDimStandardDlg::CEditDimStandardDlg(DimStandard* curStandard,CWnd* pParent /*=NULL*/)
	: CDialog(CEditDimStandardDlg::IDD, pParent)
{
	m_pDimStd=curStandard;
	m_workStd=*m_pDimStd;
	//{{AFX_DATA_INIT(CEditDimStandardDlg)
	m_Standard.LoadString((m_workStd.nType==kDsEnglish)?IDS_ANSI_UNITS:IDS_METRIC_UNITS);
	m_Units.LoadString((m_workStd.nType==kDsEnglish)?IDS_INCHES:IDS_MILLIMETERS);
	//}}AFX_DATA_INIT
}


void CEditDimStandardDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditDimStandardDlg)
	DDX_Control(pDX, IDC_WIT_EXTENSION, m_WitExt);
	DDX_Control(pDX, IDC_OUTSIDE_LINE_LENGTH, m_OutsideLineLen);
	DDX_Control(pDX, IDC_CIRCLE_CENTER_LENGTH, m_CircleCenterLen);
	DDX_Control(pDX, IDC_CIRCLE_CENTER_GAP, m_CircleCenterGap);
	DDX_Control(pDX, IDC_CIRCLE_CENTER_EXT, m_CircleCenterExt);
	DDX_Control(pDX, IDC_DIM_TEXT_GAP, m_DimTextGap);
	DDX_Control(pDX, IDC_LEADER_LENGTH, m_LeaderLen);
	DDX_Control(pDX, IDC_TOL_SPACE_SCALE, m_TolSpaceScale);
	DDX_Control(pDX, IDC_TOL_TEXT_SCALE, m_TolTextScale);
	DDX_Control(pDX, IDC_WIT_LINE_GAP, m_WitLineGap);
	DDX_Text(pDX, IDC_UNITS, m_Units);
	DDX_Text(pDX, IDC_CURRENT_STANDARD, m_Standard);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditDimStandardDlg, CDialog)
	//{{AFX_MSG_MAP(CEditDimStandardDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditDimStandardDlg message handlers

BOOL CEditDimStandardDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	double dMax=m_workStd.nType==kDsEnglish?4:100;
	short nPrecis=m_workStd.nType==kDsEnglish?4:1;
	m_WitExt.SetMinMax(0,dMax);
	m_WitLineGap.SetMinMax(0,dMax);
	m_DimTextGap.SetMinMax(0,dMax);
	m_LeaderLen.SetMinMax(0,dMax);
	m_OutsideLineLen.SetMinMax(0,dMax);
	m_TolSpaceScale.SetMinMax(25,200);
	m_TolTextScale.SetMinMax(25,200);
	m_CircleCenterLen.SetMinMax(0,dMax);
	m_CircleCenterGap.SetMinMax(0,dMax);
	m_CircleCenterExt.SetMinMax(0,dMax);

	m_WitExt.SetPrecision(nPrecis);
	m_WitLineGap.SetPrecision(nPrecis);
	m_DimTextGap.SetPrecision(nPrecis);
	m_LeaderLen.SetPrecision(nPrecis);
	m_OutsideLineLen.SetPrecision(nPrecis);
	m_CircleCenterLen.SetPrecision(nPrecis);
	m_CircleCenterGap.SetPrecision(nPrecis);
	m_CircleCenterExt.SetPrecision(nPrecis);

	UpdateValues(false);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CEditDimStandardDlg::UpdateValues(bool bSaveAndValidate)
{
	if(bSaveAndValidate)
	{
		m_WitExt.GetValue(m_workStd.WitnessExt);
		m_WitLineGap.GetValue(m_workStd.WitnessLineGap);
		m_DimTextGap.GetValue(m_workStd.DimTextGap);
		m_LeaderLen.GetValue(m_workStd.LeaderLen);
		m_OutsideLineLen.GetValue(m_workStd.OutsideLineLen);
		m_TolSpaceScale.GetValue(m_workStd.nToleranceSpaceScale);
		m_TolTextScale.GetValue(m_workStd.nToleranceTextScale);
		m_CircleCenterLen.GetValue(m_workStd.CircleCenterLen);
		m_CircleCenterGap.GetValue(m_workStd.CircleCenterGap);
		m_CircleCenterExt.GetValue(m_workStd.CircleCenterExt);
	}
	else
	{
		m_WitExt.SetValue(m_workStd.WitnessExt);
		m_WitLineGap.SetValue(m_workStd.WitnessLineGap);
		m_DimTextGap.SetValue(m_workStd.DimTextGap);
		m_LeaderLen.SetValue(m_workStd.LeaderLen);
		m_OutsideLineLen.SetValue(m_workStd.OutsideLineLen);
		m_TolSpaceScale.SetValue(m_workStd.nToleranceSpaceScale);
		m_TolTextScale.SetValue(m_workStd.nToleranceTextScale);
		m_CircleCenterLen.SetValue(m_workStd.CircleCenterLen);
		m_CircleCenterGap.SetValue(m_workStd.CircleCenterGap);
		m_CircleCenterExt.SetValue(m_workStd.CircleCenterExt);
	}
}

void CEditDimStandardDlg::OnOK() 
{
	UpdateValues();
	*m_pDimStd=m_workStd;

	CDialog::OnOK();
}
