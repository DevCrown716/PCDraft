#if !defined(AFX_CHOOSECOLORDLG_H__CA2BBBA6_B391_11D2_B53E_DFAD8EDBEC7E__INCLUDED_)
#define AFX_CHOOSECOLORDLG_H__CA2BBBA6_B391_11D2_B53E_DFAD8EDBEC7E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseColorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChooseColorDlg dialog

class CChooseColorDlg : public CColorDialog
{
	DECLARE_DYNAMIC(CChooseColorDlg)

public:
	COLORREF m_Color;
	CChooseColorDlg(COLORREF clrInit = 0, CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(CChooseColorDlg)
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSECOLORDLG_H__CA2BBBA6_B391_11D2_B53E_DFAD8EDBEC7E__INCLUDED_)
