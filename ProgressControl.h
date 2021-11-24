#pragma once
#include <afxwin.h>
class CProgressControl :
   public CWnd
{
public:

   CFont          m_font;
   double         m_dRate;
   CString        m_str;

   CProgressControl();

   BOOL Create(CWnd * pwndParent)
   {

      return CWnd::Create("static", "", WS_CHILD | WS_VISIBLE, CRect(), pwndParent, 0);

   }

   void set_rate(double dRate) { m_dRate = dRate; RedrawWindow(); }

   DECLARE_MESSAGE_MAP()

   afx_msg void OnPaint();


};

