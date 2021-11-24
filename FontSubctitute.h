#if !defined(AFX_FONTSUBCTITUTE_H__5F0D8751_FC0F_40BC_A803_36131A23093A__INCLUDED_)
#define AFX_FONTSUBCTITUTE_H__5F0D8751_FC0F_40BC_A803_36131A23093A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FontSubctitute.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFontSubctitute dialog

class CFontSubctitute : public CFontDialog
{
	DECLARE_DYNAMIC(CFontSubctitute)

public:
	CFontSubctitute(LPLOGFONT lplfInitial = NULL,
		DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);
#ifndef _AFX_NO_RICHEDIT_SUPPORT
	CFontSubctitute(const CHARFORMAT& charformat,
		DWORD dwFlags = CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);
#endif

protected:
	//{{AFX_MSG(CFontSubctitute)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTSUBCTITUTE_H__5F0D8751_FC0F_40BC_A803_36131A23093A__INCLUDED_)
