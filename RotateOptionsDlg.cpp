// RotateOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "RotateOptionsDlg.h"

#include "phmath.h"
#include "PCDraftDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRotateOptionsDlg dialog


CRotateOptionsDlg::CRotateOptionsDlg(Angle angle,CPCDraftDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CRotateOptionsDlg::IDD, pParent)
{
	m_Angle=angle;
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CRotateOptionsDlg)
	m_bClockwise = FALSE;
	m_nAbout = 0;
	//}}AFX_DATA_INIT
}

void CRotateOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRotateOptionsDlg)
	DDX_Control(pDX, IDC_DEGREE, m_Degree);
	DDX_Control(pDX, IDC_MINUTE, m_Minute);
	DDX_Control(pDX, IDC_SECOND, m_Second);
	DDX_Control(pDX, IDC_SECOND_SYMBOL, m_SecondSymbol);
	DDX_Control(pDX, IDC_MINUTE_SYMBOL, m_MinuteSymbol);
	DDX_Check(pDX, IDC_CLOCKWISE, m_bClockwise);
	DDX_Radio(pDX, IDC_ABOUT_CENTER, m_nAbout);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRotateOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CRotateOptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotateOptionsDlg message handlers

BOOL CRotateOptionsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	short nDegree,nMinute,nSecond;
	mtAng2DMS(m_Angle,&nDegree,&nMinute,&nSecond);

	int nAngleUnit=m_pDoc->GetAngleUnit();
	if(nAngleUnit<kDegMinSeconds)
	{
		m_Second.ShowWindow(SW_HIDE);
		m_SecondSymbol.ShowWindow(SW_HIDE);
		m_Second.SetValue(0);
	}
	else m_Second.SetValue(nSecond);

	if(nAngleUnit<kDegreesMinutes)
	{
		m_Minute.ShowWindow(SW_HIDE);
		m_MinuteSymbol.ShowWindow(SW_HIDE);

		m_Degree.SetPrecision(m_pDoc->m_nAnglePlaces);
		double dblAngle=mtFixToReal(m_Angle);
		if(nAngleUnit<kDecimalDegrees)	// user defined
			dblAngle=-(dblAngle*nAngleUnit)/360.0 ; // Convert from degrees to user units
		m_Degree.SetValue(dblAngle);
	}
	else
	{
		m_Minute.SetValue(nMinute);
		m_Degree.SetValue(nDegree);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CRotateOptionsDlg::OnOK() 
{
	CDialog::OnOK();

	int nAngleUnit=m_pDoc->GetAngleUnit();
	if(nAngleUnit<kDegreesMinutes)
	{
		double dblAngle;
		m_Degree.GetValue(dblAngle);
		if(nAngleUnit<kDecimalDegrees)	// user defined
			dblAngle=-(dblAngle*360.0)/nAngleUnit ; // Convert from user units to degrees
		m_Angle.SetFromDoubleTruncated(dblAngle);
	}
	else
	{
		int nDegree,nMinute,nSecond;
		m_Degree.GetValue(nDegree);
		m_Minute.GetValue(nMinute);
		m_Second.GetValue(nSecond);
		m_Angle=mtDMS2Ang(nDegree,nMinute,nSecond);
	}
}
