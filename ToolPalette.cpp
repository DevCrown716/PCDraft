// ToolPalette.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "ToolPalette.h"
#include "CustomMenus.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define kToolTimerID	254

/////////////////////////////////////////////////////////////////////////////
// CToolPalette

CToolPalette::CToolPalette()
{
}

CToolPalette::~CToolPalette()
{
}


BEGIN_MESSAGE_MAP(CToolPalette, CToolBar)
	//{{AFX_MSG_MAP(CToolPalette)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolPalette message handlers

static int buttons[kNumToolButtons] = {
	ID_TOOL_ARROW,
	ID_TOOL_ROTATE,
	ID_TOOL_MARQUEE,
	ID_TOOL_PAN,
	ID_TOOL_CONSTR_LINE,
	ID_TOOL_PERP_LINE,
	ID_TOOL_RECT,
	ID_TOOL_TEXT,
	ID_TOOL_REG_POLY_SIDE_SIDE,
	ID_TOOL_RC_P,
	ID_TOOL_CIRCLE_D,
	ID_TOOL_ARC_R,
	ID_TOOL_POLY,
	ID_TOOL_CURVE,
	ID_TOOL_PARALLEL_LINE,
   ID_TOOL_STAR_DIAMETER, // Star -  Infinisys Ltd
	ID_TOOL_MARKER,
	//ID_TOOL_ROTATE_90, // Star -  Infinisys Ltd
	ID_TOOL_ATTRIB_DROP,
	ID_TOOL_ZOOM,
	ID_TOOL_1_1
};
// PC Draft 7 - Shortcut Keys
int tool_palette_count() 
{
   return kNumToolButtons;
}
// PC Draft 7 - Shortcut Keys
int tool_palette_button_id(int iIndex) 
{
   return buttons[iIndex];
}
// PC Draft 7 - Shortcut Keys
static int shortcuts[kNumToolButtons] = {
   'V',
   'R',
   'M',
   'H',
   'L',
   'S',
   'U',
   'T',
   'P',
   'O',
   'C',
   'A',
   'I',
   'B',
   'W',
   'K',
   'X',
   'D',
   'Z',
   -1
};
// PC Draft 7 - Shortcut Keys
int tool_palette_button_shortcut(int iIndex)
{
   return shortcuts[iIndex];
}
// PC Draft 7 - Shortcut Keys
int tool_palette_get_shortcut_index(int iShortcut)
{
   if (iShortcut >= 'a' && iShortcut <= 'z')
   {
      iShortcut += 'A' - 'a';
   }
   for (int iIndex = 0; iIndex < tool_palette_count(); iIndex++)
   {
      if (iShortcut == tool_palette_button_shortcut(iIndex))
      {
         return iIndex;
      }
   }
   return -1;
}
// PC Draft 7 - Shortcut Keys
int tool_palette_get_shortcut_command(int iShortcut)
{
   int iIndex = tool_palette_get_shortcut_index(iShortcut);

   if (iIndex < 0 || iIndex >= tool_palette_count())
   {

      return -1;

   }
   return tool_palette_button_id(iIndex);
}
static int bitmaps[kNumToolButtons]=
{
	IDB_TOOL_ARROW,
	IDB_TOOL_ROTATE,
	IDB_TOOL_MARQUEE,
	IDB_TOOL_PAN,
	IDB_TOOL_CONSTR_LINE,
	IDB_TOOL_PERP_LINE,
	IDB_TOOL_RECT,
	IDB_TOOL_TEXT,
	IDB_TOOL_REGULAR_POLY,
	IDB_TOOL_RC,
	IDB_TOOL_CIRCLE,
	IDB_TOOL_ARC,
	IDB_TOOL_POLY,
	IDB_TOOL_CURVE,
	IDB_TOOL_PARALLEL_LINE,
   IDB_TOOL_STAR, // Star -  Infinisys Ltd
	IDB_TOOL_MARKER,
//	IDB_TOOL_ROTATE_90, // Star -  Infinisys Ltd
	IDB_TOOL_ATTRIB_DROP,
	IDB_TOOL_ZOOM,
	IDB_TOOL_1_1
};

BOOL CToolPalette::Create(CWnd * pParentWnd, bool bVisible)
{

      int iStyle = 0;

      if (bVisible)
      {

         iStyle = WS_VISIBLE;

      }

	BOOL bResult=CToolBar::Create(pParentWnd,WS_CHILD|iStyle|CBRS_NOALIGN|CBRS_SIZE_FIXED|CBRS_FLOATING,IDR_TOOL_PALETTE);

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

		CBitmap mark,mask;
		mark.LoadBitmap(IDB_TOOL_MARK);
		CGDIBitmapDC markDC(NULL,&mark);
		mask.LoadBitmap(IDB_TOOL_MASK);
		CGDIBitmapDC maskDC(NULL,&mask);

		for(int i=0;i<kNumToolButtons;i++)
		{
			BITMAP bmp;
			bt.idCommand=buttons[i];
			m_bitmaps[i].LoadMappedBitmap(bitmaps[i]);
			m_bitmaps[i].GetBitmap(&bmp);
			bt.dwData=bmp.bmWidth/20;
			if(bt.dwData>1)
			{
				CGDIBitmapDC memDC(NULL,&m_bitmaps[i]);
				for(DWORD j=0;j<bt.dwData;j++)
				{
					int x=20*j+17;
					memDC.BitBlt(x,15,3,3,&maskDC,0,0,SRCAND);
					memDC.BitBlt(x,15,3,3,&markDC,0,0,SRCPAINT);
				}
			}
			ctrl.AddBitmap(bt.dwData,m_bitmaps+i);
			ctrl.AddButtons(1,&bt);

			bt.fsState^=TBSTATE_WRAP;
			m_base[i]=bt.iBitmap;
			bt.iBitmap+=bt.dwData;
		}
		EnableDocking(0);
	}
	return bResult;
}

void CToolPalette::OnLButtonDown(UINT nFlags, CPoint point) 
{

	m_nHitID=OnToolHitTest(point,NULL);

   for (int iButton = 0; iButton < kNumToolButtons; iButton++)
   {

      int iImage;

      UINT nID, nStyle;

      GetButtonInfo(iButton, nID, nStyle, iImage);

      if (nID == m_nHitID)
      {
         TRACE("");
      }
      else
      {

         SetButtonStyle(iButton, nStyle & ~(TBBS_CHECKED | TBSTATE_PRESSED | TBSTATE_MARKED | 0x00030000));

      }

   }


	if(m_nHitID>=ID_TOOL_MULTI_FIRST && m_nHitID<=ID_TOOL_MULTI_LAST
      || m_nHitID == ID_TOOL_TEXT)
	{
		SetTimer(kToolTimerID,300,NULL);
		m_nHitFlags=nFlags;
		m_ptHit=point;
	}
   else
   {

      m_nHitID = 0;

   }

	CToolBar::OnLButtonDown(nFlags, point);

}


void CToolPalette::OnTimer(UINT_PTR nIDEvent) 
{
	if(nIDEvent==kToolTimerID)
	{
		KillTimer(kToolTimerID);

		CPoint pt;
		GetCursorPos(&pt);
		ScreenToClient(&pt);
		if(m_nHitID==OnToolHitTest(pt,NULL))
		{
			if(m_nHitID==ID_TOOL_TEXT)
			{
				CMenu theMenu;
				theMenu.LoadMenu(IDR_PCDRAFTTYPE);
				CMenu* pPopup=theMenu.GetSubMenu(2);
				ClientToScreen(&pt);
				pPopup->TrackPopupMenu(TPM_LEFTBUTTON|TPM_LEFTALIGN,pt.x,pt.y,AfxGetMainWnd());
			}
			else
			{
                int i = kNumToolButtons - 1;
				for(;i>0;i--)
				{
					if(m_nHitID>=buttons[i]) break;
				}

				CBitmap bmpMenu;
				bmpMenu.LoadMappedBitmap(bitmaps[i]);
				CToolMenu theMenu(&bmpMenu); //m_nHitID-ID_TOOL_RADIO_FIRST]);
				ClientToScreen(&pt);
				int cmd=theMenu.TrackPopupMenu(pt,this);
				
				if(cmd>=0)
				{
					ASSERT(CommandToIndex(m_nHitID)==i); //-ID_TOOL_RADIO_FIRST,
					
					int iImage;
					
					UINT nID,nStyle;

					GetButtonInfo(i,nID,nStyle,iImage);
					SetButtonInfo(i,buttons[i]+cmd,nStyle, m_base[i]+cmd);
               AfxGetMainWnd()->SendMessage(WM_COMMAND, buttons[i] + cmd);
               GetToolBarCtrl().CheckButton(buttons[i], 0);
				}
			}
			m_nHitID=0;
			SendMessage(WM_LBUTTONUP,m_nHitFlags,MAKELPARAM(m_ptHit.x,m_ptHit.y));
		}
	}
	else CToolBar::OnTimer(nIDEvent);
}

void CToolPalette::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_nHitID)
	{
		KillTimer(kToolTimerID);
		m_nHitID=0;
	}

	CToolBar::OnLButtonUp(nFlags, point);
}

void CToolPalette::SelectButtonWithId(UINT nId)
{
	if(nId<ID_TOOL_MULTI_FIRST){
		return;
	}
	if(nId>ID_TOOL_MULTI_LAST){
		return;
	}

//	if(CommandToIndex(nId)>=0){
	//	return;
	//}
    int i = kNumToolButtons;
	for(; --i>=0; ){
		if(buttons[i]<=int(nId)){
			break;
		}
	}
   if (i < 0)
   {
      return;
   }
	int cmd = nId-buttons[i];

	int iImage;
	UINT nUnusedId,nStyle;
	GetButtonInfo(i,nUnusedId,nStyle,iImage);
	SetButtonInfo(i,buttons[i]+cmd,nStyle, m_base[i]+cmd);

}


CSize CToolPalette::CalcDynamicLayout(int nLength,DWORD dwMode)
{

   CSize size = CToolBar::CalcDynamicLayout(nLength, dwMode);

   //size.cx = size.cx * 96 / get_screen_dpi().cx;

   //size.cy = size.cy * 96 / get_screen_dpi().cy;

   return size;

}