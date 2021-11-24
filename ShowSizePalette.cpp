// ShowSizePalette.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ShowSizePalette.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CShowSizePalette

CShowSizePalette::CShowSizePalette()
{
}

CShowSizePalette::~CShowSizePalette()
{
}

void CShowSizePalette::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CShowSizePalette)
	DDX_Control(pDX, ID_TOOL_SHOWSIZE4, m_s4);
	DDX_Control(pDX, ID_TOOL_SHOWSIZE3, m_s3);
	DDX_Control(pDX, ID_TOOL_SHOWSIZE2, m_s2);
	DDX_Control(pDX, ID_TOOL_SHOWSIZE1, m_s1);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CShowSizePalette, CDialogBar)
	//{{AFX_MSG_MAP(CShowSizePalette)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CShowSizePalette message handlers

BOOL CShowSizePalette::Create(CWnd *pParentWnd)
{
	BOOL bResult=CDialogBar::Create(pParentWnd,IDD_SHOWSIZE_PALETTE,CBRS_NOALIGN|CBRS_SIZE_FIXED|CBRS_FLOATING,IDR_SHOWSIZE_PALETTE);
	if(bResult)
	{
		ModifyStyle(WS_VISIBLE,0,0);
		SetBorders(1,1,1,1);
		EnableDocking(0);
	}

	return bResult;
}
