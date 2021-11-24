#if !defined(AFX_SELECTSPECIALDLG_H__948D4BC3_5892_11D2_B477_9B4FC958F3A4__INCLUDED_)
#define AFX_SELECTSPECIALDLG_H__948D4BC3_5892_11D2_B477_9B4FC958F3A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelectSpecialDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectSpecialDlg dialog

class CSelectSpecialDlg : public CDialog
{
// Construction
public:
	BOOL m_bUse[5];
	CString m_sValues[5];
	int m_nFillValue;
	int m_nPenValue;
	int m_nLineWidthValue;
	int m_nLineStyleValue;
	void SelectPattern(UINT nTitle,int& value);
	void SelectColor(UINT nTitle, int& value);
	CSelectSpecialDlg(CWnd* pParent = NULL);   // standard constructor

	enum  ObjectClass
	{
		kAllClass = 0,
		kText,
		kGeneralShape,
		kDimension
	};

	enum  GeneralShapeType
	{
		kAllGeneralShape = 0,
		kLines,
		kRectangles,
		kRoundedRect,
		kCircles,
		kCircularArcs,
		kEllipticalArcs,
		kOvals,
		kBezierCurves,
		kSplineCurves,
		kPolygons,
		kPolylines,
		kParallelLines,
		kParallelPolygons,
		kParallelPolylines
	};

	enum  DimensionType
	{
		kAllDimension = 0,
		kHorizontal,
		kVertical,
		kSlope,
		kPerpendicular,
		kDiameter,
		kRadius,
		kCircleCenter,
		kAngular
	};

// Dialog Data
	//{{AFX_DATA(CSelectSpecialDlg)
	enum { IDD = IDD_SELECT_SPECIAL };
	CStatic	m_PenPatternPicker;
	CStatic	m_PenColorPicker;
	CStatic	m_FillPatternPicker;
	CStatic	m_FillColorPicker;
	CStatic	m_LineWeightPicker;
	CButton	m_LineWeight;
	CStatic	m_LineStylePicker;
	CButton	m_LineStyle;
	CButton	m_ObjectFill;
	CButton	m_ObjectPen;
	CComboBox	m_ObjectType;
	int		m_nObjectClass;
	int		m_nFillKind;
	int		m_nPenKind;
	BOOL	m_bUseFill;
	BOOL	m_bUsePen;
	BOOL	m_bUseLineStyle;
	BOOL	m_bUseLineWeight;
	int		m_nObjectType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectSpecialDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectSpecialDlg)
	afx_msg void OnSelchangeObjectClass();
	afx_msg void OnObjectPen();
	afx_msg void OnObjectFill();
	afx_msg void OnObjectInfo();
	afx_msg void OnPenColor();
	afx_msg void OnPenPattern();
	afx_msg void OnFillColor();
	afx_msg void OnFillPattern();
	afx_msg void OnLineStyle();
	afx_msg void OnLineWeight();
	afx_msg void OnLineStylePicker();
	afx_msg void OnLineWeightPicker();
	afx_msg void OnFillColorPicker();
	afx_msg void OnFillPatternPicker();
	afx_msg void OnPenColorPicker();
	afx_msg void OnPenPatternPicker();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTSPECIALDLG_H__948D4BC3_5892_11D2_B477_9B4FC958F3A4__INCLUDED_)
