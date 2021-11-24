// SelectSpecialDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "SelectSpecialDlg.h"
#include "SelectObjectInfoDlg.h"
#include "PCDraftDoc.h"
#include "CustomMenus.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectSpecialDlg dialog


CSelectSpecialDlg::CSelectSpecialDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectSpecialDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectSpecialDlg)
	m_nObjectClass = 0;
	m_nFillKind = 0;
	m_nPenKind = 0;
	m_bUseFill = FALSE;
	m_bUsePen = FALSE;
	m_bUseLineStyle = FALSE;
	m_bUseLineWeight = FALSE;
	m_nObjectType = -1;
	//}}AFX_DATA_INIT
	for(int i=0;i<5;i++)
	{
		m_bUse[i]=FALSE;
		m_sValues[i].Empty();
	}
	m_nFillValue=0;
	m_nPenValue=1;
	m_nLineWidthValue=1;
	m_nLineStyleValue=0;
}


void CSelectSpecialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectSpecialDlg)
	DDX_Control(pDX, IDC_PEN_PATTERN_PICKER, m_PenPatternPicker);
	DDX_Control(pDX, IDC_PEN_COLOR_PICKER, m_PenColorPicker);
	DDX_Control(pDX, IDC_FILL_PATTERN_PICKER, m_FillPatternPicker);
	DDX_Control(pDX, IDC_FILL_COLOR_PICKER, m_FillColorPicker);
	DDX_Control(pDX, IDC_LINE_WEIGHT_PICKER, m_LineWeightPicker);
	DDX_Control(pDX, IDC_LINE_WEIGHT, m_LineWeight);
	DDX_Control(pDX, IDC_LINE_STYLE_PICKER, m_LineStylePicker);
	DDX_Control(pDX, IDC_LINE_STYLE, m_LineStyle);
	DDX_Control(pDX, IDC_OBJECT_FILL, m_ObjectFill);
	DDX_Control(pDX, IDC_OBJECT_PEN, m_ObjectPen);
	DDX_Control(pDX, IDC_OBJECT_TYPE, m_ObjectType);
	DDX_CBIndex(pDX, IDC_OBJECT_CLASS, m_nObjectClass);
	DDX_Radio(pDX, IDC_FILL_COLOR, m_nFillKind);
	DDX_Radio(pDX, IDC_PEN_COLOR, m_nPenKind);
	DDX_Check(pDX, IDC_OBJECT_FILL, m_bUseFill);
	DDX_Check(pDX, IDC_OBJECT_PEN, m_bUsePen);
	DDX_Check(pDX, IDC_LINE_STYLE, m_bUseLineStyle);
	DDX_Check(pDX, IDC_LINE_WEIGHT, m_bUseLineWeight);
	DDX_CBIndex(pDX, IDC_OBJECT_TYPE, m_nObjectType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectSpecialDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectSpecialDlg)
	ON_CBN_SELCHANGE(IDC_OBJECT_CLASS, OnSelchangeObjectClass)
	ON_BN_CLICKED(IDC_OBJECT_PEN, OnObjectPen)
	ON_BN_CLICKED(IDC_OBJECT_FILL, OnObjectFill)
	ON_BN_CLICKED(IDC_OBJECT_INFO, OnObjectInfo)
	ON_BN_CLICKED(IDC_PEN_COLOR, OnPenColor)
	ON_BN_CLICKED(IDC_PEN_PATTERN, OnPenPattern)
	ON_BN_CLICKED(IDC_FILL_COLOR, OnFillColor)
	ON_BN_CLICKED(IDC_FILL_PATTERN, OnFillPattern)
	ON_BN_CLICKED(IDC_LINE_STYLE, OnLineStyle)
	ON_BN_CLICKED(IDC_LINE_WEIGHT, OnLineWeight)
	ON_BN_CLICKED(IDC_LINE_STYLE_PICKER, OnLineStylePicker)
	ON_BN_CLICKED(IDC_LINE_WEIGHT_PICKER, OnLineWeightPicker)
	ON_BN_CLICKED(IDC_FILL_COLOR_PICKER, OnFillColorPicker)
	ON_BN_CLICKED(IDC_FILL_PATTERN_PICKER, OnFillPatternPicker)
	ON_BN_CLICKED(IDC_PEN_COLOR_PICKER, OnPenColorPicker)
	ON_BN_CLICKED(IDC_PEN_PATTERN_PICKER, OnPenPatternPicker)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectSpecialDlg message handlers

void CSelectSpecialDlg::OnSelchangeObjectClass() 
{
	int nFirst,nLast;

	UpdateData();
	m_ObjectType.ResetContent();
	switch(m_nObjectClass)
	{
	case 2:		// general shape
		nFirst=IDS_GENERAL_SHAPE_FIRST;
		nLast=IDS_GENERAL_SHAPE_LAST;
		break;
	case 3:		// dimension shape
		nFirst=IDS_DIMENSION_TYPE_FIRST;
		nLast=IDS_DIMENSION_TYPE_LAST;
		break;
	default:
		m_ObjectType.ShowWindow(SW_HIDE);
		return;
	}

	CString str;
	for(int i=nFirst;i<=nLast;i++)
	{
		VERIFY(str.LoadString(i));
		m_ObjectType.AddString(str);
	}
	m_ObjectType.SetCurSel(0);
	m_ObjectType.ShowWindow(SW_SHOW);
}

void CSelectSpecialDlg::OnObjectPen() 
{
	bool state=m_ObjectPen.GetCheck()!=0;
	GetDlgItem(IDC_PEN_COLOR)->EnableWindow(state);
	GetDlgItem(IDC_PEN_PATTERN)->EnableWindow(state);
	GetDlgItem(IDC_PEN_COLOR_PICKER)->EnableWindow(state && m_nPenKind==0);
	GetDlgItem(IDC_PEN_PATTERN_PICKER)->EnableWindow(state && m_nPenKind==1);
}

void CSelectSpecialDlg::OnObjectFill() 
{
	bool state=m_ObjectFill.GetCheck()!=0;
	GetDlgItem(IDC_FILL_COLOR)->EnableWindow(state);
	GetDlgItem(IDC_FILL_PATTERN)->EnableWindow(state);
	GetDlgItem(IDC_FILL_COLOR_PICKER)->EnableWindow(state && m_nFillKind==0);
	GetDlgItem(IDC_FILL_PATTERN_PICKER)->EnableWindow(state && m_nFillKind==1);
}

void CSelectSpecialDlg::OnObjectInfo() 
{
	CPCDraftDoc *pDoc=CPCDraftDoc::GetActiveDocument();

	if(pDoc)
	{
		CSelectObjectInfoDlg theDlg;

		theDlg.m_sLabel1=pDoc->m_sFieldNames[0]+':';
		theDlg.m_sLabel2=pDoc->m_sFieldNames[1]+':';
		theDlg.m_sLabel3=pDoc->m_sFieldNames[2]+':';
		theDlg.m_sLabel4=pDoc->m_sFieldNames[3]+':';
		theDlg.m_sLabel5=pDoc->m_sFieldNames[4]+':';

		theDlg.m_bUse1=m_bUse[0];
		theDlg.m_bUse2=m_bUse[1];
		theDlg.m_bUse3=m_bUse[2];
		theDlg.m_bUse4=m_bUse[3];
		theDlg.m_bUse5=m_bUse[4];
	
		theDlg.m_sValue1=m_sValues[0];
		theDlg.m_sValue2=m_sValues[1];
		theDlg.m_sValue3=m_sValues[2];
		theDlg.m_sValue4=m_sValues[3];
		theDlg.m_sValue5=m_sValues[4];

		if(theDlg.DoModal()==IDOK)
		{
			m_bUse[0]=theDlg.m_bUse1;
			m_bUse[1]=theDlg.m_bUse2;
			m_bUse[2]=theDlg.m_bUse3;
			m_bUse[3]=theDlg.m_bUse4;
			m_bUse[4]=theDlg.m_bUse5;
		
			m_sValues[0]=theDlg.m_sValue1;
			m_sValues[1]=theDlg.m_sValue2;
			m_sValues[2]=theDlg.m_sValue3;
			m_sValues[3]=theDlg.m_sValue4;
			m_sValues[4]=theDlg.m_sValue5;
		};
	}
}

void CSelectSpecialDlg::OnPenColor() 
{
	UpdateData();
	m_PenColorPicker.EnableWindow();
	m_PenPatternPicker.EnableWindow(FALSE);
}

void CSelectSpecialDlg::OnPenPattern() 
{
	UpdateData();
	m_PenColorPicker.EnableWindow(FALSE);
	m_PenPatternPicker.EnableWindow();
}

void CSelectSpecialDlg::OnFillColor() 
{
	UpdateData();
	m_FillColorPicker.EnableWindow();
	m_FillPatternPicker.EnableWindow(FALSE);
}

void CSelectSpecialDlg::OnFillPattern() 
{
	UpdateData();
	m_FillColorPicker.EnableWindow(FALSE);
	m_FillPatternPicker.EnableWindow();
}

void CSelectSpecialDlg::OnLineStyle() 
{
	m_LineStylePicker.EnableWindow(m_LineStyle.GetCheck());
}

void CSelectSpecialDlg::OnLineWeight() 
{
	m_LineWeightPicker.EnableWindow(m_LineWeight.GetCheck());
}

void CSelectSpecialDlg::OnLineStylePicker() 
{
	CPCDraftDoc *pDoc=CPCDraftDoc::GetActiveDocument();

	if(pDoc)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CLineStyleMenu theMenu;
		theMenu.Create(pDoc->m_DashStyles,ID_STYLES_BASE);
		theMenu.CheckMenuItem(ID_STYLES_BASE+m_nLineStyleValue,MF_BYCOMMAND|MF_CHECKED);
		int cmd=theMenu.Track(this);
		if(cmd>=0)
			m_nLineStyleValue=cmd;
	}
}

static BYTE lineWeigths[]= {0x81,0xA1,0xC1,1,2,3,5,7,9,11,13};

void CSelectSpecialDlg::OnLineWeightPicker() 
{
	CPoint pt;
	GetCursorPos(&pt);

	CBitmapMenu theMenu(IDB_WIDTH_MENU,11);
	for(int i=0;i<11;i++)
	{
		if(m_nLineWidthValue==lineWeigths[i])
		{
			theMenu.CheckMenuItem(i,MF_BYPOSITION|MF_CHECKED);
			break;
		}
	}
	int cmd=theMenu.Track(this);
	if(cmd>=0)
		m_nLineWidthValue=lineWeigths[cmd];
}

void CSelectSpecialDlg::SelectColor(UINT nTitle,int& value)
{
	CPCDraftDoc *pDoc=CPCDraftDoc::GetActiveDocument();
	if(pDoc)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CColorMenu theMenu;
		theMenu.Create(&pDoc->m_Palette,nTitle);
		theMenu.CheckMenuItem(ID_COLORS_BASE+value+1,MF_BYCOMMAND|MF_CHECKED);
		int cmd=theMenu.Track(this);
		if(cmd>=0) value=cmd-1;
	}
}

void CSelectSpecialDlg::SelectPattern(UINT nTitle,int& value)
{
	CPCDraftDoc *pDoc=CPCDraftDoc::GetActiveDocument();
	if(pDoc)
	{
		CPoint pt;
		GetCursorPos(&pt);

		CPatternMenu theMenu;
      theMenu.Create(pDoc->m_packeditema.GetData(),pDoc->m_packeditema.GetCount(),nTitle,ID_PATTERNS_BASE);      // Gradient -  Infinisys Ltd
		theMenu.CheckMenuItem(ID_PATTERNS_BASE+value+1,MF_BYCOMMAND|MF_CHECKED);
		int cmd=theMenu.Track(this);
		if(cmd>=0) value=cmd-1;
	}
}

void CSelectSpecialDlg::OnFillColorPicker() 
{
	SelectColor(IDS_FILLCOLOR,m_nFillValue);
}

void CSelectSpecialDlg::OnFillPatternPicker() 
{
	SelectPattern(IDS_FILLPATTERN,m_nFillValue);
}

void CSelectSpecialDlg::OnPenColorPicker() 
{
	SelectColor(IDS_PENCOLOR,m_nPenValue);
}

void CSelectSpecialDlg::OnPenPatternPicker() 
{
	SelectPattern(IDS_PENPATTERN,m_nPenValue);
}

BOOL CSelectSpecialDlg::OnInitDialog() 
{
	CString strText;
	CComboBox* pType=(CComboBox*)GetDlgItem(IDC_OBJECT_CLASS);
	for(int i=IDS_TYPEFIRST;i<=IDS_TYPELAST;i++)
	{
		strText.LoadString(i);
		pType->AddString(strText);
	}
	CDialog::OnInitDialog();
	return TRUE;
}
