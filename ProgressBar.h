////////////////////////////////////////////////////////////////
// MSDN Magazine January 2003
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0 or Visual Studio .NET on Windows XP. 
// Tab size=3.
//
#pragma once

#define MYWM_PROGRESS (WM_APP + 100)

#include "ProgressControl.h"

//////////////////
// Status bar with progress control.
//
class CProgStatusBar : public CStatusBar {
public:
   CProgStatusBar();
   virtual ~CProgStatusBar();
   CProgressControl& GetProgressControl() {
      return m_progresscontrol;
   }
   void OnProgress(UINT pct);
   CProgressControl m_progresscontrol;          // the progress bar

protected:
   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
   afx_msg void OnSize(UINT nType, int cx, int cy);
   DECLARE_MESSAGE_MAP()
   DECLARE_DYNAMIC(CProgStatusBar)
};