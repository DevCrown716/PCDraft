#if !defined(AFX_ATTRIBSETTINGSWND_H__DAFC6637_A993_43EF_8F78_794F73F3ADC9__INCLUDED_)
#define AFX_ATTRIBSETTINGSWND_H__DAFC6637_A993_43EF_8F78_794F73F3ADC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AttribSettingsWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAttribSettingsWnd window

extern UINT UM_ATTRIBSETTINGS_UPDATE;
extern UINT UM_ATTRIBSETTINGS_INFO;

class CAttribSettingsWnd : public CWnd
{
// Construction
public:

   bool     m_bOpacity;
   CEdit *  m_pedit;
   CString  m_str;
   CRect    m_rect;
   bool     m_bSimple;

	CAttribSettingsWnd();
	CAttribSettingsWnd(UINT ResNullBmpID, UINT ResMainBmp, UINT ResMask1Bmp, UINT ResMask2Bmp, COLORREF crBack, bool bOpacity = false); // Interface Update - Infinisys Ltd.

// Attributes
public:
   COLORREF m_crBack; // Interface Update - Infinisys Ltd.
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAttribSettingsWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAttribSettingsWnd();

	// Generated message map functions
protected:
	UINT m_nMainBmpID;
	UINT m_nNullBmpID;
	int m_nHeight;
	int m_nWidth;
	int m_nDrawRegim;
	CBitmap MainBmp;
	CBitmap RecievedBmp;
	CBitmap NullBmp;
	CBitmap Mask1;
	CBitmap Mask2;
	//{{AFX_MSG(CAttribSettingsWnd)
	afx_msg void OnPaint();
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	afx_msg LPARAM OnRecieveBmp(WPARAM wParam, LPARAM lParam);
   afx_msg LPARAM OnAttribSettingsInfo(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATTRIBSETTINGSWND_H__DAFC6637_A993_43EF_8F78_794F73F3ADC9__INCLUDED_)
