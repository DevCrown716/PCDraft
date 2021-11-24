// CircleDupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "CircleDupDlg.h"

#include "PCDraftDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCircleDupDlg dialog


CCircleDupDlg::CCircleDupDlg(CPCDraftDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CCircleDupDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CCircleDupDlg)
	m_bRotateCopies = TRUE;
	m_nDistribute = 0;
	m_nByObject = 0;
	m_nCircles = 1;
	m_nCopies = 1;
	m_bUseMouse = TRUE;
	//}}AFX_DATA_INIT
}


void CCircleDupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCircleDupDlg)
	DDX_Control(pDX, IDC_ANGLE, m_Angle);
	DDX_Control(pDX, IDC_RADIUS, m_Radius);
	DDX_Control(pDX, IDC_SHIFT, m_Shift);
	DDX_Control(pDX, IDC_CENTRE_Y, m_CentreY);
	DDX_Control(pDX, IDC_CENTRE_X, m_CentreX);
	DDX_Control(pDX, IDC_USE_MOUSE_CLICK, m_UseMouse);
	DDX_Check(pDX, IDC_ROTATE_COPIES, m_bRotateCopies);
	DDX_Radio(pDX, IDC_PRORTIONAL, m_nDistribute);
	DDX_Radio(pDX, IDC_BY_CENTER, m_nByObject);
	DDX_Text(pDX, IDC_CIRCLES, m_nCircles);
	DDX_Text(pDX, IDC_COPIES, m_nCopies);
	DDV_MinMaxInt(pDX, m_nCopies, 1, 9999);
	DDX_Check(pDX, IDC_USE_MOUSE_CLICK, m_bUseMouse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCircleDupDlg, CDialog)
	//{{AFX_MSG_MAP(CCircleDupDlg)
	ON_BN_CLICKED(IDC_USE_MOUSE_CLICK, OnUseMouseClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCircleDupDlg message handlers

BOOL CCircleDupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_Angle.SetDocument(m_pDoc);
	m_Radius.SetDocument(m_pDoc);
	m_Shift.SetDocument(m_pDoc);
	m_CentreX.SetDocument(m_pDoc);
	m_CentreY.SetDocument(m_pDoc);
	m_Angle.SetValue(kAngleValue,360);
	m_Radius.SetValue(kUnitlessValue,0);
	m_Shift.SetValue(kAngleValue,0);
	m_CentreX.SetValue(kUnitlessValue,0);
	m_CentreY.SetValue(kUnitlessValue,0);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CCircleDupDlg::OnUseMouseClick() 
{
	bool bState=m_UseMouse.GetCheck()==0;
	m_CentreX.EnableWindow(bState);
	m_CentreY.EnableWindow(bState);
}

