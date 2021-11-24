#include "stdafx.h"
#include "ProgressControl.h"


BEGIN_MESSAGE_MAP(CProgressControl, CWnd)
   ON_WM_PAINT()
END_MESSAGE_MAP()


CProgressControl::CProgressControl()
{

   m_font.CreatePointFont(92, "Segoe UI");
   m_dRate = 0.0;

}

void CProgressControl::OnPaint()
{

   CPaintDC dc(this); 

   CRect rectClient;

   GetClientRect(rectClient);

   CRect rectProgress(rectClient);

   rectProgress.left = 0;

   double dRate = m_dRate;

   if (dRate < 0.0)
      dRate = 0.0;
   else if (dRate > 1.0)
      dRate = 1.0;

   rectProgress.right = dRate * rectProgress.Width();
   
   dc.FillSolidRect(rectClient, RGB(230, 230, 230));
   
   dc.FillSolidRect(rectProgress, RGB(100, 200, 120));

   dc.SetBkMode(TRANSPARENT);

   dc.SetTextColor(RGB(255, 255, 255));

   rectClient.left += 10;

   dc.SelectObject(&m_font);

   dc.DrawText(m_str + CString('.', (GetTickCount() / 500 % 4)), rectClient, DT_LEFT | DT_VCENTER);


}
