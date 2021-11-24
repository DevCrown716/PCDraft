#if !defined(AFX_ALIGNPALETTE_H__5DD206CB_B4AA_47E7_AF5F_BB632D3E1090__INCLUDED_)
#define AFX_ALIGNPALETTE_H__5DD206CB_B4AA_47E7_AF5F_BB632D3E1090__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlignPalette.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlignPalette window

class CAlignPalette : public CToolBar
{
// Construction
public:
	CAlignPalette();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlignPalette)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(CWnd * pParentWnd);
	virtual ~CAlignPalette();

	// Generated message map functions
protected:
	//{{AFX_MSG(CAlignPalette)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALIGNPALETTE_H__5DD206CB_B4AA_47E7_AF5F_BB632D3E1090__INCLUDED_)
