// DimPalette.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "DimPalette.h"
#include "CustomMenus.h"
#include "CustomMiniDockFrameWnd.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDimPalette

CDimPalette::CDimPalette()
{
	m_bShowExtra=false;
}

CDimPalette::~CDimPalette()
{
}


BEGIN_MESSAGE_MAP(CDimPalette, CToolBar)
	//{{AFX_MSG_MAP(CDimPalette)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(UM_MAXIMIZE_CLICK,OnMaximizeClick)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDimPalette message handlers
#define	kNumDimButtons	8

#ifndef _PERSONAL_EDITION

#define	kNumStableButtons	8

#else

#define	kNumStableButtons	4

#endif

#define kFormatIndex	kNumDimButtons

static int buttons[kNumDimButtons] = {
	ID_TOOL_DIM_HORIZ,
	ID_TOOL_DIM_VERT,
	ID_TOOL_DIM_SLOPE,
	ID_TOOL_DIM_PERP,
	ID_TOOL_DIM_DIAMETER,
	ID_TOOL_DIM_RADIUS,
	ID_TOOL_DIM_CENTER,
	ID_TOOL_DIM_ANGULAR
};

static int bitmaps[kNumDimButtons]=
{
	IDB_TOOL_DIM_HORIZ,
	IDB_TOOL_DIM_VERT,
	IDB_TOOL_DIM_SLOPE,
	IDB_TOOL_DIM_PERP,
	IDB_TOOL_DIM_DIAMETER,
	IDB_TOOL_DIM_RADIUS,
	IDB_TOOL_DIM_CENTER,
	IDB_TOOL_DIM_ANGULAR
};

BOOL CDimPalette::Create(CWnd *pParentWnd)
{
	BOOL bResult=CToolBar::Create(pParentWnd,WS_CHILD|CBRS_NOALIGN|CBRS_SIZE_FIXED|CBRS_FLOATING,IDR_DIM_PALETTE);

	if(bResult)
	{
		CToolBarCtrl& ctrl=GetToolBarCtrl();

		CToolTipCtrl* tips=((CMainFrame*)pParentWnd)->GetToolTip();

		ctrl.SetToolTips(tips);

		SetBorders(1,1,1,1);
		SetSizes(CSize(27,24),CSize(20,18));

		TBBUTTON bt;
		memset(&bt,0,sizeof(TBBUTTON));
		bt.fsState=TBSTATE_ENABLED;
		bt.fsStyle=TBSTYLE_BUTTON;
		bt.iString=-1;
		for(int i=0;i<kNumStableButtons;i++)
		{
			bt.iBitmap=ctrl.AddBitmap(1,bitmaps[i]);
			bt.idCommand=buttons[i];
			ctrl.AddButtons(1,&bt);

			bt.fsState^=TBSTATE_WRAP;
		}

		AddExtra();
#ifndef _PERSONAL_EDITION
		tips->AddTool(GetDlgItem(ID_TOOL_DIM_ENDMARK));
		tips->AddTool(GetDlgItem(ID_TOOL_DIM_TYPE));
		tips->AddTool(GetDlgItem(ID_TOOL_DIM_FORMAT));
#endif// _PERSONAL_EDITION
		EnableDocking(0);
	}

	return bResult;
}

void CDimPalette::InvertResizeOpt()
{
	if(m_bShowExtra) RemoveExtra();
	else AddExtra();
}

LRESULT CDimPalette::OnMaximizeClick(WPARAM wParam,LPARAM lParam)
{
	((CCustomMiniDockFrameWnd*)GetParentFrame())->InvertMaximized();
	InvertResizeOpt();
	GetParentFrame()->RecalcLayout();
	return 0;
}

void CDimPalette::AddExtra()
{
	CToolBarCtrl& ctrl=GetToolBarCtrl();
#ifndef _PERSONAL_EDITION
	
	TBBUTTON bt;
	memset(&bt,0,sizeof(TBBUTTON));
	bt.fsState=TBSTATE_ENABLED;
	bt.iString=-1;
	bt.iBitmap=-1;
	bt.idCommand=ID_TOOL_DIM_ENDMARK;
	bt.fsStyle=TBSTYLE_BUTTON;
	ctrl.AddButtons(1,&bt);

	bt.idCommand=0;
	bt.fsStyle=TBSTYLE_SEP;
	bt.fsState^=TBSTATE_WRAP;
	ctrl.AddButtons(1,&bt);

	bt.idCommand=ID_TOOL_DIM_FORMAT;
	bt.fsStyle=TBSTYLE_BUTTON;
	bt.fsState^=TBSTATE_WRAP;
	ctrl.AddButtons(1,&bt);

	bt.idCommand=0;
	bt.fsStyle=TBSTYLE_SEP;
	bt.fsState^=TBSTATE_WRAP;
	ctrl.AddButtons(1,&bt);

	SetButtonInfo(8,ID_TOOL_DIM_FORMAT,TBBS_SEPARATOR,56);
	SetButtonInfo(10,ID_TOOL_DIM_FORMAT,TBBS_SEPARATOR,56);

	CRect rc;

	if(m_wndEndmarks.GetSafeHwnd()) m_wndEndmarks.ShowWindow(SW_SHOW);
	else
	{
		GetItemRect(8,&rc);
		rc.InflateRect(-1,-2,-2,-9);
		m_wndEndmarks.Create(rc,this,ID_TOOL_DIM_ENDMARK);
//		m_stEndmarks.SetBitmap(::LoadBitmap(AfxGetResourceHandle(),MAKEINTRESOURCE(IDB_ENDMARK_PICKER)));
	}
	if(!m_font.GetSafeHandle())
		m_font.CreateStockObject(DEFAULT_GUI_FONT);
//		m_font.CreateFont(6,0,0,0,FW_NORMAL,0,0,0,DEFAULT_CHARSET,OUT_TT_PRECIS,CLIP_TT_ALWAYS,DEFAULT_QUALITY,DEFAULT_PITCH,"MS Sans Serif");
	if(m_cbType.GetSafeHwnd()) m_cbType.ShowWindow(SW_SHOW);
	else
	{

		GetItemRect(8,&rc);
		rc.InflateRect(1,-18,2,100);
		m_cbType.Create(WS_VISIBLE|CBS_DROPDOWNLIST|CBS_HASSTRINGS,rc,this,ID_TOOL_DIM_TYPE);
		m_cbType.ModifyStyle(WS_BORDER,0,0);
		m_cbType.SetFont(&m_font);
		CString strText;
		strText.LoadString(IDS_SINGLE);
		m_cbType.AddString(strText);
		strText.LoadString(IDS_CHAINED);
		m_cbType.AddString(strText);
		strText.LoadString(IDS_BASELINE);
		m_cbType.AddString(strText);
		m_cbType.SetCurSel(0);
	}

	if(m_btnFormat.GetSafeHwnd()) m_btnFormat.ShowWindow(SW_SHOW);
	else
	{
		GetItemRect(10,&rc);
		rc.InflateRect(0,-12,0,5);
		CString strText;
		strText.LoadString(IDS_FORMAT);
		m_btnFormat.Create(strText,WS_VISIBLE|BS_PUSHBUTTON,rc,this,ID_TOOL_DIM_FORMAT);
		m_btnFormat.SetFont(&m_font);
	}

#else

	TBBUTTON bt;
	memset(&bt,0,sizeof(TBBUTTON));
	bt.fsState=TBSTATE_ENABLED;
	bt.fsStyle=TBSTYLE_BUTTON;
	bt.iString=-1;

	for(int i=kNumStableButtons;i<kNumDimButtons;i++){
			bt.iBitmap=ctrl.AddBitmap(1,bitmaps[i]);
			bt.idCommand=buttons[i];
			ctrl.AddButtons(1,&bt);

			bt.fsState^=TBSTATE_WRAP;
	}	

#endif
	m_bShowExtra=true;
}

void CDimPalette::RemoveExtra()
{
#ifndef _PERSONAL_EDITION

	m_wndEndmarks.ShowWindow(SW_HIDE);
	m_cbType.ShowWindow(SW_HIDE);
	m_btnFormat.ShowWindow(SW_HIDE);

	CToolBarCtrl& ctrl=GetToolBarCtrl();
	ctrl.DeleteButton(11);
	ctrl.DeleteButton(10);
	ctrl.DeleteButton(9);
	ctrl.DeleteButton(8);

#else

	CToolBarCtrl& ctrl=GetToolBarCtrl();
	for(int i=kNumDimButtons;--i>=kNumStableButtons;){
		ctrl.DeleteButton(i);
	}	
	
#endif

	m_bShowExtra=false;
}
