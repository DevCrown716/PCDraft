// ChooseColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ChooseColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChooseColorDlg

IMPLEMENT_DYNAMIC(CChooseColorDlg, CColorDialog)

CChooseColorDlg::CChooseColorDlg(COLORREF clrInit, CWnd* pParentWnd) :
	CColorDialog(clrInit, CC_FULLOPEN|CC_ENABLETEMPLATE, pParentWnd)
{
	m_Color=clrInit;
	m_cc.hInstance=(HWND)AfxGetResourceHandle();
	m_cc.lpTemplateName=MAKEINTRESOURCE(IDD_CHOOSE_COLOR);
}


BEGIN_MESSAGE_MAP(CChooseColorDlg, CColorDialog)
	//{{AFX_MSG_MAP(CChooseColorDlg)
	ON_WM_DRAWITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CChooseColorDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS) 
{
	if(lpDIS->CtlID==IDC_ORIGINAL)
	{
		CDC* pDC = CDC::FromHandle(lpDIS->hDC);
		CRect rc(lpDIS->rcItem);

		CGDIBackColor bc(pDC,m_Color);
		pDC->ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
	}
	else CColorDialog::OnDrawItem(nIDCtl, lpDIS);
}
