// FontSubctitute.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "FontSubctitute.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFontSubctitute

IMPLEMENT_DYNAMIC(CFontSubctitute, CFontDialog)

CFontSubctitute::CFontSubctitute(LPLOGFONT lplfInitial, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd) : 
	CFontDialog(lplfInitial, dwFlags, pdcPrinter, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(CFontSubctitute, CFontDialog)
	//{{AFX_MSG_MAP(CFontSubctitute)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

