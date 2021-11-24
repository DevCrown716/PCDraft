// EditDimFormatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "EditDimFormatDlg.h"
#include "EditDimStandardDlg.h"
#include "PCDraftDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditDimFormatDlg dialog


CEditDimFormatDlg::CEditDimFormatDlg(DimFormat* curFormat,DimStandard* curStandard,CPCDraftDoc* pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CEditDimFormatDlg::IDD, pParent)
{
	m_pDimFmt=curFormat;
	m_workFmt=*m_pDimFmt;
	m_pDimStd=curStandard;
	m_workStd=*m_pDimStd;
	m_pDoc=pDoc;
	m_nChanges=0;

	//{{AFX_DATA_INIT(CEditDimFormatDlg)
	m_sDimType.LoadString(IDS_DIMTYPEFIRST+m_workFmt.nType);
	m_sTol1Measure.LoadString((m_workFmt.nType==kAngular)?IDS_DEGREE_SYMBOL:((m_workStd.nType==kDsEnglish)?IDS_INCH_ABBR:IDS_MILLIMETER_ABBR));
	m_sTol2Measure = m_sTol1Measure;
	//}}AFX_DATA_INIT
			

}


void CEditDimFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditDimFormatDlg)
	DDX_Control(pDX, IDC_LABEL, m_Label);
	DDX_Control(pDX, IDC_DIM_ROUNDOFF, m_DimRoundoff);
	DDX_Control(pDX, IDC_DIM_LABEL, m_DimLabel);
	DDX_Control(pDX, IDC_TEXT_CENTERED, m_TextCentered);
	DDX_Control(pDX, IDC_TEXT_FRAMED, m_TextFramed);
	DDX_Control(pDX, IDC_ARROWS_INSIDE, m_ArrowsInside);
	DDX_Control(pDX, IDC_WITNESS_LINES, m_WitnessLines);
	DDX_Control(pDX, IDC_TEXT_DISPLAY, m_TextDisplay);
	DDX_Control(pDX, IDC_LEADER, m_Leader);
	DDX_Control(pDX, IDC_RDSYMBOL, m_RDSymbol);
	DDX_Control(pDX, IDC_TOLERANCE, m_Tolerance);
	DDX_Control(pDX, IDC_TOL2, m_Tol2);
	DDX_Control(pDX, IDC_TOL1, m_Tol1);
	DDX_Text(pDX, IDC_DIM_TYPE, m_sDimType);
	DDX_Text(pDX, IDC_TOL1_MEASURE, m_sTol1Measure);
	DDX_Text(pDX, IDC_TOL2_MEASURE, m_sTol2Measure);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditDimFormatDlg, CDialog)
	//{{AFX_MSG_MAP(CEditDimFormatDlg)
	ON_BN_CLICKED(IDC_STANDARDS, OnStandards)
	ON_CBN_SELCHANGE(IDC_TOLERANCE, OnSelchangeTolerance)
	ON_CBN_SELCHANGE(IDC_LEADER, OnSelchangeLeader)
	ON_CBN_SELCHANGE(IDC_TEXT_DISPLAY, OnSelchangeTextDisplay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditDimFormatDlg message handlers

BOOL CEditDimFormatDlg::OnInitDialog() 
{
	CString strText;
	CComboBox* pCB=(CComboBox*)GetDlgItem(IDC_TEXT_DISPLAY);
	for(int i=IDS_DISPLAYFIRST;i<=IDS_DISPLAYLAST;i++)
	{
		strText.LoadString(i);
		pCB->AddString(strText);
	}
	pCB=(CComboBox*)GetDlgItem(IDC_LEADER);
	for(int i=IDS_LEADERFIRST;i<=IDS_LEADERLAST;i++)
	{
		strText.LoadString(i);
		pCB->AddString(strText);
	}
	pCB=(CComboBox*)GetDlgItem(IDC_RDSYMBOL);
    for (int i = IDS_RDSYMBOLFIRST; i <= IDS_RDSYMBOLLAST; i++)
	{
		strText.LoadString(i);
		pCB->AddString(strText);
	}
	pCB=(CComboBox*)GetDlgItem(IDC_TOLERANCE);
    for (int i = IDS_TOLERANCEFIRST; i <= IDS_TOLERANCELAST; i++)
	{
		strText.LoadString(i);
		pCB->AddString(strText);
	}

	CDialog::OnInitDialog();

	m_WitnessLines.EnableWindow(m_workFmt.nType!=kRadial);
	m_TextDisplay.EnableWindow(m_workFmt.nType!=kAngular);
	m_Leader.EnableWindow(m_workFmt.nType!=kAngular);
	m_RDSymbol.EnableWindow(m_workFmt.nType==kRadial);
	int nPrecision=(m_workFmt.nType==kAngular)?1:4;
	m_Tol1.SetPrecision(nPrecision);
	m_Tol2.SetPrecision(nPrecision);

	strText.LoadString(IDS_ROUNDOFF);
	m_DimRoundoff.AddString(strText);
	CStringArray strList;
	m_pDoc->GetActiveGridSnaps(strList);
    for (int i = 0; i<strList.GetSize(); i++)
	{
		m_DimRoundoff.AddString(strList[i]);
	}
	UpdateValues(false);
	OnSelchangeTolerance();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CEditDimFormatDlg::OnStandards() 
{
	CEditDimStandardDlg theDlg(&m_workStd);
	theDlg.DoModal();
}

void CEditDimFormatDlg::OnSelchangeTolerance() 
{
	int nTol=m_Tolerance.GetCurSel();
	m_Tol1.EnableWindow(nTol>0);
	m_Tol2.EnableWindow(nTol>1);
	m_DimRoundoff.EnableWindow(m_workFmt.nType!=kAngular && nTol<3);
}

void CEditDimFormatDlg::UpdateValues(bool bSaveAndValidate)
{
	if(bSaveAndValidate)
	{
		m_workFmt.bTextCentered=m_TextCentered.GetCheck()?true:false;
		m_workFmt.bTextFramed=m_TextFramed.GetCheck()?true:false;
		m_workFmt.bArrowsInside=m_ArrowsInside.GetCheck()?true:false;
		m_workFmt.bUseWitness=m_WitnessLines.GetCheck()?true:false;
		m_workFmt.nTextDisplay=m_TextDisplay.GetCurSel();
		m_workFmt.nLeader=m_Leader.GetCurSel();
		m_workFmt.nRDSymbol=m_RDSymbol.GetCurSel();
		m_workFmt.nTolerance=m_Tolerance.GetCurSel();
		m_Tol1.GetValue(m_workFmt.Tolerance1);
		m_Tol2.GetValue(m_workFmt.Tolerance2);
		m_workFmt.bUseLabel=m_DimLabel.GetCheck()?true:false;
		m_Label.GetWindowText(m_workFmt.sLabel);
		m_workFmt.nRoundoff=m_DimRoundoff.GetCurSel();
	}
	else
	{
		m_TextCentered.SetCheck(m_workFmt.bTextCentered);
		m_TextFramed.SetCheck(m_workFmt.bTextFramed);
		m_ArrowsInside.SetCheck(m_workFmt.bArrowsInside);
		m_WitnessLines.SetCheck(m_workFmt.bUseWitness);
		m_TextDisplay.SetCurSel(m_workFmt.nTextDisplay);
		m_Leader.SetCurSel(m_workFmt.nLeader);
		m_RDSymbol.SetCurSel(m_workFmt.nRDSymbol);
		m_Tolerance.SetCurSel(m_workFmt.nTolerance);
		m_Tol1.SetValue(m_workFmt.Tolerance1);
		m_Tol2.SetValue(m_workFmt.Tolerance2);
		m_DimLabel.SetCheck(m_workFmt.bUseLabel);
		m_Label.SetWindowText(m_workFmt.sLabel);
		m_DimRoundoff.SetCurSel(m_workFmt.nRoundoff);
	}
}

void CEditDimFormatDlg::OnOK() 
{
	UpdateValues();
	m_nChanges=m_pDimFmt->Compare(m_workFmt);
	*m_pDimFmt=m_workFmt;
	if(m_pDimStd->Compare(m_workStd)) m_nChanges|=dfUseStandard;
	*m_pDimStd=m_workStd;
	
	CDialog::OnOK();
}

void CEditDimFormatDlg::OnSelchangeLeader() 
{
	if(m_Leader.GetCurSel()>0 && m_TextDisplay.GetCurSel()>0)
	{
		m_TextDisplay.SetCurSel(0);
	}
}

void CEditDimFormatDlg::OnSelchangeTextDisplay() 
{
	if(m_TextDisplay.GetCurSel()>0 && m_Leader.GetCurSel()>0)
	{
		m_Leader.SetCurSel(0);
	}
}
