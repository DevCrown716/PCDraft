#if !defined(AFX_ATTRIBPALETTE_H__EB77B1BA_6058_11D2_B490_FC73BE557065__INCLUDED_)
#define AFX_ATTRIBPALETTE_H__EB77B1BA_6058_11D2_B490_FC73BE557065__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AttribPalette.h : header file
//

#include "AttribSettingsWnd.h"
#include "LineWidthWnd.h"
#include "ShowBmpWnd.h"
#include "EndmarksWnd.h"
#include "TransparencySettingsWnd.h"

class CPCDraftView;
/////////////////////////////////////////////////////////////////////////////
// CAttribPalette window


class CAttribPalette : public CDialogBar
{
// Construction
public:
	CAttribPalette();

// Attributes
public:
   CPCDraftView * m_pview;
	CAttribSettingsWnd m_wndFill;
	CAttribSettingsWnd m_wndPFill;
	CAttribSettingsWnd m_wndPen;
   CTransparencySettingsWnd m_wndFillTransparency; // Opacity/Transparency - Infinisys Ltd
   CTransparencySettingsWnd m_wndPFillTransparency; // Opacity/Transparency - Infinisys Ltd
   CTransparencySettingsWnd m_wndPenTransparency; // Opacity/Transparency - Infinisys Ltd
	CLineWidthWnd m_wndLineWidth;
	CShowBmpWnd m_wndLineStyle;
	CEndmarksWnd m_wndEndmarks;
   CEdit m_editFill; // Opacity/Transparency - Infinisys Ltd
   CEdit m_editPFill; // Opacity/Transparency - Infinisys Ltd
   CEdit m_editPen; // Opacity/Transparency - Infinisys Ltd
   CSepWnd m_wndSep1; // Opacity/Transparency - Infinisys Ltd
   CSepWnd m_wndSep2; // Opacity/Transparency - Infinisys Ltd
   CSepWnd m_wndSep3; // Opacity/Transparency - Infinisys Ltd
	bool m_bShowParallel;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttribPalette)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(CWnd * pParentWnd);
	virtual ~CAttribPalette();

	// Generated message map functions
protected:
   //virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
   virtual LRESULT WindowProc(UINT nMsg,WPARAM wParam,LPARAM lParam); // Interface Update - Infinisys Ltd.
   afx_msg void OnNcLButtonDown(UINT n,CPoint pt); // Interface Update - Infinisys Ltd.
   afx_msg void OnPaint(); // Interface Update - Infinisys Ltd.
   afx_msg void OnNcPaint(); // Interface Update - Infinisys Ltd.
	void OffsetWindow(UINT nID,int nDelta);
	void ShowHideParallel();
	afx_msg LRESULT OnMaximizeClick(WPARAM wParam,LPARAM lParam);
	//{{AFX_MSG(CAttribPalette)
	//}}AFX_MSG
   // Opacity/Transparency - Infinisys Ltd
   afx_msg void OnOK();

	DECLARE_MESSAGE_MAP()


   
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIBPALETTE_H__EB77B1BA_6058_11D2_B490_FC73BE557065__INCLUDED_)
