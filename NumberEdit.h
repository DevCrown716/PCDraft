#if !defined(AFX_NUMBEREDIT_H__96BE9733_BA10_11D2_B549_CFFE1499017F__INCLUDED_)
#define AFX_NUMBEREDIT_H__96BE9733_BA10_11D2_B549_CFFE1499017F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NumberEdit.h : header file
//

#include "wrfixed.h"

class CPCDraftDoc;

/////////////////////////////////////////////////////////////////////////////
// CNumberEdit window

class CNumberEdit : public CEdit
{
// Construction
public:
	CNumberEdit(int iValueType = 0); // PCDraft 7 Infinisys Ltd. Initial Value Type

// Attributes
public:
   bool     m_bOk;// PCDraft 7 Infinisys Ltd. Initial Value Type

private:
	bool m_bMode;
	int m_nValueType;
	CPCDraftDoc* m_pDoc;

	WrFixed	m_fValue;

	int m_nPrecision;
	bool m_bUseMinMax;
	double m_dMin,m_dMax;
	WrFixed m_fMin,m_fMax;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNumberEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetValue(int nValueType,WrFixed fValue);
	void GetValue(WrFixed& fValue);
	void SetPrecision(int nPrecision);
	void SetMinMax(double dMin,double dMax);
	void SetMinMax(WrFixed fMin,WrFixed fMax);
	void SetValue(double dValue);
	void GetValue(double& dValue);
	void SetValue(int nValue);
	void GetValue(int& nValue);
	void SetDocument(CPCDraftDoc* pDoc) { m_pDoc=pDoc; }

	// Generated message map functions
protected:
	void UpdateText();
	void Validate(double& dValue);
	void Validate(WrFixed& fValue);
	void Show(double dValue);
	//{{AFX_MSG(CNumberEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillfocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NUMBEREDIT_H__96BE9733_BA10_11D2_B549_CFFE1499017F__INCLUDED_)
