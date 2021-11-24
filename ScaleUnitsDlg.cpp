// ScaleUnitsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PCDraft.h"
#include "ScaleUnitsDlg.h"
#include "PCDraftDoc.h"
#include "Layer.h"
#include "DimStandardDlg.h"



      // Free Scale -  Infinisys Ltd
int gcd(int n,int d)
{
   int r; //remainder


   while((n%d != 0) || (d <= 0))


   {
      r=n%d;
      n=d;
      d=r;
   }
   if(n%d == 0)
   {
      return d;
   }
   else
      return FALSE;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScaleUnitsDlg dialog

CScaleUnitsDlg::CScaleUnitsDlg(CPCDraftDoc* pDoc,bool bAutoScale,CWnd* pParent /*=NULL*/)
	: CDialog(CScaleUnitsDlg::IDD, pParent), m_pDoc(pDoc),m_bAutoScale(bAutoScale)
{
	//{{AFX_DATA_INIT(CScaleUnitsDlg)
	m_nAnglePlaces = pDoc->m_nAnglePlaces;
	m_nPlaces = pDoc->m_nPlaces;
	m_bHideMetricUnits = pDoc->m_bHideMetricUnits;
	m_nUserDefined = 0;
	//}}AFX_DATA_INIT
	m_nUnit=pDoc->m_nUnit;
	m_nAngleUnit=pDoc->GetAngleUnit();
	if(m_bAutoScale) m_nScaleIndex=m_pDoc->m_bEnglishUnits?26:23; // 'Automatic' scale index
	else m_nScaleIndex=m_pDoc->m_nDefScale;
   m_pDoc->GetScaleRatio(m_pDoc->m_bEnglishUnits,m_nScaleIndex,m_longratioCustom);       // Free Scale -  Infinisys Ltd
   m_nUserDefinedDenominator = m_longratioCustom.fD;       // Free Scale -  Infinisys Ltd
   m_nUserDefinedNumerator = m_longratioCustom.fN;       // Free Scale -  Infinisys Ltd
   m_DimStd=pDoc->m_DimStd;
   m_bCustomScale = false;
}


void CScaleUnitsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScaleUnitsDlg)
	DDX_Control(pDX, IDC_DIMENSION_STATUS, m_DimStatus);
	DDX_Control(pDX, IDC_UNITS_PLACES, m_UnitsPlaces);
	DDX_Control(pDX, IDC_DEFAULT_SCALE, m_DefaultScale);
	DDX_Control(pDX, IDC_UNITS_DISPLAY, m_UnitsDisplay);
   DDX_Control(pDX,IDC_USER_DEFINED_NUMERATOR,m_editNumerator);       // Free Scale -  Infinisys Ltd
   DDX_Control(pDX,IDC_USER_DEFINED_DENOMINATOR,m_editDenominator);       // Free Scale -  Infinisys Ltd
   if(!m_bAutoScale || (m_nScaleIndex != (m_pDoc->m_bEnglishUnits?26:23)))
   {
      DDV_MinMaxUInt(pDX,IDC_USER_DEFINED_NUMERATOR,1,65535);       // Free Scale -  Infinisys Ltd
      DDV_MinMaxUInt(pDX,IDC_USER_DEFINED_DENOMINATOR,1,65535);       // Free Scale -  Infinisys Ltd
      DDX_Text(pDX,IDC_USER_DEFINED_NUMERATOR,m_nUserDefinedNumerator);       // Free Scale -  Infinisys Ltd
      DDX_Text(pDX,IDC_USER_DEFINED_DENOMINATOR,m_nUserDefinedDenominator);       // Free Scale -  Infinisys Ltd
   }
	DDX_CBIndex(pDX, IDC_ANGLE_PLACES, m_nAnglePlaces);
	DDX_CBIndex(pDX, IDC_UNITS_PLACES, m_nPlaces);
	DDX_Check(pDX, IDC_HIDE_METRIC_UNITS, m_bHideMetricUnits);
	DDX_Text(pDX, IDC_USER_DEFINED_VALUE, m_nUserDefined);
	//}}AFX_DATA_MAP
	if(m_nAngleUnit<0) DDV_MinMaxInt(pDX, m_nUserDefined, 1, 30000);
}

BEGIN_MESSAGE_MAP(CScaleUnitsDlg, CDialog)
	//{{AFX_MSG_MAP(CScaleUnitsDlg)
	ON_BN_CLICKED(IDC_ANGLE_DECIMAL_DEGREES, OnAngleDecimalDegrees)
	ON_BN_CLICKED(IDC_ANGLE_DEGREES_MIN, OnAngleDegreesMin)
	ON_BN_CLICKED(IDC_ANGLE_DEGREES_MIN_SEC, OnAngleDegreesMinSec)
	ON_BN_CLICKED(IDC_ANGLE_USER_DEFINED, OnAngleUserDefined)
	ON_BN_CLICKED(IDC_UNIT_ENGLISH, OnUnitEnglish)
	ON_BN_CLICKED(IDC_UNIT_METRIC, OnUnitMetric)
   ON_BN_CLICKED(IDB_APPLY,OnUserDefinedApply)       // Free Scale -  Infinisys Ltd
	ON_CBN_SELCHANGE(IDC_UNITS_DISPLAY, OnSelchangeUnitsDisplay)
	ON_BN_CLICKED(IDC_CUSTOM, OnCustom)
	ON_CBN_SELCHANGE(IDC_DEFAULT_SCALE, OnSelchangeDefaultScale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScaleUnitsDlg message handlers
#ifndef _PERSONAL_EDITION

void CScaleUnitsDlg::UpdateDimStatus()
{
	DimStandard std;
	std.FillDefaults((m_nUnit&kUEnglishMask)?kDsEnglish:kDsMetric);
	CString sText;
	sText.LoadString(m_DimStd.Compare(std)?IDS_CUSTOM:IDS_STANDARD);
	m_DimStatus.SetWindowText(sText);
}

#endif

void CScaleUnitsDlg::SetupUnit(int nUnit)
{

   CString strNum;
   CString strDen;

   // Free Scale -  Infinisys Ltd
   if(!m_bAutoScale || (m_nScaleIndex != (m_pDoc->m_bEnglishUnits?26:23)))
   {

      m_editNumerator.GetWindowText(strNum);
      m_longratioCustom.fN = max(1,atoi(strNum));
      m_editDenominator.GetWindowText(strDen);
      m_longratioCustom.fD = max(1,atoi(strDen));

   }
   else
   {

      m_longratioCustom.fN = 65535;
      m_longratioCustom.fD = 65535;

   }

   if(m_longratioCustom.fN != 0xffff || m_longratioCustom.fD != 0xffff)
   {

      int iDiv = m_longratioCustom.fN / m_longratioCustom.fD;

      int iRem = m_longratioCustom.fN % m_longratioCustom.fD;

      int iGcd = gcd(m_longratioCustom.fN,m_longratioCustom.fD);

      m_longratioCustom.fN = m_longratioCustom.fN / iGcd;

      m_longratioCustom.fD = m_longratioCustom.fD / iGcd;

      strNum.Format("%d",m_longratioCustom.fN);

      strDen.Format("%d",m_longratioCustom.fD);

      if(m_pDoc->FindScale(m_nScaleIndex,m_nUnit,m_longratioCustom))
      {

         //m_DefaultScale.SetCurSel(m_nScaleIndex - 1);

         m_bCustomScale = false;

      }
      else
      {

         m_bCustomScale = true;

      }

   }
   else
   {

      m_bCustomScale = false;

      strNum = "65535";


      strDen = "65535";

   }


	UINT nUnitCtrl;
	UINT nHideMetricsUnits;
	UINT nUnitsDisplayFirst,nUnitsDisplayLast;
	UINT nDefaultScaleFirst,nDefaultScaleLast;
	CString theValue;
	UINT i,nDimensionStandart;
	if(nUnit & kUEnglishMask)
	{
		nHideMetricsUnits=SW_HIDE;
		nUnitsDisplayFirst=IDS_ENGLISH_DISPLAY_FIRST;
		nUnitsDisplayLast=IDS_ENGLISH_DISPLAY_LAST;
		nDefaultScaleFirst=IDS_ENGLISH_SCALE_FIRST;
		nDefaultScaleLast=IDS_ENGLISH_SCALE_LAST;
		nDimensionStandart=IDS_ANSI_UNITS;
		nUnitCtrl=IDC_UNIT_ENGLISH;
	}
	else
	{
		nHideMetricsUnits=SW_SHOW;
		nUnitsDisplayFirst=IDS_METRIC_DISPLAY_FIRST;
		nUnitsDisplayLast=IDS_METRIC_DISPLAY_LAST;
		nDefaultScaleFirst=IDS_METRIC_SCALE_FIRST;
		nDefaultScaleLast=IDS_METRIC_SCALE_LAST;
		nDimensionStandart=IDS_METRIC_UNITS;
		nUnitCtrl=IDC_UNIT_METRIC;
	}
//	CheckDlgButton(nUnitCtrl,TRUE);
//	GetDlgItem(nUnitCtrl)->SetFocus();
	GetDlgItem(IDC_HIDE_METRIC_UNITS)->ShowWindow(nHideMetricsUnits);
#ifndef _PERSONAL_EDITION
	VERIFY(theValue.LoadString(nDimensionStandart));
	GetDlgItem(IDC_DIMENSION_STANDARD)->SetWindowText(theValue);
#endif

	m_UnitsDisplay.ResetContent();
	for(i=nUnitsDisplayFirst;i<=nUnitsDisplayLast;i++)
	{
		VERIFY(theValue.LoadString(i));
		m_UnitsDisplay.AddString(theValue);
	}
	m_UnitsDisplay.SetCurSel(nUnit & 0xFF);
	m_DefaultScale.ResetContent();
	for(i=nDefaultScaleFirst;i<=nDefaultScaleLast;i++)
	{
		VERIFY(theValue.LoadString(i));
		m_DefaultScale.AddString(theValue);
	}
	      // Free Scale -  Infinisys Ltd
   int iCustomScaleSel = -1;
   if(m_bCustomScale)
   {
      VERIFY(theValue.LoadString(IDS_CUSTOM_SCALE));
      theValue = strNum + " : " + strDen + " " + theValue;
      iCustomScaleSel = m_DefaultScale.AddString(theValue);
   }
	if(m_bAutoScale)
	{
		theValue.LoadString(IDS_AUTOMATIC);
		m_DefaultScale.AddString(theValue);
	}
   if(m_bCustomScale)
   {
      m_nScaleIndex = -1;
      m_DefaultScale.SetCurSel(iCustomScaleSel);
   }
   else
   {
      m_pDoc->MatchScale(m_nScaleIndex,nUnit,m_nUnit);
      m_DefaultScale.SetCurSel(m_nScaleIndex - 1);
   }

	UINT nCmdShow=(nUnit & kUEnglishFracMask)?SW_HIDE:SW_SHOW;
	GetDlgItem(IDC_UNITS_PLACES_STATIC)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_UNITS_PLACES)->ShowWindow(nCmdShow);
	m_nUnit=nUnit;
#ifndef _PERSONAL_EDITION
	UpdateDimStatus();
#endif
   UpdateUserDefinedEdit();

}

void CScaleUnitsDlg::SetupAngleUnit()
{
	if(m_nAngleUnit<0)
	{
//		CheckDlgButton(IDC_ANGLE_USER_DEFINED,TRUE);
		CString sText;
		sText.Format("%d",Abs(m_nAngleUnit));
		GetDlgItem(IDC_USER_DEFINED_VALUE)->SetWindowText(sText);
		GetDlgItem(IDC_USER_DEFINED_VALUE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_USER_DEFINED_STATIC)->ShowWindow(SW_SHOW);
	}
	else
	{
//		CheckDlgButton(IDC_ANGLE_DECIMAL_DEGREES+m_nAngleUnit,TRUE);
		GetDlgItem(IDC_USER_DEFINED_VALUE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_USER_DEFINED_STATIC)->ShowWindow(SW_HIDE);
	}
	UINT nCmdShow=(m_nAngleUnit<=kDecimalDegrees)?SW_SHOW:SW_HIDE;
	GetDlgItem(IDC_ANGLE_PLACES_STATIC)->ShowWindow(nCmdShow);
	GetDlgItem(IDC_ANGLE_PLACES)->ShowWindow(nCmdShow);
}

void CScaleUnitsDlg::OnAngleDecimalDegrees() 
{
	if(m_nAngleUnit!=kDecimalDegrees)
	{
		m_nAngleUnit=kDecimalDegrees;
		SetupAngleUnit();
	}
}

void CScaleUnitsDlg::OnAngleDegreesMin() 
{
	if(m_nAngleUnit!=kDegreesMinutes)
	{
		m_nAngleUnit=kDegreesMinutes;
		SetupAngleUnit();
	}
}

void CScaleUnitsDlg::OnAngleDegreesMinSec() 
{
	if(m_nAngleUnit!=kDegMinSeconds)
	{
		m_nAngleUnit=kDegMinSeconds;
		SetupAngleUnit();
	}
}

void CScaleUnitsDlg::OnAngleUserDefined() 
{
	if(m_nAngleUnit>=0)
	{
		m_nAngleUnit=-16;
		SetupAngleUnit();
	}
}

void CScaleUnitsDlg::OnUnitEnglish() 
{
	if(!(m_nUnit & kUEnglishMask))
	{
		m_DimStd.FillDefaults(kDsEnglish);
		SetupUnit(kUFracFeetInches);
	}
}

void CScaleUnitsDlg::OnUnitMetric() 
{
	if(m_nUnit & kUEnglishMask)
	{
		m_DimStd.FillDefaults(kDsMetric);
		SetupUnit(kUCentimeters);
	}
}

void CScaleUnitsDlg::OnSelchangeUnitsDisplay() 
{
	int nUnit=m_UnitsDisplay.GetCurSel();
	if(m_nUnit & kUEnglishMask)
	{
		if(nUnit>=3)
		{
			nUnit+=kUEnglishFracMask;
		}
		else
		{
			nUnit+=kUEnglishDecMask;
		}
	}
	SetupUnit(nUnit);
}

BOOL CScaleUnitsDlg::OnInitDialog() 
{
	CString strText;
	CComboBox* pUnits=(CComboBox*)GetDlgItem(IDC_UNITS_PLACES);
	CComboBox* pAngle=(CComboBox*)GetDlgItem(IDC_ANGLE_PLACES);
    int i = IDS_PLACESFIRST;
	for(;i<IDS_PLACESLAST;i++)
	{
		strText.LoadString(i);
		pUnits->AddString(strText);
		pAngle->AddString(strText);
	}
	strText.LoadString(i);
	pUnits->AddString(strText);

	CDialog::OnInitDialog();

	SetupUnit(m_nUnit);

	CheckDlgButton((m_nUnit & kUEnglishMask)?IDC_UNIT_ENGLISH:IDC_UNIT_METRIC,TRUE);
	SetupAngleUnit();
	CheckDlgButton((m_nAngleUnit<0)?IDC_ANGLE_USER_DEFINED:(IDC_ANGLE_DECIMAL_DEGREES+m_nAngleUnit),TRUE);


   UpdateUserDefinedEdit();

	return FALSE;	// Manual focus setup
}

void CScaleUnitsDlg::OnCustom() 
{
	CDimStandardDlg theDlg(&m_DimStd);
#ifndef _PERSONAL_EDITION
	if(theDlg.DoModal()==IDOK) UpdateDimStatus();
#endif
}

void CScaleUnitsDlg::OnOK() 
{
	if(!UpdateData()) return;
	if(m_nAngleUnit<0) m_nAngleUnit=-m_nUserDefined;
	if(m_pDoc->SetupUnits(m_nUnit,m_bHideMetricUnits?true:false,m_nAngleUnit,m_nPlaces,m_nAnglePlaces,m_nScaleIndex,&m_DimStd,IDS_UNITS_NOT_CHANGED, &m_longratioCustom))
	{
		m_pDoc->CheckDocSize(true);
		EndDialog(IDOK);
	}
	else EndDialog(IDCANCEL);
}

void CScaleUnitsDlg::OnUserDefinedApply() //      // Free Scale -  Infinisys Ltd
{

   CString strNum;
   CString strDen;

   m_editNumerator.GetWindowText(strNum);
   m_editDenominator.GetWindowText(strDen);

   if(atoi(strNum) != 0 || atoi(strDen) != 0)
   {
      
      m_nScaleIndex = -1;

   }



   SetupUnit(m_nUnit);



}


void CScaleUnitsDlg::OnSelchangeDefaultScale() 
{
	m_nScaleIndex=m_DefaultScale.GetCurSel()+1;
   UpdateUserDefinedEdit();       // Free Scale -  Infinisys Ltd

}

      // Free Scale -  Infinisys Ltd
void CScaleUnitsDlg::UpdateUserDefinedEdit()
{
   LongRatio longRatio;
   if(m_nScaleIndex == -1)
   {
      longRatio = m_longratioCustom;
   }
   else
   {
      m_pDoc->GetScaleRatio((m_nUnit&kUEnglishMask),m_nScaleIndex,longRatio);
   }
   CString strNum;
   strNum.Format("%d",longRatio.fN);
   if(m_bAutoScale && (m_nScaleIndex == ( m_pDoc->m_bEnglishUnits?26:23)))
   {
      m_editNumerator.SetWindowText("");
   }
   else
   {
      m_editNumerator.SetWindowText(strNum);
   }
   CString strDen;
   strDen.Format("%d",longRatio.fD);
   if(m_bAutoScale && (m_nScaleIndex == (m_pDoc->m_bEnglishUnits?26:23)))
   {
      m_editDenominator.SetWindowText("");
   }
   else
   {
      m_editDenominator.SetWindowText(strDen);
   }


}



