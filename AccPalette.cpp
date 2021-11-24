// AccPalette.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "AccPalette.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAccPalette

CAccPalette::CAccPalette()
{
}

CAccPalette::~CAccPalette()
{
}


BEGIN_MESSAGE_MAP(CAccPalette, CToolBar)
	//{{AFX_MSG_MAP(CAccPalette)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAccPalette message handlers
#define	kNumAccButtons	13

static int buttons[kNumAccButtons] = {
	ID_TOOL_HANDLE,
	ID_TOOL_GLUE,
	ID_TOOL_UNGLUE,
	ID_TOOL_COMBINE,
	ID_TOOL_SUBTRACT,
	ID_TOOL_EXTEND,
	ID_TOOL_TRIM,
	ID_TOOL_FILLET,
	ID_TOOL_CHAMFER,
	ID_TOOL_MIRROR,
	ID_TOOL_LINK,
	ID_TOOL_UNLINK,
	ID_TOOL_MULTIUNLINK
};

static int bitmaps[kNumAccButtons]=
{
	IDB_TOOL_HANDLE,
	IDB_TOOL_GLUE,
	IDB_TOOL_UNGLUE,
	IDB_TOOL_COMBINE,
	IDB_TOOL_SUBTRACT,
	IDB_TOOL_EXTEND,
	IDB_TOOL_TRIM,
	IDB_TOOL_FILLET,
	IDB_TOOL_CHAMFER,
	IDB_TOOL_MIRROR,
	IDB_TOOL_LINK,
	IDB_TOOL_UNLINK,
	IDB_TOOL_MULTIUNLINK
};

BOOL CAccPalette::Create(CWnd *pParentWnd)
{
	BOOL bResult=CToolBar::Create(pParentWnd,WS_CHILD|CBRS_NOALIGN|CBRS_SIZE_FIXED|CBRS_FLOATING,IDR_ACC_PALETTE);

	if(bResult)
	{
		CToolBarCtrl& ctrl=GetToolBarCtrl();

		ctrl.SetToolTips(((CMainFrame*)pParentWnd)->GetToolTip());

		SetBorders(1,1,1,1);
		SetSizes(CSize(27,24),CSize(20,18));

		TBBUTTON bt;
		memset(&bt,0,sizeof(TBBUTTON));
		bt.fsState=TBSTATE_ENABLED;
		bt.fsStyle=TBSTYLE_BUTTON;
		bt.iString=-1;
		for(int i=0;i<kNumAccButtons;i++)
		{
			bt.iBitmap=ctrl.AddBitmap(1,bitmaps[i]);
			bt.idCommand=buttons[i];
			ctrl.AddButtons(1,&bt);
		}
		EnableDocking(0);
	}

	return bResult;
}
