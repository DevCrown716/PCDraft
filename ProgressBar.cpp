////////////////////////////////////////////////////////////////
// MSDN Magazine EJanuary 2003
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0 or Visual Studio .NET on Windows XP. 
// Tab size=3.
//
#include "StdAfx.h"
#include "Resource.h"
#include "ProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CProgStatusBar, CStatusBar)
BEGIN_MESSAGE_MAP(CProgStatusBar, CStatusBar)
   ON_WM_CREATE()
   ON_WM_SIZE()
END_MESSAGE_MAP()

CProgStatusBar::CProgStatusBar()
{
}

CProgStatusBar::~CProgStatusBar()
{
}

/////////////////
// Status bar created: create progress bar too.
//
int CProgStatusBar::OnCreate(LPCREATESTRUCT lpcs)
{
   lpcs->style |= WS_CLIPCHILDREN;
   VERIFY(CStatusBar::OnCreate(lpcs) == 0);
   VERIFY(m_progresscontrol.Create(this));
   return 0;
}

//////////////////
// Status bar was sized: adjust size of progress bar to same as first
// pane (ready message). Note that the progress bar may or may not be
// visible.
//
void CProgStatusBar::OnSize(UINT nType, int cx, int cy)
{
   

   CStatusBar::OnSize(nType, cx, cy); // call base class

      CRect rc;                          // rectangle 
   GetItemRect(1, &rc);               // item 0 = first pane, "ready" 
                                      // message
   m_progresscontrol.MoveWindow(&rc, FALSE);// move progress bar
}

//////////////////
// Set progress bar position. pct is an integer from 0 to 100:
//
//  0 = hide progress bar and display ready message (done);
// >0 = (assumed 0-100) set progress bar position
//
// You should call this from your main frame to update progress.
// (See Mainfrm.cpp)
//
void CProgStatusBar::OnProgress(UINT pct)
{

   CProgressControl& pc = m_progresscontrol;

   DWORD dwOldStyle = pc.GetStyle();

   DWORD dwNewStyle = dwOldStyle;

   if (pct > 0)
   {
    
      // positive progress: show prog bar
      dwNewStyle |= WS_VISIBLE;

   }
   else
   {

      // prog <= 0: hide prog bar
      dwNewStyle &= ~WS_VISIBLE;

   }

   if (dwNewStyle != dwOldStyle)
   {

      // change state of hide/show
      SetWindowText(NULL);                               // clear old text
      SetWindowLong(pc.m_hWnd, GWL_STYLE, dwNewStyle);   // change style

   }

   m_progresscontrol.set_rate((double)pct / 1000000.0);

   if (pct == 0)
   {
      // display MFC idle (ready) message.
      GetParent()->SendMessage(WM_SETMESSAGESTRING, AFX_IDS_IDLEMESSAGE);

   }

}



