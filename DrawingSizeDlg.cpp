// DrawingSizeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PCDraft.h"
#include "DrawingSizeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	IDC_DRAWING_SIZE	2001

/////////////////////////////////////////////////////////////////////////////
// CDrawingSizeDlg dialog


CDrawingSizeDlg::CDrawingSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDrawingSizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDrawingSizeDlg)
	m_nPageNum = 0;
	m_bLastPageFirst = FALSE;
	m_bRegistrationMarks = FALSE;
	//}}AFX_DATA_INIT
}


void CDrawingSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDrawingSizeDlg)
	DDX_Control(pDX, IDC_SIZE_INCHES, m_SizeInches);
	DDX_Control(pDX, IDC_SIZE_CM, m_SizeCM);
	DDX_Radio(pDX, IDC_PAGE_NUMBERING_1324, m_nPageNum);
	DDX_Check(pDX, IDC_PRINT_LAST_PAGE_FIRST, m_bLastPageFirst);
	DDX_Check(pDX, IDC_PRINT_REGISTRATION_MARKS, m_bRegistrationMarks);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDrawingSizeDlg, CDialog)
	//{{AFX_MSG_MAP(CDrawingSizeDlg)
	//}}AFX_MSG_MAP
    ON_MESSAGE(DSN_SIZECHANGED,OnDrawingSizeChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDrawingSizeDlg message handlers

BOOL CDrawingSizeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_PAGE_NUMBERING_1324))->SetIcon(AfxGetApp()->LoadIcon(IDI_PAGE_NUMBERING_1324));
	((CButton*)GetDlgItem(IDC_PAGE_NUMBERING_1234))->SetIcon(AfxGetApp()->LoadIcon(IDI_PAGE_NUMBERING_1234));

	CRect rc;
	GetDlgItem(IDC_DRAWING_SIZE_PLACE_HOLDER)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_wndDrawingSize.Create(rc,this,IDC_DRAWING_SIZE);
	OnDrawingSizeChanged(0,0);

	return TRUE;
}


LRESULT CDrawingSizeDlg::OnDrawingSizeChanged(WPARAM wParam, LPARAM lParam)
{
	CSize theSize=m_wndDrawingSize.GetSize();
	CString theText;
	theText.Format("%.2f x %.2f cm",double(theSize.cx)*2.54/72,double(theSize.cy)*2.54/72);
	AfxSetWindowText(m_SizeCM.GetSafeHwnd(),theText);
	theText.Format("%.2f x %.2f inches",double(theSize.cx)/72,double(theSize.cy)/72);
	AfxSetWindowText(m_SizeInches.GetSafeHwnd(),theText);
	return 0;
}
