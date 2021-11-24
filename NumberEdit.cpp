// NumberEdit.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "NumberEdit.h"

#include "PCDraftDoc.h"
#include "Layer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNumberEdit

CNumberEdit::CNumberEdit(int iValueType)
{
   m_bOk = false;
   m_nValueType = iValueType;
	m_nPrecision=2;
	m_bUseMinMax=false;
	m_dMin=0;
	m_dMax=0;
}

BEGIN_MESSAGE_MAP(CNumberEdit, CEdit)
	//{{AFX_MSG_MAP(CNumberEdit)
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNumberEdit message handlers

void CNumberEdit::SetValue(double dValue)
{
	m_nValueType=kDoubleValue;
	Validate(dValue);
	Show(dValue);
}

void CNumberEdit::SetValue(int nValue)
{
	m_nValueType=kDoubleValue;
	m_nPrecision=0;
	double dValue=(double)nValue;
	Validate(dValue);
	Show(dValue);
}

void CNumberEdit::GetValue(double& dValue)
{
	CString str;
	GetWindowText(str);
	dValue=atof(str);
}

void CNumberEdit::GetValue(int& nValue)
{
	CString str;
	GetWindowText(str);
	nValue=atol(str);
}

void CNumberEdit::SetMinMax(double dMin, double dMax)
{
	m_dMin=dMin;
	m_dMax=dMax;
	m_bUseMinMax=true;
}

void CNumberEdit::SetMinMax(WrFixed fMin, WrFixed fMax)
{
	m_fMin=fMin;
	m_fMax=fMax;
	m_bUseMinMax=true;
}

void CNumberEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar>127 || CString("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz").Find(nChar)>=0) return;
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CNumberEdit::Show(double dValue)
{
	CString str;
	str.Format("%.*f",m_nPrecision,dValue);
	DWORD dwCurSel=GetSel();
	AfxSetWindowText(GetSafeHwnd(),str);
	SetSel(dwCurSel);
}

void CNumberEdit::Validate(double &dValue)
{
	if(m_bUseMinMax)
	{
		if(dValue<m_dMin) dValue=m_dMin;
		else if(dValue>m_dMax) dValue=m_dMax;
	}
}

void CNumberEdit::Validate(WrFixed& fValue)
{
	if(m_bUseMinMax)
	{
		if(fValue<m_fMin) fValue=m_fMin;
		else if(fValue>m_fMax) fValue=m_fMax;
	}
}

void CNumberEdit::SetPrecision(int nPrecision)
{
	m_nPrecision=nPrecision;
}

void CNumberEdit::OnKillfocus() 
{
	CString sValue;
	GetWindowText(sValue);

	switch(m_nValueType)
	{
	case kDoubleValue:
		{
			double dValue;
			GetValue(dValue);
			Validate(dValue);
			Show(dValue);
		}
		break;
	case kUnitlessValue:
		{
			double dValue;
			if(m_pDoc->GetDimUnit()->numStr2User(sValue,dValue))
			{
				LongRatio ratio;
				m_pDoc->GetActiveScaleRatio(ratio);
				m_fValue=m_pDoc->GetDimUnit()->numUser2DB(ratio,dValue);
				Validate(m_fValue);
			}
			UpdateText();
		}
		break;
	case kAngleValue:
		{
			Angle value;
			if(m_pDoc->GetDimUnit()->numStr2Angle(sValue,value))
			{
				m_fValue=value;
				Validate(m_fValue);
			}
			UpdateText();
		}
		break;
	}
}

void CNumberEdit::SetValue(int nValueType, WrFixed fValue)
{
	m_nValueType=nValueType;
	m_fValue=fValue;
	UpdateText();
}

void CNumberEdit::GetValue(WrFixed& fValue)
{
	fValue=m_fValue;
}

void CNumberEdit::UpdateText()
{
	CString sValue;
	switch(m_nValueType)
	{
	case kUnitlessValue:
		{
			LongRatio ratio;
			m_pDoc->GetActiveScaleRatio(ratio);
			sValue=m_pDoc->GetDimUnit()->numUser2Str(m_pDoc->GetDimUnit()->numDB2User(ratio,m_fValue),false);
		}
		break;
	case kAngleValue:
		sValue=m_pDoc->GetDimUnit()->numAngle2Str(m_fValue);
		break;
	}
	AfxSetWindowText(GetSafeHwnd(),sValue);
   GetParent()->PostMessageA(WM_APP + 123, GetDlgCtrlID());
}

BOOL CNumberEdit::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && pMsg->wParam==VK_RETURN)
		OnKillfocus();
	return CEdit::PreTranslateMessage(pMsg);
}
