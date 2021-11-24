// PositionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "PositionDlg.h"

#include "PCDraftDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPositionDlg dialog


CPositionDlg::CPositionDlg(CPCDraftDoc *pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CPositionDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CPositionDlg)
	//}}AFX_DATA_INIT
}


void CPositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPositionDlg)
	DDX_Control(pDX, IDC_MEASURE_X, m_MeasureX);
	DDX_Control(pDX, IDC_MEASURE_Y, m_MeasureY);
	DDX_Control(pDX, IDC_POSITION_X, m_PositionX);
	DDX_Control(pDX, IDC_POSITION_Y, m_PositionY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPositionDlg, CDialog)
	//{{AFX_MSG_MAP(CPositionDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPositionDlg message handlers

BOOL CPositionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString sMeasure;

	if(m_pDoc->m_bEnglishUnits)
	{
		m_unitFactor=72.0;
		sMeasure.LoadString(IDS_INCHES_ABBR2);
	}
	else
	{
		m_unitFactor=28.0;
		sMeasure.LoadString(IDS_CENTIMETER_ABBR);
	}
	m_MeasureX.SetWindowText(sMeasure);
	m_MeasureY.SetWindowText(sMeasure);

	LongRect cumExt=m_pDoc->GetCumulativeExtents();
	m_PositionX.SetValue(mtFixToReal(cumExt.left)/m_unitFactor);
	m_PositionY.SetValue(mtFixToReal(cumExt.top)/m_unitFactor);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CPositionDlg::OnOK() 
{
	CDialog::OnOK();

	LongPoint delta;
	LongRect cumExt=m_pDoc->GetCumulativeExtents();

	double value;
	m_PositionX.GetValue(value);
	m_Delta.x=value*m_unitFactor-cumExt.left;
	m_PositionY.GetValue(value);
	m_Delta.y=value*m_unitFactor-cumExt.top;
}
