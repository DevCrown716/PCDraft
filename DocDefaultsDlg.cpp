// DocDefaultsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "DocDefaultsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocDefaultsDlg dialog


CDocDefaultsDlg::CDocDefaultsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDocDefaultsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDocDefaultsDlg)
	m_bDimEndMarks = FALSE;
	m_bDimFormats = FALSE;
	m_bDimStandards = FALSE;
	m_bDrawAndPageSetup = FALSE;
	m_bFieldNames = FALSE;
	m_bFillPatternsColors = FALSE;
	m_bHideGridLines = FALSE;
	m_bHidePageBreaks = FALSE;
	m_bLineSettings = FALSE;
	m_bOpenAtCenter = FALSE;
	m_bScaleAndLayerSetup = FALSE;
	m_bShowRulers = FALSE;
	m_bSnapToObject = FALSE;
	m_bTextSettings = FALSE;
	//}}AFX_DATA_INIT
	m_bEnableOK=FALSE;
}


void CDocDefaultsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDocDefaultsDlg)
	DDX_Check(pDX, IDC_DIM_ENDMARKS, m_bDimEndMarks);
	DDX_Check(pDX, IDC_DIM_FORMATS, m_bDimFormats);
	DDX_Check(pDX, IDC_DIM_STANDARDS, m_bDimStandards);
	DDX_Check(pDX, IDC_DRAW_AND_PAGE_SETUP, m_bDrawAndPageSetup);
	DDX_Check(pDX, IDC_FIELD_NAMES, m_bFieldNames);
	DDX_Check(pDX, IDC_FILL_PATTERNS_COLORS, m_bFillPatternsColors);
	DDX_Check(pDX, IDC_HIDE_GRID_LINES, m_bHideGridLines);
	DDX_Check(pDX, IDC_HIDE_PAGE_BREAKS, m_bHidePageBreaks);
	DDX_Check(pDX, IDC_LINE_SETTINGS, m_bLineSettings);
	DDX_Check(pDX, IDC_OPEN_AT_CENTER, m_bOpenAtCenter);
	DDX_Check(pDX, IDC_SCALE_AND_LAYER_SETUP, m_bScaleAndLayerSetup);
	DDX_Check(pDX, IDC_SHOW_RULERS, m_bShowRulers);
	DDX_Check(pDX, IDC_SNAP_TO_OBJECT, m_bSnapToObject);
	DDX_Check(pDX, IDC_TEXT_SETTINGS, m_bTextSettings);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDocDefaultsDlg, CDialog)
	//{{AFX_MSG_MAP(CDocDefaultsDlg)
	ON_BN_CLICKED(IDC_APP_DEFAULTS, OnAppDefaults)
	ON_BN_CLICKED(IDC_SCALE_AND_LAYER_SETUP, OnSaveSettingsChanged)
	ON_BN_CLICKED(IDC_DIM_STANDARDS, OnSaveSettingsChanged)
	ON_BN_CLICKED(IDC_DIM_FORMATS, OnSaveSettingsChanged)
	ON_BN_CLICKED(IDC_DIM_ENDMARKS, OnSaveSettingsChanged)
	ON_BN_CLICKED(IDC_DRAW_AND_PAGE_SETUP, OnSaveSettingsChanged)
	ON_BN_CLICKED(IDC_FILL_PATTERNS_COLORS, OnSaveSettingsChanged)
	ON_BN_CLICKED(IDC_LINE_SETTINGS, OnSaveSettingsChanged)
	ON_BN_CLICKED(IDC_TEXT_SETTINGS, OnSaveSettingsChanged)
	ON_BN_CLICKED(IDC_SNAP_TO_OBJECT, OnSaveSettingsChanged)
	ON_BN_CLICKED(IDC_FIELD_NAMES, OnSaveSettingsChanged)
	ON_BN_CLICKED(IDC_OPEN_AT_CENTER, OnSettingsChanged)
	ON_BN_CLICKED(IDC_HIDE_PAGE_BREAKS, OnSettingsChanged)
	ON_BN_CLICKED(IDC_HIDE_GRID_LINES, OnSettingsChanged)
	ON_BN_CLICKED(IDC_SHOW_RULERS, OnSettingsChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDocDefaultsDlg message handlers

void CDocDefaultsDlg::OnAppDefaults() 
{
	CPCDraftApp *pApp=(CPCDraftApp*)AfxGetApp();
	m_bEnableOK|=pApp->DoAppDefaults()?TRUE:FALSE;
	EnableOK();
}

void CDocDefaultsDlg::OnSaveSettingsChanged() 
{
	UpdateData();
	EnableOK();
}

void CDocDefaultsDlg::OnSettingsChanged() 
{
	m_bEnableOK=TRUE;
	GetDlgItem(IDOK)->EnableWindow();
}

void CDocDefaultsDlg::EnableOK()
{
	GetDlgItem(IDOK)->EnableWindow(m_bEnableOK || m_bDimEndMarks || m_bDimFormats || m_bDimStandards || m_bDrawAndPageSetup || m_bFieldNames || m_bFillPatternsColors || m_bLineSettings || m_bScaleAndLayerSetup || m_bSnapToObject || m_bTextSettings);
}
