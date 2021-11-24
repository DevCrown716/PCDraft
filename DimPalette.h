#if !defined(AFX_DIMPALETTE_H__EB77B1B9_6058_11D2_B490_FC73BE557065__INCLUDED_)
#define AFX_DIMPALETTE_H__EB77B1B9_6058_11D2_B490_FC73BE557065__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DimPalette.h : header file
//

#include "EndmarksWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CDimPalette window

class CDimPalette : public CToolBar
{
// Construction
public:
	CDimPalette();

// Attributes
public:
	bool m_bShowExtra;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDimPalette)
	//}}AFX_VIRTUAL

// Implementation
public:
	void RemoveExtra();
	void AddExtra();
	CFont m_font;
#ifndef _PERSONAL_EDITION
	CComboBox m_cbType;
//	CStatic m_stEndmarks;
	CEndmarksWnd m_wndEndmarks;
	CButton m_btnFormat;
#endif

	BOOL Create(CWnd * pParentWnd);
	virtual ~CDimPalette();

	// Generated message map functions
protected:
	void InvertResizeOpt();
	afx_msg LRESULT OnMaximizeClick(WPARAM wParam,LPARAM lParam);
	//{{AFX_MSG(CDimPalette)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIMPALETTE_H__EB77B1B9_6058_11D2_B490_FC73BE557065__INCLUDED_)
