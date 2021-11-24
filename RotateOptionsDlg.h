#if !defined(AFX_ROTATEOPTIONSDLG_H__948D4BC2_5892_11D2_B477_9B4FC958F3A4__INCLUDED_)
#define AFX_ROTATEOPTIONSDLG_H__948D4BC2_5892_11D2_B477_9B4FC958F3A4__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// RotateOptionsDlg.h : header file
//

#include "NumberEdit.h"
#include "lpoint.h"	// Added by ClassView
/////////////////////////////////////////////////////////////////////////////
// CRotateOptionsDlg dialog

class CPCDraftDoc;

class CRotateOptionsDlg : public CDialog
{
// Construction
public:
	CRotateOptionsDlg(Angle angle,CPCDraftDoc* pDoc,CWnd* pParent = NULL);   // standard constructor

	Angle m_Angle;

// Dialog Data
	//{{AFX_DATA(CRotateOptionsDlg)
	enum { IDD = IDD_ROTATE_OPTIONS };
	CNumberEdit	m_Degree;
	CNumberEdit	m_Minute;
	CNumberEdit	m_Second;
	CStatic	m_SecondSymbol;
	CStatic	m_MinuteSymbol;
	BOOL	m_bClockwise;
	int		m_nAbout;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRotateOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPCDraftDoc* m_pDoc;

	// Generated message map functions
	//{{AFX_MSG(CRotateOptionsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROTATEOPTIONSDLG_H__948D4BC2_5892_11D2_B477_9B4FC958F3A4__INCLUDED_)
