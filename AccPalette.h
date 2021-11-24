#if !defined(AFX_ACCPALETTE_H__EB77B1B8_6058_11D2_B490_FC73BE557065__INCLUDED_)
#define AFX_ACCPALETTE_H__EB77B1B8_6058_11D2_B490_FC73BE557065__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AccPalette.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAccPalette window

class CAccPalette : public CToolBar
{
// Construction
public:
	CAccPalette();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAccPalette)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(CWnd * pParentWnd);
	virtual ~CAccPalette();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAccPalette)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCPALETTE_H__EB77B1B8_6058_11D2_B490_FC73BE557065__INCLUDED_)
