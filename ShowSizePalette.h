#if !defined(AFX_SHOWSIZEPALETTE_H__A551B697_C4E3_11D2_A052_00A0C9A0C319__INCLUDED_)
#define AFX_SHOWSIZEPALETTE_H__A551B697_C4E3_11D2_A052_00A0C9A0C319__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShowSizePalette.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShowSizePalette window

class CShowSizePalette : public CDialogBar
{
// Construction
public:
	CShowSizePalette();

// Dialog Data
	//{{AFX_DATA(CShowSizePalette)
	enum { IDD = IDD_SHOWSIZE_PALETTE };
	CStatic	m_s4;
	CStatic	m_s3;
	CStatic	m_s2;
	CStatic	m_s1;
	//}}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShowSizePalette)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(CWnd * pParentWnd);
	virtual ~CShowSizePalette();

	// Generated message map functions
protected:
	//{{AFX_MSG(CShowSizePalette)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHOWSIZEPALETTE_H__A551B697_C4E3_11D2_A052_00A0C9A0C319__INCLUDED_)
