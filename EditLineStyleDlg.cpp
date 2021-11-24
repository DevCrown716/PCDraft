// EditLineStyleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "EditLineStyleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditLineStyleDlg dialog


CEditLineStyleDlg::CEditLineStyleDlg(unsigned char* style ,CWnd* pParent /*=NULL*/)
	: CDialog(CEditLineStyleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditLineStyleDlg)
	m_nUnits = 0;
	//}}AFX_DATA_INIT
	m_Style=style;
	memcpy(m_EditStyle,m_Style,16);
	m_edit.SetParentDlg(this);
}


void CEditLineStyleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditLineStyleDlg)
	DDX_Control(pDX, IDC_WHITE, m_white);
	DDX_Control(pDX, IDC_DRAW, m_draw);
	DDX_Control(pDX, IDC_BLACK, m_black);
	DDX_Control(pDX, IDC_EDIT, m_edit);
	DDX_Control(pDX, IDC_RULER, m_ruler);
	DDX_Radio(pDX, IDC_INCH, m_nUnits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditLineStyleDlg, CDialog)
	//{{AFX_MSG_MAP(CEditLineStyleDlg)
	ON_BN_CLICKED(IDC_INCH, OnUnits)
	ON_WM_DRAWITEM()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_REVERT, OnRevert)
	ON_BN_CLICKED(IDC_CM, OnUnits)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditLineStyleDlg message handlers

void CEditLineStyleDlg::OnUnits() 
{
	UpdateData();
	m_ruler.SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),m_nUnits?IDB_DASH_CM:IDB_DASH_INCH,0,NULL,0));
}

BOOL CEditLineStyleDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ruler.SetBitmap(::CreateMappedBitmap(AfxGetResourceHandle(),IDB_DASH_INCH,0,NULL,0));
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CEditLineStyleDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS) 
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	CRect rc(lpDIS->rcItem);

	pDC->ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL); //fill background
	
	CGDIBackColor bc(pDC,GetSysColor(COLOR_BTNTEXT));

	switch(lpDIS->CtlID)
	{
	case IDC_SAMPLE:
		rc.top=(rc.top+rc.bottom)/2;
		rc.bottom=rc.top+1;

		if(*m_EditStyle)
		{
			int i=1;
			int right=lpDIS->rcItem.right;
			while(rc.left<right)
			{
				rc.right=rc.left+m_EditStyle[i];
				if(rc.right>right) rc.right=right;
				if(i%2) pDC->ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
				i++;
				if(m_EditStyle[i]==0xFF || i>14) i=1;
				rc.left=rc.right;
			}
		}
		else
		{
			pDC->ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
		}
		break;
	case IDC_POS:
		for(int i=1;i<=m_EditStyle[0];i++)
		{
			rc.left+=m_EditStyle[i]*3;
			rc.right=rc.left+1;
			pDC->ExtTextOut(0,0,ETO_OPAQUE|ETO_CLIPPED,&rc,NULL,0,NULL);
		}
		break;
	}
}

void CEditLineStyleDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_white.ShowWindow(SW_HIDE);
	m_black.ShowWindow(SW_HIDE);
}

void CEditLineStyleDlg::OnRevert() 
{
	memcpy(m_EditStyle,m_Style,16);
	m_edit.CalcLength();
	m_edit.Invalidate();
	GetDlgItem(IDC_SAMPLE)->Invalidate();
}

void CEditLineStyleDlg::OnOK() 
{
	memcpy(m_Style,m_EditStyle,16);
	CDialog::OnOK();
}

void CEditLineStyleDlg::Update()
{
	GetDlgItem(IDC_REVERT)->EnableWindow(memcmp(m_Style,m_EditStyle,16));
}
