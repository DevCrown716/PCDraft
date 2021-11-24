// AlignPalette.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "AlignPalette.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlignPalette

CAlignPalette::CAlignPalette()
{
}

CAlignPalette::~CAlignPalette()
{
}


BEGIN_MESSAGE_MAP(CAlignPalette, CToolBar)
	//{{AFX_MSG_MAP(CAlignPalette)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlignPalette message handlers
#define	kNumAlignButtons	10

static int buttons[kNumAlignButtons] = {
	ID_ALIGN_BOTTOM,
	ID_ALIGN_TOP,
	ID_ALIGN_VCENTER,
	ID_ALIGN_DISTH_CENTER,
	ID_ALIGN_DISTH_DATUM,
	ID_ALIGN_LEFT,
	ID_ALIGN_RIGHT,
	ID_ALIGN_HCENTER,
	ID_ALIGN_DISTV_CENTER,
	ID_ALIGN_DISTV_DATUM
};

static int bitmaps[kNumAlignButtons]=
{
	IDB_ALIGN_BOTTOM,
	IDB_ALIGN_TOP,
	IDB_ALIGN_VCENTER,
	IDB_ALIGN_DISTH_CENTER,
	IDB_ALIGN_DISTH_DATUM,
	IDB_ALIGN_LEFT,
	IDB_ALIGN_RIGHT,
	IDB_ALIGN_HCENTER,
	IDB_ALIGN_DISTV_CENTER,
	IDB_ALIGN_DISTV_DATUM
};

BOOL CAlignPalette::Create(CWnd *pParentWnd)
{
	BOOL bResult=CToolBar::Create(pParentWnd,WS_CHILD|CBRS_NOALIGN|CBRS_SIZE_FIXED|CBRS_FLOATING,IDR_ALIGN_PALETTE);

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
		for(int i=0;i<kNumAlignButtons;i++)
		{
			if(i==4) bt.fsState|=TBSTATE_WRAP;
			else bt.fsState&=~TBSTATE_WRAP;
			bt.iBitmap=ctrl.AddBitmap(1,bitmaps[i]);
			bt.idCommand=buttons[i];
			ctrl.AddButtons(1,&bt);
		}
		EnableDocking(0);
	}

	return bResult;
}
