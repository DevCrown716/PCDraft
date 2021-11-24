#if !defined(AFX_ALIGNMENTOPTIONSWND_H__04E99218_B83D_11D2_B546_AB6770E0047F__INCLUDED_)
#define AFX_ALIGNMENTOPTIONSWND_H__04E99218_B83D_11D2_B546_AB6770E0047F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AlignmentOptionsWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAlignmentOptionsWnd window

enum
{
	alignBit_Datum			= 0,
	alignBit_Top			= 1,
	alignBit_Left			= 2,
	alignBit_Bottom			= 3,
	alignBit_Right			= 4,
	alignBit_Spare1			= 5,
	alignBit_Spare2			= 6,
	alignBit_Spare3			= 7
};

enum
{
	alignMask_None			= 0,
	alignMask_Datum			= 1L << alignBit_Datum,
	alignMask_Top			= 1L << alignBit_Top,
	alignMask_Left			= 1L << alignBit_Left,
	alignMask_Bottom		= 1L << alignBit_Bottom,
	alignMask_Right			= 1L << alignBit_Right,
	alignMask_TopLeft		= alignMask_Top | alignMask_Left,
	alignMask_TopRight		= alignMask_Top | alignMask_Right,
	alignMask_BottomLeft	= alignMask_Bottom | alignMask_Left,
	alignMask_BottomRight	= alignMask_Bottom | alignMask_Right,
	alignMask_HorizCenters	= alignMask_Left | alignMask_Right,
	alignMask_VertCenters	= alignMask_Top | alignMask_Bottom,
	alignMask_Center		= alignMask_Top | alignMask_Bottom | alignMask_Left | alignMask_Right,
	alignMask_Grid			= alignMask_Center
};

typedef	DWORD	AlignMask;

class CAlignmentOptionsWnd : public CWnd
{
// Construction
public:
	CAlignmentOptionsWnd();

// Attributes
public:

// Operations
public:
	void BuildMatrix(AlignMask nAlignSet);
	void BuildAlignSet(AlignMask& nAlignSet);
	bool TestMatrix(short r,short c) { return m_Cells[r][c]; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlignmentOptionsWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAlignmentOptionsWnd();

	// Generated message map functions
protected:
	CBitmap	m_Bitmap;
	bool	m_Cells[3][3];

	void ClearMatrix();
	bool VerifyExistingMatrix(AlignMask newAlignSet);
	bool TrackRect(CRect& rc);
	bool ToggleMatrix(short r,short c);
	void ClearGridExcept(short r1,short c1);

	//{{AFX_MSG(CAlignmentOptionsWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALIGNMENTOPTIONSWND_H__04E99218_B83D_11D2_B546_AB6770E0047F__INCLUDED_)
