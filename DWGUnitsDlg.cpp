// DWGUnitsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "pcdraftdoc.h"
#include "DWGUnitsDlg.h"
#include <float.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDWGUnitsDlg dialog


CDWGUnitsDlg::CDWGUnitsDlg(CPCDraftDoc* pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CDWGUnitsDlg::IDD, pParent), m_pDoc(pDoc)
{
	if(m_pDoc->m_bEnglishUnits){
		m_Units = 12.0;
		m_nMetric = 0;
	}else{
		m_Units = 1000.0;
		m_nMetric = 1;
	}

	//{{AFX_DATA_INIT(CDWGUnitsDlg)

	//}}AFX_DATA_INIT
}


void CDWGUnitsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDWGUnitsDlg)
	DDX_Text(pDX, IDC_ED_DWG_UNITS, m_Units);
	DDX_Radio(pDX, IDC_DWG_FEET, m_nMetric);
	//}}AFX_DATA_MAP
	if(pDX->m_bSaveAndValidate){
		if(m_Units<=0){
			CString prompt;
			prompt.LoadString(ID_DWGUNITS_GREATER_THAN_ZERO);
			AfxMessageBox(prompt, MB_ICONEXCLAMATION);
			prompt.Empty();
			pDX->Fail();
		}
	}

}


BEGIN_MESSAGE_MAP(CDWGUnitsDlg, CDialog)
	//{{AFX_MSG_MAP(CDWGUnitsDlg)
	ON_BN_CLICKED(IDC_DWG_FEET, OnDwgFeet)
	ON_BN_CLICKED(IDC_DWG_METRE, OnDwgMetre)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDWGUnitsDlg message handlers


static BOOL AFXAPI _AfxSimpleFloatParse(LPCTSTR lpszText, double& d)
{
	ASSERT(lpszText != NULL);
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	TCHAR chFirst = lpszText[0];
	d = _tcstod(lpszText, (LPTSTR*)&lpszText);
	if (d == 0.0 && chFirst != '0')
		return FALSE;   // could not convert
	while (*lpszText == ' ' || *lpszText == '\t')
		lpszText++;

	if (*lpszText != '\0')
		return FALSE;   // not terminated properly

	return TRUE;
}





void CDWGUnitsDlg::OnDwgFeet() 
{
	CString units;
	GetDlgItemText(IDC_ED_DWG_UNITS, units);
	units.TrimLeft();
	units.TrimRight();
	double val=0.;
	_AfxSimpleFloatParse(units, val);
	if(val==1000.0){
		TCHAR szBuffer[32];
		_stprintf(szBuffer, _T("%.*g"), DBL_DIG, 12.0);
		SetDlgItemText(IDC_ED_DWG_UNITS, szBuffer);
	}
	
}

void CDWGUnitsDlg::OnDwgMetre() 
{
	CString units;
	GetDlgItemText(IDC_ED_DWG_UNITS, units);
	units.TrimLeft();
	units.TrimRight();
	double val;
	_AfxSimpleFloatParse(units, val);
	if(val==12.0){
		TCHAR szBuffer[32];
		_stprintf(szBuffer, _T("%.*g"), DBL_DIG, 1000.0);
		SetDlgItemText(IDC_ED_DWG_UNITS, szBuffer);
	}
}

void CDWGUnitsDlg::OnOK() 
{
	if(!UpdateData(TRUE)){
		return;
	}
	EndDialog(IDOK);
}
