#if !defined(AFX_ALIGNMENTOPTIONSDLG_H__C4A92261_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
#define AFX_ALIGNMENTOPTIONSDLG_H__C4A92261_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// AlignmentOptionsDlg.h : header file
//
#include "AlignmentOptionsWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CAlignmentOptionsDlg dialog

enum AlignMode
{
	alignMode_None,
	alignMode_RefObj,
	alignMode_RefPt,
	alignMode_ToGrid
};

class CAlignmentOptionsDlg : public CDialog
{
// Construction
public:
	CAlignmentOptionsDlg(AlignMode mode,AlignMask selMask,AlignMask refMask,CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAlignmentOptionsDlg)
	enum { IDD = IDD_ALIGNMENT_OPTIONS };
	CStatic	m_Ref;
	CButton	m_SelDatum;
	CButton	m_RefDatum;
	CStatic	m_Feedback;
	CStatic	m_Note;
	//}}AFX_DATA
	CAlignmentOptionsWnd m_SelOpt;
	CAlignmentOptionsWnd m_RefOpt;

	AlignMode m_mode;
	AlignMask m_selected;
	AlignMask m_reference;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlignmentOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	short ValidAlignmentSelectionTest();
	void CreateTextDescription(CString &text,bool isReferenceGrid);
	void DatumChanged(bool checkIt,AlignMask& nAlignSet);
	void SetMode();
	void ShowHideRefGrid(bool showIt);
	void ModeChanged(AlignMode newMode);
	void UpdateAlignStatus();

	afx_msg void OnSelectedOptions();
	afx_msg void OnReferenceOptions();
	// Generated message map functions
	//{{AFX_MSG(CAlignmentOptionsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnAlignToObj();
	afx_msg void OnAlignToPoint();
	afx_msg void OnAlignToGrid();
	afx_msg void OnReferenceDatum();
	afx_msg void OnSelectedDatum();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALIGNMENTOPTIONSDLG_H__C4A92261_5961_11D2_B479_FED1ADE6F2A4__INCLUDED_)
