// LinearDupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "LinearDupDlg.h"

#include "PCDraftDoc.h"
#include "phmath.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinearDupDlg dialog


CLinearDupDlg::CLinearDupDlg(CPCDraftDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CLinearDupDlg::IDD, pParent)
{
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CLinearDupDlg)
	m_nDupType = 0;
	m_nDir2ParamType = 0;
	m_nDir1ParamType = 0;
	m_Dir1Copies = 1;
	m_Dir2Copies = 1;
	//}}AFX_DATA_INIT
}


void CLinearDupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinearDupDlg)
	DDX_Control(pDX, IDC_DIR2_PARAM2_VALUE, m_Dir2Value2);
	DDX_Control(pDX, IDC_DIR2_PARAM1_VALUE, m_Dir2Value1);
	DDX_Control(pDX, IDC_DIR1_PARAM2_VALUE, m_Dir1Value2);
	DDX_Control(pDX, IDC_DIR1_PARAM1_VALUE, m_Dir1Value1);
	DDX_Control(pDX, IDC_DIR1_PARAM2_LABEL, m_dir1label2);
	DDX_Control(pDX, IDC_DIR1_PARAM1_LABEL, m_dir1label1);
	DDX_Control(pDX, IDC_DIR2_PARAM2_LABEL, m_dir2label2);
	DDX_Control(pDX, IDC_DIR2_PARAM1_LABEL, m_dir2label1);
	DDX_Radio(pDX, IDC_DIRECTION, m_nDupType);
	DDX_Radio(pDX, IDC_DIR2_DISTANCE_ANGLE, m_nDir2ParamType);
	DDX_Radio(pDX, IDC_DIR1_DISTANCE_ANGLE, m_nDir1ParamType);
	DDX_Text(pDX, IDC_DIR1_COPIES, m_Dir1Copies);
	DDV_MinMaxInt(pDX, m_Dir1Copies, 1, 9999);
	DDX_Text(pDX, IDC_DIR2_COPIES, m_Dir2Copies);
	DDV_MinMaxInt(pDX, m_Dir2Copies, 1, 9999);
	//}}AFX_DATA_MAP
	
}


BEGIN_MESSAGE_MAP(CLinearDupDlg, CDialog)
	//{{AFX_MSG_MAP(CLinearDupDlg)
	ON_BN_CLICKED(IDC_2DIRECTION, On2direction)
	ON_BN_CLICKED(IDC_DIRECTION, OnDirection)
	ON_BN_CLICKED(IDC_DIR1_DISTANCE_ANGLE, OnDir1DistanceAngle)
	ON_BN_CLICKED(IDC_DIR1_OFFSET, OnDir1Offset)
	ON_BN_CLICKED(IDC_DIR2_DISTANCE_ANGLE, OnDir2DistanceAngle)
	ON_BN_CLICKED(IDC_DIR2_OFFSET, OnDir2Offset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinearDupDlg message handlers

BOOL CLinearDupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Dir1Value1.SetDocument(m_pDoc);
	m_Dir1Value2.SetDocument(m_pDoc);
	m_Dir2Value1.SetDocument(m_pDoc);
	m_Dir2Value2.SetDocument(m_pDoc);
	m_Dir1Value1.SetValue(kUnitlessValue,0);
	m_Dir1Value2.SetValue(kAngleValue,0);
	m_Dir2Value1.SetValue(kUnitlessValue,0);
	m_Dir2Value2.SetValue(kAngleValue,0);
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CLinearDupDlg::UpdateControls()
{
	GetDlgItem(IDC_DIR2_COPIES)->EnableWindow(m_nDupType);
	GetDlgItem(IDC_DIR2_DISTANCE_ANGLE)->EnableWindow(m_nDupType);
	GetDlgItem(IDC_DIR2_OFFSET)->EnableWindow(m_nDupType);
	GetDlgItem(IDC_DIR2_PARAM1_LABEL)->EnableWindow(m_nDupType);
	GetDlgItem(IDC_DIR2_PARAM1_VALUE)->EnableWindow(m_nDupType);
	GetDlgItem(IDC_DIR2_PARAM2_LABEL)->EnableWindow(m_nDupType);
	GetDlgItem(IDC_DIR2_PARAM2_VALUE)->EnableWindow(m_nDupType);
}

void CLinearDupDlg::On2direction() 
{
	UpdateData();
	UpdateControls();
}

void CLinearDupDlg::OnDirection() 
{
	UpdateData();
	UpdateControls();
}

void CLinearDupDlg::OnDir1DistanceAngle() 
{
	if(m_nDir1ParamType==1)
	{
		CString strText;
		strText.LoadString(IDS_DUPDISTANCE);
		m_dir1label1.SetWindowText(strText);
		strText.LoadString(IDS_DUPANGLE);
		m_dir1label2.SetWindowText(strText);

		LongPoint offset;
		m_Dir1Value1.GetValue(offset.x);
		m_Dir1Value2.GetValue(offset.y);

		m_Dir1Value1.SetValue(kUnitlessValue,Pythag(offset));
		m_Dir1Value2.SetValue(kAngleValue,mtAngle(LongPoint::kZeroPt,offset));
		m_nDir1ParamType=0;
	}
}

void CLinearDupDlg::OnDir1Offset() 
{
	if(m_nDir1ParamType==0)
	{
		CString strText;
		strText.LoadString(IDS_DUPDELTAX);
		m_dir1label1.SetWindowText(strText);
		strText.LoadString(IDS_DUPDELTAY);
		m_dir1label2.SetWindowText(strText);

		Coord dist;
		Angle angle;
		m_Dir1Value1.GetValue(dist);
		m_Dir1Value2.GetValue(angle);

		m_Dir1Value1.SetValue(kUnitlessValue,dist*mtCos(angle));
		m_Dir1Value2.SetValue(kUnitlessValue,dist*mtSin(angle));

		m_nDir1ParamType=1;
	}
}

void CLinearDupDlg::OnDir2DistanceAngle() 
{
	if(m_nDir2ParamType==1)
	{
		CString strText;
		strText.LoadString(IDS_DUPDISTANCE);
		m_dir2label1.SetWindowText(strText);
		strText.LoadString(IDS_DUPANGLE);
		m_dir2label2.SetWindowText(strText);

		LongPoint offset;
		m_Dir2Value1.GetValue(offset.x);
		m_Dir2Value2.GetValue(offset.y);

		m_Dir2Value1.SetValue(kUnitlessValue,Pythag(offset));
		m_Dir2Value2.SetValue(kAngleValue,mtAngle(LongPoint::kZeroPt,offset));
		m_nDir2ParamType=0;
	}
}

void CLinearDupDlg::OnDir2Offset() 
{
	if(m_nDir2ParamType==0)
	{
		CString strText;
		strText.LoadString(IDS_DUPDELTAX);
		m_dir2label1.SetWindowText(strText);
		strText.LoadString(IDS_DUPDELTAY);
		m_dir2label2.SetWindowText(strText);

		Coord dist;
		Angle angle;
		m_Dir2Value1.GetValue(dist);
		m_Dir2Value2.GetValue(angle);

		m_Dir2Value1.SetValue(kUnitlessValue,dist*mtCos(angle));
		m_Dir2Value2.SetValue(kUnitlessValue,dist*mtSin(angle));

		m_nDir2ParamType=1;
	}
}
