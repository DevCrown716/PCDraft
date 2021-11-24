// ApplicationDefaultsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PCDraft.h"
#include "ApplicationDefaultsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CApplicationDefaultsDlg dialog


CApplicationDefaultsDlg::CApplicationDefaultsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CApplicationDefaultsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CApplicationDefaultsDlg)
	m_bCrossCursor = FALSE;
	m_nWindowAtStartUp = -1;
	m_bHideLineStyle = FALSE;
	m_bShowCursorPosition = FALSE;
	m_bToolDisplay = FALSE;
	m_bToolSaveLocation = FALSE;
	m_bZoomLineWeights = FALSE;
	m_bLinkedDimensions = FALSE;
	m_bRemoveLinks = FALSE;
	//}}AFX_DATA_INIT
}


void CApplicationDefaultsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CApplicationDefaultsDlg)
	DDX_Check(pDX, IDC_CROSS_CURSOR, m_bCrossCursor);
	DDX_Radio(pDX, IDC_WINDOW_AT_START_UP, m_nWindowAtStartUp);
	DDX_Check(pDX, IDC_HIDE_LINE_STYLE, m_bHideLineStyle);
	DDX_Check(pDX, IDC_SHOW_CURSOR_POSITION, m_bShowCursorPosition);
	DDX_Check(pDX, IDC_TOOL_DISPLAY, m_bToolDisplay);
	DDX_Check(pDX, IDC_TOOL_SAVE_LOCATION, m_bToolSaveLocation);
	DDX_Check(pDX, IDC_ZOOM_LINE_WEIGHTS, m_bZoomLineWeights);
	DDX_Check(pDX, IDC_LINKED_DIMENSIONS, m_bLinkedDimensions);
	DDX_Check(pDX, IDC_REMOVE_LINKS, m_bRemoveLinks);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CApplicationDefaultsDlg, CDialog)
	//{{AFX_MSG_MAP(CApplicationDefaultsDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CApplicationDefaultsDlg message handlers
