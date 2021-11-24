// DimFormatDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "DimFormatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDimFormatDlg dialog

CDimFormatDlg::CDimFormatDlg(int nDSType,int startType,DimFormat* curFormats,CWnd* pParent /*=NULL*/)
	: CDialog(CDimFormatDlg::IDD, pParent)
{
	m_bReactOnChanges = TRUE;
	m_nDSType=nDSType;
	m_nType=startType;
	m_pDimFmts=curFormats;
	m_workFmts[0]=m_pDimFmts[0];
	m_workFmts[1]=m_pDimFmts[1];
	m_workFmts[2]=m_pDimFmts[2];
	//{{AFX_DATA_INIT(CDimFormatDlg)
	//}}AFX_DATA_INIT
}


void CDimFormatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDimFormatDlg)
	DDX_Control(pDX, IDC_REVERT, m_Revert);
	DDX_Control(pDX, IDC_DIM_TYPE, m_DimType);
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
	DDX_Control(pDX, IDC_TOL1_MEASURE, m_Tol1Measure);
	DDX_Control(pDX, IDC_TOL2_MEASURE, m_Tol2Measure);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDimFormatDlg, CDialog)
	//{{AFX_MSG_MAP(CDimFormatDlg)
	ON_BN_CLICKED(IDC_REVERT, OnRevert)
	ON_CBN_SELCHANGE(IDC_DIM_TYPE, OnSelchangeDimType)
	ON_CBN_SELCHANGE(IDC_TOLERANCE, OnSelchangeTolerance)
	ON_CBN_SELCHANGE(IDC_LEADER, OnSelchangeLeader)
	ON_CBN_SELCHANGE(IDC_TEXT_DISPLAY, OnSelchangeTextDisplay)
	ON_BN_CLICKED(IDC_WITNESS_LINES, OnWitnessLines)
	ON_BN_CLICKED(IDC_ARROWS_INSIDE, OnArrowsInside)
	ON_BN_CLICKED(IDC_TEXT_CENTERED, OnTextCentered)
	ON_BN_CLICKED(IDC_TEXT_FRAMED, OnTextFramed)
	ON_EN_CHANGE(IDC_TOL1, OnChangeTol1)
	ON_EN_CHANGE(IDC_TOL2, OnChangeTol2)
	ON_CBN_SELCHANGE(IDC_RDSYMBOL, OnSelchangeRdsymbol)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDimFormatDlg message handlers

BOOL CDimFormatDlg::OnInitDialog() 
{
	CString strText;
	CComboBox* pCB=(CComboBox*)GetDlgItem(IDC_DIM_TYPE);
	for(int i=IDS_DIMTYPEFIRST;i<=IDS_DIMTYPELAST;i++)
	{
		strText.LoadString(i);
		pCB->AddString(strText);
	}
	pCB=(CComboBox*)GetDlgItem(IDC_TEXT_DISPLAY);
	for(int i=IDS_DISPLAYFIRST;i<=IDS_DISPLAYLAST;i++)
	{
		strText.LoadString(i);
		pCB->AddString(strText);
	}
	pCB=(CComboBox*)GetDlgItem(IDC_LEADER);
    for (int i = IDS_LEADERFIRST; i <= IDS_LEADERLAST; i++)
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

	m_DimType.SetCurSel(m_nType);
	UpdateValues(false);
	UpdateControls();
	OnChangeControls();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDimFormatDlg::OnRevert() 
{
	if(m_Revert.GetCheck()==1)
	{
		UpdateValues();
		m_saveFmt=m_workFmts[m_nType];
		m_workFmts[m_nType].FillDefaults(m_nType,m_nDSType);
	}
	else m_workFmts[m_nType]=m_saveFmt;

	UpdateValues(false);
	UpdateControls();
}

void CDimFormatDlg::OnSelchangeDimType() 
{
	UpdateValues();
	m_nType=m_DimType.GetCurSel();

	UpdateValues(false);
	UpdateControls();

	OnChangeControls();
}

void CDimFormatDlg::OnSelchangeTolerance() 
{
	int nTol=m_Tolerance.GetCurSel();
	m_Tol1.EnableWindow(nTol>0);
	m_Tol2.EnableWindow(nTol>1);

	OnChangeControls();
}

void CDimFormatDlg::UpdateValues(bool bSaveAndValidate)
{
	if(bSaveAndValidate)
	{
		m_workFmts[m_nType].bTextCentered=m_TextCentered.GetCheck()?true:false;
		m_workFmts[m_nType].bTextFramed=m_TextFramed.GetCheck()?true:false;
		m_workFmts[m_nType].bArrowsInside=m_ArrowsInside.GetCheck()?true:false;
		m_workFmts[m_nType].bUseWitness=m_WitnessLines.GetCheck()?true:false;
		m_workFmts[m_nType].nTextDisplay=m_TextDisplay.GetCurSel();
		m_workFmts[m_nType].nLeader=m_Leader.GetCurSel();
		m_workFmts[m_nType].nRDSymbol=m_RDSymbol.GetCurSel();
		m_workFmts[m_nType].nTolerance=m_Tolerance.GetCurSel();
		m_Tol1.GetValue(m_workFmts[m_nType].Tolerance1);
		m_Tol2.GetValue(m_workFmts[m_nType].Tolerance2);
	}
	else
	{
		m_TextCentered.SetCheck(m_workFmts[m_nType].bTextCentered);
		m_TextFramed.SetCheck(m_workFmts[m_nType].bTextFramed);
		m_ArrowsInside.SetCheck(m_workFmts[m_nType].bArrowsInside);
		m_WitnessLines.SetCheck(m_workFmts[m_nType].bUseWitness);
		m_TextDisplay.SetCurSel(m_workFmts[m_nType].nTextDisplay);
		m_Leader.SetCurSel(m_workFmts[m_nType].nLeader);
		m_RDSymbol.SetCurSel(m_workFmts[m_nType].nRDSymbol);
		m_Tolerance.SetCurSel(m_workFmts[m_nType].nTolerance);

		m_bReactOnChanges = FALSE;
		m_Tol1.SetValue(m_workFmts[m_nType].Tolerance1);
		m_Tol2.SetValue(m_workFmts[m_nType].Tolerance2);
		m_bReactOnChanges = TRUE;
	}
}

void CDimFormatDlg::UpdateControls()
{
	m_WitnessLines.EnableWindow(m_nType!=kRadial);
	m_TextDisplay.EnableWindow(m_nType!=kAngular);
	m_Leader.EnableWindow(m_nType!=kAngular);
	m_RDSymbol.EnableWindow(m_nType==kRadial);
	CString strSymbol;
	if(m_nType==kAngular)
	{
		strSymbol.LoadString(IDS_DEGREE_SYMBOL);
		m_Tol1.SetPrecision(1);
		m_Tol1Measure.SetWindowText(strSymbol);
		m_Tol2.SetPrecision(1);
		m_Tol2Measure.SetWindowText(strSymbol);
	}
	else
	{
		strSymbol.LoadString(m_nDSType==kDsEnglish?IDS_INCH_ABBR:IDS_MILLIMETER_ABBR);
		m_Tol1.SetPrecision(4);
		m_Tol1Measure.SetWindowText(strSymbol);
		m_Tol2.SetPrecision(4);
		m_Tol2Measure.SetWindowText(strSymbol);
	}

	m_bReactOnChanges = FALSE;
	OnSelchangeTolerance();
	m_bReactOnChanges = TRUE;
}

void CDimFormatDlg::OnOK() 
{
	UpdateValues();
	m_pDimFmts[0]=m_workFmts[0];
	m_pDimFmts[1]=m_workFmts[1];
	m_pDimFmts[2]=m_workFmts[2];

	CDialog::OnOK();
}

void CDimFormatDlg::OnSelchangeLeader() 
{
	if(m_Leader.GetCurSel()>0 && m_TextDisplay.GetCurSel()>0)
	{
		m_TextDisplay.SetCurSel(0);
	}
	OnChangeControls();
}

void CDimFormatDlg::OnSelchangeTextDisplay() 
{
	if(m_TextDisplay.GetCurSel()>0 && m_Leader.GetCurSel()>0)
	{
		m_Leader.SetCurSel(0);
	}
	OnChangeControls();
}

void CDimFormatDlg::OnWitnessLines() 
{
	OnChangeControls();
}

void CDimFormatDlg::OnArrowsInside() 
{
	OnChangeControls();
}

void CDimFormatDlg::OnTextCentered() 
{
	OnChangeControls();
}

void CDimFormatDlg::OnTextFramed() 
{
	OnChangeControls();
}

void CDimFormatDlg::OnChangeTol1() 
{
	OnChangeControls();
}

void CDimFormatDlg::OnChangeTol2() 
{
	OnChangeControls();
}

void CDimFormatDlg::OnSelchangeRdsymbol() 
{
	OnChangeControls();
}

void CDimFormatDlg::OnChangeControls()
{
	if (!m_bReactOnChanges)
		return;

	UpdateValues();

	DimFormat tmpFmt;
	tmpFmt.FillDefaults(m_nType,m_nDSType);

	BOOL bCmp = (tmpFmt.Compare(m_workFmts[m_nType]) == 0);
	m_Revert.SetCheck(bCmp);
	m_Revert.EnableWindow(!bCmp);
}
